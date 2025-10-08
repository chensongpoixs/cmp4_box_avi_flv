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

#include "libmedia_codec/video_codecs/spatial_layer.h"

namespace libmedia_codec {

bool SpatialLayer::operator==(const SpatialLayer& other) const {
  return (width == other.width && height == other.height &&
          maxFramerate == other.maxFramerate &&
          numberOfTemporalLayers == other.numberOfTemporalLayers &&
          maxBitrate == other.maxBitrate &&
          targetBitrate == other.targetBitrate &&
          minBitrate == other.minBitrate && qpMax == other.qpMax &&
          active == other.active);
}

}  // namespace webrtc
