#pragma once
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

#include "cores/VideoPlayer/VideoRenderers/VideoShaders/WinVideoFilter.h"
#include "guilib/GUIShaderDX.h"
#include "filesystem/File.h"
#include "cores/RetroPlayer/VideoShaderPreset.h"

/*
class CVideoShader : public CWinShader
{
public:
  bool Create(std::string shaderPath = "");
  void Render(CD3DTexture &sourceTexture, unsigned sourceWidth, unsigned sourceHeight, CRect sourceRect, const CPoint points[4]
    , CD3DTexture *target);
  void Render(CD3DTexture &sourceTexture, unsigned sourceWidth, unsigned sourceHeight, CRect sourceRect, CRect destRect
    , CD3DTexture *target);

private:

  void PrepareParameters(unsigned sourceWidth, unsigned sourceHeight, CRect sourceRect, const CPoint points[4]);
  void SetShaderParameters(CD3DTexture &sourceTexture);


  unsigned m_sourceWidth{ 0 };
  unsigned m_sourceHeight{ 0 };
  CRect m_sourceRect{ 0.f, 0.f, 0.f, 0.f };
  CPoint m_destPoints[4] =
  {
    { 0.f, 0.f },
    { 0.f, 0.f },
    { 0.f, 0.f },
    { 0.f, 0.f },
  };

  struct CUSTOMVERTEX {
    FLOAT x, y, z;
    FLOAT tu, tv;
  };
};
*/

class CVideoPixelShader : public CD3DPixelShader
{
public:
  CVideoPixelShader() : CD3DPixelShader() {}
  //void OnLostDevice() override;
  //void OnResetDevice() override;
  //void OnDestroyDevice() override;
  //~CVideoPixelShader() override;
  //void OnDestroyDevice(bool fatal) override;
  //void OnCreateDevice() override;
};

class CVideoVertexShader : public CD3DVertexShader
{
public:
  CVideoVertexShader() : CD3DVertexShader() {}
  //void OnLostDevice() override;
  //void OnResetDevice() override;
  //void OnDestroyDevice() override;
  //~CVideoVertexShader() override;
  //void OnDestroyDevice(bool fatal) override;
  //void OnCreateDevice() override;
};


// libretro's "Common shaders"
// Spec here: https://github.com/libretro/common-shaders/blob/master/docs/README
// Some parts of the implementation based on CGUIShaderDX
class CVideoShader : public ID3DInclude
{
public:
  CVideoShader();
  explicit CVideoShader(const std::string& shaderSource);
  ~CVideoShader();

  bool Create(const std::string& shaderSource, const std::string& shaderPath = "");
  bool Render(CD3DTexture &sourceTexture, CRect sourceRect, const CPoint points[4], CD3DTexture *target);
  void DrawQuad(Vertex& v1, Vertex& v2, Vertex& v3, Vertex& v4);
  void DrawIndexed(unsigned indexCount, unsigned startIndex, unsigned startVertex);
  void Draw(unsigned vertexCount, unsigned startVertex);
  void ApplyChanges();
  void Release();

  // ID3DInclude interface
  __declspec(nothrow) HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes) override;
  __declspec(nothrow) HRESULT __stdcall Close(LPCVOID pData) override;

private:
  bool Compile(void);
  bool DoCompileShader(const std::string& shaderSource, const char* pEntrypoint, const char* pTarget, ID3DBlob **byteCode);
  bool CreateBuffers(void);
  bool CreateSamplers(void);
  void ApplyStateBlock();
  void RestoreBuffers();
  void Begin();
  void End();

  bool m_bCreated;

  // Vertex buffer
  ID3D11Buffer* m_pVertexBuffer;

  // test
  ID3D11Buffer* m_pInputBuffer;

  // Point/nearest neighbor sampler
  ID3D11SamplerState* m_pSampNearest;

  // Shader bytecode
  // Compile() outputs to these
  ID3DBlob* m_pVertexShaderCode;
  ID3DBlob* m_pPixelShaderCode;

  // VideoShader instances
  CVideoVertexShader m_vertexShader;
  CVideoPixelShader m_pixelShader;

  // Currently loaded shader's source code
  std::string m_shaderSource;

  // Currenlt loaded shader's relative path
  std::string m_shaderPath;

  struct cbWorldViewProj
  {
    XMMATRIX world;
    XMMATRIX view;
    XMMATRIX projection;
  };
  struct cbViewPort
  {
    float TopLeftX;
    float TopLeftY;
    float Width;
    float Height;
  };
  struct cbWorld
  {
    XMMATRIX wvp;
    float blackLevel;
    float colorRange;
  };
  struct cbInput
  {
    XMFLOAT2 video_size;
    XMFLOAT2 texture_size;
    XMFLOAT2 output_size;
    float frame_count;
    float frame_direction;
  };


  ID3D11Buffer* m_pWVPBuffer;
  ID3D11Buffer* m_pVPBuffer;
  bool m_bIsWVPDirty;
  bool m_bIsVPDirty;
  cbViewPort          m_cbViewPort;
  cbWorldViewProj     m_cbWorldViewProj;
};
