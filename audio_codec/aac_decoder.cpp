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
				   date:  2025-10-16



 ******************************************************************************/
#include "libmedia_codec/audio_codec/aac_decoder.h"
#include "rtc_base/logging.h"
namespace libmedia_codec
{
	AacDecoder::AacDecoder()
	{
	}
	AacDecoder::~AacDecoder()
	{
	}

	bool AacDecoder::Init(const std::string & config)
	{
		handle_ = NeAACDecOpen();
		unsigned long samplerate = 0;
		unsigned char channels = 0;
		//设置
		auto ret = NeAACDecInit2(handle_, (unsigned char*)config.c_str(), config.size(), &samplerate, &channels);
		if (ret >= 0)
		{
			LIBMEIDA_CODEC_LOG(LS_INFO) << "AACDecoder::Init ok, samplerate:" << samplerate << " channels:" << channels;
		}
		else
		{
			LIBMEIDA_CODEC_LOG_T_F(LS_WARNING) << "AACDecoder::Init failed.ret=" << ret;
			return false;
		}
		return true;
	}
	rtc::Buffer AacDecoder::Decode(unsigned char *aac, size_t aac_size)
	{
		NeAACDecFrameInfo frame_info;

		char *data = (char*)NeAACDecDecode(handle_, &frame_info, aac, aac_size);
		if (data&&frame_info.samples > 0 && frame_info.error == 0)
		{
			int32_t bytes = frame_info.samples * frame_info.channels;
			return rtc::Buffer(data, bytes);
		}
		else if (frame_info.error > 0)
		{
			LIBMEIDA_CODEC_LOG_T_F(LS_WARNING) << "decode failed.error:" << frame_info.error;
		}
		return rtc::Buffer(NULL, 0);
	}
}
