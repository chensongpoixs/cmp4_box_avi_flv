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


#include "libmedia_codec/vcm_packet.h"

#include "libmedia_transfer_protocol/rtp_headers.h"

namespace libmedia_codec {

VCMPacket::VCMPacket()
    : payloadType(0),
      timestamp(0),
      ntp_time_ms_(0),
      seqNum(0),
      dataPtr(NULL),
      sizeBytes(0),
      markerBit(false),
      timesNacked(-1),
      completeNALU(kNaluUnset),
      insertStartCode(false),
      video_header() {
  video_header.playout_delay = {-1, -1};
}

VCMPacket::VCMPacket(const uint8_t* ptr,
                     size_t size,
                     const libmedia_transfer_protocol::RTPHeader& rtp_header,
                     const libmedia_transfer_protocol::RTPVideoHeader& videoHeader,
                     int64_t ntp_time_ms,
                     webrtc::Timestamp receive_time)
    : payloadType(rtp_header.payloadType),
      timestamp(rtp_header.timestamp),
      ntp_time_ms_(ntp_time_ms),
      seqNum(rtp_header.sequenceNumber),
      dataPtr(ptr),
      sizeBytes(size),
      markerBit(rtp_header.markerBit),
      timesNacked(-1),
      completeNALU(kNaluIncomplete),
      insertStartCode(videoHeader.codec == libmedia_codec::kVideoCodecH264 &&
                      videoHeader.is_first_packet_in_frame),
      video_header(videoHeader),
      packet_info(rtp_header, receive_time) {
  if (is_first_packet_in_frame() && markerBit) {
    completeNALU = kNaluComplete;
  } else if (is_first_packet_in_frame()) {
    completeNALU = kNaluStart;
  } else if (markerBit) {
    completeNALU = kNaluEnd;
  } else {
    completeNALU = kNaluIncomplete;
  }

  // TODO(nisse): Delete?
  // Playout decisions are made entirely based on first packet in a frame.
  if (!is_first_packet_in_frame()) {
    video_header.playout_delay = {-1, -1};
  }
}

VCMPacket::~VCMPacket() = default;

}  // namespace webrtc
