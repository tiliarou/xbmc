/*
 *      Copyright (C) 2010-2017 Team XBMC
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

#include "cores/VideoPlayer/DVDCodecs/Video/DVDVideoCodec.h"
#include "ServiceBroker.h"
#include "cores/VideoPlayer/DVDCodecs/DVDFactoryCodec.h"
#include "settings/Settings.h"
#include "settings/lib/Setting.h"
#include "windowing/WindowingFactory.h"
#include <string>
#include <vector>

//******************************************************************************
// RPVideoPicture
//******************************************************************************

RPVideoPicture::RPVideoPicture() = default;

RPVideoPicture::~RPVideoPicture()
{
  if (videoBuffer)
  {
    videoBuffer->Release();
  }
}

RPVideoPicture& RPVideoPicture::CopyRef(const RPVideoPicture &pic)
{
  if (videoBuffer)
    videoBuffer->Release();
  *this = pic;
  if (videoBuffer)
    videoBuffer->Acquire();
  return *this;
}

RPVideoPicture& RPVideoPicture::SetParams(const RPVideoPicture &pic)
{
  if (videoBuffer)
    videoBuffer->Release();
  *this = pic;
  videoBuffer = nullptr;
  return *this;
}

RPVideoPicture::RPVideoPicture(RPVideoPicture const&) = default;
RPVideoPicture& RPVideoPicture::operator=(RPVideoPicture const&) = default;
