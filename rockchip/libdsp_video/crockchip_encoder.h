/***********************************************************************************************
created: 		2025-06-15

author:			chensong

purpose:		_C_ROCKCHIP_DECDER_H_
************************************************************************************************/

#ifndef _C_ROCKCHIP_ENCODER_H_
#define _C_ROCKCHIP_ENCODER_H_
#include <cstdio>
#include <cstdlib>
#include <iostream>


#if defined(_WIN32)
//#include <vld.h>
#endif
#include "craw_packet.h"
#include <mutex>
#include <list>

#include <string.h>
#include <condition_variable>

#include <rockchip/rk_mpi.h>
#include <rockchip/mpp_buffer.h>
#include <atomic>
#include <atomic>
#include <thread>
//#include <mpp_mem.h>
//#include <mpp_env.h>
//#include <mpp_time.h>
//#include <mpp_common.h>

namespace dsp
{

	struct video_info_param
	{
		int32_t width;
		int32_t height;
		int32_t fmt;
		int32_t type;
		int32_t max_bitrate;
		int32_t min_bitrate;
		int32_t avg_bitrate;
		int32_t gop_size;
		int32_t rc_mode;
		int32_t gop_mode;
		int32_t input_fps;
		int32_t output_fps;
	};


	class crockchip_encoder
	{
	public:
		explicit crockchip_encoder()
		: m_ctx(NULL)
		, m_mpi(NULL)
		, m_stoped(true)
		, m_video_info_param(){}

		virtual ~crockchip_encoder() {}

	public:

		bool init(MppCodingType  type, video_info_param param);


		bool push(MppFrame &frame);
	private:
		bool _set_enc_cfg(MppEncCfg  *cfg);

	private:

		
		void _encoder_thread();
		void _work_thread();
	private:
		MppCtx m_ctx; 
		MppApi* m_mpi;

		
		std::atomic_bool    m_stoped;

		MppCodingType      m_coding_type;
		int32_t      m_width;
		int32_t      m_height;
		int32_t      m_hor_stride;
		int32_t      m_ver_stride;
		int32_t      m_fmt;
		
		int32_t      m_rc_mode;


		int32_t      m_fps_in_flex = 0;
		int32_t      m_fps_in_num;
		int32_t      m_fps_in_den;
		int32_t      m_fps_out_flex = 0;
		int32_t      m_fps_out_num;
		int32_t      m_fps_out_den;
		int32_t      m_gop_size;
		int32_t      m_bitrate;// avg bps;
		int32_t      m_max_bitrate;
		int32_t      m_min_bitrate;
		video_info_param      m_video_info_param;
		std::mutex            m_queue_lock;
		std::list<MppFrame>   m_queue_list;
		std::condition_variable  m_condition;
		std::thread     m_thread;

	};

}

#endif // 