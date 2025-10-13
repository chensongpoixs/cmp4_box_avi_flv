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

#include "libmedia_codec/audio_codec/adts_header.h"
#include "rtc_base/logging.h"
 
namespace libmedia_codec
{


	namespace {
		//定义adts头的长度
		static const int32_t   kAdtsHeaderLength = 7;

 
	}

	int32_t AdtsHeader::parse(const uint8_t * data, int32_t len)
	{
		if (len < kAdtsHeaderLength)
		{
			RTC_LOG(LS_WARNING) << "parse header tail samll !!! size : " << len;
			return -1;
		}

		//GetBitContext gb;


		int size, rdb, ch, sr;
		int aot, crc_abs;
		// sync  0xfff 
		if (data[0] != 0xff && data[1] != 0xf0)
		{
			RTC_LOG(LS_WARNING) << " adts parse  failed  sync 0xfff";
			return  -1;
		}

		 
		//if (get_bits(gbc, 12) != 0xfff)
		//	return AAC_AC3_PARSE_ERROR_SYNC;

		//skip_bits1(gbc);             /* id */
		//skip_bits(gbc, 2);           /* layer */
		//crc_abs = get_bits1(gbc);    /* protection_absent */
		//aot = get_bits(gbc, 2);  /* profile_objecttype */
		//sr = get_bits(gbc, 4);  /* sample_frequency_index */
		//if (!ff_mpeg4audio_sample_rates[sr])
		//	return AAC_AC3_PARSE_ERROR_SAMPLE_RATE;
		//skip_bits1(gbc);             /* private_bit */
		//ch = get_bits(gbc, 3);       /* channel_configuration */

		//skip_bits1(gbc);             /* original/copy */
		//skip_bits1(gbc);             /* home */

		///* adts_variable_header */
		//skip_bits1(gbc);             /* copyright_identification_bit */
		//skip_bits1(gbc);             /* copyright_identification_start */
		//size = get_bits(gbc, 13);    /* aac_frame_length 一个ADTS帧的长度包括ADTS头和AAC原始流 */
		//if (size < AV_AAC_ADTS_HEADER_SIZE)
		//	return AAC_AC3_PARSE_ERROR_FRAME_SIZE;

		//skip_bits(gbc, 11);          /* adts_buffer_fullness ：0x7FF 说明是码率可变的码流 */
		//rdb = get_bits(gbc, 2);      /* number_of_raw_data_blocks_in_frame */

		data[5];

		//int32_t  *  p =(int32_t *) data;
		// 4* 8 ==> 32 
		// 7 * 8 ==> 56;

		// frame_length = |  |30 |    56 - 26 |13 13  |



		int32_t   frame_length = (data[3] >> 6);
		frame_length >>= 8;
		frame_length |= data[4];
		frame_length >>= 1;
		frame_length |= ((data[5] >>7) <<7);

		acc_adts_header_info_.frame_length = 0;
		return int32_t();
	}
}