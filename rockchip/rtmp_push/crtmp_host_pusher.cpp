/***********************************************************************************************
created: 		2025-06-15

author:			chensong

purpose:		_C_ROCKCHIP_DECDER_H_
************************************************************************************************/
#include "crtmp_host_pusher.h"
#include "rtmpstreamer.h"

namespace dsp
{
	bool crtmp_host_pusher::init(const char* url)
	{
		m_impl = AnyRtmpstreamer::Create(*this);
		m_impl->StartStream(url);
		return true;
	}
	void crtmp_host_pusher::OnAACData(uint8_t* pdata, int len, uint32_t ts)
	{
		m_impl->OnAACData(pdata, len, ts);
	}
	void crtmp_host_pusher::OnH264Data(uint8_t* pdata, int len, uint32_t ts)
	{
		m_impl->OnH264Data(pdata, len, ts);
	}
}