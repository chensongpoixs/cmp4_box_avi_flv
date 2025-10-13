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
				   date:  2025-10-11

1. ADTS内容及结构

    ADTS 头中相对有用的信息 采样率、声道数、帧长度。想想也是，我要是解码器的话，你给我一堆得AAC音频ES流我也解不出来。
每一个带ADTS头信息的AAC流会清晰的告送解码器他需要的这些信息。

一般情况下ADTS的头信息都是7个字节，分为2部分：

adts_fixed_header();

adts_variable_header();


syncword ：同步头 总是0xFFF, all bits must be 1，代表着一个ADTS帧的开始

ID：MPEG Version: 0 for MPEG-4, 1 for MPEG-2

Layer：always: '00'

profile：表示使用哪个级别的AAC，有些芯片只支持AAC LC 。在MPEG-2 AAC中定义了3种：

sampling_frequency_index：表示使用的采样率下标，通过这个下标在 Sampling Frequencies[ ]数组中查找得知采样率的值。
There are 13 supported frequencies:

0: 96000 Hz
1: 88200 Hz
2: 64000 Hz
3: 48000 Hz
4: 44100 Hz
5: 32000 Hz
6: 24000 Hz
7: 22050 Hz
8: 16000 Hz
9: 12000 Hz
10: 11025 Hz
11: 8000 Hz
12: 7350 Hz
13: Reserved
14: Reserved
15: frequency is written explictly
channel_configuration: 表示声道数
0: Defined in AOT Specifc Config
1: 1 channel: front-center
2: 2 channels: front-left, front-right
3: 3 channels: front-center, front-left, front-right
4: 4 channels: front-center, front-left, front-right, back-center
5: 5 channels: front-center, front-left, front-right, back-left, back-right
6: 6 channels: front-center, front-left, front-right, back-left, back-right, LFE-channel
7: 8 channels: front-center, front-left, front-right, side-left, side-right, back-left, back-right, LFE-channel
8-15: Reserved


adts_variable_header();
'
frame_length : 一个ADTS帧的长度包括ADTS头和AAC原始流.

adts_buffer_fullness：0x7FF 说明是码率可变的码流
 ******************************************************************************/



#ifndef _C_ADTS_HEADER_H_
#define _C_ADTS_HEADER_H_

#include <stddef.h>
#include <stdint.h>

#include <utility>

#include "libmedia_codec/channel_layout.h"
#include "libmedia_transfer_protocol/rtp_packet_infos.h"
#include "rtc_base/constructor_magic.h"

namespace libmedia_codec {

	typedef struct AACADTSHeaderInfo {
		uint32_t sample_rate;
		uint32_t samples;
		uint32_t bit_rate;
		uint8_t  crc_absent;
		uint8_t  object_type;
		uint8_t  sampling_index;
		uint8_t  chan_config;
		uint8_t  num_aac_frames;
		uint32_t frame_length;
	} AACADTSHeaderInfo;



	class AdtsHeader
	{
	public:
		AdtsHeader() = default;
		~AdtsHeader() = default;

		int32_t parse(const uint8_t * data, int32_t size);


		AACADTSHeaderInfo  acc_adts_header_info_;
	};

}


#endif // 