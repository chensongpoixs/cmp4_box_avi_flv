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


#ifndef _C__AUDIO_PROCESSING_H_
#define _C__AUDIO_PROCESSING_H_

#include <modules/audio_processing/include/audio_processing.h>
#include <common_audio/resampler/include/push_resampler.h>
#include "libmedia_codec/audio_frame.h"
namespace libmedia_codec {

class AudioProcessingFilter   {
public:
    AudioProcessingFilter();
    ~AudioProcessingFilter()  ;

    bool Start()  ; 
    void Stop()  ;
    void OnNewMediaFrame(std::shared_ptr<libmedia_codec::AudioFrame> frame)  ;
    
private: 
    rtc::scoped_refptr<webrtc::AudioProcessing> audio_processing_;
    int encoder_clock_rate_ = 48000;
    size_t encoder_channels_ = 2;
    webrtc::PushResampler<int16_t> capture_resampler_;
};

} // namespace  

#endif //  