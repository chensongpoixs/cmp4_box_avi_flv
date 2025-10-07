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

#include "libmedia_codec/video_codecs/h264_nal_parse.h"
#include "rtc_base/logging.h"
#include "libmedia_transfer_protocol/rtp_rtcp/byte_io.h"
namespace  libmedia_codec
{
	namespace {
		static const uint8_t start_sequence[4] = {0x00, 0X00, 0x00, 0X01};

		static const int32_t NAL_MASK = 0x1f;
	}

	H264NalParse::H264NalParse()
	:NalParseInterface()
	{}
	H264NalParse::~H264NalParse()
	{
	}

	
	int32_t H264NalParse::parse_packet(const uint8_t * buf, size_t len)
	{

		uint8_t nal;
		uint8_t type;
		int result = 0;

		if (!len) {
			//av_log(ctx, AV_LOG_ERROR, "Empty H.264 RTP packet\n");
			RTC_LOG(LS_WARNING) << "Empty H.264 RTP packet";
			return -1;
		}
		nal = buf[0];
		type = nal & 0x1f;

		/* Simplify the case (these are all the NAL types used internally by
	* the H.264 codec). */
		if (type >= 1 && type <= 23)
		{
			type = 1;
		}
		switch (type) {
		case 0:                    // undefined, but pass them through
		case 1:
			//if ((result = av_new_packet(pkt, len + sizeof(start_sequence))) < 0)
			//	return result;
			memcpy(buffer_stream_ + buffer_index_, start_sequence, sizeof(start_sequence));
			buffer_index_ += 4;
			memcpy(buffer_stream_ + buffer_index_  , buf, len);
			buffer_index_ += len;
			//COUNT_NAL_TYPE(data, nal);
			//bit_stream_ << start_sequence;
			//bit_stream_ << buf;
			break;

		case 24:                   // STAP-A (one packet, multiple nals)
			// consume the STAP-A NAL
			buf++;
			len--;
			result = ff_h264_handle_aggregated_packet( buf, len, 0,
				0/*NAL_COUNTERS*/, NAL_MASK);
			break;

		case 25:                   // STAP-B
		case 26:                   // MTAP-16
		case 27:                   // MTAP-24
		case 29:                   // FU-B
			//avpriv_report_missing_feature(ctx, "RTP H.264 NAL unit type %d", type);
			RTC_LOG(LS_WARNING) << "RTP H.264 NAL unit type : " << type;
			result = AVERROR_PATCHWELCOME;
			break;

		case 28:                   // FU-A (fragmented nal)
			result = h264_handle_packet_fu_a(  buf, len,
				0, NAL_MASK);
			break;

		case 30:                   // undefined
		case 31:                   // undefined
		default:
			//av_log(ctx, AV_LOG_ERROR, "Undefined type (%d)\n", type);
			RTC_LOG(LS_WARNING) << "Undefined type: " << type;
			result = AVERROR_INVALIDDATA;
			break;
		}

		//pkt->stream_index = st->index;

		return result;

		return int32_t();
	}
}