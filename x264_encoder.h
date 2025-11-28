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
				   date:  2025-09-26



 ******************************************************************************/



#ifndef _C_X264_ENCODER_H_
#define _C_X264_ENCODER_H_

#include <string>
#include "x264.h"
#include <mutex>
#include <queue>
#include <atomic>
#include <condition_variable>
#include <thread>
#include "i420_buffer.h"
#include "libmedia_codec/video_frame.h"
#include "libmedia_codec/encoded_image.h"
#include "rtc_base/third_party/sigslot/sigslot.h"
//#include <al>
namespace  libmedia_codec
{
	struct X264EncoderParam {
		// 编码速率
		std::string preset = "veryfast";
		// 编码场景
		std::string tune = "zerolatency";
		// profile
		std::string profile = "baseline";
		
		int32_t   level_idc = 31;
		// 码率控制，CQP、CRF、ABR
		std::string rate_control = "ABR";
		// 目标码率, 单位kbps
		int bitrate = 600 ;
		int max_bitrate = 1200 ;
		int buffer_size = 1200 ;
		int cf = 25;
		// 图像的宽高
		int width = 1920;
		int height = 1080;
		// 帧率
		int fps = 30;
		// GOP
		int gop = 60; // 2s

	};

 


	class X264Encoder 
		: public   sigslot::has_slots<> {
	public:
		X264Encoder();
		~X264Encoder()  ;
 
		bool Start()  ; 
		void Stop()  ;
		void OnNewMediaFrame(std::shared_ptr<libmedia_codec::VideoFrame>)  ;
		 
		 

		void SetBitrate(int32_t  bitrate);

	public:

		sigslot::signal1<std::shared_ptr<libmedia_codec::EncodedImage>  > SignalVideoEncodedImage;
	private:
		bool InitEncoder();
		void ReleaseEncoder();
		bool Encode(std::shared_ptr< libmedia_codec::VideoFrame > frame,
			std::shared_ptr<libmedia_codec::EncodedImage>& out_frame);

	private:
		 
		std::queue<std::shared_ptr< libmedia_codec::VideoFrame>> frame_queue_;
		std::mutex frame_queue_mtx_;
		std::atomic<bool> running_{ false };
		std::thread* encode_thread_ = nullptr;
		std::condition_variable cond_var_;

		X264EncoderParam encoder_param_;
		x264_param_t* x264_param_ = nullptr;
		x264_t* x264_ = nullptr;
		x264_picture_t* x264_picture_ = nullptr;

		//EncodeImageObser  *   encode_image_obser_;

		int32_t                last_bitrate_;
	};

}

#endif // _C_X264_ENCODER_H_