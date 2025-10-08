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



#ifndef _C_COMMON_VIDEO_INCLUDE_QUALITY_LIMITATION_REASON_H_
#define _C_COMMON_VIDEO_INCLUDE_QUALITY_LIMITATION_REASON_H_
 

namespace libmedia_codec {

// https://w3c.github.io/webrtc-stats/#rtcqualitylimitationreason-enum
enum class QualityLimitationReason {
  kNone,
  kCpu,
  kBandwidth,
  kOther,
};

}  // namespace webrtc

#endif  // COMMON_VIDEO_INCLUDE_QUALITY_LIMITATION_REASON_H_
