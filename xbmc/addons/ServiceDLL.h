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
#pragma once

#include "AddonDll.h"
#include "DllService.h"
#include "Service.h"

namespace ADDON
{
  class CServiceDLL: public ADDON::CAddonDll<DllService, ServiceDLL, SRV_PROPS>
  {
  public:
	static std::unique_ptr<CServiceDLL> FromExtension(ADDON::AddonProps props, const cp_extension_t *ext);

	CServiceDLLCGameClient(ADDON::AddonProps props);

	virtual ~CServiceDLL();

    bool Start();
    void Stop();

    CService::START_OPTION GetStartOption() { return m_startOption; }
    CService::TYPE GetServiceType() { return m_type; }

  private:
    CService::TYPE         m_type;
    CService::START_OPTION m_startOption;
    std::string            m_strUserPath;
    std::string            m_strClientPath;
  };
}
