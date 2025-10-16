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



#include "libmedia_codec/audio_codec/opus_encoder.h"

#include <rtc_base/logging.h>
#include <rtc_base/thread.h>
//#include <opus/opus.h>
#include "opus.h"
#include "libmedia_codec/audio_encoder.h"

namespace libmedia_codec {

namespace {

const int kDefaultComplexity = 9;

} // namespace

OpusEncoder2::OpusEncoder2() :
    opus_app_(OPUS_APPLICATION_VOIP)
	, encode_audio_obser_(nullptr)
{
     
}

OpusEncoder2::~OpusEncoder2() {
}

bool OpusEncoder2::Start() {
    RTC_LOG(LS_INFO) << "OpusEncoder2 Start";

    if (running_) {
        RTC_LOG(LS_WARNING) << "OpusEncoder2 already Start, ignore";
        return true;
    }

    running_ = true;

    encoder_thread_ = new std::thread([=]() {
        RTC_LOG(LS_INFO) << "OpusEncoder2 encode thread start";
        rtc::SetCurrentThreadName("opus_encode_thread");

        // 创建opus编码器
        OpusEncoder* encoder = CreateEncoder();
        if (!encoder) {
            RTC_LOG(LS_WARNING) << "opus CreateEncoder() failed";
            running_ = false;
            return;
        }
        
        size_t frame_queue_size = 0;
        unsigned char encoded_buffer[1200] = {};

        while (running_) {
            // 从队列当中获取一帧音频数据
            std::shared_ptr<libmedia_codec::AudioFrame> frame;
            {
                std::unique_lock<std::mutex> lock(frame_queue_mtx_);
                frame_queue_size = frame_queue_.size();
                if (frame_queue_size > 0) {
                    frame = frame_queue_.front();
                    frame_queue_.pop();
                }

                if (!frame) {
                    cond_var_.wait(lock);
                    continue;
                }
            }

            // 获得了一帧音频数据
            // 根据音频数据动态调整编码器
            if (sample_rate_hz_ != frame->sample_rate_hz() ||
                channels_ != frame->num_channels()) 
            {
                RTC_LOG(LS_INFO) << "OpusEncoder2 encode param changed"
                    << ", sample_rate_hz: " << sample_rate_hz_
                    << ", dst_sample_rate_hz: " << frame->sample_rate_hz()
                    << ", channels: " << channels_
                    << ", dst_channels: " << frame->num_channels();
                if (encoder) {
                    sample_rate_hz_ = frame->sample_rate_hz();
                    channels_ = frame->num_channels();
                    opus_encoder_destroy(encoder);
                    encoder = CreateEncoder();
                }
            }

            if (!encoder) {
                RTC_LOG(LS_WARNING) << "opus CreateEncoder() failed";
                running_ = false;
                return;
            }

            // 开始编码数据
            int ret = opus_encode(encoder, (const opus_int16*)frame->data(),
                frame->samples_per_channel_,
                encoded_buffer, sizeof(encoded_buffer));
            if (ret < 0) {
                RTC_LOG(LS_WARNING) << "opus encode failed: " << ret;
                running_ = false;
                opus_encoder_destroy(encoder);
                return;
            }

		/*	static FILE* out_pcm_ptr = fopen("aduio.pcm", "wb+");
			if (out_pcm_ptr)
			{
				fwrite(frame->data(), 1, frame->samples_per_channel_, out_pcm_ptr);
				fflush(out_pcm_ptr);
			}*/

            // 创建新的media frame
            //auto output_frame = std::make_shared<MediaFrame>(ret);
            //output_frame->fmt.media_type = MainMediaType::kMainTypeAudio;
            //output_frame->fmt.sub_fmt = frame->fmt.sub_fmt;
            //output_frame->fmt.sub_fmt.audio_fmt.type = SubMediaType::kSubTypeOpus;
            //output_frame->data_len[0] = ret;
            //memcpy(output_frame->data[0], encoded_buffer, ret);
            //output_frame->ts = frame->ts;
			//
            //if (out_pin_) {
            //    out_pin_->PushMediaFrame(output_frame);
            //}
			auto encode_frame = std::make_shared< AudioEncoder::EncodedInfoLeaf>();
			encode_frame->encoded_bytes = ret;
			encode_frame->encoded_timestamp = frame->timestamp_;
			encode_frame->audio_encode_data.AppendData(encoded_buffer);
			encode_frame->encoder_type = AudioEncoder::CodecType::kOpus;
#if 0
			static FILE * out_audio_file = fopen("audio.opus", "wb+");
			if (out_audio_file)
			{
				fwrite(encoded_buffer, 1, ret, out_audio_file);
				fflush(out_audio_file);
			}
#endif // 
			if (encode_audio_obser_)
			{
				encode_audio_obser_->SendAudioEncode(encode_frame);
			}
        }

        opus_encoder_destroy(encoder);
    });

    return true;
}
void OpusEncoder2::SetChannel(int32_t channels)
{
	channels_ = channels;
}
void  OpusEncoder2::SetSample(int32_t  sample)
{
	sample_rate_hz_ = sample;
}

void OpusEncoder2::Stop() {
    RTC_LOG(LS_INFO) << "OpusEncoder2 Stop";
    if (!running_) {
        return;
    }

    running_ = false;
    cond_var_.notify_all();

    if (encoder_thread_ && encoder_thread_->joinable()) {
        encoder_thread_->join();
        RTC_LOG(LS_INFO) << "opus encoder thread join success";
        delete encoder_thread_;
        encoder_thread_ = nullptr;
    }
}

void OpusEncoder2::OnNewMediaFrame(std::shared_ptr<libmedia_codec::AudioFrame> frame) {
    // 在音频采集线程触发
    std::unique_lock<std::mutex> lock(frame_queue_mtx_);
    frame_queue_.push(frame);
    cond_var_.notify_one();
}

void OpusEncoder2::SetSendFrame(EncodeAudioObser * encode_audio_obser)
{
	encode_audio_obser_ = encode_audio_obser;
}

OpusEncoder* OpusEncoder2::CreateEncoder() {
    int err = 0;
    OpusEncoder* encoder = opus_encoder_create(sample_rate_hz_, channels_,
        opus_app_, &err);
    if (err != OPUS_OK || !encoder) {
        RTC_LOG(LS_WARNING) << "opus_encoder_create failed, err: " << err;
        return nullptr;
    }

    // 设置核心的编码参数
    do {
        // 设置编码码率
        if (opus_encoder_ctl(encoder, OPUS_SET_BITRATE(bitrate_)) != OPUS_OK) {
            RTC_LOG(LS_WARNING) << "OPUS_SET_BITRATE failed";
            break;
        }

        // 设置带宽(auto)
        if (opus_encoder_ctl(encoder, OPUS_SET_BANDWIDTH(OPUS_AUTO)) != OPUS_OK) {
            RTC_LOG(LS_WARNING) << "OPUS_SET_BANDWIDTH failed";
            break;
        }

        // 设置最大带宽
        if (opus_encoder_ctl(encoder, OPUS_SET_MAX_BANDWIDTH(OPUS_BANDWIDTH_FULLBAND)) != OPUS_OK) {
            RTC_LOG(LS_WARNING) << "OPUS_SET_MAX_BANDWIDTH failed";
            break;
        }

        // 设置复杂度
        if (opus_encoder_ctl(encoder, OPUS_SET_COMPLEXITY(kDefaultComplexity)) != OPUS_OK) {
            RTC_LOG(LS_WARNING) << "OPUS_SET_MAX_BANDWIDTH failed";
            break;
        }

        // 启用FEC
        if (opus_encoder_ctl(encoder, OPUS_SET_INBAND_FEC(1)) != OPUS_OK) {
            RTC_LOG(LS_WARNING) << "OPUS_SET_INBAND_FEC failed";
            break;
        }

        // 设置丢包率
        if (opus_encoder_ctl(encoder, OPUS_SET_PACKET_LOSS_PERC(5)) != OPUS_OK) {
            RTC_LOG(LS_WARNING) << "OPUS_SET_PACKET_LOSS_PERC failed";
            break;
        }

        return encoder;

    } while (false);

    opus_encoder_destroy(encoder);
    return nullptr;
}

} // namespace OpusEncoder2