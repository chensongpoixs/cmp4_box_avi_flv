/***********************************************************************************************
created: 		2025-06-15

author:			chensong

purpose:		rtmp_push
************************************************************************************************/

#ifndef _C_FLV_H_
#define _C_FLV_H_
#include <cstdio>
#include <cstdlib>
#include <iostream>
namespace dsp
{

#define AUDIODATA_AAC 10.0


	enum {
		FLV_HEADER_FLAG_HASVIDEO = 1,
		FLV_HEADER_FLAG_HASAUDIO = 4,
	};


	// tag type 
	enum FlvTagType {
		FLV_TAG_TYPE_AUDIO = 0x08,
		FLV_TAG_TYPE_VIDEO = 0x09,
		FLV_TAG_TYPE_META = 0x12,
	};
	struct cflv_header
	{
		unsigned char  flv[3]= { 0x46, 0x4C, 0x56 };
		uint8_t  version{1}; //
		uint8_t : 5;
		// 是否有音频  [AUTO-TRANSLATED:9467870a]
		// Whether there is audio
		uint8_t have_audio : 1;
		// 保留,置0  [AUTO-TRANSLATED:46985374]
		// Preserve, set to 0
		uint8_t : 1;
		// 是否有视频  [AUTO-TRANSLATED:42d0ed81]
		// Whether there is video
		uint8_t have_video : 1;
	};

	class cflv
	{
	public:
	//	explicit cflv();

	//	virtual ~cflv();

	public:
	//	bool init();

		//void udpate();

	//	void destroy();


		void _write_header();

		void _write_metadata(uint64_t ts);


		void _write_flv_metadata(char**flv_data, int32_t& flv_metadata_size);
		void _write_audio_header(uint64_t ts);
		void _write_video_header(uint64_t ts);
	private:

		int64_t    m_duration;
		int64_t    m_delay;

		bool       m_has_audio;
		bool	   m_has_video;

		/// <summary>
		///   video 
		/// </summary>
		int32_t    m_width;
		int32_t    m_height;
		int32_t    m_video_codec_type;
		int32_t    m_video_rate;


		int32_t    m_audio_codec_type;
		int32_t    m_audio_sample_rate;
		int32_t    m_audio_sample_size;
		int32_t    m_audio_chnannels;

		//int64_t    m_

	};
}


#endif // dd
