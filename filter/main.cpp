/***********************************************************************************************
created: 		2024-01-28

author:			chensong

purpose:		filter demo 

输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/

#define  _CRT_SECURE_NO_WARNINGS
#include <iostream>
 
extern "C"
{
#include <libavutil/frame.h>
#include <libavutil/avutil.h>
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/display.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavutil/error.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavutil/error.h>
#include <libavfilter/buffersink.h>
}

#ifdef _MSC_VER
#pragma comment(lib, "libavcodec.lib")
#pragma comment(lib, "libavdevice.lib")
#pragma comment(lib, "libavfilter.lib")
#pragma comment(lib, "libavformat.lib")
#pragma comment(lib, "libavutil.lib")
#pragma comment(lib, "libpostproc.lib")
#pragma comment(lib, "libswresample.lib")
#pragma comment(lib, "libswscale.lib")
#elif defined(__GNUC__)

#else 

#endif 




static AVFormatContext * g_fmt_ctx_ptr = NULL;

static AVCodecContext * g_codec_ctx_ptr = NULL;
static int32_t			g_video_stream_index = -1;


//////////// avfilter 

AVFilterGraph * g_graph = NULL;
AVFilterContext* g_buffer_ctx = NULL;
AVFilterContext * g_buffersink_ctx = NULL;



static char g_error_buffer[AV_ERROR_MAX_STRING_SIZE] = {0};

static const char * make_error_string(int32_t error_code)
{
	return ::av_make_error_string(g_error_buffer, AV_ERROR_MAX_STRING_SIZE, error_code);
}


static int open_input_file(const char * url)
{
	int32_t ret = -1;
	const AVCodec * codec = NULL;
	ret = ::avformat_open_input(&g_fmt_ctx_ptr, url, NULL, NULL);
	if (ret < 0)
	{
		printf("open url = [%s] (%s) failed !!!\n", url, make_error_string(ret));
		return ret;
	}

	ret = ::avformat_find_stream_info(g_fmt_ctx_ptr, NULL);
	if (ret < 0)
	{
		printf("find stream information  url = [%s] (%s)failed !!!\n", url, make_error_string(ret));
		return ret;
	}


	ret = ::av_find_best_stream(g_fmt_ctx_ptr, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);

	if (ret < 0)
	{
		printf("Can't find video stream url = [%s](%s) failed !!!\n", url, make_error_string(ret));
		return ret;
	}
	//::avformat_dump()
	g_video_stream_index = ret;
	//申请解码器
	g_codec_ctx_ptr = ::avcodec_alloc_context3(codec);
	if (!g_codec_ctx_ptr)
	{
		printf("alloc cocodec ctx failed !!!\n");
		return -1;

	}

	::avcodec_parameters_to_context(g_codec_ctx_ptr, g_fmt_ctx_ptr->streams[g_video_stream_index]->codecpar);


	//打开解码器

	ret = avcodec_open2(g_codec_ctx_ptr, codec, NULL);
	if (ret < 0)
	{
		printf("codec open decoder failed !!! url = [%s](%s)failed !!!\n", url, make_error_string(ret));
		return ret;
	}


	return 0;
}


/*
* 初始化过滤器 
*/
static int32_t init_filters(const char * filter_desc)
{
	int32_t ret = 0;

	
	const AVFilter * buffersrc = ::avfilter_get_by_name("buffer");
	const AVFilter*  buffersink = ::avfilter_get_by_name("buffersink");
	AVFilterInOut * inputs = ::avfilter_inout_alloc();
	AVFilterInOut*  outputs = ::avfilter_inout_alloc();
	if (!inputs)
	{
		printf("alloc inputs failed !!!\n");
		return -1;
	}
	if (!outputs)
	{
		printf("alloc outputs failed !!!\n");
		return -1;
	}
	
	char args[1024] = { 0 };
	AVRational time_base = g_fmt_ctx_ptr->streams[g_video_stream_index]->time_base;
	// video_size=wxh:pix_fmt=xx:time_base=xxx/xxxx:pixel_aspect=xxx/xx
	snprintf(args, 1024, "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
		g_codec_ctx_ptr->width, g_codec_ctx_ptr->height, g_codec_ctx_ptr->pix_fmt, 
		time_base.num, time_base.den, 
		g_codec_ctx_ptr->sample_aspect_ratio.num, g_codec_ctx_ptr->sample_aspect_ratio.den);
	
	//
	g_graph = ::avfilter_graph_alloc();

	if (!g_graph)
	{
		printf("alloc graph failed !!!\n");
		return -1;
	}

	//解析描述符 filter_desc 
	// "[0:v]crop=900:900:10:10,scale=1920:1080[v0];[1:v]crop=900:900:10:10,scale=1920:1080[v1];[v0][v1]hstack=2[out]"
	// [a][b]overlay=xxxx[out]
	// 输入 buffer filter 创建
	// 构造input 和output
	// args : 查看参数 命令 : ./ffmpeg -h filter=buffer
	ret = ::avfilter_graph_create_filter(&g_buffer_ctx, buffersrc, "in", args, NULL/*buffer 用户数据*/, g_graph);
	if (ret < 0)
	{
		printf("in -->avfilter graph create filter failed !!!");
		return ret;
	}
	


	// args 参数查看: ./ffmpeg -h filter=buffersink
	//buffersink AVOptions :
	//pix_fmts          <binary>     ..FV.......set the supported pixel formats
	enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE };

	// 输出buffer sink filter 创建
	ret = ::avfilter_graph_create_filter(&g_buffersink_ctx, buffersink, "out", NULL, NULL/*buffer 用户数据*/, g_graph);
	if (ret < 0)
	{
		printf("buffer sink filter graph create filter failed !!!");
		return -1;
	}
	ret = av_opt_set_int_list((void *)g_buffersink_ctx, "pix_fmts", pix_fmts, AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
	//比较奇怪
	// create out
	inputs->name = av_strdup("out");
	inputs->filter_ctx = g_buffersink_ctx;
	inputs->pad_idx = 0;
	inputs->next = NULL;
	// outpus 构造
	// create in
	outputs->name = av_strdup("in");
	outputs->filter_ctx = g_buffer_ctx;
	outputs->pad_idx = 0;
	outputs->next = NULL;


	//////////////////////////////////////////最重要步骤//////////////////////////////////////////////////////////////
	// create filter and add graph for filter desciption 
	ret = ::avfilter_graph_parse_ptr(g_graph, filter_desc, &inputs, &outputs, NULL );
	if (ret < 0)
	{
		printf("avfilter_graph_parse_ptr failed (%s) !!!\n", make_error_string(ret));
		return ret;
	}


	//graph 生效
	ret = ::avfilter_graph_config(g_graph, NULL);
	if (ret < 0)
	{
		printf("filter graph config failed !!!\n");
		return ret;
	}

	 ::avfilter_inout_free(&inputs);
	 ::avfilter_inout_free(&outputs);
	 inputs = NULL;
	 outputs = NULL;
	return 0;
}

static int32_t do_frame(AVFrame* filter_frame)
{

	static FILE * out_file_ptr = ::fopen("chensong.yuv", "wb+");
	if (out_file_ptr)
	{
		printf("width = %u, height = %u\n", filter_frame->width, filter_frame->height);
		::fwrite(filter_frame->data[0], 1, filter_frame->width * filter_frame->height, out_file_ptr);
		::fwrite(filter_frame->data[1], 1, filter_frame->width * filter_frame->height/4, out_file_ptr);
		::fwrite(filter_frame->data[2], 1, filter_frame->width * filter_frame->height/4, out_file_ptr);
		::fflush(out_file_ptr);
	}

	return 0;
}

/*** 
*解码器视频帧 滤镜处理
*/
static int filter_video(AVFrame* frame, AVFrame*filter_frame)
{
	// 向filter buffer中添加视频帧
	int32_t ret = ::av_buffersrc_add_frame(g_buffer_ctx, frame);
	if (ret < 0)
	{
		printf("filter buffersrc add frame failed (%s)!!!\n", make_error_string(ret));
		return ret;
	}
	//获取buffersink 处理完成后得数据

	while (true)
	{
		ret = ::av_buffersink_get_frame(g_buffersink_ctx, filter_frame);
		if (ret < 0)
		{
			//有可能buuffersink还没有处理好数据 需要继续获取得啦 ~~~~
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			{
				// 需要释放资源啦 ~~~~
				// av_frame_unref(filter_frame);
				break;
			}
			return ret;
		}

		// 直接写入文件中区
		do_frame(filter_frame);
		av_frame_unref(filter_frame);
	}
	av_frame_unref(filter_frame);
}


static int decode_frame_and_filter(AVFrame * frame, AVFrame * filter_frame)
{
	//从解码器获取一帧数据
	
	int32_t ret = ::avcodec_receive_frame(g_codec_ctx_ptr, frame);
	if (ret <0)
	{
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
		{
			//需要继续向解码推数据
		//	continue;
		}
		printf("acodec receive frame failed (%s)!!!\n", make_error_string(ret));
		return ret;
	}
	return filter_video(frame, filter_frame);
}

int main(int argc, char *argv[])
{
	const char * url = "rtmp://127.0.0.1/live/chensong";
	//过滤器 
	const char * filter_desc = "drawbox=30:10:64:64:red";
	//const char * filter_desc = "crop=300:300:64:64,scale=1920:1080";
	
	AVPacket packet;
	AVFrame *frame = NULL;
	AVFrame* filter_frame = NULL;
	int32_t ret = open_input_file(url);
	if (ret < 0)
	{
		printf("open url = [%s], failed !!!\n", url);
		return ret;
	}
	ret = init_filters(filter_desc);
	if (ret < 0)
	{
		printf("filter init failed !!!\n");
		return ret;
	}
	frame = av_frame_alloc();
	if (!frame)
	{
		printf("alloc frame failed !!!\n");
		return -1;
	}
	filter_frame = av_frame_alloc();
	if (!filter_frame)
	{
		printf("alloc filter frame failed !!!\n");
		return -1;
	}

	while (true)
	{
		//   读取一个包
		ret = ::av_read_frame(g_fmt_ctx_ptr, &packet);
		if (ret < 0)
		{
			//错误了
			break;
		}


		if (packet.stream_index == g_video_stream_index)
		{
			ret = ::avcodec_send_packet(g_codec_ctx_ptr,  &packet);
			if (ret < 0)
			{
				printf("send packet decoder failed !!!");
				continue;
			}
			if (decode_frame_and_filter(frame, filter_frame) < 0)
			{
				if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				{
					//需要继续读取数据
					continue;
				}
				if (ret < 0)
				{
					printf("decodec or filter !!!\n");
					break;
				}
				
			}
		}


	}


	return EXIT_SUCCESS;
}