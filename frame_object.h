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



#ifndef _C_MODULES_VIDEO_CODING_FRAME_OBJECT_H_
#define _C_MODULES_VIDEO_CODING_FRAME_OBJECT_H_

#include "absl/types/optional.h"
#include "libmedia_codec/encoded_frame.h"
#include "libmedia_codec/color_space.h"
namespace libmedia_codec {

class RtpFrameObject : public EncodedFrame {
 public:
  RtpFrameObject(uint16_t first_seq_num,
                 uint16_t last_seq_num,
                 bool markerBit,
                 int times_nacked,
                 int64_t first_packet_received_time,
                 int64_t last_packet_received_time,
                 uint32_t rtp_timestamp,
                 int64_t ntp_time_ms,
                 const VideoSendTiming& timing,
                 uint8_t payload_type,
                 VideoCodecType codec,
                 VideoRotation rotation,
                 VideoContentType content_type,
                 const libmedia_transfer_protocol::RTPVideoHeader& video_header,
                 const absl::optional< libmedia_codec::ColorSpace>& color_space,
	  libmedia_transfer_protocol::RtpPacketInfos packet_infos,
                 rtc::scoped_refptr<EncodedImageBuffer> image_buffer);

  ~RtpFrameObject() override;
  uint16_t first_seq_num() const;
  uint16_t last_seq_num() const;
  int times_nacked() const;
  VideoFrameType frame_type() const;
  VideoCodecType codec_type() const;
  int64_t ReceivedTime() const override;
  int64_t RenderTime() const override;
  bool delayed_by_retransmission() const override;
  const libmedia_transfer_protocol::RTPVideoHeader& GetRtpVideoHeader() const;

  uint8_t* mutable_data() { return image_buffer_->data(); }

 private:
  // Reference for mutable access.
  rtc::scoped_refptr<EncodedImageBuffer> image_buffer_;
  libmedia_transfer_protocol::RTPVideoHeader rtp_video_header_;
  VideoCodecType codec_type_;
  uint16_t first_seq_num_;
  uint16_t last_seq_num_;
  int64_t last_packet_received_time_;

  // Equal to times nacked of the packet with the highet times nacked
  // belonging to this frame.
  int times_nacked_;
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_FRAME_OBJECT_H_
