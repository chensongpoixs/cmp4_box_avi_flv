 
#ifndef __ANY_RTMP_STREAMER_H__
#define __ANY_RTMP_STREAMER_H__
//#include "avcodec.h"
//#include "rtmpcore.h"
#include "rtmpush.h"
#include "rtmpstream_interface.h"
#include <mutex>

namespace dsp {

class AnyRtmpStreamerImpl : public AnyRtmpstreamer,public AnyRtmpushCallback
{
public:
	AnyRtmpStreamerImpl(AnyRtmpstreamerEvent&callback);
	virtual ~AnyRtmpStreamerImpl(void);

	 
	virtual void SetAudioEnable(bool enabled);
	virtual void SetVideoEnable(bool enabled);
    virtual void SetAutoAdjustBit(bool enabled);
	virtual void SetVideoParameter(int w, int h, int bitrate);
	virtual void SetBitrate(int bitrate);

	void StartStream(const std::string&url);
	void StopStream();

public:
	//* For AVCodecCallback
	virtual void OnEncodeDataCallback(bool audio, uint8_t *p, uint32_t length, uint32_t ts);

	//* For AnyRtmpushCallback
	virtual void OnRtmpConnected();
	virtual void OnRtmpReconnecting(int times);
	virtual void OnRtmpDisconnect();
	virtual void OnRtmpStatusEvent(int delayMs, int netBand);

protected:
	virtual void StartEncoder();
	virtual void StopEncoder();
	virtual void OnAACData(uint8_t* pdata, int len, uint32_t ts);
	virtual  void OnH264Data(uint8_t* pdata, int len, uint32_t ts);

private:
	bool					rtmp_connected_;
    bool                    auto_adjust_bit_;
	AnyRtmpstreamerEvent		&callback_;

	// Audio
	//A_AACEncoder*			a_aac_encoder_;
	bool					a_muted_;
	bool					a_enabled_;
	int						a_sample_hz_;
	int						a_channels_;
	// Video
	//V_H264Encoder*			v_h264_encoder_;
	int						v_width;
	int						v_height;
	int						v_framerate_;
	int						v_bitrate_;

    //rtc::CriticalSection	cs_av_rtmp_;
	std::mutex cs_av_rtmp_;
	AnyRtmpPush*				av_rtmp_;
};

}	// namespace webrtc

#endif	// __ANY_RTMP_STREAMER_H__