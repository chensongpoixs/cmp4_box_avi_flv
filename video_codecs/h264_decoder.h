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
				   date:  2025-10-06



 ******************************************************************************/



#ifndef _C_H264_DECODER___H_
#define _C_H264_DECODER___H_

#include <stddef.h>
#include <stdint.h>

#include <utility>

#include "libmedia_codec/channel_layout.h"
#include "libmedia_transfer_protocol/rtp_packet_infos.h"
#include "rtc_base/constructor_magic.h"
#include "libmedia_codec/video_codecs/video_frame_buffer_pool.h"
#include "libmedia_codec/encoded_image.h"
#include "libmedia_codec/video_codec_type.h"
extern "C" {
//#include "lib"
//#include "libavcodec/avcodec.h"
//#include "libavutil/imgutils.h"

#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
}  // extern "C"


#include "libcross_platform_collection_render/video_render/cvideo_render_factory.h"
#include "libcross_platform_collection_render/video_render/cvideo_render.h"
#include "libcross_platform_collection_render/track_capture/ctrack_capture.h"
namespace libmedia_codec {

	struct AVCodecContextDeleter {
		void operator()(AVCodecContext* ptr) const { avcodec_free_context(&ptr); }
	};
	struct AVFrameDeleter {
		void operator()(AVFrame* ptr) const { av_frame_free(&ptr); }
	};

	
	class H264Decoder
	{
	public:
	public:
		explicit H264Decoder();
		virtual ~H264Decoder();
	public:
		bool Configure(libmedia_codec::VideoCodecType  codec_type, int32_t width, int32_t height/*const Settings& settings*/)  ;
		int32_t Release()  ;

		int32_t RegisterDecodeCompleteCallback(
			libcross_platform_collection_render::cvideo_renderer * callback) {
			callback_ = callback;
			return 0;
		 }

		// `missing_frames`, `fragmentation` and `render_time_ms` are ignored.
		int32_t Decode(const libmedia_codec::EncodedImage& input_image,
			bool /*missing_frames*/,
			int64_t render_time_ms = -1)  ;

		const char* ImplementationName() const  ;
	private:
		// Called by FFmpeg when it needs a frame buffer to store decoded frames in.
		// The `VideoFrame` returned by FFmpeg at `Decode` originate from here. Their
		// buffers are reference counted and freed by FFmpeg using `AVFreeBuffer2`.
		static int AVGetBuffer2(AVCodecContext* context,
			AVFrame* av_frame,
			int flags);
		// Called by FFmpeg when it is done with a video frame, see `AVGetBuffer2`.
		static void AVFreeBuffer2(void* opaque, uint8_t* data);

		bool IsInitialized() const;

		// Reports statistics with histograms.
		void ReportInit();
		void ReportError();
	private:

		// Used by ffmpeg via `AVGetBuffer2()` to allocate I420 images.
		VideoFrameBufferPool ffmpeg_buffer_pool_;
		// Used to allocate NV12 images if NV12 output is preferred.
		VideoFrameBufferPool output_buffer_pool_;
		std::unique_ptr<AVCodecContext, AVCodecContextDeleter> av_context_;
		std::unique_ptr<AVFrame, AVFrameDeleter> av_frame_;
		bool has_reported_init_;
		libcross_platform_collection_render::cvideo_renderer * callback_ = nullptr;
		rtc::scoped_refptr<I420Buffer> frame_buffer_ = nullptr;
	};
}


#endif // _C_H264_DECODER___H_