#ifndef __SERVICE_TYPES_H__
#define __SERVICE_TYPES_H__
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

#include "xbmc_addon_dll.h"

/* current service API version */
#define XBMC_SERVICE_API_VERSION "1.0.0"

/* min. service API version */
#define XBMC_SERVICE_MIN_API_VERSION "1.0.0"

#ifdef __cplusplus
extern "C" {
#endif
  typedef struct SRV_PROPS
  {
    const char* strUserPath;           /*!< @brief path to the user profile */
    const char* strClientPath;         /*!< @brief path to this add-on */
  } SRV_PROPS;

  typedef struct ServiceDLL
  {
    const char* (__cdecl* GetServiceAPIVersion)(void);
    const char* (__cdecl* GetMinimumServiceAPIVersion)(void);
    bool        (__cdecl* StartServiceDll)();
    bool        (__cdecl* StopServiceDll)();
  } ServiceDLL;
}

#endif // __SERVICE_TYPES_H__
