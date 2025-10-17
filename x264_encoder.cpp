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
#include "libmedia_codec/x264_encoder.h"
#include <string>
#include "rtc_base/platform_thread_types.h"
#include "rtc_base/logging.h"
#include "libmedia_codec/video_codec_type.h"
#include "libmedia_codec/video_frame.h"
#include "libmedia_codec/video_frame_type.h"
//#include "rtc_base/platform_thread.h"

namespace libmedia_codec
{

	namespace {
		static void LogX264(void*, int level, const char* format, va_list args) {
			char buf[1024];
			va_list args2;
			va_copy(args2, args);
			int len = vsnprintf(buf, sizeof(buf), format, args2);
			if (len > 0) {
				buf[len - 1] = 0;
			}
			va_end(args2);

			//RTC_LOG(LS_INFO) << "x264 log, level: " << level << ", msg: " << buf;
		}
	}
	X264Encoder::X264Encoder()
		: last_bitrate_(1000)
	{}
	X264Encoder::~X264Encoder() {}
	 
	bool X264Encoder::Start() 
	{
		//using namespace rtc;
		//using namespace webrtc;
		RTC_LOG_F(LS_INFO) << "X264Encoder Start";

		if (running_) {
			RTC_LOG_F( LS_WARNING) << "X264Encoder already running";
			return true;
		}

		running_ = true;

		encode_thread_ = new std::thread([=]() {
			rtc::SetCurrentThreadName("x264_encode_thread");
			RTC_LOG_F( LS_INFO) << "X264Encoder encode thread running";

			// 初始化编码器
			if (!InitEncoder()) {
				RTC_LOG_F( LS_WARNING) << "x264 init failed";
				ReleaseEncoder();
				return;
			}

			int frame_queue_size = 0;
			int32_t  prev_bitrate = last_bitrate_;
			while (running_) {
				std::shared_ptr< libmedia_codec::VideoFrame> frame;
			
				{
					std::unique_lock<std::mutex> auto_lock(frame_queue_mtx_);
					frame_queue_size = frame_queue_.size();
					if (frame_queue_size > 0) {
						frame = frame_queue_.front();
						frame_queue_.pop();
					}

					if (!frame) {
						cond_var_.wait(auto_lock);
						continue;
					}
				}
#if 0
				if (last_bitrate_ != prev_bitrate)
				{
					prev_bitrate = last_bitrate_;
				//	x264_param_->rc.i_rc_method = X264_RC_ABR;
			 
					x264_param_->rc.i_bitrate = prev_bitrate * 0.8 ;
					x264_param_->rc.i_vbv_max_bitrate = x264_param_->rc.i_bitrate;
					x264_param_->rc.i_vbv_buffer_size = x264_param_->rc.i_bitrate;
			
					//重新配置x264参数数
					x264_encoder_reconfig(x264_, x264_param_);
				}
#endif // 
				// 判断图像的宽高是否发生了变化，如果发生了变化，需要重新初始化编码器
				if (encoder_param_.width != frame->width() ||
					encoder_param_.height != frame->height())
				{
					encoder_param_.width = frame->width();
					encoder_param_.height = frame->height();
					ReleaseEncoder();
					if (!InitEncoder()) {
						ReleaseEncoder();
						return;
					}
				}

				// 拷贝图像数据
				//for (int i = 0; i < 3; ++i) {
				//	memcpy(x264_picture_->img.plane[i], frame->data[i], frame->data_len[i]);
				//}
				rtc::scoped_refptr <libmedia_codec::I420BufferInterface> i420_buffer =  frame->video_frame_buffer  ()->ToI420();
				memcpy(x264_picture_->img.plane[0], i420_buffer->DataY(), frame->width() * frame->height());
				memcpy(x264_picture_->img.plane[1], i420_buffer->DataU(), i420_buffer->StrideU() * (frame->height() + 1)/2);
				memcpy(x264_picture_->img.plane[2], i420_buffer->DataV(), i420_buffer->StrideU() * (frame->height() + 1) / 2);
#if 0
				static FILE* out_pic_ptr = fopen("pic.yuv", "wb+");
				if (out_pic_ptr)
				{
					fwrite( i420_buffer->DataY(),1, frame->width() * frame->height(), out_pic_ptr);
					fwrite( i420_buffer->DataU(),1, i420_buffer->StrideU() * (frame->height() + 1) / 2, out_pic_ptr);
					fwrite( i420_buffer->DataV(),1, i420_buffer->StrideU() * (frame->height() + 1) / 2, out_pic_ptr);
					fflush(out_pic_ptr);

				}
#endif // 
				// 编码
				std::shared_ptr<libmedia_codec::EncodedImage> out_frame;
				if (!Encode(frame, out_frame)) {
					continue;
				}
				if (encode_image_obser_ &&out_frame)
				{
					encode_image_obser_->SendVideoEncode(out_frame);
				} 
			}

			ReleaseEncoder();
		});

		return true;
	} 
	void X264Encoder::Stop() {
	
		RTC_LOG(LS_INFO) << "X264Encoder Stop";

		if (!running_) {
			return;
		}

		running_ = false;
		cond_var_.notify_all();

		if (encode_thread_ && encode_thread_->joinable()) {
			encode_thread_->join();
			RTC_LOG(LS_INFO) << "X264Encoder encode_thread join success";
			delete encode_thread_;
			encode_thread_ = nullptr;
		}
	}
	void X264Encoder::OnNewMediaFrame(std::shared_ptr<libmedia_codec::VideoFrame> frame) {
		std::unique_lock<std::mutex> auto_lock(frame_queue_mtx_);
		frame_queue_.push(frame);
		cond_var_.notify_one();
	}

	void X264Encoder::SetSendFrame(EncodeImageObser * encode_image_obser)
	{
		encode_image_obser_ = encode_image_obser;
	}
	void X264Encoder::SetBitrate(int32_t  bitrate)
	{
		
		last_bitrate_ = bitrate;
	}
	 
	bool X264Encoder::InitEncoder() {
		x264_param_ = new x264_param_t();
		memset(x264_param_, 0, sizeof(x264_param_t));
		// 设置速率和场景
		if (x264_param_default_preset(x264_param_, encoder_param_.preset.c_str(),
			encoder_param_.tune.c_str()))
		{
			return false;
		}

		// 设置图像的宽高
		x264_param_->i_width = encoder_param_.width;
		x264_param_->i_height = encoder_param_.height;
		// 设置帧率
		x264_param_->i_fps_num = encoder_param_.fps;
		x264_param_->i_fps_den = 1;
		// 设置GOP
		if (encoder_param_.gop > 0) {
			x264_param_->i_keyint_max = encoder_param_.gop;
		}
		// 需要图像的格式
		x264_param_->i_csp = X264_CSP_I420;
		// 不使用B帧, B帧会增大延迟
		x264_param_->i_bframe = 0;
		// 设置单Slice
		x264_param_->i_slice_count = 1;
		// 使用单线程
		x264_param_->i_threads = 1;
		// 每个I帧前面都携带SPS PPS
		x264_param_->b_repeat_headers = 1;

		// 设置码率控制参数
		if ("ABR" == encoder_param_.rate_control) {
			x264_param_->rc.i_rc_method = X264_RC_ABR;
			x264_param_->rc.f_rf_constant = 0.0;
			x264_param_->rc.i_bitrate = encoder_param_.bitrate;
		}
		else {
			x264_param_->rc.i_rc_method = X264_RC_CRF;
			x264_param_->rc.f_rf_constant = (float)encoder_param_.cf;
			x264_param_->rc.i_bitrate = 0;
		}

		// 设置最大码率
		if (encoder_param_.max_bitrate > 0) {
			x264_param_->rc.i_vbv_max_bitrate = encoder_param_.max_bitrate;
		}

		// 设置码率控制缓冲区
		if (encoder_param_.buffer_size > 0) {
			x264_param_->rc.i_vbv_buffer_size = encoder_param_.buffer_size;
		}

		// 根据fps来计算两帧间隔，如果是1，表示使用时间戳来计算间隔
		x264_param_->b_vfr_input = 0;

		// 设置log参数
		x264_param_->pf_log = LogX264;
		x264_param_->p_log_private = nullptr;
		x264_param_->i_log_level = X264_LOG_DEBUG;

		// 设置profile
		if (x264_param_apply_profile(x264_param_, encoder_param_.profile.c_str())) {
			return false;
		}

		// 打开编码器
		x264_ = x264_encoder_open(x264_param_);
		if (!x264_) {
			RTC_LOG(LS_WARNING) << "x264_encoder_open failed";
			return false;
		}

		// 创建pictrue
		x264_picture_ = new x264_picture_t();
		memset(x264_picture_, 0, sizeof(x264_picture_t));
		// 给picture分配空间
		int res = x264_picture_alloc(x264_picture_, x264_param_->i_csp,
			x264_param_->i_width, x264_param_->i_height);
		if (res < 0) {
			RTC_LOG(LS_WARNING) << "x264_picture_alloc failed";
			return false;
		}

		// 设置编码帧的类型
		x264_picture_->i_type = X264_TYPE_AUTO;
		last_bitrate_ = encoder_param_.bitrate;
		return true;
	}
	void X264Encoder::ReleaseEncoder() {
		if (x264_param_) {
			delete x264_param_;
			x264_param_ = nullptr;
		}

		if (x264_picture_) {
			x264_picture_clean(x264_picture_);
			delete x264_picture_;
			x264_picture_ = nullptr;
		}

		if (x264_) {
			x264_encoder_close(x264_);
			x264_ = nullptr;
		}
	}
	bool X264Encoder::Encode(std::shared_ptr< libmedia_codec::VideoFrame> frame,
		std::shared_ptr<libmedia_codec::EncodedImage>& out_frame) {
	
		// 设置时间戳
		x264_picture_->i_pts = frame->timestamp();

		int nal_num;
		x264_nal_t* nal_out;
		x264_picture_t pic_out;
		int size = x264_encoder_encode(x264_, &nal_out, &nal_num, x264_picture_, &pic_out);
		if (size < 0) {
			RTC_LOG(LS_WARNING) << "x264_encoder_encode failed: " << size;
			return false;
		}

		if (size == 0) {
			return true;
		}

		int data_size = 0;
		std::vector<x264_nal_t> nals;
		bool idr = false;
		int sps_length = 0;
		char sps[1024*8] = { 0 };
		int pps_length = 0;
		char pps[1024*8] = { 0 };

		for (int i = 0; i < nal_num; ++i) {
			x264_nal_t& nal = nal_out[i];
			int nal_type = nal.i_type;
			// 如果NALU是I帧或者P帧
			if (nal_type == NAL_SLICE_IDR || nal_type == NAL_SLICE) {
				nals.push_back(nal);
				data_size += nal.i_payload;

				if (nal_type == NAL_SLICE_IDR) {
					idr = true;
				}

			}
			else if (nal_type == NAL_SEI) {
				continue;
			}
			else if (nal_type == NAL_SPS) {
				sps_length = nal.i_payload;
				memcpy(sps, nal.p_payload, sps_length);
			}
			else if (nal_type == NAL_PPS) {
				pps_length = nal.i_payload;
				memcpy(pps, nal.p_payload, pps_length);
			}
		}

		if (idr) {
			data_size += (sps_length + pps_length);
		}
		
		out_frame = std::make_shared<libmedia_codec::EncodedImage>();
		rtc::scoped_refptr<libmedia_codec::EncodedImageBufferInterface> encoder_image=  
			libmedia_codec::EncodedImageBuffer::Create(data_size);
		//RTC_LOG_F(LS_INFO) << "encoder frame size = " << data_size;
		//out_frame->fmt.media_type = MainMediaType::kMainTypeVideo;
		//out_frame->fmt.sub_fmt.video_fmt.type = SubMediaType::kSubTypeH264;
		//out_frame->fmt.sub_fmt.video_fmt.idr = idr;
		//out_frame->ts = pic_out.i_pts;
		//out_frame->data_len[0] = data_size;
		//out_frame->capture_time_ms = frame->capture_time_ms;
		out_frame->SetTimestamp(frame->timestamp());
		out_frame->SetEncodedData(encoder_image);
		out_frame->SetEncodeTime(frame->timestamp(), pic_out.i_dts);
		out_frame->_frameType = idr?  libmedia_codec::VideoFrameType:: kVideoFrameKey : libmedia_codec::VideoFrameType::kVideoFrameDelta;
		int data_index = 0;
		for (size_t i = 0; i < nals.size(); ++i) {
			x264_nal_t& nal = nals[i];
			if (nal.i_type == NAL_SLICE_IDR) {
				// 首先拷贝SPS PPS数据
				memcpy(encoder_image->data() + data_index, sps, sps_length);
				data_index += sps_length;
				memcpy(encoder_image->data() + data_index, pps, pps_length);
				data_index += pps_length;
				// 拷贝I帧数据
				memcpy(encoder_image->data() + data_index, nal.p_payload, nal.i_payload);
				data_index += nal.i_payload;
			}
			else {
				// 拷贝P帧数据
				memcpy(encoder_image->data() + data_index, nal.p_payload, nal.i_payload);
				data_index += nal.i_payload;
			}
		}
#if 0

		static FILE *out_file_ptr = fopen("test.h264", "wb+");
		if (out_file_ptr)
		{
			fwrite(encoder_image->data(), 1, encoder_image->size(), out_file_ptr);
			fflush(out_file_ptr);
		}
#endif ///

		return true;
	}

}