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



#ifndef _C_HEVC_NAL_PARSE___H_
#define _C_HEVC_NAL_PARSE___H_

#include <stddef.h>
#include <stdint.h>

#include <utility>

#include "libmedia_codec/channel_layout.h"
#include "libmedia_transfer_protocol/rtp_packet_infos.h"
#include "rtc_base/constructor_magic.h"
#include "libmedia_codec/video_codecs/video_frame_buffer_pool.h"
#include "libmedia_codec/encoded_image.h"
#include "libmedia_codec/video_codec_type.h"
extern "C" {
	//#include "lib"
	//#include "libavcodec/avcodec.h"
	//#include "libavutil/imgutils.h"

#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
}  // extern "C"


#include "libcross_platform_collection_render/video_render/cvideo_render_factory.h"
#include "libcross_platform_collection_render/video_render/cvideo_render.h"
#include "libcross_platform_collection_render/track_capture/ctrack_capture.h"
#include "libmedia_codec/video_codecs/nal_parse_factory.h"
namespace libmedia_codec {


	class HevcNalParse : public NalParseInterface
	{
	public:
		explicit HevcNalParse();
		virtual ~HevcNalParse();


	public:
		virtual int32_t  parse_packet(const uint8_t * data, size_t size) override;


	public:
		 


	public:
		
	public:


	 
	};


}


#endif // _C_H264_NAL_DECODER___H_