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
				   date:  2025-09-26



 ******************************************************************************/


#include "libmedia_codec/video_codecs/video_decoder.h"

#include "absl/types/optional.h"
#include "libmedia_codec/video_codecs/render_resolution.h"
#include "libmedia_codec/video_codec_type.h"
#include "rtc_base/checks.h"
#include "rtc_base/strings/string_builder.h"

namespace libmedia_codec {

int32_t DecodedImageCallback::Decoded(VideoFrame& decodedImage,
                                      int64_t decode_time_ms) {
  // The default implementation ignores custom decode time value.
  return Decoded(decodedImage);
}

void DecodedImageCallback::Decoded(VideoFrame& decodedImage,
                                   absl::optional<int32_t> decode_time_ms,
                                   absl::optional<uint8_t> qp) {
  Decoded(decodedImage, decode_time_ms.value_or(-1));
}

VideoDecoder::DecoderInfo VideoDecoder::GetDecoderInfo() const {
  DecoderInfo info;
  info.implementation_name = ImplementationName();
  return info;
}

const char* VideoDecoder::ImplementationName() const {
  return "unknown";
}

std::string VideoDecoder::DecoderInfo::ToString() const {
  char string_buf[2048];
  rtc::SimpleStringBuilder oss(string_buf);

  oss << "DecoderInfo { "
      << "prefers_late_decoding = "
      << "implementation_name = '" << implementation_name << "', "
      << "is_hardware_accelerated = "
      << (is_hardware_accelerated ? "true" : "false") << " }";
  return oss.str();
}

bool VideoDecoder::DecoderInfo::operator==(const DecoderInfo& rhs) const {
  return is_hardware_accelerated == rhs.is_hardware_accelerated &&
         implementation_name == rhs.implementation_name;
}

void VideoDecoder::Settings::set_number_of_cores(int value) {
  RTC_DCHECK_GT(value, 0);
  number_of_cores_ = value;
}

}  // namespace webrtc
