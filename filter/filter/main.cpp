#include <iostream>
#include <stdio.h>
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
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
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


int main(int argc, char *argv[])
{



	return EXIT_SUCCESS;
}