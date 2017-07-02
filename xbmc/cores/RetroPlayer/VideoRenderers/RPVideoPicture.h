#pragma once

/*
 *      Copyright (C) 2005-2017 Team XBMC
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

#include "system.h"
#include "cores/VideoPlayer/Process/VideoBuffer.h"
#include "cores/VideoPlayer/DVDDemuxers/DVDDemuxPacket.h"
#include "DVDResource.h"

extern "C" {
#include "libavcodec/avcodec.h"
}

#include <vector>
#include <string>
#include <map>

class CSetting;

// when modifying these structures, make sure you update all codecs accordingly
#define FRAME_TYPE_UNDEF 0
#define FRAME_TYPE_I     1
#define FRAME_TYPE_P     2
#define FRAME_TYPE_B     3
#define FRAME_TYPE_D     4


// should be entirely filled by all codecs
struct RPVideoPicture
{
public:
  RPVideoPicture();
  ~RPVideoPicture();
  RPVideoPicture& CopyRef(const RPVideoPicture &pic);
  RPVideoPicture& SetParams(const RPVideoPicture &pic);

  CVideoBuffer *videoBuffer = nullptr;

  double pts; // timestamp in seconds, used in the CVideoPlayer class to keep track of pts
  double dts;
  unsigned int iFlags;
  double iRepeatPicture;
  double iDuration;
  unsigned int iFrameType         : 4;  //< see defines above // 1->I, 2->P, 3->B, 0->Undef
  unsigned int color_matrix       : 4;
  unsigned int color_range        : 1;  //< 1 indicate if we have a full range of color
  unsigned int chroma_position;
  unsigned int color_primaries;
  unsigned int color_transfer;
  char stereo_mode[32];

  int8_t* qp_table;                //< Quantization parameters, primarily used by filters
  int qstride;
  int qscale_type;

  unsigned int iWidth;
  unsigned int iHeight;
  unsigned int iDisplayWidth;           //< width of the picture without black bars
  unsigned int iDisplayHeight;          //< height of the picture without black bars

private:
  RPVideoPicture(RPVideoPicture const&);
  RPVideoPicture& operator=(RPVideoPicture const&);
};