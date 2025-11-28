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
				   date:  2025-10-07



 ******************************************************************************/



#ifndef _C_NAL_PARSE_FACTORY___H_
#define _C_NAL_PARSE_FACTORY___H_

#include <cstring>
#include <ostream>
#include <iostream>
#include  <memory>
namespace libmedia_codec
{


	enum ENalParseType
	{
		ENalH264Prase = 0,
		ENalHEVCPrase,
	};

	class NalParseInterface
	{
	public:
		explicit NalParseInterface() :buffer_stream_(new uint8_t[1024*1024*8]), buffer_index_(0){}

		virtual ~NalParseInterface(){}
	public:

		virtual int32_t  parse_packet(const uint8_t * data, size_t size) = 0;
	public:
		// fa ����
		int ff_h264_handle_aggregated_packet(const uint8_t *buf, int len,
			int skip_between, int *nal_counters,
			int nal_mask);
		// ���
		int h264_handle_packet_fu_a(
			const uint8_t *buf, int len,
			int *nal_counters, int nal_mask);
		int ff_h264_handle_frag_packet(const uint8_t *buf, int len,
			int start_bit, const uint8_t *nal_header,
			int nal_header_len);
	public:
		uint8_t  *    buffer_stream_;
		int32_t       buffer_index_ =0;
	};


	class NalParseFactory
	{
	public:
		static std::unique_ptr<NalParseInterface>   Create(ENalParseType type);
	};
}

#endif // _C_NAL_PARSE_FACTORY___H_