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



#ifndef _C_AUDIO_DECODER_H_
#define _C_AUDIO_DECODER_H_

#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include "libmedia_codec/audio_frame.h"
#include "libmedia_codec/audio_encoder.h"
#include "opus.h"
#include "libmedia_codec/video_codecs/h264_decoder.h"
 //class OpusEncoder;
extern "C" {
	//#include "lib"
	//#include "libavcodec/avcodec.h"
	//#include "libavutil/imgutils.h"
#include <libavutil/frame.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/ffversion.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
//#include "libpostproc/postprocess.h"
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
}  // extern "C"
#include "rtc_base/buffer.h"
#include "rtc_base/buffer.h"
#include "rtc_base/byte_buffer.h"
#include "libcross_platform_collection_render/audio_capture/audio_capture.h"

namespace libmedia_codec {

	//namespace rtc
	//{
	//	class Buffer;
	//}
	struct AVCodecParserContextDeleter {
		void operator()(AVCodecParserContext* ptr) const { av_parser_close(ptr); }
	};


	struct SwrContextDeleter {
		void operator()(SwrContext* ptr) const { swr_close(ptr); }
	};
	class AudioDecoder
	{
	public:
		AudioDecoder() = default;
		~AudioDecoder() {
			if (audio_out_buffer_)
			{
				delete[] audio_out_buffer_;
			}
		}

	public:
		bool Configure(/*libmedia_codec::VideoCodecType  codec_type, int32_t width, int32_t height*//*const Settings& settings*/);
		void RegisterDecodeCompleteCallback(libcross_platform_collection_render::AudioCapture * callback)
		{
			callback_ = callback;

			//h264_decoder_.RegisterDecodeCompleteCallback(callback);
		}
		int32_t Decode(const rtc::Buffer& data,
			bool /*missing_frames*/,
			int64_t render_time_ms = -1);
	public:
		// Used by ffmpeg via `AVGetBuffer2()` to allocate I420 images.
		//VideoFrameBufferPool ffmpeg_buffer_pool_;
		// Used to allocate NV12 images if NV12 output is preferred.
		//VideoFrameBufferPool output_buffer_pool_;
		std::unique_ptr<AVCodecContext, AVCodecContextDeleter> av_context_;
		std::unique_ptr<AVCodecParserContext, AVCodecParserContextDeleter>     av_codec_parser_context_;
		std::unique_ptr<AVFrame, AVFrameDeleter> av_frame_;
		std::unique_ptr< SwrContext, SwrContextDeleter> audio_swr_ctx_;
		libcross_platform_collection_render::AudioCapture                       *callback_;
		uint8_t *audio_out_buffer_;
		bool has_reported_init_;
	};

}


#endif // 