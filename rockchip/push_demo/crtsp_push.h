/***********************************************************************************************
created: 		2025-06-15

author:			chensong

purpose:		nv_cuda_ decoder
************************************************************************************************/

#ifndef _C_RTSP_PUSH_H_
#define _C_RTSP_PUSH_H_
#include <stdio.h>
#include <iostream>
extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/mathematics.h"
#include "libavutil/time.h"
 
#include <libavutil/frame.h>
#include <libavutil/avutil.h>
		//#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/display.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavfilter/buffersink.h>
}

#include <list>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
namespace  dsp
{
	class RawPacket
	{
	public:
		RawPacket(const unsigned char* _data, const size_t _size = 0, int64_t pts_ = 0, const bool _containsKeyFrame = false);
		const unsigned char* Data() const noexcept { return data.data(); }
		size_t Size() const noexcept { return data.size(); }
		bool ContainsKeyFrame() const noexcept { return containsKeyFrame; }
		int64_t  Pts() const noexcept { return pts; }
	private:
		std::vector<unsigned char> data;
		int64_t		pts;
		bool containsKeyFrame = false;
	};
	class crtsp_push
	{
	private:
		typedef std::condition_variable ccond;
	public:
		crtsp_push()
		: m_url("")
		, m_octx(NULL)
		, m_stoped(true)
		, m_packet_queue()
		, m_index_frame(0)
		, m_packet(NULL){}
		~crtsp_push(){}

	public:


		bool init(const std::string& url);
		void push_frame(AVPacket * packet);
		void destroy();

	private:
		void _handler_packet_item(const RawPacket * packet);
		bool _init();
	private:
		void _work_pthread();
	private:
		std::string       m_url;
		AVFormatContext* m_octx = NULL;


		std::thread  m_thread;
		std::atomic_bool   m_stoped;
		std::mutex				m_lock;
		std::list< RawPacket>  m_packet_queue;
		ccond m_condition;
		std::atomic<int64_t>      m_index_frame;
		AVPacket* m_packet;
	};
}


#endif // _C_RTSP_PUSH_H_