/***********************************************************************************************
created: 		2025-06-15

author:			chensong

purpose:		rtmp_push
************************************************************************************************/

#ifndef _C_RTMP_PUSH_H_
#define _C_RTMP_PUSH_H_
#include <stdio.h>
#include <iostream>
#include <list>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "crtsp_push.h"
#include "librtmp/rtmp.h"
namespace  dsp
{
	class crtmp_push
	{
	private:
		typedef std::condition_variable ccond;
	public:
		crtmp_push()
			: m_url("")
			, m_rtmp()
			, m_stoped(true)
			, m_packet_queue()
			, m_index_frame(0)
			, m_packet(NULL) {}
		~crtmp_push() {}

	public:


		bool init(const std::string & url);
		void push_frame(AVPacket * packet);
		void destroy();

	private:
		void _handler_packet_item(const RawPacket * packet);
		bool _init();
	private:
		void _work_pthread();
	private:
		std::string       m_url;

		RTMP m_rtmp;

		std::thread  m_thread;
		std::atomic_bool   m_stoped;
		std::mutex				m_lock;
		std::list< RawPacket>  m_packet_queue;
		ccond m_condition;
		std::atomic<int64_t>      m_index_frame;
		AVPacket* m_packet;
	};
}

#endif // _C_RTMP_PUSH_H_