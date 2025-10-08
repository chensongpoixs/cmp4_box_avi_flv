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

#ifndef _C_MODULES_VIDEO_CODING_UTILITY_SIMULCAST_RATE_ALLOCATOR_H_
#define _C_MODULES_VIDEO_CODING_UTILITY_SIMULCAST_RATE_ALLOCATOR_H_

#include <stddef.h>
#include <stdint.h>

#include <vector>

#include "libmedia_codec/video_bitrate_allocation.h"
#include "libmedia_codec/video_bitrate_allocator.h"
#include "libmedia_codec/video_codecs/video_codec.h"
#include "rtc_base/constructor_magic.h"
#include "rtc_base/experiments/rate_control_settings.h"
#include "rtc_base/experiments/stable_target_rate_experiment.h"

namespace libmedia_codec {

class SimulcastRateAllocator : public VideoBitrateAllocator {
 public:
  explicit SimulcastRateAllocator(const VideoCodec& codec);
  ~SimulcastRateAllocator() override;

  VideoBitrateAllocation Allocate(
      VideoBitrateAllocationParameters parameters) override;
  const VideoCodec& GetCodec() const;

  static float GetTemporalRateAllocation(int num_layers,
                                         int temporal_id,
                                         bool base_heavy_tl3_alloc);

  void SetLegacyConferenceMode(bool mode) override;

 private:
  void DistributeAllocationToSimulcastLayers(
      webrtc::DataRate total_bitrate,
      webrtc::DataRate stable_bitrate,
      VideoBitrateAllocation* allocated_bitrates);
  void DistributeAllocationToTemporalLayers(
      VideoBitrateAllocation* allocated_bitrates) const;
  std::vector<uint32_t> DefaultTemporalLayerAllocation(int bitrate_kbps,
                                                       int max_bitrate_kbps,
                                                       int simulcast_id) const;
  std::vector<uint32_t> ScreenshareTemporalLayerAllocation(
      int bitrate_kbps,
      int max_bitrate_kbps,
      int simulcast_id) const;
  int NumTemporalStreams(size_t simulcast_id) const;

  const VideoCodec codec_;
  const webrtc::StableTargetRateExperiment stable_rate_settings_;
  const webrtc::RateControlSettings rate_control_settings_;
  std::vector<bool> stream_enabled_;
  bool legacy_conference_mode_;

  RTC_DISALLOW_COPY_AND_ASSIGN(SimulcastRateAllocator);
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_UTILITY_SIMULCAST_RATE_ALLOCATOR_H_
