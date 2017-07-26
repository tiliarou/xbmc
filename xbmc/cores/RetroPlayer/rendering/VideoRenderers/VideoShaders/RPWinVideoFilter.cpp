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
*  along with KODI; see the file COPYING.  If not, see
*  <http://www.gnu.org/licenses/>.
*
*/

#include "RPWinVideoFilter.h"
#include "commons/ilog.h"
#include "utils/win32/Win32Log.h"
#include "windowing/windows/WinSystemWin32DX.h"
#include "utils/URIUtils.h"
#include "guilib/GraphicContext.h"


CVideoShader::CVideoShader() :
  m_bCreated(false)
  , m_pSampNearest(nullptr)
  , m_pVertexBuffer(nullptr)
  , m_pInputBuffer(nullptr)
  , m_pVertexShaderCode(nullptr)
  , m_pPixelShaderCode(nullptr)
  , m_pWVPBuffer(nullptr), m_pVPBuffer(nullptr), m_bIsWVPDirty(false), m_bIsVPDirty(false)
{
  m_vertexShader = CVideoVertexShader();
  m_pixelShader = CVideoPixelShader();
}

CVideoShader::CVideoShader(const std::string& shaderPath)
{
  Create(shaderPath);
}

CVideoShader::~CVideoShader()
{
  Release();
}

bool CVideoShader::Create(const std::string& shaderSource, const std::string& shaderPath)
{
  m_bCreated = false;
  m_shaderSource = shaderSource;
  m_shaderPath = shaderPath;

  // Create input layout
  D3D11_INPUT_ELEMENT_DESC layout[] =
  {
    { "SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
  };

  if (!Compile())
  {
    CLog::Log(LOGERROR, "%s - videoshader compilation failed", __FUNCTION__);
    return false;
  }

  bool bSuccess = true;
  if (!m_vertexShader.Create(m_pVertexShaderCode, layout, ARRAYSIZE(layout)))
    bSuccess = false;

  if (bSuccess && !m_pixelShader.Create(m_pPixelShaderCode))
    bSuccess = false;

  if (!bSuccess)
  {
    m_vertexShader.Release();
    m_pixelShader.Release();
  }

  if (!bSuccess || !CreateBuffers() || !CreateSamplers())
    return false;

  m_bCreated = true;
  return true;
}

bool CVideoShader::CreateBuffers()
{
  ID3D11Device* pDevice = g_Windowing.Get3D11Device();

  // create vertex buffer
  CD3D11_BUFFER_DESC bufferDesc(sizeof(Vertex) * 4, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
  if (FAILED(pDevice->CreateBuffer(&bufferDesc, NULL, &m_pVertexBuffer)))
  {
    CLog::Log(LOGERROR, __FUNCTION__ " - Failed to create videoshader vertex buffer.");
    return false;
  }

  // Create the constant buffer for WVP
  size_t buffSize = (sizeof(cbWorld) + 15) & ~15;
  CD3D11_BUFFER_DESC cbWVPDesc(buffSize, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
  if (FAILED(pDevice->CreateBuffer(&cbWVPDesc, NULL, &m_pWVPBuffer)))
  {
    CLog::Log(LOGERROR, __FUNCTION__ " - Failed to create the constant buffer.");
    return false;
  }

  m_bIsWVPDirty = true;

  CRect viewPort;
  g_Windowing.GetViewPort(viewPort);

  // initial data for viewport buffer
  m_cbViewPort.TopLeftX = viewPort.x1;
  m_cbViewPort.TopLeftY = viewPort.y1;
  m_cbViewPort.Width = viewPort.Width();
  m_cbViewPort.Height = viewPort.Height();

  cbWVPDesc.ByteWidth = sizeof(cbViewPort);
  D3D11_SUBRESOURCE_DATA initWVPSubresource = { &m_cbViewPort, 0, 0 };
  // create viewport buffer
  if (FAILED(pDevice->CreateBuffer(&cbWVPDesc, &initWVPSubresource, &m_pVPBuffer)))
    return false;

  // TODO
  cbInput inputInitData =
  {
    { 160, 144 },
    { viewPort.Width(), viewPort.Height() },
    { viewPort.Width(), viewPort.Height() },
    { 0 },
    { 1 }
  };
  auto inputBufSize = (sizeof(cbInput) + 15) & ~15;
  CD3D11_BUFFER_DESC cbInputDesc(inputBufSize, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
  D3D11_SUBRESOURCE_DATA initInputSubresource = { &inputInitData, 0, 0 };
  if (FAILED(pDevice->CreateBuffer(&cbInputDesc, &initInputSubresource, &m_pInputBuffer)))
  {
    CLog::Log(LOGERROR, __FUNCTION__ " - Failed to create constant buffer for video shader input data.");
    return false;
  }

  return true;
}

bool CVideoShader::CreateSamplers()
{
  // Describe the Sampler State
  D3D11_SAMPLER_DESC sampDesc;
  ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
  sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
  sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;   // TODO: Make this border. wrap for debugging
  sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sampDesc.MinLOD = 0;
  sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

  if (FAILED(g_Windowing.Get3D11Device()->CreateSamplerState(&sampDesc, &m_pSampNearest)))
    return false;

  g_Windowing.Get3D11Context()->PSSetSamplers(0, 1, &m_pSampNearest);

  return true;
}

void CVideoShader::ApplyStateBlock()
{
  if (!m_bCreated)
    return;

  ID3D11DeviceContext* pContext = g_Windowing.Get3D11Context();

  m_vertexShader.BindShader();
//  pContext->VSSetConstantBuffers(0, 1, &m_pWVPBuffer);
  pContext->VSSetConstantBuffers(0, 1, &m_pInputBuffer);

  m_pixelShader.BindShader();
  //pContext->PSSetConstantBuffers(0, 1, &m_pWVPBuffer);
  pContext->PSSetConstantBuffers(0, 1, &m_pInputBuffer);

  pContext->PSSetSamplers(0, 1, &m_pSampNearest);

  RestoreBuffers();
}

void CVideoShader::RestoreBuffers(void)
{
  const unsigned stride = sizeof(Vertex);
  const unsigned offset = 0;

  ID3D11DeviceContext* pContext = g_Windowing.Get3D11Context();
  // Set the vertex buffer to active in the input assembler so it can be rendered.
  pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
  // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
  pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void CVideoShader::Begin()
{
  if (!m_bCreated)
    return;

  m_pixelShader.BindShader();
}

void CVideoShader::End()
{
  if (!m_bCreated)
    return;
}

bool CVideoShader::Render(CD3DTexture& sourceTexture, CRect sourceRect,
  const CPoint points[4], CD3DTexture* target)
{
  ID3D11DeviceContext* pContext = g_Windowing.Get3D11Context();
  ComPtr<ID3D11RenderTargetView> oldRT;

  // The render target will be overridden: save the caller's original RT
  if (target)
    pContext->OMGetRenderTargets(1, &oldRT, nullptr);

  ApplyStateBlock();

  XMFLOAT4 xcolor;
  auto color = 0x5000;
  CD3DHelper::XMStoreColor(&xcolor, color);
  CRect coords = CRect(0.0f, 0.0f, 1.0f, 1.0f);

  Vertex verts[4] = {
    { XMFLOAT3(points[0].x, points[0].y, 0), xcolor, XMFLOAT2(coords.x1, coords.y1), XMFLOAT2(0.0f, 0.0f) },
    { XMFLOAT3(points[1].x, points[1].y, 0), xcolor, XMFLOAT2(coords.x2, coords.y1), XMFLOAT2(0.0f, 0.0f) },
    { XMFLOAT3(points[2].x, points[2].y, 0), xcolor, XMFLOAT2(coords.x2, coords.y2), XMFLOAT2(0.0f, 0.0f) },
    { XMFLOAT3(points[3].x, points[3].y, 0), xcolor, XMFLOAT2(coords.x1, coords.y2), XMFLOAT2(0.0f, 0.0f) },
  };

  if (target)
    g_Windowing.Get3D11Context()->OMSetRenderTargets(1, target->GetAddressOfRTV(), nullptr);

  // set step params


  Begin();
//  if (view && numViews > 0)
//    pGUIShader->SetShaderViews(numViews, view);
  DrawQuad(verts[0], verts[1], verts[2], verts[3]);
  //DrawIndexed(4, 0, 4);

  End();

  if (oldRT)
    pContext->OMSetRenderTargets(1, oldRT.GetAddressOf(), nullptr);

  return false;
}


void CVideoShader::DrawQuad(Vertex& v1, Vertex& v2, Vertex& v3, Vertex& v4)
{
  if (!m_bCreated)
    return;

  ApplyChanges();

  ID3D11DeviceContext* pContext = g_Windowing.Get3D11Context();

  // update vertex buffer
  D3D11_MAPPED_SUBRESOURCE resource;
  if (SUCCEEDED(pContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource)))
  {
    // we are using strip topology
    Vertex vertices[4] = { v2, v3, v1, v4 };
    memcpy(resource.pData, &vertices, sizeof(Vertex) * 4);
    pContext->Unmap(m_pVertexBuffer, 0);
    // Draw primitives
    pContext->Draw(4, 0);
  }

  // update input cbuffer
  D3D11_MAPPED_SUBRESOURCE resourceInput;
  if (SUCCEEDED(pContext->Map(m_pInputBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resourceInput)))
  {
    // we are using strip topology
    cbInput inputInitData =
    {
      { 160, 144 },
      { 1000, 800 },
      { 1000, 800 },
      { 0 },
      { 1 }
    };
    memcpy(resourceInput.pData, &inputInitData, sizeof(cbInput));
    pContext->Unmap(m_pInputBuffer, 0);
    // Draw primitives
    pContext->Draw(4, 0);
  }
}
/* TODO
void CVideoShader::SetViewPort(D3D11_VIEWPORT viewPort)
{
  if (!m_pVPBuffer)
    return;

  if (viewPort.TopLeftX != m_cbViewPort.TopLeftX
    || viewPort.TopLeftY != m_cbViewPort.TopLeftY
    || viewPort.Width != m_cbViewPort.Width
    || viewPort.Height != m_cbViewPort.Height)
  {
    m_cbViewPort.TopLeftX = viewPort.TopLeftX;
    m_cbViewPort.TopLeftY = viewPort.TopLeftY;
    m_cbViewPort.Width = viewPort.Width;
    m_cbViewPort.Height = viewPort.Height;
    m_bIsVPDirty = true;
  }
}
*/
void CVideoShader::DrawIndexed(unsigned int indexCount, unsigned int startIndex, unsigned int startVertex)
{
  if (!m_bCreated)
    return;

  ApplyChanges();
  g_Windowing.Get3D11Context()->DrawIndexed(indexCount, startIndex, startVertex);
}

void CVideoShader::Draw(unsigned int vertexCount, unsigned int startVertex)
{
  if (!m_bCreated)
    return;

  ApplyChanges();
  g_Windowing.Get3D11Context()->Draw(vertexCount, startVertex);
}

void CVideoShader::ApplyChanges(void)
{
  ID3D11DeviceContext* pContext = g_Windowing.Get3D11Context();
  D3D11_MAPPED_SUBRESOURCE res;
  /*
  if (m_bIsWVPDirty)
  {
    if (SUCCEEDED(pContext->Map(m_pWVPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res)))
    {
      XMMATRIX worldView = XMMatrixMultiply(m_cbWorldViewProj.world, m_cbWorldViewProj.view);
      XMMATRIX worldViewProj = XMMatrixMultiplyTranspose(worldView, m_cbWorldViewProj.projection);

      cbWorld* buffer = (cbWorld*)res.pData;
      buffer->wvp = worldViewProj;
      buffer->blackLevel = (g_Windowing.UseLimitedColor() ? 16.f / 255.f : 0.f);
      buffer->colorRange = (g_Windowing.UseLimitedColor() ? (235.f - 16.f) / 255.f : 1.0f);

      pContext->Unmap(m_pWVPBuffer, 0);
      m_bIsWVPDirty = false;
    }
  }
  */
  // update view port buffer
  if (m_bIsVPDirty)
  {
    if (SUCCEEDED(pContext->Map(m_pVPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res)))
    {
      *(cbViewPort*)res.pData = m_cbViewPort;
      pContext->Unmap(m_pVPBuffer, 0);
      m_bIsVPDirty = false;
    }
  }
}

void CVideoShader::Release()
{
  SAFE_RELEASE(m_pSampNearest);
  SAFE_RELEASE(m_pPixelShaderCode);
  SAFE_RELEASE(m_pVertexShaderCode);
  SAFE_RELEASE(m_pVertexBuffer);
  SAFE_RELEASE(m_pWVPBuffer);
  SAFE_RELEASE(m_pVPBuffer);
  m_bCreated = false;
}

bool CVideoShader::Compile()
{
  // compile vertex shader
  if (!DoCompileShader(m_shaderSource, "main_vertex", "vs_4_0", &m_pVertexShaderCode))
  {
    CLog::Log(LOGERROR, "%s - couldn't compile vertex shader in %s", __FUNCTION__, m_shaderPath.c_str());
    return false;
  }
  //m_vertexShader.Create(m_pVertexShaderCode->GetBufferPointer(), m_pVertexShaderCode->GetBufferSize());
  CLog::Log(LOGERROR, "%s - vertex shader compiled! %s", __FUNCTION__, m_shaderPath.c_str());

  // compile pixel shader
  if (!DoCompileShader(m_shaderSource, "main_fragment", "ps_4_0", &m_pPixelShaderCode))
  {
    CLog::Log(LOGERROR, "%s - couldn't compile pixel shader in %s", __FUNCTION__, m_shaderPath.c_str());
    return false;
  }

  //m_pixelShader.Create(m_pPixelShaderCode->GetBufferPointer(), m_pPixelShaderCode->GetBufferSize());
  CLog::Log(LOGERROR, "%s - pixel shader compiled! %s", __FUNCTION__, m_shaderPath.c_str());
  return true;
}

bool CVideoShader::DoCompileShader(const std::string& shaderSource, const char* pEntrypoint, const char* pTarget, ID3DBlob** byteCode)
{
  // Compile shader
  LPCVOID pSrcData = shaderSource.c_str();
  SIZE_T SrcDataSize = shaderSource.size();
  LPCSTR pSourceName = URIUtils::GetFileName(m_shaderPath).c_str(); //TODO: fix
  const D3D_SHADER_MACRO pDefines[] = { { "HLSL_4", "" }, { 0 } };
  ID3DInclude* pInclude = this;
  UINT Flags = 0;
  ID3DBlob* ppErrorMsgs = nullptr;

#if _DEBUG
  Flags |= D3DCOMPILE_DEBUG;
  //Disable optimizations to further improve shader debugging
  Flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
  // All of this is new code so make sure we take care of all warnings too
  // Flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#else
  // According to MSDN this might increases compile time significantly
  // Going to have to test it
  // Flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

  HRESULT compileRes = D3DCompile(pSrcData, SrcDataSize, pSourceName, pDefines, pInclude,
    pEntrypoint, pTarget, Flags, 0, byteCode, &ppErrorMsgs);


  if (compileRes == S_OK)
  {
#if _DEBUG
    // Print warnings
    if (ppErrorMsgs)
    {
      std::string warnings;
      warnings.assign((const char*)ppErrorMsgs->GetBufferPointer(), ppErrorMsgs->GetBufferSize());
      CLog::Log(LOGWARNING, "%s - shader compilation warnings: %s", __FUNCTION__, warnings.c_str());
    }
#endif
    return true;
  }
  else if (ppErrorMsgs)
  {
    std::string error;
    error.assign((const char*)ppErrorMsgs->GetBufferPointer(), ppErrorMsgs->GetBufferSize());
    CLog::Log(LOGERROR, "%s - %s", __FUNCTION__, error.c_str());
  }
  else
    CLog::Log(LOGERROR, "%s - call to D3DCompile() failed with %" PRId32, __FUNCTION__, compileRes);
  return false;
}

// ID3DInclude interface
HRESULT CVideoShader::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
{
  XFILE::CFile includeFile;
  const std::string filePath(
    URIUtils::CanonicalizePath(
      URIUtils::AddFileToFolder(
      URIUtils::GetBasePath(m_shaderPath),
      pFileName),
    '/'));

  if (!includeFile.Open(filePath))
  {
    CLog::Log(LOGERROR, "%s: Could not open VideoShader include file: %s", __FUNCTION__, filePath.c_str());
    return E_FAIL;
  }

  auto length = static_cast<size_t>(includeFile.GetLength());
  void *pData = malloc(length);
  if (includeFile.Read(pData, length) != length)
  {
    free(pData);
    return E_FAIL;
  }
  *ppData = pData;
  *pBytes = length;

  return S_OK;
}

HRESULT CVideoShader::Close(LPCVOID pData)
{
  free(const_cast<void*>(pData));
  return S_OK;
}