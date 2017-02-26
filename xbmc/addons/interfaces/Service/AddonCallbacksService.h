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

#include "AddonCallbacks.h"
#include "kodi_service_types.h"

namespace KodiAPI
{
namespace Service
{

class CServiceDLL;

/*!
 * \brief Callbacks for a service add-on to Kodi
 */
class CAddonCallbacksService
{
public:
   CAddonCallbacksService(CAddon* addon);
  ~CAddonCallbacksService(void);

  /*!
   * \return The callback table
   */
  CB_ServiceLib *GetCallbacks() { return m_callbacks; }

private:
  static CServiceDLL* GetAddon(void *addonData);

  CB_ServiceLib* m_callbacks; /*!< callback addresses */
  CAddon*        m_addon;     /*!< the addon */
};

}
}
