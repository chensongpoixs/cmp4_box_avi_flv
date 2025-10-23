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



#ifndef _C_AAC_DECODER_H_
#define _C_AAC_DECODER_H_

#include "faad.h"
#include <string>
#include "rtc_base/buffer.h"
namespace libmedia_codec
{
	class AacDecoder
	{
	public:
		AacDecoder();
		~AacDecoder();
		/**
		 *  
		 * @param config   ADIF 格式 ( Audio Data Interchange Format ⾳频数据交换格式 ) 是一种用于 存储 AAC 数据的文件格式 , 适合静态音频文件 ;

						ADIF 格式 的 ACC 音频文件 结构简单 , 只 包含一个 文件头部 和 紧随其后的 连续音频数据 ;
		  
		 */
		bool Init(const std::string & config);

		bool Init(unsigned char * adts, int32_t size);
		/**
		  @param sample hz 
		  @param channel 通道数
		*/
		bool Init(uint16_t  sample, uint16_t channel);
		rtc::Buffer Decode(unsigned char *aac, size_t aac_size);

	public:

		NeAACDecHandle handle_;
	};
}

#endif // _C_AAC_DECODER_H_
