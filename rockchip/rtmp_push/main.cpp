#include <iostream>
extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/mathematics.h"
#include "libavutil/time.h"
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
#include <libavcodec/bsf.h>
}
#include "crtmp_host_pusher.h"

int avError(int errNum) {
	char buf[1024];
	av_strerror(errNum, buf, sizeof(buf));
	std::cout << "Error: " << buf << std::endl;
	return -1;
}

int main(int argc, char* argv[]) {
	//const char* fileAddress = "../input.mp4";
	// rtsp://admin:Ymksc1216@192.168.31.245:554/ch1/main/av_stream
	//const char* fileAddress = "rtsp://admin:hik12345@192.168.1.29:554/H264/ch1/main/av_stream";// D: / deps / ffmpeg - release - full - shared / ffmpeg - 7.1.1 - full_build - shared / bin / test.h264";
	const char* fileAddress = "rtsp://admin:hik12345@192.168.1.7:554/H264/ch1/main/av_stream";
	//const char* fileAddress = "rtmp://192.168.31.16:1935/live/13";
	const char* rtmpAddress = "rtmp://127.0.0.1:1935/live/stream";


	dsp::crtmp_host_pusher  pusher;
	if (!pusher.init(rtmpAddress))
	{
		printf("pusher init failed !!\n");
	}
	
	//av_register_all();
	avformat_network_init();
	AVDictionary* options = NULL;
	//	//设置参数，设置为TCP推流， 默认UDP
	//	//AVDictionary* format_opts = NULL;
	//	//	av_dict_set(&format_opts, "stimeout", std::to_string(2 * 1000000).c_str(), 0);
	av_dict_set(&options, "rtsp_transport", "tcp", 0);
	//	av_dict_set(&options, "flush_packets", "0", 0);
	av_dict_set(&options, "rtsp_flags", "prefer_tcp", 0);
	AVFormatContext* ictx = NULL;
	int ret = avformat_open_input(&ictx, fileAddress, 0, &options);
	if (ret < 0)
	{
		return avError(ret);
	}

	ret = avformat_find_stream_info(ictx, &options);
	if (ret < 0) return avError(ret);
	
	AVPacket avPacket;
	long long startTime = av_gettime();
	AVRational otime;
	otime.num = 1;
	otime.den = 25;
	int64_t index_frame = 0;
	std::time_t  start_time = std::time(NULL);
	bool send = true;
	const AVBitStreamFilter* bsf = av_bsf_get_by_name("h264_mp4toannexb");
	AVBSFContext* bsf_ctx = nullptr;
	av_bsf_alloc(bsf, &bsf_ctx);
	av_bsf_init(bsf_ctx);
	while (true) {
		ret = av_read_frame(ictx, &avPacket);
		if (ret < 0)
		{
			break;
		}
	 

		if (avPacket.stream_index == AVMEDIA_TYPE_VIDEO) {
			/*AVRational tb = ictx->streams[avPacket.stream_index]->time_base;
			long long now = av_gettime() - startTime;
			long long dts = avPacket.dts * (1000 * 1000 * av_q2d(tb));
			if (dts > now) av_usleep(dts - now);*/
			printf("sendvideo \n");
			if (av_bsf_send_packet(bsf_ctx, &avPacket) == 0) 
			{
				// 接收过滤后的 packet
				while (av_bsf_receive_packet(bsf_ctx, &avPacket) == 0)
				{
					// 此时 packet.data 包含了带有起始码的 H.264 数据
					// 写入其他帧数据（带有起始码的 NAL 单元）
					pusher.OnH264Data(avPacket.data, avPacket.size, avPacket.pts / 9);
					//pusher.push_frame(&avPacket);
					//avPacket.data
					av_packet_unref(&avPacket);

					//if (avPacket) av_packet_unref(avPacket);//释放packet内存
				}
				
				
			}
		}
		else if (avPacket.stream_index == AVMEDIA_TYPE_AUDIO)
		{
			printf("send audio \n");
			pusher.OnAACData(avPacket.data, avPacket.size, avPacket.pts/9);
			av_packet_unref(&avPacket);
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