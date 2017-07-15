/*
*      Copyright (C) 2017 Team Kodi
 *     http://kodi.tv
 *
 * This Program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This Program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this Program; see the file COPYING.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */

#include "ShaderPreset.h"
#include "utils/log.h"

using namespace KODI;
using namespace SHADERPRESET;

CShaderPresetAddon::CShaderPresetAddon(const ADDON::BinaryAddonBasePtr& addonInfo)
  : IAddonInstanceHandler(ADDON_INSTANCE_SHADERPRESET, addonInfo)
{
  ResetProperties();
}

CShaderPresetAddon::~CShaderPresetAddon(void)
{
  DestroyAddon();
}

void CShaderPresetAddon::ResetProperties(void)
{
  // Initialise members
  m_struct = {{ 0 }};
  m_struct.toKodi.kodiInstance = this;
}


config_file_t_ *CShaderPresetAddon::ConfigFileNew(const char *path)
{
   return m_struct.toAddon.config_file_new(&m_struct, path);
}

config_file_t_ *CShaderPresetAddon::ConfigFileNewFromString(const char *from_string)
{
   return m_struct.toAddon.config_file_new_from_string(&m_struct, from_string);
}

void CShaderPresetAddon::ConfigFileFree(config_file_t_ *conf)
{
   return m_struct.toAddon.config_file_free(&m_struct, conf);
}

bool CShaderPresetAddon::ShaderPresetRead(config_file_t_ *conf, video_shader_ *shader)
{
   return m_struct.toAddon.video_shader_read_conf_cgp(&m_struct, conf, shader);
}

void CShaderPresetAddon::ShaderPresetWrite(config_file_t_ *conf, video_shader_ *shader)
{
   return m_struct.toAddon.video_shader_write_conf_cgp(&m_struct, conf, shader);
}

void CShaderPresetAddon::ShaderPresetResolveRelative(video_shader_ *shader, const char *ref_path)
{
   return m_struct.toAddon.video_shader_resolve_relative(&m_struct, shader, ref_path);
}

bool CShaderPresetAddon::ShaderPresetResolveCurrentParameters(config_file_t_ *conf, video_shader_ *shader)
{
   return m_struct.toAddon.video_shader_resolve_current_parameters(&m_struct, conf, shader);
}

bool CShaderPresetAddon::ShaderPresetResolveParameters(config_file_t_ *conf, video_shader_ *shader)
{
   return m_struct.toAddon.video_shader_resolve_parameters(&m_struct, conf, shader);
}
