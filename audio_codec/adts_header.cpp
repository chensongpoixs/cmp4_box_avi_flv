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

		const int ff_mpeg4audio_sample_rates[16] = {
		   96000, 88200, 64000, 48000, 44100, 32000,
		   24000, 22050, 16000, 12000, 11025, 8000, 7350
		};
	}
	int getAdtsFrame(FILE*fp, uint8_t *pAdtsFrameData, T_AdtsHeader *ptAdtsHeaderInfo)
	{
		uint32_t readBytes = 0;

		if (!fp || !pAdtsFrameData || !ptAdtsHeaderInfo)
			return -1;

		// ADTS header size is AAC_ADTS_HEADER_SIZE(=7) bytes
		readBytes = fread(pAdtsFrameData, 1, kAdtsHeaderLength, fp);
		if (readBytes <= 0)
			return -2;

		ptAdtsHeaderInfo->syncword = (pAdtsFrameData[0] << 4) | (pAdtsFrameData[1] >> 4);
		ptAdtsHeaderInfo->id = (pAdtsFrameData[1] & 0x08) >> 3;
		ptAdtsHeaderInfo->layer = (pAdtsFrameData[1] & 0x06) >> 1;
		ptAdtsHeaderInfo->protection_absent = pAdtsFrameData[1] & 0x01;
		ptAdtsHeaderInfo->profile = (pAdtsFrameData[2] & 0xc0) >> 6;
		ptAdtsHeaderInfo->sampling_freq_index = (pAdtsFrameData[2] & 0x3c) >> 2;
		ptAdtsHeaderInfo->private_bit = (pAdtsFrameData[2] & 0x02) >> 1;
		ptAdtsHeaderInfo->channel_configuration = (((pAdtsFrameData[2] & 0x01) << 2) | ((pAdtsFrameData[3] & 0xc0) >> 6));
		ptAdtsHeaderInfo->original_copy = (pAdtsFrameData[3] & 0x20) >> 5;
		ptAdtsHeaderInfo->home = (pAdtsFrameData[3] & 0x10) >> 4;
		ptAdtsHeaderInfo->copyright_identification_bit = (pAdtsFrameData[3] & 0x08) >> 3;
		ptAdtsHeaderInfo->copyright_identification_start = (pAdtsFrameData[3] & 0x04) >> 2;
		ptAdtsHeaderInfo->aac_frame_length = ((pAdtsFrameData[3] & 0x03) << 11) |
			((pAdtsFrameData[4] & 0xFF) << 3) |
			((pAdtsFrameData[5] & 0xE0) >> 5);
		ptAdtsHeaderInfo->adts_buffer_fullness = ((pAdtsFrameData[5] & 0x1f) << 6 | (pAdtsFrameData[6] & 0xfc) >> 2);
		ptAdtsHeaderInfo->number_of_raw_data_blocks_in_frame = (pAdtsFrameData[6] & 0x03);

		if (ptAdtsHeaderInfo->syncword != 0xFFF)
			return -3;

		/* read the remaining frame of ADTS data outside the AAC_ADTS_HEADER_SIZE(=7) bytes header,
		 * and it should be written after offsetting the header by AAC_ADTS_HEADER_SIZE(=7) bytes
		 */
		readBytes = fread(pAdtsFrameData + kAdtsHeaderLength, 1, ptAdtsHeaderInfo->aac_frame_length - kAdtsHeaderLength, fp);
		if (readBytes <= 0)
			return -4;

		return 0;
	}
	int getAdtsFrame(const  uint8_t *pAdtsFrameData, int32_t  size, T_AdtsHeader *ptAdtsHeaderInfo)
	{
		uint32_t readBytes = 0;

		if (  !pAdtsFrameData || !ptAdtsHeaderInfo)
			return -1;

		// ADTS header size is AAC_ADTS_HEADER_SIZE(=7) bytes
		if (size <= kAdtsHeaderLength)
		{
			return -1;
		 }

		ptAdtsHeaderInfo->syncword = (pAdtsFrameData[0] << 4) | (pAdtsFrameData[1] >> 4);
		ptAdtsHeaderInfo->id = (pAdtsFrameData[1] & 0x08) >> 3;
		ptAdtsHeaderInfo->layer = (pAdtsFrameData[1] & 0x06) >> 1;
		ptAdtsHeaderInfo->protection_absent = pAdtsFrameData[1] & 0x01;
		ptAdtsHeaderInfo->profile = (pAdtsFrameData[2] & 0xc0) >> 6;
		ptAdtsHeaderInfo->sampling_freq_index = (pAdtsFrameData[2] & 0x3c) >> 2;
		ptAdtsHeaderInfo->private_bit = (pAdtsFrameData[2] & 0x02) >> 1;
		ptAdtsHeaderInfo->channel_configuration = (((pAdtsFrameData[2] & 0x01) << 2) | ((pAdtsFrameData[3] & 0xc0) >> 6));
		ptAdtsHeaderInfo->original_copy = (pAdtsFrameData[3] & 0x20) >> 5;
		ptAdtsHeaderInfo->home = (pAdtsFrameData[3] & 0x10) >> 4;
		ptAdtsHeaderInfo->copyright_identification_bit = (pAdtsFrameData[3] & 0x08) >> 3;
		ptAdtsHeaderInfo->copyright_identification_start = (pAdtsFrameData[3] & 0x04) >> 2;
		ptAdtsHeaderInfo->aac_frame_length = ((pAdtsFrameData[3] & 0x03) << 11) |
			((pAdtsFrameData[4] & 0xFF) << 3) |
			((pAdtsFrameData[5] & 0xE0) >> 5);
		ptAdtsHeaderInfo->adts_buffer_fullness = ((pAdtsFrameData[5] & 0x1f) << 6 | (pAdtsFrameData[6] & 0xfc) >> 2);
		ptAdtsHeaderInfo->number_of_raw_data_blocks_in_frame = (pAdtsFrameData[6] & 0x03);

		if (ptAdtsHeaderInfo->syncword != 0xFFF)
			return -3;

		/* read the remaining frame of ADTS data outside the AAC_ADTS_HEADER_SIZE(=7) bytes header,
		 * and it should be written after offsetting the header by AAC_ADTS_HEADER_SIZE(=7) bytes
		 */
		//readBytes = fread(pAdtsFrameData + kAdtsHeaderLength, 1, ptAdtsHeaderInfo->aac_frame_length - kAdtsHeaderLength, fp);
		//if (readBytes <= 0)
		//	return -4;

		return 0;
	}
	int32_t AdtsHeader::parse(const uint8_t * data, int32_t len)
	{
		if (len < kAdtsHeaderLength)
		{
			RTC_LOG(LS_WARNING) << "parse header tail samll !!! size : " << len;
			return -1;
		}
		{
			// 1. syncword ：同步头 总是0xFFF, all bits must be 1，代表着一个ADTS帧的开始
			//if (data[0] == '\ff' && (data[1] & 0XF0) == '\f0')
			uint32_t  syncword = data[0];
			syncword <<= 4;
			uint8_t  p = data[1];
			p >>= 4;
			syncword |= p;
			if (syncword != 0XFFF)
			{
				LIBMEIDA_CODEC_LOG_T_F(LS_WARNING) << "adts header sync word:" << syncword << ", failed !!!";
				return -1;
			}
		}
		{
			// 2. id ID：MPEG Version: 0 for MPEG-4, 1 for MPEG-2
			//[1111 0000]
			uint8_t   id = data[1] & 0X0000000F;
			LIBMEIDA_CODEC_LOG(LS_INFO) << "adts header id:" << id;
		}
		{
			// 3. layer   2bit
			// 4. protection_absent   1bit 
			acc_adts_header_info_.crc_absent = data[1] & 0X000001;
			LIBMEIDA_CODEC_LOG(LS_INFO) << "adts header crc_absent:" << acc_adts_header_info_.crc_absent;
		}
		{
			// 5. profile_objecttype  2bit 
			uint8_t  aot = data[2];
			aot <<= 6;
			LIBMEIDA_CODEC_LOG(LS_INFO) << "adts header profile_objecttype:" << aot;
		}
		{
			// 6. sample_frequency_index   4 bit
			uint8_t   sr = data[2];
			sr <<= 2;
			sr >>= 4;
			LIBMEIDA_CODEC_LOG(LS_INFO) << "adts header sample_frequency_index:" << sr;
			acc_adts_header_info_.sampling_index = sr;
			acc_adts_header_info_.sample_rate = ff_mpeg4audio_sample_rates[sr];
		}
		{
			// 7. private_bit   1bit
			// 8. channel_configuration  3bit
			uint8_t  ch = data[2];
			ch <<= 7;
			uint8_t  pch = data[3];
			ch |= pch>>6;
			LIBMEIDA_CODEC_LOG(LS_INFO) << "adts header channel_configuration:" << ch;
			acc_adts_header_info_.num_aac_frames = ch;
		}
		{
			// frame size
			//const uint8_t *data = frame.data();
			uint8_t p = data[3];
			int32_t   frame_length = (p & 0x03);
			frame_length <<= 8;
			int32_t p2 = data[4];
			frame_length |= p2;
			frame_length <<= 3;
			int32_t p3 = data[5];
			int32_t  ff = ((p3 >> 5));
			frame_length |= ff;


			acc_adts_header_info_.frame_length = frame_length;
			//rtc::Buffer  new_aac_data(frame.data() + 7, frame_length);
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


	/*
	
	
	//获取一帧AAC
int getADTSframe(unsigned char* buffer, int buf_size, unsigned char* data ,int* data_size){
	int size = 0;
 
	if(!buffer || !data || !data_size ){
		return -1;
	}
 
	while(1){
		if(buf_size  < 7 ){
			return -1;
		}
		//Sync words
		if((buffer[0] == 0xff) && ((buffer[1] & 0xf0) == 0xf0) ){
			size |= ((buffer[3] & 0x03) << 11);     //帧长度13位中的 high 2 bit，左移11bit
 
			size |= buffer[4] << 3;              //帧长度13位中的 middle 8 bit,
								      //buffer[4]先变成int(4字节)左移3bit,再与size异或
 
			size |= ((buffer[5]) >> 5);        //帧长度13位中的low 3bit，取buffer[5]的前3bit
			break;
		}
		--buf_size;
		++buffer;
	}
 
	if(buf_size < size){
		return 1;
	}
 
	memcpy(data, buffer, size);
	*data_size = size;
 
	return 0;
}
 
int simplest_aac_parser(char *url)
{
	int data_size = 0;
	int size = 0;
	int cnt=0;
	int offset=0;
 
	//FILE *myout=fopen("output_log.txt","wb+");
	FILE *myout=stdout;
 
	unsigned char *aacframe = (unsigned char *)malloc(1024*5);
	unsigned char *aacbuffer = (unsigned char *)malloc(1024*1024);
 
	FILE *ifile = fopen(url, "rb");
	if(!ifile){
		printf("Open file error");
		return -1;
	}
 
	printf("-----+- ADTS Frame Table -+------+\n");
	printf(" NUM | Profile | Frequency| Size |\n");
	printf("-----+---------+----------+------+\n");
 
	while(!feof(ifile)){
		data_size = fread(aacbuffer + offset, 1, 1024*1024- offset, ifile);
		unsigned char* input_data = aacbuffer;
 
		while(1)
		{
			int ret = getADTSframe(input_data, data_size, aacframe, &size);
			if(ret == -1){
				break;
			}else if(ret == 1){
                //将读到的数据保存，回到fread重新读
				memcpy(aacbuffer,input_data, data_size);
				offset = data_size;
				break;
			}
 
			char profile_str[10] ={0};
			char frequence_str[10]={0};
            
            //aacframe[2]中的前两个bit
			unsigned char profile = aacframe[2]&0xC0;
			profile = profile>>6;
			switch(profile){
				case 0: sprintf(profile_str,"Main");break;
				case 1: sprintf(profile_str,"LC");break;
				case 2: sprintf(profile_str,"SSR");break;
				default:sprintf(profile_str,"unknown");break;
			}
 
			//sampling_frequency_index:表示使用的采样率下标，通过这个下标在 Sampling         
            //Frequencies[ ]数组中查找得知采样率的值。
 
			unsigned char sampling_frequency_index = aacframe[2]&0x3C;
			sampling_frequency_index = sampling_frequency_index >> 2;
			switch(sampling_frequency_index){
			case 0: sprintf(frequence_str,"96000Hz");break;
			case 1: sprintf(frequence_str,"88200Hz");break;
			case 2: sprintf(frequence_str,"64000Hz");break;
			case 3: sprintf(frequence_str,"48000Hz");break;
			case 4: sprintf(frequence_str,"44100Hz");break;
			case 5: sprintf(frequence_str,"32000Hz");break;
			case 6: sprintf(frequence_str,"24000Hz");break;
			case 7: sprintf(frequence_str,"22050Hz");break;
			case 8: sprintf(frequence_str,"16000Hz");break;
			case 9: sprintf(frequence_str,"12000Hz");break;
			case 10: sprintf(frequence_str,"11025Hz");break;
			case 11: sprintf(frequence_str,"8000Hz");break;
			default:sprintf(frequence_str,"unknown");break;
			}
 
 
			fprintf(myout,"%5d| %8s|  %8s| %5d|\n",cnt,profile_str ,frequence_str,size);
			data_size -= size;
			input_data += size;
			cnt++;
 
			if (cnt == 50)break;
		}   
 
	}
	fclose(ifile);
	free(aacbuffer);
	free(aacframe);
 
	return 0;
}
	*/
}