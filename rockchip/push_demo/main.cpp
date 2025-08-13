#include <iostream>
extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/mathematics.h"
#include "libavutil/time.h"
}
#include "crtsp_push.h"

int avError(int errNum) {
	char buf[1024];
	av_strerror(errNum, buf, sizeof(buf));
	std::cout << "Error: " << buf << std::endl;
	return -1;
}

int main(int argc, char* argv[]) {
	//const char* fileAddress = "../input.mp4";
	// rtsp://admin:Ymksc1216@192.168.31.245:554/ch1/main/av_stream
	const char* fileAddress = "rtsp://admin:hik12345@192.168.1.29:554/H264/ch1/main/av_stream";// D: / deps / ffmpeg - release - full - shared / ffmpeg - 7.1.1 - full_build - shared / bin / test.h264";
	const char* rtmpAddress = "rtmp://127.0.0.1:1935/live/stream";



	dsp::crtsp_push  pusher;
	if (!pusher.init(rtmpAddress))
	{
		printf("pusher init failed !!\n");
	}
	//av_register_all();
	avformat_network_init();

	AVFormatContext* ictx = NULL;
	int ret = avformat_open_input(&ictx, fileAddress, 0, NULL);
	if (ret < 0)
	{
		return avError(ret);
	}

	ret = avformat_find_stream_info(ictx, 0);
	if (ret < 0) return avError(ret);

	//AVFormatContext* octx = NULL;
	/*ret = avformat_alloc_output_context2(&octx, NULL, "rtsp", rtmpAddress);
	if (ret < 0) 
	{
		return avError(ret);
	}*/
	//使用tcp协议传输
	//(octx->priv_data, "rtsp_transport", "tcp", 0);
	//使用tcp协议传输
	//av_opt_set(octx->priv_data, "rtsp_transport", "tcp", 0);
	//检查所有流是否都有数据，如果没有数据会等待max_interleave_delta微秒
//	octx->max_interleave_delta = 1000000;
//	for (int i = 0; i < ictx->nb_streams; i++) 
//	{
//		AVStream* in_stream = ictx->streams[i];
//		AVStream* out_stream = avformat_new_stream(octx, NULL);
//		if (!out_stream) {
//			/*blog(LOG_ERROR, "media_remux: Failed to allocate output"
//				" stream");*/
//			return false;
//		}
//
//		ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
//
//		if (ret < 0) {
//			//blog(LOG_ERROR, "media_remux: Failed to copy parameters");
//			return false;
//		}
//
//		av_dict_copy(&out_stream->metadata, in_stream->metadata, 0);
//		//AVStream* outStream = avformat_new_stream(octx, avcodec_find_decoder(ictx->streams[i]->codecpar->codec_id));
//		////AVStream* outStream = avformat_new_stream(octx, ictx->streams[i]->codec->codec);
//		//if (!outStream)
//		//{
//		//	return avError(0);
//		//}
//		//ret = avcodec_parameters_copy(outStream->codecpar, ictx->streams[i]->codecpar);
//		//if (ret < 0) 
//		//{
//		//	return avError(ret);
//		//}
//		//outStream->
//		//outStream->codec->codec_tag = 0;
//		out_stream->codecpar->codec_tag = 0;
//	}
//	// 强制使用TCP传输
//	  	AVDictionary* options = NULL;
//	//设置参数，设置为TCP推流， 默认UDP
//	//AVDictionary* format_opts = NULL;
//	//	av_dict_set(&format_opts, "stimeout", std::to_string(2 * 1000000).c_str(), 0);
//	av_dict_set(&options, "rtsp_transport", "tcp", 0);
//	av_dict_set(&options, "flush_packets", "0", 0);
//	av_dict_set(&options, "rtsp_flags", "prefer_tcp", 0);
//
//	av_dict_set(&options, "avoid_negative_ts", "1", 0);  // 禁止负时间戳的自动调整
//	av_dict_set(&options, "fflags", "-genpts", 0);        // 禁用自动生成时间戳
////	av_opt_set(&octx->priv_data, "rtsp_transport", "tcp", 0);
//	/*
//	AVIOContext **s, const char *url, int flags,
//               const AVIOInterruptCB *int_cb, AVDictionary **options*/
//	if (!(octx->oformat->flags & AVFMT_NOFILE)) 
//		{
//			ret = avio_open(&octx->pb, rtmpAddress, AVIO_FLAG_WRITE);
//			if (ret < 0) {
//				return avError(ret);
//			}
//		}
//	
//	 
//	ret = avformat_write_header(octx, &options);
//	if (ret < 0) 
//	{
//		return avError(ret);
//	}

	AVPacket avPacket;
	long long startTime = av_gettime();
	AVRational otime;
	otime.num = 1;
	otime.den = 25;
	int64_t index_frame = 0;
	std::time_t  start_time = std::time(NULL);
	bool send = true;
	while (true) {
		ret = av_read_frame(ictx, &avPacket);
		if (ret < 0)
		{
			break;
		}
		/*++index_frame;
		avPacket.pts = index_frame * (90000 / 25);
		avPacket.dts = index_frame * (90000 / 25);*/
		//avPacket.data = 
		//AVRational itime = ictx->streams[avPacket.stream_index]->time_base;
		///AVRational otime = octx->streams[avPacket.stream_index]->time_base;
		/*avPacket.pts = av_rescale_q_rnd(avPacket.pts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_NEAR_INF));
		avPacket.dts = av_rescale_q_rnd(avPacket.dts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_NEAR_INF));
		avPacket.duration = av_rescale_q_rnd(avPacket.duration, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_NEAR_INF));
		avPacket.pos = -1;*/

		if (avPacket.stream_index == AVMEDIA_TYPE_VIDEO) {
			/*AVRational tb = ictx->streams[avPacket.stream_index]->time_base;
			long long now = av_gettime() - startTime;
			long long dts = avPacket.dts * (1000 * 1000 * av_q2d(tb));
			if (dts > now) av_usleep(dts - now);*/
			 
				printf("send\n");
				pusher.push_frame(&avPacket);
				//avPacket.data
			 
		}
		else
		{
			av_packet_unref(&avPacket);
		}

		//
	}

	avformat_close_input(&ictx);
	//avio_close(octx->pb);
	//avformat_free_context(octx);

	return 0;
}