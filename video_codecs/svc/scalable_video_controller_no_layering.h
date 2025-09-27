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
#ifndef _C_MODULES_VIDEO_CODING_SVC_SCALABLE_VIDEO_CONTROLLER_NO_LAYERING_H_
#define _C_MODULES_VIDEO_CODING_SVC_SCALABLE_VIDEO_CONTROLLER_NO_LAYERING_H_

#include <vector>

#include "libmedia_transfer_protocol/rtp/dependency_descriptor.h"
#include "libmedia_codec/video_bitrate_allocation.h"
#include "libmedia_codec/generic_frame_descriptor/generic_frame_info.h"
#include "libmedia_codec/video_codecs/svc/scalable_video_controller.h"

namespace libmedia_codec {

class ScalableVideoControllerNoLayering : public ScalableVideoController {
 public:
  ~ScalableVideoControllerNoLayering() override;

  StreamLayersConfig StreamConfig() const override;
  libmedia_transfer_protocol::FrameDependencyStructure DependencyStructure() const override;

  std::vector<LayerFrameConfig> NextFrameConfig(bool restart) override;
  GenericFrameInfo OnEncodeDone(const LayerFrameConfig& config) override;
  void OnRatesUpdated(const VideoBitrateAllocation& bitrates) override;

 private:
  bool start_ = true;
  bool enabled_ = true;
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_SVC_SCALABLE_VIDEO_CONTROLLER_NO_LAYERING_H_
