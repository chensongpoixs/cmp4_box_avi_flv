#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#include <cstdio>
#include <cstdlib>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/error.h>
#include <libavutil/frame.h>
#include <libavutil/log.h>#include <libavcodec/codec.h>

#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
}

#include <libyuv.h>
AVFormatContext * open_dev()
{
	int ret = 0;
	char errors[1024] = {0};

	AVFormatContext *fmt_ctx = NULL;
	AVDictionary * options = NULL;

	/// [video device]:[audio device]
	char *devicename = "desktop";

	avdevice_register_all();


	AVInputFormat * iformat = av_find_input_format("gdigrab");


	av_dict_set(&options, "video_size", "1920X1080", 0);
	av_dict_set(&options, "framerate", "30", 0);
	av_dict_set(&options, "pixel_formt", "rgb", 0);
	//AV_PIX_FMT_BGR24

	if ((ret = avformat_open_input(&fmt_ctx, devicename, iformat, &options)) < 0)
	{
		av_strerror(ret, errors, 1024);
		printf("Failed to open video device [%d]%s\n", ret, errors);
		return NULL;
	}
	return fmt_ctx;
}
int main(int argc, char *argv[])
{

	int ret = 0;

	AVFormatContext *fmt_ctx = NULL;

	AVPacket pkt;

	av_log_set_level(AV_LOG_DEBUG);


	FILE *out_file_ptr = ::fopen("video.yuv", "wb+");


	fmt_ctx = open_dev();
	//将位图数据argb转换为yuv I420 转换后的数据分别保存在 ybuffer、ubuffer和vbuffer里面
	//Test.argbtoi420(byteArray, w * 4, ybuffer, w, ubuffer, (w + 1) / 2, vbuffer, (w + 1) / 2, w, h);
	int32_t width = 1920;
	int32_t height = 1080;








	unsigned char * yuv_ptr = static_cast<unsigned char *>(std::malloc(sizeof(unsigned char) *(1920 * 1080 *2)));
	while ((ret = av_read_frame(fmt_ctx, &pkt)) == 0)
	{
		av_log(NULL, AV_LOG_INFO, "packet size is %d(%p)\n", pkt.size, pkt.data);


		/*libyuv::ConvertToI420(pkt.data, 0, yuv_ptr, width, yuv_ptr + (width *height),
		(width + 1) / 2, yuv_ptr + (width *height ) + ((width >> 1) *(height >> 1)),
		(width + 1) / 2, 0, 0, 1920, 1080, 1920,
		1080, libyuv::kRotate0, libyuv::FOURCC_ARGB);*/

		// width * height * 4; // rgba  -> a -> 透明度 



		//AV_PIX_FMT_RGBA
		fwrite(pkt.data, 1, 1920 * 1080  * 3, out_file_ptr);
		::fflush(out_file_ptr);
		av_packet_unref(&pkt);
	}

	fclose(out_file_ptr);
	return EXIT_SUCCESS;
}