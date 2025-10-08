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


#ifndef _C_MODULES_VCM_VIDEO_CODING_ENCODED_FRAME_H_
#define _C_MODULES_VCM_VIDEO_CODING_ENCODED_FRAME_H_

#include <vector>

#include "libmedia_codec/encoded_image.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_video_header.h"
#include "libmedia_codec/video_codecs/video_codec_interface.h"
#include "libmedia_codec/video_codecs/video_coding_defines.h"
#include "rtc_base/system/rtc_export.h"

namespace libmedia_codec {

class RTC_EXPORT VCMEncodedFrame : public EncodedImage {
 public:
  VCMEncodedFrame();
  VCMEncodedFrame(const VCMEncodedFrame&);

  ~VCMEncodedFrame();
  /**
   *   Set render time in milliseconds
   */
  void SetRenderTime(const int64_t renderTimeMs) {
    _renderTimeMs = renderTimeMs;
  }

  VideoPlayoutDelay PlayoutDelay() const { return playout_delay_; }

  void SetPlayoutDelay(VideoPlayoutDelay playout_delay) {
    playout_delay_ = playout_delay;
  }

  /**
   *   Get the encoded image
   */
  const libmedia_codec::EncodedImage& EncodedImage() const {
    return static_cast<const  libmedia_codec::EncodedImage&>(*this);
  }

  //using EncodedImage::ColorSpace;
  //using EncodedImage::data;
  //using EncodedImage::GetEncodedData;
  //using EncodedImage::NtpTimeMs;
  //using EncodedImage::PacketInfos;
  //using EncodedImage::set_size;
  //using EncodedImage::SetColorSpace;
  //using EncodedImage::SetEncodedData;
  //using EncodedImage::SetPacketInfos;
  //using EncodedImage::SetSpatialIndex;
  //using EncodedImage::SetSpatialLayerFrameSize;
  //using EncodedImage::SetTimestamp;
  //using EncodedImage::size;
  //using EncodedImage::SpatialIndex;
  //using EncodedImage::SpatialLayerFrameSize;
  //using EncodedImage::Timestamp;

  /**
   *   Get render time in milliseconds
   */
  int64_t RenderTimeMs() const { return _renderTimeMs; }
  /**
   *   Get frame type
   */
  libmedia_codec::VideoFrameType FrameType() const { return _frameType; }
  /**
   *   Set frame type
   */
  void SetFrameType(libmedia_codec::VideoFrameType frame_type) {
    _frameType = frame_type;
  }
  /**
   *   Get frame rotation
   */
  VideoRotation rotation() const { return rotation_; }
  /**
   *  Get video content type
   */
  VideoContentType contentType() const { return content_type_; }
  /**
   * Get video timing
   */
  EncodedImage::Timing video_timing() const { return timing_; }
  EncodedImage::Timing* video_timing_mutable() { return &timing_; }
  /**
   *   True if there's a frame missing before this frame
   */
  bool MissingFrame() const { return _missingFrame; }
  /**
   *   Payload type of the encoded payload
   */
  uint8_t PayloadType() const { return _payloadType; }
  /**
   *   Get codec specific info.
   *   The returned pointer is only valid as long as the VCMEncodedFrame
   *   is valid. Also, VCMEncodedFrame owns the pointer and will delete
   *   the object.
   */
  const CodecSpecificInfo* CodecSpecific() const { return &_codecSpecificInfo; }
  void SetCodecSpecific(const CodecSpecificInfo* codec_specific) {
    _codecSpecificInfo = *codec_specific;
  }

 protected:
  void Reset();

  void CopyCodecSpecific(const libmedia_transfer_protocol::RTPVideoHeader* header);

  int64_t _renderTimeMs;
  uint8_t _payloadType;
  bool _missingFrame;
  CodecSpecificInfo _codecSpecificInfo;
   VideoCodecType _codec;
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_ENCODED_FRAME_H_
