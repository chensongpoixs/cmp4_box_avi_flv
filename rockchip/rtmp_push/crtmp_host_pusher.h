/***********************************************************************************************
created: 		2025-06-15

author:			chensong

purpose:		_C_ROCKCHIP_DECDER_H_
************************************************************************************************/

#ifndef _C_RTMP_HOST_PUSHER_H_
#define _C_RTMP_HOST_PUSHER_H_
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "rtmpstream_interface.h"
namespace dsp
{
	//class AnyRtmpStreamerImpl;
	class crtmp_host_pusher : public AnyRtmpstreamerEvent
	{
	public:
		crtmp_host_pusher():m_impl (NULL){}

		virtual ~crtmp_host_pusher(){}

	public:
		virtual bool ExternalVideoEncoder() { return true; }
		virtual void OnStreamOk() {}
		virtual void OnStreamReconnecting(int times) {}
		virtual void OnStreamFailed(int code) {}
		virtual void OnStreamClosed() {}
		virtual void OnStreamStatus(int delayMs, int netBand) {}
	public:

		bool init(const char* url);



		void OnAACData(uint8_t* pdata, int len, uint32_t ts);
		void OnH264Data(uint8_t* pdata, int len, uint32_t ts);


		//void SetVideoParameter()
	private:
		AnyRtmpstreamer* m_impl;
	};
}


#endif // _C_RTMP_HOST_PUSHER_H_