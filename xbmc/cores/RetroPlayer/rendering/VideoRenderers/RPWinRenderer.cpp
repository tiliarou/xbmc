/*
 *      Copyright (C) 2017 Team Kodi
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this Program; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "RPWinRenderer.h"

#include "commons/ilog.h"
#include "cores/RetroPlayer/RetroPlayerDefines.h"
#include "cores/RetroPlayer/VideoShaderPreset.h"
#include "cores/VideoPlayer/VideoRenderers/VideoShaders/WinVideoFilter.h"
#include "filesystem/Directory.h"
#include "filesystem/SpecialProtocol.h"
#include "guilib/GraphicContext.h"
#include "settings/MediaSettings.h"
#include "utils/URIUtils.h"
#include "utils/win32/Win32Log.h"
#include "windowing/windows/WinSystemWin32DX.h"

CRPWinRenderer::CRPWinRenderer()
  : CWinRenderer()
  , m_bPresetNeedsUpdate(true)
{
  m_pVideoShaders.reserve(SHADER_PRESET_MAX_SHADERS);
  m_pShaderTextures.reserve(SHADER_PRESET_MAX_SHADERS);

  SetShaderPreset("nearest.cgp");
}

CRPWinRenderer::~CRPWinRenderer()
{
  DisposeVideoShaders();
  // The gui is going to render after this, so apply the state required
  g_Windowing.ApplyStateBlock();
}

CBaseRenderer* CRPWinRenderer::Create(CVideoBuffer *buffer)
{
  return new CRPWinRenderer();
}

//void CRPWinRenderer::RenderUpdate(bool clear, unsigned flags, unsigned alpha)
//{
//  if (clear)
//    g_graphicsContext.Clear(g_Windowing.UseLimitedColor() ? 0x101010 : 0);
//
//  if (!m_bConfigured)
//    return;
//
//  g_Windowing.SetAlphaBlendEnable(alpha < 255);
//  ManageTextures();
//  ManageRenderArea();
//  Render(flags, g_Windowing.GetBackBuffer());
//}

// TODO: Remove backend-specific stuff and move this elsewhere (RM?)
void CRPWinRenderer::UpdateVideoShaders()
{
  if (m_bPresetNeedsUpdate)
  {
    m_bPresetNeedsUpdate = false;

    if (m_videoShaderPath.empty())
      return;

    DisposeVideoShaders();

    static const auto shaderFormat = "hlsl";   // "hlsl" or "glsl". Windows uses HLSL shaders (with .cg extension for compat. purposes)
    static const auto presetPath = URIUtils::AddFileToFolder("libretro", shaderFormat, m_videoShaderPath);
    static const auto presetPathBase = URIUtils::GetBasePath(presetPath);

    m_pPreset.reset(new SHADERPRESET::CVideoShaderPreset(presetPath));

    if (!m_pPreset->Init())
      return;

    auto& preset = *m_pPreset;

    auto backBuffer = g_Windowing.GetBackBuffer();
    auto numPasses = std::min(preset.m_Passes, static_cast<unsigned>(SHADER_PRESET_MAX_SHADERS));

    for (unsigned shaderIdx = 0; shaderIdx < numPasses; ++shaderIdx) {
      // For reach pass, create the texture
      auto& pass = preset.m_Pass[shaderIdx];

      std::unique_ptr<CD3DTexture> texture(new CD3DTexture());
      auto res = texture->Create(backBuffer->GetWidth(), backBuffer->GetHeight(), 1,
        D3D11_USAGE_DEFAULT, DXGI_FORMAT_B8G8R8A8_UNORM, nullptr, 0);
      if (!res)
      {
        CLog::Log(LOGERROR, "Couldn't create a texture for video shader %s.", pass.source.path);
        break;
      }

       m_pShaderTextures.push_back(std::move(texture));

      // And the shader itself
      std::unique_ptr<CVideoShader> videoShader(new CVideoShader());

      auto shaderSrc = pass.source.string.vertex; // not just vertex, also fragment source
      auto absoluteAddonBasePath = m_pPreset->GetLibraryBasePath();
      auto shaderPath = URIUtils::AddFileToFolder(absoluteAddonBasePath, presetPathBase, pass.source.path);

      if (!videoShader->Create(shaderSrc, shaderPath))
      {
        CLog::Log(LOGERROR, "Couldn't create a video shader");
        break;
      }

      m_pVideoShaders.push_back(std::move(videoShader));
    }
  }
}

void CRPWinRenderer::DisposeVideoShaders()
{
  m_pVideoShaders.clear();
  m_pShaderTextures.clear();
  m_pPreset.reset();
}

void CRPWinRenderer::Render(DWORD flags, CD3DTexture* target)
{
  if (!m_renderBuffers[m_iYV12RenderBuffer].loaded)
  {
    if (!m_renderBuffers[m_iYV12RenderBuffer].UploadBuffer())

      g_Windowing.ApplyStateBlock();
      return;
  }

  UpdateVideoFilter();
  UpdateVideoShaders();

  switch (m_renderMethod)
  {
  case RENDER_DXVA:
    // TODO: Disable DXVA
    RenderHW(flags, target);
    break;
  case RENDER_PS:
    RenderPS(target);
    break;
  case RENDER_SW:
    RenderSW(target);
    break;
  default:

  g_Windowing.ApplyStateBlock();
    return;
  }

  if (m_bUseHQScaler)
    RenderHQ(target);

  g_Windowing.ApplyStateBlock();
}

void CRPWinRenderer::RenderPS(CD3DTexture* target)
{
  bool useVideoShaders = false;

  if (m_pVideoShaders.size() > 0 || m_pShaderTextures.size() > 0)
  {
    // Each pass must have its own texture and the opposite is also true
    useVideoShaders = m_pVideoShaders.size() == m_pShaderTextures.size();
    if (!useVideoShaders)
    {
      CLog::Log(LOGWARNING, __FUNCTION__": VideoShaders: a shader or texture failed to init. Disabling video shaders.");
      DisposeVideoShaders();
    }
  }

  if (m_bUseHQScaler)
    target = &m_IntermediateTarget;

  CD3D11_VIEWPORT viewPort(0.0f, 0.0f, static_cast<float>(target->GetWidth()), static_cast<float>(target->GetHeight()));

  if (m_bUseHQScaler)
    g_Windowing.ResetScissors();

  // reset view port
  g_Windowing.Get3D11Context()->RSSetViewports(1, &viewPort);

  // select destination rectangle
  CPoint destPoints[4];
  if (m_renderOrientation)
  {
    for (size_t i = 0; i < 4; i++)
      destPoints[i] = m_rotatedDestCoords[i];
  }
  else
  {
    CRect destRect = m_bUseHQScaler ? m_sourceRect : g_graphicsContext.StereoCorrection(m_destRect);
    destPoints[0] = { destRect.x1, destRect.y1 };
    destPoints[1] = { destRect.x2, destRect.y1 };
    destPoints[2] = { destRect.x2, destRect.y2 };
    destPoints[3] = { destRect.x1, destRect.y2 };
  }

  // render video frame
  // TODO: Abstract multipass rendering pipeline to a separate function and use it in SW too
  if (useVideoShaders)
  {
    // Render color shader to the first pass's texture
    m_colorShader->Render(m_sourceRect, destPoints,
      CMediaSettings::GetInstance().GetCurrentVideoSettings().m_Contrast,
      CMediaSettings::GetInstance().GetCurrentVideoSettings().m_Brightness,
      &m_renderBuffers[m_iYV12RenderBuffer], m_pShaderTextures.front().get());

    // Apply all passes except the last one (needs to be written to the backbuffer)
    for (unsigned shaderIdx = 0; shaderIdx < m_pVideoShaders.size() - 1; ++shaderIdx)
    {
      m_pVideoShaders[shaderIdx]->Render(*m_pShaderTextures[shaderIdx],
        m_destRect, destPoints, m_pShaderTextures[shaderIdx + 1].get());
    }

    // Apply last pass and write to target (backbuffer)
    m_pVideoShaders.back()->Render(*m_pShaderTextures.back(), m_destRect, destPoints, target);
  }
  else // Render just the color shader and output straight to the backbuffer
  {
    m_colorShader->Render(m_sourceRect, destPoints,
      CMediaSettings::GetInstance().GetCurrentVideoSettings().m_Contrast,
      CMediaSettings::GetInstance().GetCurrentVideoSettings().m_Brightness,
      &m_renderBuffers[m_iYV12RenderBuffer], target);
  }
  // Restore our view port.
  g_Windowing.RestoreViewPort();
}

void CRPWinRenderer::RenderSW(CD3DTexture* target)
{
  // TODO:

  //CWinRenderer::RenderSW(&m_pShaderTexture1);

  //m_testShader1->Render(m_pShaderTexture1, m_sourceWidth, m_sourceHeight,
  //  m_sourceRect, m_destRect, &m_pShaderTexture2);

  //m_testShader2->Render(m_pShaderTexture2, m_sourceWidth, m_sourceHeight,
  //  m_sourceRect, m_destRect, target);
}

void CRPWinRenderer::SetShaderPreset(const std::string& shaderPresetPath)
{
  m_bPresetNeedsUpdate = true;
  m_videoShaderPath = shaderPresetPath;
}
