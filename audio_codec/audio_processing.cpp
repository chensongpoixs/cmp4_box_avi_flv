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

#include "libmedia_codec/audio_codec/audio_processing.h"

#include <rtc_base/logging.h>
#include <api/audio/audio_frame.h>
#include <common_audio/include/audio_util.h>
 

namespace libmedia_codec {

namespace {

void InitializeCaptureFrame(int input_sample_rate_hz, // 原始采样率
    int send_sample_rate_hz, // 最终发送的采样率
    size_t input_num_channels, // 原始采样的声道数
    size_t send_num_channels, // 最终发送的声道数
    webrtc::AudioFrame* audio_frame) 
{
    int min_processing_sample_rate = std::min(input_sample_rate_hz,
        send_sample_rate_hz);
    for (int native_sample_rate : webrtc::AudioProcessing::kNativeSampleRatesHz) {
        audio_frame->sample_rate_hz_ = native_sample_rate;
        // 在不丢失信息的情况下，尽可能用最小的采样率进行音频处理
        if (audio_frame->sample_rate_hz_ > min_processing_sample_rate) {
            break;
        }
    }

    audio_frame->num_channels_ = std::min(input_num_channels,
        send_num_channels);
}

void DownmixChannels(const int16_t* src_audio,
    size_t src_channels,
    size_t samples_per_channel,
    size_t dst_channels,
    int16_t* dst_audio) 
{
    // 当原始采集的声道数大于1，目标声道数=1
    // 4 -> 1, 2 -> 1
    if (src_channels > 1 && dst_channels == 1) {
        // 取多个声道的平均值
        webrtc::DownmixInterleavedToMono(src_audio, samples_per_channel,
            src_channels, dst_audio);
        return;
    }
    else if (src_channels == 4 && dst_channels == 2) {
        // 4 -> 2
        // 原始数据是交错存储的, 1234 1234 ...
        for (size_t i = 0; i < samples_per_channel; ++i) {
            // 原始音频数据前两个声道的平均值，设置成目标的第一个声道
            dst_audio[i * 2] = static_cast<int32_t>(src_audio[4 * i] + src_audio[4 * i + 1]) >> 2;
            // 原始音频数据后两个声道的平均值，设置成目标的第二个声道
            dst_audio[i * 2 + 1] = static_cast<int32_t>(src_audio[4 * i + 2] + src_audio[4 * i + 3]) >> 2;
        }
        return;
    }
}

void RemixAndResample(const int16_t* src_data,
    size_t samples_per_channel,
    size_t num_channels,
    size_t sample_rate_hz,
    webrtc::PushResampler<int16_t>* resampler,
    webrtc::AudioFrame* dst_frame) 
{
    const int16_t* audio_ptr = src_data;
    size_t audio_ptr_num_channels = num_channels;

    int16_t downmixed_audio[webrtc::AudioFrame::kMaxDataSizeSamples];

    // 如果原始采集数据的声道数大于目标声道数，此时需要进行向下的声道混合处理
    if (num_channels > dst_frame->num_channels_) {
        DownmixChannels(src_data, num_channels, samples_per_channel,
            dst_frame->num_channels_, downmixed_audio);
        audio_ptr = downmixed_audio;
        audio_ptr_num_channels = dst_frame->num_channels_;
    }

    // 重采样
    if (resampler->InitializeIfNeeded(sample_rate_hz, dst_frame->sample_rate_hz_,
        audio_ptr_num_channels) == -1) 
    {
        RTC_LOG(LS_ERROR) << "InitializeIfNeeded failed, "
            << "sample_rate_hz: " << sample_rate_hz
            << ", dst_frame->sample_rate_hz: " << dst_frame->sample_rate_hz_
            << ", num_channels: " << audio_ptr_num_channels;
        return;
    }

    size_t src_length = samples_per_channel * audio_ptr_num_channels;
    size_t out_length = resampler->Resample(audio_ptr, src_length,
        dst_frame->mutable_data(), webrtc::AudioFrame::kMaxDataSizeSamples);
    if (out_length == -1) {
        RTC_LOG(LS_ERROR) << "Resample failed, audio_ptr: " << audio_ptr
            << ", src_length: " << src_length
            << ", dst_ptr: " << dst_frame->mutable_data();
        return;
    }

    dst_frame->samples_per_channel_ = out_length / audio_ptr_num_channels;
}

int ProcessAudioFrame(webrtc::AudioProcessing* ap, webrtc::AudioFrame* frame) {
    if (!ap || !frame) {
        return webrtc::AudioProcessing::Error::kNullPointerError;
    }

    webrtc::StreamConfig input_config(frame->sample_rate_hz_, frame->num_channels_);
    webrtc::StreamConfig output_config(frame->sample_rate_hz_, frame->num_channels_);
    // 音频处理
    int result = ap->ProcessStream(frame->data(), input_config, output_config,
        frame->mutable_data());
    return result;
}

void ProcessCaptureFrame(uint32_t total_delay_ms, bool key_pressed,
    webrtc::AudioProcessing* audio_processing,
    webrtc::AudioFrame* audio_frame)         
{
    if (audio_processing) {
        audio_processing->set_stream_delay_ms(total_delay_ms);
        audio_processing->set_stream_key_pressed(key_pressed);
        int err = ProcessAudioFrame(audio_processing, audio_frame);
        if (err != 0) {
            RTC_LOG(LS_WARNING) << "ProcessingStream() failed, err: " << err;
        }
    }
}

} // namespace

AudioProcessingFilter::AudioProcessingFilter() :
    audio_processing_(webrtc::AudioProcessingBuilder().Create())
{ 
}

AudioProcessingFilter::~AudioProcessingFilter() {
}

bool AudioProcessingFilter::Start() {
    return true;
}
//
//void AudioProcessingFilter::Setup(const std::string& json_config) {
//    JsonValue value;
//    value.FromJson(json_config);
//    JsonObject jobj = value.ToObject();
//    JsonObject j_audio_processing_filter = jobj["audio_processing_filter"].ToObject();
//    bool aec_enable = j_audio_processing_filter["AEC"].ToBool();
//    bool vad_enable = j_audio_processing_filter["VAD"].ToBool();
//    bool high_pass_filter_enable = j_audio_processing_filter["high_pass_filter"].ToBool();
//    bool ans_enable = j_audio_processing_filter["ANS"].ToBool();
//    bool agc_enable = j_audio_processing_filter["AGC"].ToBool();
//
//    encoder_clock_rate_ = j_audio_processing_filter["encoder_clock_rate"].ToInt();
//    encoder_channels_ = j_audio_processing_filter["encoder_channels"].ToInt();
//
//    // 获得apm模块默认的配置
//    webrtc::AudioProcessing::Config apm_config = audio_processing_->GetConfig();
//    // AEC
//    apm_config.echo_canceller.enabled = aec_enable;
//    //apm_config.echo_canceller.mobile_mode = true;
//
//    // VAD
//    apm_config.high_pass_filter.enabled = high_pass_filter_enable;
//    apm_config.voice_detection.enabled = vad_enable;
//
//    // ANS
//    apm_config.noise_suppression.enabled = true;
//    // 降噪等级
//    apm_config.noise_suppression.level 
//        = webrtc::AudioProcessing::Config::NoiseSuppression::kHigh;
//
//    // AGC
//    apm_config.gain_controller1.enabled = agc_enable;
//#if defined(WEBRTC_WIN)
//    apm_config.gain_controller1.mode = apm_config.gain_controller1.kAdaptiveAnalog;
//#endif
//
//    // 应用功能设置
//    audio_processing_->ApplyConfig(apm_config);
//}

void AudioProcessingFilter::Stop() {
}

void AudioProcessingFilter::OnNewMediaFrame(std::shared_ptr<libmedia_codec::AudioFrame> frame) {
    auto audio_frame = std::make_unique<webrtc::AudioFrame>();
    // 最小采样率和声道数处理
    // 我们采集的声道数和采样率，与我们最终的发送声道数和采样率是不同的
    // 音频处理模块，它能够处理的采样率有几个固定的档位
    //InitializeCaptureFrame(frame->fmt.sub_fmt.audio_fmt.samples_per_sec,
    //    encoder_clock_rate_,
    //    frame->fmt.sub_fmt.audio_fmt.channels,
    //    encoder_channels_,
    //    audio_frame.get());

    //// 此时，我们可以获得音频处理的最小采样率和声道数
    //// 但是，原始采集数据的音频采样率和声道数，可能与这个采样率和声道数不相同
    //// 所以，需要进行声道混合和重采样的处理
    //RemixAndResample((int16_t*)frame->data[0],
    //    frame->fmt.sub_fmt.audio_fmt.samples_per_channel,
    //    frame->fmt.sub_fmt.audio_fmt.channels,
    //    frame->fmt.sub_fmt.audio_fmt.samples_per_sec,
    //    &capture_resampler_,
    //    audio_frame.get());

    //// 3A处理
    //ProcessCaptureFrame(frame->fmt.sub_fmt.audio_fmt.total_delay_ms,
    //    frame->fmt.sub_fmt.audio_fmt.key_pressed,
    //    audio_processing_.get(),
    //    audio_frame.get());

    //// copy预处理之后的音频数据
    //memcpy(frame->data[0], audio_frame->data(), audio_frame->samples_per_channel_
    //    * audio_frame->num_channels_ 
    //    * frame->fmt.sub_fmt.audio_fmt.nbytes_per_sample);
    //
    //if (out_pin_) {
    //    out_pin_->PushMediaFrame(frame);
    //}
}

} // namespace  