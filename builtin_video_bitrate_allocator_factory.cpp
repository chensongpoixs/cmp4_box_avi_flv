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


#include "libmedia_codec/builtin_video_bitrate_allocator_factory.h"

#include <memory>

#include "absl/base/macros.h"
#include "libmedia_codec/video_bitrate_allocator.h"
#include "libmedia_codec/video_codecs/video_codec.h"
#include "libmedia_codec/video_codecs/svc/svc_rate_allocator.h"
#include "libmedia_codec/video_codecs/utility/simulcast_rate_allocator.h"

namespace libmedia_codec {

namespace {

class BuiltinVideoBitrateAllocatorFactory
    : public VideoBitrateAllocatorFactory {
 public:
  BuiltinVideoBitrateAllocatorFactory() = default;
  ~BuiltinVideoBitrateAllocatorFactory() override = default;

  std::unique_ptr<VideoBitrateAllocator> CreateVideoBitrateAllocator(
      const VideoCodec& codec) override {
    switch (codec.codecType) {
      case kVideoCodecAV1:
      case kVideoCodecVP9:
        return std::make_unique<SvcRateAllocator>(codec);
      default:
        return std::make_unique<SimulcastRateAllocator>(codec);
    }
  }
};

}  // namespace

std::unique_ptr<VideoBitrateAllocatorFactory>
CreateBuiltinVideoBitrateAllocatorFactory() {
  return std::make_unique<BuiltinVideoBitrateAllocatorFactory>();
}

}  // namespace webrtc
