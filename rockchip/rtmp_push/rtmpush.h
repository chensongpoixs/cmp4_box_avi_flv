 
#ifndef __ANY_RTMP_PUSH_H__
#define __ANY_RTMP_PUSH_H__
#include <thread>
#include <list>
#include <vector>
#include <mutex>
#include <string>
enum RTMP_STATUS
{
	RS_STM_Init,
	RS_STM_Handshaked,
	RS_STM_Connected,
	RS_STM_Published,
	RS_STM_Closed
};

enum ENC_DATA_TYPE{
    VIDEO_DATA,
    AUDIO_DATA,
    META_DATA
};

typedef struct EncData
{
	EncData(void) :_data(NULL), _dataLen(0),
		_bVideo(false), _dts(0) {}
	uint8_t*_data;
	int _dataLen;
	bool _bVideo;
	uint32_t _dts;
	ENC_DATA_TYPE _type;
}EncData;

class AnyRtmpushCallback
{
public:
	AnyRtmpushCallback(void){};
	virtual ~AnyRtmpushCallback(void){};

	virtual void OnRtmpConnected() = 0;
	virtual void OnRtmpReconnecting(int times) = 0;
	virtual void OnRtmpDisconnect() = 0;
	virtual void OnRtmpStatusEvent(int delayMs, int netBand) = 0;
};

class AnyRtmpPush  
{
public:
	AnyRtmpPush(AnyRtmpushCallback&callback, const std::string&url);
	virtual ~AnyRtmpPush(void);

	void Close();

	void EnableOnlyAudioMode();
	void SetAudioParameter(int samplerate/*44100*/, int pcmbitsize/*16*/, int channel/*1*/);
	void SetVideoParameter(int width, int height, int videodatarate, int framerate);

	void SetH264Data(uint8_t* pdata, int len, uint32_t ts);
	void SetAacData(uint8_t* pdata, int len, uint32_t ts);
	void GotH264Nal(uint8_t* pdata, int len, uint32_t ts);

protected:
	//* For Thread
	virtual void Run();

	void CallConnect();
	void CallDisconnect();
	void CallStatusEvent(int delayMs, int netBand);
	void DoSendData();
    void setMetaData();
    void setMetaData(uint8_t* pData, int nLen, uint32_t ts);

private:
	AnyRtmpushCallback&	callback_;
	bool				running_;
	bool				need_keyframe_;
	bool				only_audio_mode_;
	int					retrys_;
	std::string			str_url_;
	std::chrono::steady_clock::time_point			stat_time_;
	uint32_t			net_band_;

	//rtc::CriticalSection	cs_list_enc_;
	std::mutex    cs_list_enc_;
	std::list<EncData*>		lst_enc_data_;

	std::thread m_thead;

private:
	//* For RTMP
	// 0 = Linear PCM, platform endian
	// 1 = ADPCM
	// 2 = MP3
	// 7 = G.711 A-law logarithmic PCM
	// 8 = G.711 mu-law logarithmic PCM
	// 10 = AAC
	// 11 = Speex
	char sound_format_;
	// 0 = 5.5 kHz
	// 1 = 11 kHz
	// 2 = 22 kHz
	// 3 = 44 kHz
	char sound_rate_;
	// 0 = 8-bit samples
	// 1 = 16-bit samples
	char sound_size_;
	// 0 = Mono sound
	// 1 = Stereo sound
	char sound_type_;

    /*rtc::CriticalSection*/ std::mutex	cs_rtmp_;
	RTMP_STATUS		rtmp_status_;
	void*			rtmp_;

	int video_width_;
	int video_height_;
	int video_framerate_;
	int video_datarate_;

	int sound_samplerate_;
};

#endif	// __ANY_RTMP_PUSH_H__