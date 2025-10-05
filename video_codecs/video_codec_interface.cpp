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
				   date:  2025-10-06



 ******************************************************************************/

#include "libmedia_codec/video_codecs/video_codec_interface.h"

namespace libmedia_codec {
CodecSpecificInfo::CodecSpecificInfo() : codecType(kVideoCodecGeneric) {
  memset(&codecSpecific, 0, sizeof(codecSpecific));
}

CodecSpecificInfo::CodecSpecificInfo(const CodecSpecificInfo&) = default;
CodecSpecificInfo::~CodecSpecificInfo() = default;

}  // namespace webrtc
