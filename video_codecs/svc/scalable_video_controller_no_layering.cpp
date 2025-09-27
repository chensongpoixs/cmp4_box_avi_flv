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
#include "libmedia_codec/video_codecs/svc/scalable_video_controller_no_layering.h"

#include <utility>
#include <vector>

#include "libmedia_transfer_protocol/rtp/dependency_descriptor.h"
#include "rtc_base/checks.h"

namespace libmedia_codec {

ScalableVideoControllerNoLayering::~ScalableVideoControllerNoLayering() =
    default;

ScalableVideoController::StreamLayersConfig
ScalableVideoControllerNoLayering::StreamConfig() const {
  StreamLayersConfig result;
  result.num_spatial_layers = 1;
  result.num_temporal_layers = 1;
  result.uses_reference_scaling = false;
  return result;
}

libmedia_transfer_protocol::FrameDependencyStructure
ScalableVideoControllerNoLayering::DependencyStructure() const {
	libmedia_transfer_protocol::FrameDependencyStructure structure;
  structure.num_decode_targets = 1;
  structure.num_chains = 1;
  structure.decode_target_protected_by_chain = {0};

  libmedia_transfer_protocol::FrameDependencyTemplate key_frame;
  key_frame.decode_target_indications = { libmedia_transfer_protocol::DecodeTargetIndication::kSwitch};
  key_frame.chain_diffs = {0};
  structure.templates.push_back(key_frame);

  libmedia_transfer_protocol::FrameDependencyTemplate delta_frame;
  delta_frame.decode_target_indications = { libmedia_transfer_protocol::DecodeTargetIndication::kSwitch};
  delta_frame.chain_diffs = {1};
  delta_frame.frame_diffs = {1};
  structure.templates.push_back(delta_frame);

  return structure;
}

std::vector<ScalableVideoController::LayerFrameConfig>
ScalableVideoControllerNoLayering::NextFrameConfig(bool restart) {
  if (!enabled_) {
    return {};
  }
  std::vector<LayerFrameConfig> result(1);
  if (restart || start_) {
    result[0].Id(0).Keyframe().Update(0);
  } else {
    result[0].Id(0).ReferenceAndUpdate(0);
  }
  start_ = false;
  return result;
}

GenericFrameInfo ScalableVideoControllerNoLayering::OnEncodeDone(
    const LayerFrameConfig& config) {
  RTC_DCHECK_EQ(config.Id(), 0);
  GenericFrameInfo frame_info;
  frame_info.encoder_buffers = config.Buffers();
  if (config.IsKeyframe()) {
    for (auto& buffer : frame_info.encoder_buffers) {
      buffer.referenced = false;
    }
  }
  frame_info.decode_target_indications = { libmedia_transfer_protocol::DecodeTargetIndication::kSwitch};
  frame_info.part_of_chain = {true};
  return frame_info;
}

void ScalableVideoControllerNoLayering::OnRatesUpdated(
    const VideoBitrateAllocation& bitrates) {
  enabled_ = bitrates.GetBitrate(0, 0) > 0;
}

}  // namespace webrtc
