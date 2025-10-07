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

#include "libmedia_codec/video_codecs/h264_nal_decoder.h"
#include "rtc_base/logging.h"
#include "libmedia_transfer_protocol/rtp_rtcp/byte_io.h"
namespace  libmedia_codec
{
	namespace {
		static const uint8_t start_sequence[4] = {0x00, 0X00, 0x00, 0X01};

		static const int32_t NAL_MASK = 0x1f;
	}

	H264NalDecoder::H264NalDecoder() 
	:buffer_stream_(new uint8_t [1024* 1024 * 8])
	{}
	H264NalDecoder::~H264NalDecoder()
	{
	}

	int H264NalDecoder::ff_h264_handle_aggregated_packet(const uint8_t *buf, int len,
		int skip_between, int *nal_counters,
		int nal_mask)
	{
		//std::stringstream  nal_aggregated;
		int pass = 0;
		int total_length = 0;
		//uint8_t *dst = NULL;
		int ret;

		// first we are going to figure out the total size
		for (pass = 0; pass < 2; pass++) {
			const uint8_t *src = buf;
			int src_len = len;

			while (src_len > 2)
			{
				// // Add NAL unit length field.
				// 读取入NALU length field
				uint16_t nal_size = libmedia_transfer_protocol::ByteReader<uint16_t>::ReadBigEndian(src);
				//uint16_t nal_size = AV_RB16(src);

				// consume the length of the aggregate
				src += 2;
				src_len -= 2;

				if (nal_size <= src_len) {
					if (pass == 0) {
						// counting
						//total_length += sizeof(start_sequence) + nal_size;
					}
					else {
						// copying
						//nal_aggregated << start_sequence;
						//nal_aggregated << std::string((char *)src, nal_size);
						memcpy(buffer_stream_ + buffer_index_, start_sequence, sizeof(start_sequence));
						buffer_index_ += sizeof(start_sequence);
						memcpy(buffer_stream_ + buffer_index_, src, nal_size);
						//if (nal_counters)
						//	nal_counters[(*src) & nal_mask]++;
						buffer_index_ += nal_size;
					}
				}
				else {
					//av_log(ctx, AV_LOG_ERROR,
					//	"nal size exceeds length: %d %d\n", nal_size, src_len);
					RTC_LOG(LS_ERROR) << "nal size exceeds length:" << nal_size << " " << src_len;
					return -1;// AVERROR_INVALIDDATA;
				}

				// eat what we handled
				src += nal_size + skip_between;
				src_len -= nal_size + skip_between;
			}

			if (pass == 0) {
				/* now we know the total size of the packet (with the
				 * start sequences added) */
				//if ((ret = av_new_packet(pkt, total_length)) < 0)
				//	return ret;
				//dst = pkt->data;
			}
		}
		//bit_stream_ << nal_aggregated.str();
		return 0;
		return 0;
	}

	int H264NalDecoder::h264_handle_packet_fu_a(
		const uint8_t *buf, int len,
		int *nal_counters, int nal_mask)
	{
		uint8_t fu_indicator, fu_header, start_bit, nal_type, nal;

		if (len < 3) {
			//av_log(ctx, AV_LOG_ERROR, "Too short data for FU-A H.264 RTP packet\n");
			RTC_LOG(LS_WARNING) << "Too short data for FU-A H.264 RTP packet !!!";
			return AVERROR_INVALIDDATA;
		}

		fu_indicator = buf[0];
		fu_header = buf[1];
		start_bit = fu_header >> 7;
		nal_type = fu_header & 0x1f;
		nal = fu_indicator & 0xe0 | nal_type;

		// skip the fu_indicator and fu_header
		buf += 2;
		len -= 2;

		//if (start_bit && nal_counters)
		//	nal_counters[nal_type & nal_mask]++;
		return ff_h264_handle_frag_packet(  buf, len, start_bit, &nal, 1);
		//return 0;
	}
	int H264NalDecoder::ff_h264_handle_frag_packet(const uint8_t *buf, int len,
		int start_bit, const uint8_t *nal_header,
		int nal_header_len)
	{
		//std::stringstream cmd;
		int ret;
		int tot_len = len;
		int pos = 0;
		if (start_bit)
		{
			tot_len += sizeof(start_sequence) + nal_header_len;
		}
		//if ((ret = av_new_packet(pkt, tot_len)) < 0)
		//	return ret;
		if (start_bit) {
			
			memcpy(buffer_stream_ + buffer_index_, start_sequence, sizeof(start_sequence));
			buffer_index_ += sizeof(start_sequence);
			memcpy(buffer_stream_ + buffer_index_, nal_header, nal_header_len);
			buffer_index_ += nal_header_len;
			//cmd << start_sequence;
			//cmd << nal_header;

		}
		memcpy(buffer_stream_ + buffer_index_, buf, len);
		buffer_index_ += len;
		//cmd << std::string((char*)buf, len);
		//bit_stream_ << cmd.str();
		return 0;
	}
	int32_t H264NalDecoder::parse_packet(const uint8_t * buf, size_t len)
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