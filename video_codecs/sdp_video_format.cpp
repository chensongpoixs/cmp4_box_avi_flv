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

#include "libmedia_codec/video_codecs/sdp_video_format.h"

#include "absl/strings/match.h"
#include "libmedia_codec/video_codecs/h264_profile_level_id.h"
#include "libmedia_codec/video_codecs/video_codec.h"
#include "libmedia_codec/video_codecs/vp9_profile.h"
#include "rtc_base/checks.h"
#include "rtc_base/strings/string_builder.h"

namespace libmedia_codec {

namespace {

std::string H264GetPacketizationModeOrDefault(
    const SdpVideoFormat::Parameters& params) {
  constexpr char kH264FmtpPacketizationMode[] = "packetization-mode";
  const auto it = params.find(kH264FmtpPacketizationMode);
  if (it != params.end()) {
    return it->second;
  }
  // If packetization-mode is not present, default to "0".
  // https://tools.ietf.org/html/rfc6184#section-6.2
  return "0";
}

bool H264IsSamePacketizationMode(const SdpVideoFormat::Parameters& left,
                                 const SdpVideoFormat::Parameters& right) {
  return H264GetPacketizationModeOrDefault(left) ==
         H264GetPacketizationModeOrDefault(right);
}

// Some (video) codecs are actually families of codecs and rely on parameters
// to distinguish different incompatible family members.
bool IsSameCodecSpecific(const SdpVideoFormat& format1,
                         const SdpVideoFormat& format2) {
  // The assumption when calling this function is that the two formats have the
  // same name.
  RTC_DCHECK(absl::EqualsIgnoreCase(format1.name, format2.name));

  VideoCodecType codec_type = PayloadStringToCodecType(format1.name);
  switch (codec_type) {
    case kVideoCodecH264:
      return H264IsSameProfile(format1.parameters, format2.parameters) &&
             H264IsSamePacketizationMode(format1.parameters,
                                         format2.parameters);
    case kVideoCodecVP9:
      return VP9IsSameProfile(format1.parameters, format2.parameters);
    default:
      return true;
  }
}
}  // namespace

SdpVideoFormat::SdpVideoFormat(const std::string& name) : name(name) {}

SdpVideoFormat::SdpVideoFormat(const std::string& name,
                               const Parameters& parameters)
    : name(name), parameters(parameters) {}

SdpVideoFormat::SdpVideoFormat(const SdpVideoFormat&) = default;
SdpVideoFormat::SdpVideoFormat(SdpVideoFormat&&) = default;
SdpVideoFormat& SdpVideoFormat::operator=(const SdpVideoFormat&) = default;
SdpVideoFormat& SdpVideoFormat::operator=(SdpVideoFormat&&) = default;

SdpVideoFormat::~SdpVideoFormat() = default;

std::string SdpVideoFormat::ToString() const {
  rtc::StringBuilder builder;
  builder << "Codec name: " << name << ", parameters: {";
  for (const auto& kv : parameters)
    builder << " " << kv.first << "=" << kv.second;
  builder << " }";

  return builder.str();
}

bool SdpVideoFormat::IsSameCodec(const SdpVideoFormat& other) const {
  // Two codecs are considered the same if the name matches (case insensitive)
  // and certain codec-specific parameters match.
  return absl::EqualsIgnoreCase(name, other.name) &&
         IsSameCodecSpecific(*this, other);
}

bool SdpVideoFormat::IsCodecInList(
    rtc::ArrayView<const   SdpVideoFormat> formats) const {
  for (const auto& format : formats) {
    if (IsSameCodec(format)) {
      return true;
    }
  }
  return false;
}

bool operator==(const SdpVideoFormat& a, const SdpVideoFormat& b) {
  return a.name == b.name && a.parameters == b.parameters;
}

}  // namespace webrtc
