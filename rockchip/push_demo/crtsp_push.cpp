/***********************************************************************************************
created: 		2025-06-15

author:			chensong

purpose:		nv_cuda_ decoder
************************************************************************************************/
#include "crtsp_push.h"

namespace dsp
{



	static const std::time_t    g_connect_timeout = 30;

	static const char *  avError(int errNum) {
		static char buf[1024] = {0};
		av_strerror(errNum, buf, sizeof(buf));
		 
		return buf;
	}
	RawPacket::RawPacket(const unsigned char* data_, const size_t size, int64_t pts_, const bool containsKeyFrame_) :
		data(data_, data_ + size), pts(pts_), containsKeyFrame(containsKeyFrame_) {};

	bool crtsp_push::init(const std::string& url)
	{

		
		m_url = url;
		m_index_frame = 0;
		
		bool ret = _init();
		if (ret)
		{
			m_stoped = false;
			m_thread = std::thread(&crtsp_push::_work_pthread, this);
		}
		return ret;
		avformat_network_init();
		//int32_t ret = 0;
		ret = avformat_alloc_output_context2(&m_octx, NULL, "rtsp", url.c_str());
		if (ret < 0)
		{
			printf("[%s][%d]avformat_alloc_output_context2 [url = %s]failed %s\n", __FUNCTION__, __LINE__, url.c_str(),  avError(ret));
			return false;
			//return avError(ret);
		}
		m_octx->max_interleave_delta = 1000000;;

		const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
		if (!codec)
		{
			printf("[%s][%d]avcodec_find_encoder [url = %s]failed %s\n", __FUNCTION__, __LINE__, url.c_str(), avError(ret));
			return false;
		}
		 
		//avctx->width = width;
		//avctx->height = height;
		//avctx->time_base = (AVRational){ 1, 25 };
		//avctx->framerate = (AVRational){ 25, 1 };
		//avctx->sample_aspect_ratio = (AVRational){ 1, 1 };
		//avctx->pix_fmt = AV_PIX_FMT_VAAPI;
		//4000 * 1024;//
		 
		 

		AVRational rate;
		rate.num = 1;
		rate.den = 25;
		//m_codec_ctx_ptr->time_base = { 1, 25 };//rate;
		//m_codec_ctx_ptr->framerate = { 25, 1 };
		/* frames per second */
	//	m_codec_ctx_ptr->time_base = rate;// (AVRational) { 1, 25 };
	 

		AVStream* out_stream = avformat_new_stream(m_octx, codec);
		out_stream->codecpar->codec_tag = 0;
		// 分配编码器上下文
		AVCodecContext * m_codec_ctx_ptr = avcodec_alloc_context3(codec);
		if (!m_codec_ctx_ptr) {
			//LOG_ERROR("Failed to allocate the encoder context");
			//avformat_free_context(ofmt_ctx);
			printf("[%s][%d] avcodec_alloc_context3 \n", __FUNCTION__, __LINE__);
			return false;
			return false;
		}

		// 设置编码器上下文参数
		m_codec_ctx_ptr->height = 1920;
		m_codec_ctx_ptr->width = 1080;
		m_codec_ctx_ptr->pix_fmt = AV_PIX_FMT_YUV420P;
	 
		m_codec_ctx_ptr->time_base = rate;
		AVDictionary* options = NULL;
		//设置参数，设置为TCP推流， 默认UDP
		//AVDictionary* format_opts = NULL;
		//	av_dict_set(&format_opts, "stimeout", std::to_string(2 * 1000000).c_str(), 0);
		av_dict_set(&options, "rtsp_transport", "tcp", 0);
		av_dict_set(&options, "flush_packets", "0", 0);
		av_dict_set(&options, "rtsp_flags", "prefer_tcp", 0);

		av_dict_set(&options, "avoid_negative_ts", "1", 0);  // 禁止负时间戳的自动调整
		av_dict_set(&options, "fflags", "-genpts", 0);        // 禁用自动生成时间戳
	//	av_opt_set(&octx->priv_data, "rtsp_transport", "tcp", 0);
		/*
		AVIOContext **s, const char *url, int flags,
				   const AVIOInterruptCB *int_cb, AVDictionary **options*/
		if (!(m_octx->oformat->flags & AVFMT_NOFILE))
		{
			ret = avio_open(&m_octx->pb, url.c_str(), AVIO_FLAG_WRITE);
			if (ret < 0) {
				printf("[%s][%d] %s\n", __FUNCTION__, __LINE__, avError(ret));
				return false;
			}
		}
		ret = avcodec_parameters_from_context(m_octx->streams[0]->codecpar, m_codec_ctx_ptr);

		ret = avformat_write_header(m_octx, &options);
		if (ret < 0)
		{
			//return avError(ret);
			printf("[%s][%d] %s\n", __FUNCTION__, __LINE__, avError(ret));
			return false;
		}
		//m_octx->d
		//octx->de = 1000000;
		m_stoped = false;
		m_url = url;
		m_index_frame = 0;
		m_thread = std::thread(&crtsp_push::_work_pthread, this);
		return true;
	}
	void crtsp_push::push_frame(AVPacket* packet)
	{
		
		int64_t index_frame = ++m_index_frame;;
		{
			std::lock_guard<std::mutex> lock(m_lock);
		
			
			/*avPacket.pts = m_index_frame * (90000 / 25);
			avPacket.dts = index_frame * (90000 / 25);*/
			m_packet_queue.emplace_back(RawPacket(packet->data, packet->size, index_frame * (10000 / 25)));

		}
		{
			m_condition.notify_one();
		}
		/*int ret = av_interleaved_write_frame(m_octx, packet);
		if (ret != 0)
		{
			printf("[%s][%d]warr ret = %u\n", __FUNCTION__, __LINE__, ret);
		}*/
	}
	void crtsp_push::destroy()
	{
		if (m_octx)
		{
			if (m_octx->pb)
			{
				//if (avio_feof(m_octx->pb))
				avio_flush(m_octx->pb);
				avio_close(m_octx->pb);
				m_octx->pb = NULL;
			}
			avformat_flush(m_octx);
			::avformat_close_input(&m_octx);
			avformat_free_context(m_octx);
			m_octx = NULL;
		}
	}
	void crtsp_push::_handler_packet_item(const RawPacket* packet)
	{
		if (!m_packet)
		{
			m_packet = ::av_packet_alloc();
		}
		if (!m_packet)
		{
			printf("[%s][%d]warr   packet alloc =  \n", __FUNCTION__, __LINE__);
			return;
		}

		m_packet->data = (unsigned char *)packet->Data();
		m_packet->size = packet->Size();
		m_packet->pos = -1;
		m_packet->dts = packet->Pts();
		m_packet->pts = packet->Pts();
		m_packet->stream_index = 0;
		int ret = av_interleaved_write_frame(m_octx, m_packet);
		if (ret != 0)
		{
			printf("[%s][%d][%u]warr ret = %u\n", __FUNCTION__, __LINE__, packet->Pts(), ret);
		}
	}
	bool crtsp_push::_init()
	{
		int32_t ret = 0;
		ret = avformat_alloc_output_context2(&m_octx, NULL, "flv", m_url.c_str());
		if (ret < 0)
		{
			printf("[%s][%d]avformat_alloc_output_context2 [url = %s]failed %s\n", __FUNCTION__, __LINE__, m_url.c_str(), avError(ret));
			return false;
			//return avError(ret);
		}
		m_octx->max_interleave_delta = 1000000;;

		const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
		if (!codec)
		{
			printf("[%s][%d]avcodec_find_encoder [url = %s]failed %s\n", __FUNCTION__, __LINE__, m_url.c_str(), avError(ret));
			return false;
		}

		//avctx->width = width;
		//avctx->height = height;
		//avctx->time_base = (AVRational){ 1, 25 };
		//avctx->framerate = (AVRational){ 25, 1 };
		//avctx->sample_aspect_ratio = (AVRational){ 1, 1 };
		//avctx->pix_fmt = AV_PIX_FMT_VAAPI;
		//4000 * 1024;//



		AVRational rate;
		rate.num = 1;
		rate.den = 25;
		//m_codec_ctx_ptr->time_base = { 1, 25 };//rate;
		//m_codec_ctx_ptr->framerate = { 25, 1 };
		/* frames per second */
	//	m_codec_ctx_ptr->time_base = rate;// (AVRational) { 1, 25 };


		AVStream* out_stream = avformat_new_stream(m_octx, codec);
		out_stream->codecpar->codec_tag = 5;
		// 分配编码器上下文
		AVCodecContext* m_codec_ctx_ptr = avcodec_alloc_context3(codec);
		if (!m_codec_ctx_ptr) {
			//LOG_ERROR("Failed to allocate the encoder context");
			//avformat_free_context(ofmt_ctx);
			printf("[%s][%d]avcodec_alloc_context3 [url = %s]failed %s\n", __FUNCTION__, __LINE__, m_url.c_str(), avError(ret));
			return false;
			return false;
		}

		// 设置编码器上下文参数
		m_codec_ctx_ptr->height = 1920;
		m_codec_ctx_ptr->width = 1080;
		m_codec_ctx_ptr->pix_fmt = AV_PIX_FMT_YUV420P;

		m_codec_ctx_ptr->time_base = rate;
		AVDictionary* options = NULL;
		//设置参数，设置为TCP推流， 默认UDP
		//AVDictionary* format_opts = NULL;
		//	av_dict_set(&format_opts, "stimeout", std::to_string(2 * 1000000).c_str(), 0);
		av_dict_set(&options, "rtsp_transport", "tcp", 0);
		av_dict_set(&options, "flush_packets", "0", 0);
		av_dict_set(&options, "rtsp_flags", "prefer_tcp", 0);

		av_dict_set(&options, "avoid_negative_ts", "1", 0);  // 禁止负时间戳的自动调整
		av_dict_set(&options, "fflags", "-genpts", 0);        // 禁用自动生成时间戳
	//	av_opt_set(&octx->priv_data, "rtsp_transport", "tcp", 0);
		/*
		AVIOContext **s, const char *url, int flags,
				   const AVIOInterruptCB *int_cb, AVDictionary **options*/
		if (!(m_octx->oformat->flags & AVFMT_NOFILE))
		{
			ret = avio_open(&m_octx->pb, m_url.c_str(), AVIO_FLAG_WRITE);
			if (ret < 0) {
				printf("[%s][%d]avio_open [url = %s]failed %s\n", __FUNCTION__, __LINE__, m_url.c_str(), avError(ret));
				return false;
			}
		}
		ret = avcodec_parameters_from_context(m_octx->streams[0]->codecpar, m_codec_ctx_ptr);

		ret = avformat_write_header(m_octx, NULL);
		if (ret < 0)
		{
			//return avError(ret);
			printf("[%s][%d]avformat_write_header [url = %s]failed %s\n", __FUNCTION__, __LINE__, m_url.c_str(), avError(ret));
			return false;
		}

		//m_index_frame.store(0);
		return true;
	}
	void crtsp_push::_work_pthread()
	{
		std::time_t start_time = std::time(NULL);
		RawPacket* packet_ptr;
		bool init_ = true;
		while (!m_stoped)
		{
			auto timeout_time = std::chrono::system_clock::now() + std::chrono::seconds(5);
			{
				std::unique_lock<std::mutex> lock(m_lock);
				m_condition.wait_until(lock, timeout_time,[this]() { return m_packet_queue.size() > 0 || m_stoped; });
			}

			if (std::time(NULL) - start_time > g_connect_timeout)
			{

				destroy();
			//	init(m_url);
				init_ = false;
				start_time = std::time(NULL);
			}

			while (!m_packet_queue.empty() && !m_stoped) {
				{
					std::lock_guard<std::mutex> lock{ m_lock };
					packet_ptr = &m_packet_queue.front();
					// m_packet_queue.pop_front();
				}

				if (!packet_ptr) {
					continue;
				}
				if (!init_)
				{
					init_ = _init();
					 
				 }
				if (init_)
				{
					_handler_packet_item((packet_ptr));
				}
				else
				{
					printf("[%s][%d] init rtsp !!!\n", __FUNCTION__, __LINE__);
				}
				 
				{
					std::lock_guard<std::mutex> lock{ m_lock };
					//packet_ptr = &m_packet_queue.front();
					m_packet_queue.pop_front();
				}
				start_time = std::time(NULL);
			}
			

		}
	}
}