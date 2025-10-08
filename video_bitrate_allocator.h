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


#ifndef _C_API_VIDEO_VIDEO_BITRATE_ALLOCATOR_H_
#define _C_API_VIDEO_VIDEO_BITRATE_ALLOCATOR_H_

#include "api/units/data_rate.h"
#include "libmedia_codec/video_bitrate_allocation.h"

namespace libmedia_codec {

struct VideoBitrateAllocationParameters {
  VideoBitrateAllocationParameters(uint32_t total_bitrate_bps,
                                   uint32_t framerate);
  VideoBitrateAllocationParameters(webrtc::DataRate total_bitrate, double framerate);
  VideoBitrateAllocationParameters(webrtc::DataRate total_bitrate,
                                   webrtc::DataRate stable_bitrate,
                                   double framerate);
  ~VideoBitrateAllocationParameters();

  webrtc::DataRate total_bitrate;
  webrtc::DataRate stable_bitrate;
  double framerate;
};

class VideoBitrateAllocator {
 public:
  VideoBitrateAllocator() {}
  virtual ~VideoBitrateAllocator() {}

  virtual VideoBitrateAllocation GetAllocation(uint32_t total_bitrate_bps,
                                               uint32_t framerate);

  virtual VideoBitrateAllocation Allocate(
      VideoBitrateAllocationParameters parameters);

  // Deprecated: Only used to work around issues with the legacy conference
  // screenshare mode and shouldn't be needed by any subclasses.
  virtual void SetLegacyConferenceMode(bool enabled);
};

class VideoBitrateAllocationObserver {
 public:
  VideoBitrateAllocationObserver() {}
  virtual ~VideoBitrateAllocationObserver() {}

  virtual void OnBitrateAllocationUpdated(
      const VideoBitrateAllocation& allocation) = 0;
};

}  // namespace webrtc

#endif  // API_VIDEO_VIDEO_BITRATE_ALLOCATOR_H_
