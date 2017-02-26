#ifndef __SERVICE_DLL_H__
#define __SERVICE_DLL_H__
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

#include "kodi_service_types.h"
#include "xbmc_addon_dll.h"

#ifdef __cplusplus
extern "C" {
#endif

  const char* GetServiceAPIVersion();

  const char* GetMinimumServiceAPIVersion();

  bool StartServiceDll();

  bool StopServiceDll();

  void __declspec(dllexport) get_addon(ServiceDLL* pClient)
  {
    pClient->GetServiceAPIVersion        = GetServiceAPIVersion;
    pClient->GetMininumServiceAPIVersion = GetMinimumServiceAPIVersion;
    pClient->StartServiceDll             = StartServiceDll;
    pClient->StopServiceDll              = StopServiceDll;
  };

#ifdef __cplusplus
};
#endif

#endif // __SERVICE_DLL_H__
