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
				   date:  2025-10-10



 ******************************************************************************/

#include "libmedia_codec/audio_codec/audio_decoder.h"
#include "rtc_base/logging.h"
#include "rtc_base/buffer.h"
#include "rtc_base/byte_buffer.h"
namespace libmedia_codec
{
	namespace {


		// #define MAX_AUDIO_FRAME_SIZE (192000)
		static const int32_t  kMaxAudioFrameSize = 192000;

		struct ScopedPtrAVFreePacket {
			void operator()(AVPacket* packet) { av_packet_free(&packet); }
		};
		typedef std::unique_ptr<AVPacket, ScopedPtrAVFreePacket> ScopedAVPacket;

		ScopedAVPacket MakeScopedAVPacket() {
			ScopedAVPacket packet(av_packet_alloc());
			return packet;
		}

	}
	bool AudioDecoder::Configure()
	{
		// Initialize AVCodecContext.
		av_context_.reset(avcodec_alloc_context3(nullptr));

		av_context_->codec_type = AVMEDIA_TYPE_AUDIO;
		av_context_->codec_id = AV_CODEC_ID_AAC;;
		 

#if 0
		av_context_->coded_width = width;
		av_context_->coded_height = height;

		av_context_->pix_fmt = kPixelFormatDefault;
		av_context_->extradata = nullptr;
		av_context_->extradata_size = 0;

		// If this is ever increased, look at `av_context_->thread_safe_callbacks` and
		// make it possible to disable the thread checker in the frame buffer pool.
		av_context_->thread_count = 1;
		av_context_->thread_type = FF_THREAD_SLICE;

		// Function used by FFmpeg to get buffers to store decoded frames in.
		av_context_->get_buffer2 = AVGetBuffer2;
		// `get_buffer2` is called with the context, there `opaque` can be used to get
		// a pointer `this`.
		av_context_->opaque = this;

#endif // 

		const AVCodec* codec = avcodec_find_decoder(av_context_->codec_id);
		if (!codec) {
			// This is an indication that FFmpeg has not been initialized or it has not
			// been compiled/initialized with the correct set of codecs.
		//	RTC_LOG(LS_ERROR) << "";
			//RTC_LOG(LS_ERROR) << "FFmpeg Audio decoder not found.";
			 
			return false;
		}

		av_codec_parser_context_.reset( av_parser_init(codec->id));
		if (!av_codec_parser_context_) {
			//fprintf(stderr, "Parser not found\n");
			//exit(1);
			RTC_LOG(LS_ERROR) << "Parser not found";
			return false;
		}
		int res = avcodec_open2(av_context_.get(), codec, nullptr);
		if (res < 0) {
		//	RTC_LOG(LS_ERROR) << "avcodec_open2 error: " << res;
			//Release();
			//ReportError();
			return false;
		}
		audio_out_buffer_ = new uint8_t[kMaxAudioFrameSize * 2];// (uint8_t*)av_malloc();

		audio_swr_ctx_.reset(swr_alloc_set_opts(NULL,
			2  /*audio_codec_ctx->channel_layout*/,
			AV_SAMPLE_FMT_S16,
			av_context_->sample_rate,
			av_context_->channel_layout,
			av_context_->sample_fmt,
			av_context_->sample_rate,
			0, NULL));

		swr_init(audio_swr_ctx_.get());
		av_frame_.reset(av_frame_alloc());

		//int32_t  buffer_pool_size = 120;
		//if (!ffmpeg_buffer_pool_.Resize(buffer_pool_size) ||
		//	!output_buffer_pool_.Resize(buffer_pool_size)) {
		//	return false;
		//}

		return true;
		return false;
	}
	int32_t AudioDecoder::Decode(const rtc::Buffer & frame, bool, int64_t render_time_ms)
	{

		ScopedAVPacket packet = MakeScopedAVPacket();
		if (!packet) {
			//ReportError();
			return -1;
		}
		// packet.data has a non-const type, but isn't modified by
		// avcodec_send_packet.
		//packet->data = const_cast<uint8_t*>(frame.begin());
		//if (frame.size() >
		//	static_cast<size_t>(std::numeric_limits<int>::max())) {
		//	//ReportError();
		//	return -1;
		//}
		//packet->size = static_cast<int>(frame.size());
		int32_t ret = av_parser_parse2(av_codec_parser_context_.get(), av_context_.get(), &packet->data, &packet->size,
			frame.begin(), frame.size(),
			AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
		if (ret < 0) {
			fprintf(stderr, "Error while parsing\n");
			RTC_LOG(LS_ERROR) << "Error while parsing failed !@!!!!!";
			return -1;
		}
		av_context_->reordered_opaque = render_time_ms;

		int result = avcodec_send_packet(av_context_.get(), packet.get());

		if (result < 0) {
			char bufferf[120] = { 0 };
			std::string re = av_make_error_string(bufferf, sizeof(bufferf), result);
			RTC_LOG(LS_ERROR) << "avcodec_send_packet error: " << result << " , result " << re;
 
			//ReportError();
			return -1;
		}

		result = avcodec_receive_frame(av_context_.get(), av_frame_.get());
		if (result < 0) {
			RTC_LOG(LS_ERROR) << "avcodec_receive_frame error: " << result;
			//ReportError();
			return -1;
		}

		// We don't expect reordering. Decoded frame timestamp should match
		// the input one.
		RTC_DCHECK_EQ(av_frame_->reordered_opaque, render_time_ms);


		result = swr_convert(audio_swr_ctx_.get(),
			&audio_out_buffer_,
			kMaxAudioFrameSize * 2,
			(const uint8_t**)av_frame_->data,
			av_frame_->nb_samples);
		if (result <= 0)
		{
			RTC_LOG(LS_ERROR) << "swr_convert error: " << result;
			return -1;
		}

		int out_size = av_samples_get_buffer_size(0,
			av_get_channel_layout_nb_channels(av_context_->channel_layout), /*out_channels_*/
			result,
			AV_SAMPLE_FMT_S16,/*out_sample_rate_*/
			1);
		//	sleep_time_ = (out_sample_rate_ * 16 * 2 / 8) / out_size;
		//int32_t  data_size = av_get_bytes_per_sample(av_context_->sample_fmt);;
		if (callback_)
		{
			rtc::Buffer frame(audio_out_buffer_, out_size);
			callback_->AppAudioData(std::move(frame));
		}
#if 0
		static FILE * outfile = fopen("test.pcm", "wb+");
		if (outfile)
		{
			fwrite(av_frame_->data[0] ,1, data_size *av_frame_->nb_samples,  outfile);
			fflush(outfile);
		}
#endif // 


		//for (int i = 0; i < av_frame_->nb_samples; i++)
		//{
		//
		//	for (int ch = 0; ch < av_context_->channels; ch++)
		//	{
		//		fwrite(av_frame_->data[ch] + data_size * i, 1, data_size, outfile);
		//		fflush(outfile);
		//	}
		//}
		return int32_t();
	}
}