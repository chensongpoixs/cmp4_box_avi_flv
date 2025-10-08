/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Please visit https://chensongpoixs.github.io for detail
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 ******************************************************************************/
 /*****************************************************************************
				   Author: chensong
				   date:  2025-10-02



 ******************************************************************************/



#ifndef _C_OPUS_ENCODER_H_
#define _C_OPUS_ENCODER_H_

#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include "libmedia_codec/audio_frame.h"
#include "libmedia_codec/audio_encoder.h"
#include "opus.h"
//class OpusEncoder;

namespace libmedia_codec {
	class EncodeAudioObser
	{
	public:
		virtual ~EncodeAudioObser() {}
		virtual void   SendAudioEncode(std::shared_ptr<libmedia_codec::AudioEncoder::EncodedInfoLeaf> f) = 0;
	};
class OpusEncoder2  {
public:
	OpusEncoder2();
    ~OpusEncoder2()  ;

    bool Start()  ;
     
    void Stop()  ;
     
	void  OnNewMediaFrame(std::shared_ptr<libmedia_codec::AudioFrame> frame);
    
	void SetSendFrame(EncodeAudioObser  *   encode_audio_obser);

private:
    OpusEncoder* CreateEncoder();

private: 
    std::thread* encoder_thread_ = nullptr;
    std::atomic<bool> running_{ false };
    int opus_app_;
    size_t sample_rate_hz_ = 48000;
    size_t channels_ = 1;
    uint32_t bitrate_ = 48000; // 48kbps
    std::queue<std::shared_ptr<libmedia_codec::AudioFrame>> frame_queue_;
    std::mutex frame_queue_mtx_;
    std::condition_variable cond_var_;
	EncodeAudioObser  *   encode_audio_obser_;
};

} // namespace  

#endif //  