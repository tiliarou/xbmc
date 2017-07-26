/*
 *      Copyright (C) 2017 Team XBMC
 *      http://xbmc.org
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

texture2D g_Texture;
float2    g_viewPort;

SamplerState MySampler : IMMUTABLE
{
  AddressU = WRAP;
  AddressV = WRAP;
  Filter   = MIN_MAG_MIP_POINT;
};

struct VS_INPUT
{
  float4 Position   : POSITION;
  float2 TextureUV  : TEXCOORD0;
};

struct VS_OUTPUT
{
  float2 TextureUV  : TEXCOORD0;
  float4 Position   : SV_POSITION;
};

//
// VS for rendering in screen space
//
VS_OUTPUT VS_SHADER(VS_INPUT In)
{
  VS_OUTPUT output  = (VS_OUTPUT)0;
  output.Position.x =  (In.Position.x / (g_viewPort.x  / 2.0)) - 1;
  output.Position.y = -(In.Position.y / (g_viewPort.y / 2.0)) + 1;
  output.Position.z = output.Position.z;
  output.Position.w = 1.0;
  output.TextureUV  = In.TextureUV;

  return output;
}
 
float4 output4(float4 color, float2 uv)
{
  return color;
}

float4 output(float3 color, float2 uv)
{
    return output4(float4(color, 1.0), uv);
}

float3 m_params; // 0 - range (0 - full, 1 - limited), 1 - contrast, 2 - brightness

float4 OUTPUT_PS(VS_OUTPUT In) : SV_TARGET
{
  float4 color = g_Texture.Sample(MySampler, In.TextureUV);
  
  // color = saturate(0.0625 + color * 219.0 / 255.0);

  // TODO: Do we need m_params?
  //color *= m_params.y * 2.0;
  //color += m_params.z - 0.9;
    
  color *= 0.5;     // for testing
  color.a = 1.0;
  
  return output4(color, In.TextureUV);
}

technique10 OUTPUT_T
{
  pass P0
  {
    SetVertexShader( CompileShader( vs_4_0_level_9_1, VS_SHADER() ) );
    SetPixelShader( CompileShader( ps_4_0_level_9_1, OUTPUT_PS() ) );
  }
};