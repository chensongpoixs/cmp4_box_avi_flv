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


#ifndef _C_MODULES_VIDEO_CODING_PACKET_H_
#define _C_MODULES_VIDEO_CODING_PACKET_H_

#include <stddef.h>
#include <stdint.h>

#include "absl/types/optional.h"
#include "libmedia_transfer_protocol/rtp_headers.h"
#include "libmedia_transfer_protocol/rtp_packet_info.h"
#include "api/units/timestamp.h"
#include "libmedia_codec/video_frame_type.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_generic_frame_descriptor.h"
#include "libmedia_transfer_protocol/rtp_rtcp/rtp_video_header.h"

namespace libmedia_codec {

// Used to indicate if a received packet contain a complete NALU (or equivalent)
enum VCMNaluCompleteness {
  kNaluUnset = 0,     // Packet has not been filled.
  kNaluComplete = 1,  // Packet can be decoded as is.
  kNaluStart,         // Packet contain beginning of NALU
  kNaluIncomplete,    // Packet is not beginning or end of NALU
  kNaluEnd,           // Packet is the end of a NALU
};

class VCMPacket {
 public:
  VCMPacket();

  VCMPacket(const uint8_t* ptr,
            size_t size,
            const libmedia_transfer_protocol::RTPHeader& rtp_header,
            const libmedia_transfer_protocol::RTPVideoHeader& video_header,
            int64_t ntp_time_ms,
            webrtc::Timestamp receive_time);

  ~VCMPacket();

  libmedia_codec::VideoCodecType codec() const { return video_header.codec; }
  int width() const { return video_header.width; }
  int height() const { return video_header.height; }

  bool is_first_packet_in_frame() const {
    return video_header.is_first_packet_in_frame;
  }
  bool is_last_packet_in_frame() const {
    return video_header.is_last_packet_in_frame;
  }

  uint8_t payloadType;
  uint32_t timestamp;
  // NTP time of the capture time in local timebase in milliseconds.
  int64_t ntp_time_ms_;
  uint16_t seqNum;
  const uint8_t* dataPtr;
  size_t sizeBytes;
  bool markerBit;
  int timesNacked;

  VCMNaluCompleteness completeNALU;  // Default is kNaluIncomplete.
  bool insertStartCode;  // True if a start code should be inserted before this
                         // packet.
  libmedia_transfer_protocol::RTPVideoHeader video_header;
  absl::optional<libmedia_transfer_protocol::RtpGenericFrameDescriptor> generic_descriptor;

  libmedia_transfer_protocol::RtpPacketInfo packet_info;
};

}  // namespace webrtc
#endif  // MODULES_VIDEO_CODING_PACKET_H_
