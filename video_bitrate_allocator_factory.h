/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Please visit https://chensongpoixs.github.io for detail
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 ******************************************************************************/
 /*****************************************************************************
				   Author: chensong
				   date:  2025-09-25



 ******************************************************************************/


#ifndef _C_API_VIDEO_VIDEO_BITRATE_ALLOCATOR_FACTORY_H_
#define _C_API_VIDEO_VIDEO_BITRATE_ALLOCATOR_FACTORY_H_

#include <memory>

#include "libmedia_codec/video_bitrate_allocator.h"
#include "libmedia_codec/video_codecs/video_codec.h"

namespace libmedia_codec {

// A factory that creates VideoBitrateAllocator.
// NOTE: This class is still under development and may change without notice.
class VideoBitrateAllocatorFactory {
 public:
  virtual ~VideoBitrateAllocatorFactory() = default;
  // Creates a VideoBitrateAllocator for a specific video codec.
  virtual std::unique_ptr<VideoBitrateAllocator> CreateVideoBitrateAllocator(
      const VideoCodec& codec) = 0;
};

}  // namespace webrtc

#endif  // API_VIDEO_VIDEO_BITRATE_ALLOCATOR_FACTORY_H_
