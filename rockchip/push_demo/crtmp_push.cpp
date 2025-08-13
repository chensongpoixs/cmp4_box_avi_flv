/***********************************************************************************************
created: 		2025-06-15

author:			chensong

purpose:		rtmp_push
************************************************************************************************/
#include "crtmp_push.h"
#include "librtmp/log.h"
namespace dsp
{

	static void RTMPLogCallback(int level, const char* fmt, va_list  args)
	{
		switch (level) {
		default:
		case RTMP_LOGCRIT:    level = AV_LOG_FATAL;   break;
		case RTMP_LOGERROR:   level = AV_LOG_ERROR;   break;
		case RTMP_LOGWARNING: level = AV_LOG_WARNING; break;
		case RTMP_LOGINFO:    level = AV_LOG_INFO;    break;
		case RTMP_LOGDEBUG:   level = AV_LOG_VERBOSE; break;
		case RTMP_LOGDEBUG2:  level = AV_LOG_DEBUG;   break;
		}

		//av_vlog(NULL, level, fmt, args);
		//av_log(NULL, level, "\n");
	}


	bool crtmp_push::init(const std::string& url)
	{
		/*if (!m_rtmp_ptr)
		{
			printf("rtmp  != nullptr !!!\n");
			return false;
		}*/

		RTMP_LogSetLevel(RTMP_LOGCRIT);
		RTMP_LogSetCallback(&RTMPLogCallback);

		RTMP_Init(&m_rtmp);

		printf("RTMP init OK !!!\n");
		printf("RTMP parse url %s .... \n", url.c_str());

		if (RTMP_SetupURL(&m_rtmp, (char*)url.c_str()))
		{
			printf("RTMP parse url failed !!!  [%s] .... \n", url.c_str());
			return false;
		}
		printf("RTMP parse url OK !!!  [%s] .... \n", url.c_str());
		m_rtmp.Link.lFlags |= RTMP_LF_BUFX;


		RTMP_EnableWrite(&m_rtmp);

		uint32_t bufferTime = (10 * 60 * 60 * 1000)	/* 10 hours default */;
		RTMP_SetBufferMS(&m_rtmp, bufferTime);

		printf("rtmp set buffer [%u] ms \n", bufferTime);
		printf("rtmp connect url [%s] ... \n", url.c_str());
		bool ret = RTMP_Connect(&m_rtmp, NULL);
		if (!ret)
		{
			printf("rtmp connect url [%s] failed !!!  \n", url.c_str());
			return false;
		}

		printf("rtmp connect url [%s] OK !!!  \n", url.c_str());

		// 设置流 seek
		int32_t seekTime = 0; // default 0
		ret = RTMP_ConnectStream(&m_rtmp, seekTime);
		if (!ret)
		{
			printf("rtmp connect stream url [%s] failed !!!  \n", url.c_str());
			return false;
		}
		printf("rtmp connect stream url [%s] OK !!!  \n", url.c_str());


		return true;
	}
}