/***********************************************************************************************
created: 		2025-06-15

author:			chensong

purpose:		_C_ROCKCHIP_DECDER_H_
************************************************************************************************/

#ifndef _C_ROCKCHIP_DECDER_H_
#define _C_ROCKCHIP_DECDER_H_
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

#include <rk_mpi.h>
#include <atomic>
#include <atomic>
//#include <mpp_mem.h>
//#include <mpp_env.h>
//#include <mpp_time.h>
//#include <mpp_common.h>

namespace dsp
{


	typedef enum MppDecBufMode_e {
		MPP_DEC_BUF_HALF_INT,
		MPP_DEC_BUF_INTERNAL,
		MPP_DEC_BUF_EXTERNAL,
		MPP_DEC_BUF_MODE_BUTT,
	} MppDecBufMode;
	class crockchip_decder
	{
	private:
		typedef        std::mutex						clock_type;
		typedef        std::lock_guard<clock_type>      clock_guard;
		typedef			std::condition_variable			ccond;
	public:
		explicit crockchip_decder()
			: m_stoped(true)
			, m_ctx(NULL)
			, m_mpi(NULL)
			, m_cfg(NULL)
			, m_coding_type(MPP_VIDEO_CodingUnused)
			, m_packet_queue()
			, m_packet(NULL)
			, m_buffer_group(NULL)
			, m_decbuf_mode(MPP_DEC_BUF_HALF_INT)
			, m_bufs(NULL)
			, m_buf_count(0)
			, m_buf_size(0)
		{}


		virtual ~crockchip_decder(){}


	public:
		bool init(MppCodingType type);

		void destroy();


	public:


		void stop();



		void push_packet(void * data,  uint32_t size, int64_t pts, int64_t dts);

	private:

		void _decder_thread();
		void _work_thread();


		bool _handler_packet(const craw_packet* packet);


		bool _init_dec_mem(int32_t size, int32_t count);
		void _deinit_dec_mem();
	private:
		std::atomic_bool  m_stoped;
		MppCtx			m_ctx = NULL;
		MppApi*			m_mpi = NULL;
		MppDecCfg  m_cfg;
		MppCodingType   m_coding_type;

		clock_type				 m_quene_lock;
		std::list<craw_packet>   m_packet_queue;
		ccond					 m_condition;
		MppPacket                m_packet;


		std::thread				 m_decder_thread;




		std::thread		         m_thread;




		// zero memcopy
		MppBufferGroup           m_buffer_group;
		MppDecBufMode            m_decbuf_mode;
		MppBuffer*				 m_bufs;
		int32_t					 m_buf_count;
		int32_t					 m_buf_size;
	};

}


#endif // _C_ROCKCHIP_DECDER_H_
