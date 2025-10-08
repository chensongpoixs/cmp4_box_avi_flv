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

#include "libmedia_codec/video_bitrate_allocator.h"

namespace libmedia_codec {

VideoBitrateAllocationParameters::VideoBitrateAllocationParameters(
    uint32_t total_bitrate_bps,
    uint32_t framerate)
    : total_bitrate(webrtc::DataRate::BitsPerSec(total_bitrate_bps)),
      stable_bitrate(webrtc::DataRate::BitsPerSec(total_bitrate_bps)),
      framerate(static_cast<double>(framerate)) {}

VideoBitrateAllocationParameters::VideoBitrateAllocationParameters(
	webrtc::DataRate total_bitrate,
    double framerate)
    : total_bitrate(total_bitrate),
      stable_bitrate(total_bitrate),
      framerate(framerate) {}

VideoBitrateAllocationParameters::VideoBitrateAllocationParameters(
    webrtc::DataRate total_bitrate,
    webrtc::DataRate stable_bitrate,
    double framerate)
    : total_bitrate(total_bitrate),
      stable_bitrate(stable_bitrate),
      framerate(framerate) {}

VideoBitrateAllocationParameters::~VideoBitrateAllocationParameters() = default;

VideoBitrateAllocation VideoBitrateAllocator::GetAllocation(
    uint32_t total_bitrate_bps,
    uint32_t framerate) {
  return Allocate({webrtc::DataRate::BitsPerSec(total_bitrate_bps),
                   webrtc::DataRate::BitsPerSec(total_bitrate_bps),
                   static_cast<double>(framerate)});
}

VideoBitrateAllocation VideoBitrateAllocator::Allocate(
    VideoBitrateAllocationParameters parameters) {
  return GetAllocation(parameters.total_bitrate.bps(), parameters.framerate);
}

void VideoBitrateAllocator::SetLegacyConferenceMode(bool enabled) {}

}  // namespace webrtc
