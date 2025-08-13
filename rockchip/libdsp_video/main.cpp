
#include <iostream>


#include "crockchip_decoder.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/mathematics.h"
#include "libavutil/time.h"

#include <libavutil/frame.h>
#include <libavutil/avutil.h>
	//#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/display.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
#include <libavcodec/bsf.h>

}

int main(int argc, char* argv[])
{
/*
[_handler_packet][505] decode_put_packet packet failed [ret = 4294966284]  !!!
decode_get_frame get info changed found
decoder require buffer w:h [1280:720] stride [1280:720] size 1843200
mpp[25630]: mpp_info: mpp version: ac16b2c63 author: Yanjun Liao  2025-07-31 fix[h264e_api_v2]: Fix bit_real calc in skip mode
mpp_create OK !!!
mpp[25630]: mpp_enc: MPP_ENC_SET_RC_CFG bps 500000 [500000 : 500000] fps [25:25] gop 250
mpp[25630]: mpp_enc: mode cbr bps [500000:500000:500000] fps fix [25/1] -> fix [25/1] gop i [250] v [0]
handler packet failed !!!
[_handler_packet][505] decode_put_packet packet failed [ret = 4294966284]  !!!
[_work_thread][462][decode get new frame] [width = 1280][height = 720][fmt = 0]
[_work_thread][462][decode get new frame] [width = 1280][height = 720][fmt = 0]
encoded frame 1    size 22510
handler packet failed !!!
[_handler_packet][505] decode_put_packet packet failed [ret = 4294966284]  !!!
encoded frame 2    size 2673
[_work_thread][462][decode get new frame] [width = 1280][height = 720][fmt = 0]
[_work_thread][462][decode get new frame] [width = 1280][height = 720][fmt = 0]
[_work_thread][462][decode get new frame] [width = 1280][height = 720][fmt = 0]
encoded frame 3    size 2814

/////////////////////////////
quire buffer w:h [1280:720] stride [1280:720] size 1843200
mpp[25086]: mpp_info: mpp version: 79806e631 author: Johnson Ding 2025-05-22 fix[avsd_plus]: Fix page fault when filtering field data
mpp_create OK !!!
mpp[25086]: mpp_enc: MPP_ENC_SET_RC_CFG bps 1000000 [1000000 : 1000000] fps [25:25] gop 250
mpp[25086]: mpp_enc: mode cbr bps [1000000:1000000:1000000] fps fix [25/1] -> fix [25/1] gop i [250] v [0]
mpp[25086]: mpp_buffer: check buffer found NULL pointer from get_packet

*/
//ffmpeg   -i "rtsp://admin:hik12345@192.168.1.4:554/H264/ch1/main/av_stream" -vcodec libx264 -preset ultrafast -f flv rtsp://127.0.0.1/live/test1
	const char* url = "rtsp://admin:hik12345@192.168.1.29:554/H264/ch1/main/av_stream";

	dsp::crockchip_decoder   docoder;
	avformat_network_init();

	docoder.init(MPP_VIDEO_CodingAVC);
	AVFormatContext* ictx = NULL;
	AVDictionary* options = NULL;
	//	//设置参数，设置为TCP推流， 默认UDP
	//	//AVDictionary* format_opts = NULL;
	//	//	av_dict_set(&format_opts, "stimeout", std::to_string(2 * 1000000).c_str(), 0);
	av_dict_set(&options, "rtsp_transport", "tcp", 0);
	//	av_dict_set(&options, "flush_packets", "0", 0);
	av_dict_set(&options, "rtsp_flags", "prefer_tcp", 0);
	int ret = avformat_open_input(&ictx, url, 0, &options);
	if (ret < 0)
	{
		printf("[%s][%d]\n", __FUNCTION__, __LINE__);
		return  -1;
		//return avError(ret);
	}

	ret = avformat_find_stream_info(ictx, &options);
	const AVBitStreamFilter* bsf = av_bsf_get_by_name("h264_mp4toannexb");
	AVBSFContext* bsf_ctx = nullptr;
	av_bsf_alloc(bsf, &bsf_ctx);
	av_bsf_init(bsf_ctx);
	AVPacket avPacket;
	while (true)
	{
		ret = av_read_frame(ictx, &avPacket);
		if (ret < 0)
		{
			break;
		}
		std::this_thread::sleep_for(std::chrono::second(500));
		if (avPacket.stream_index == AVMEDIA_TYPE_VIDEO) {
			/*AVRational tb = ictx->streams[avPacket.stream_index]->time_base;
			long long now = av_gettime() - startTime;
			long long dts = avPacket.dts * (1000 * 1000 * av_q2d(tb));
			if (dts > now) av_usleep(dts - now);*/

			printf("send\n");
			//docoder.push_packet(avPacket.data, avPacket.size, avPacket.pts, avPacket.dts);
			if (av_bsf_send_packet(bsf_ctx, &avPacket) == 0)
			{
				// 接收过滤后的 packet
				while (av_bsf_receive_packet(bsf_ctx, &avPacket) == 0)
				{
					// 此时 packet.data 包含了带有起始码的 H.264 数据
					// 写入其他帧数据（带有起始码的 NAL 单元）
					  docoder.push_packet(avPacket.data, avPacket.size, avPacket.pts, avPacket.dts);
					//pusher.OnH264Data(avPacket.data, avPacket.size, avPacket.pts / 9);
					//pusher.push_frame(&avPacket);
					//avPacket.data
					av_packet_unref(&avPacket);

					//if (avPacket) av_packet_unref(avPacket);//释放packet内存
				}


			}
		}
		else 
		{
			av_packet_unref(&avPacket);
		}

	}
	return 0;
}
