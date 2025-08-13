# 音视频基础知识

## 一、[audio_video_example](https://chensongpoixs.github.io/caudio_video/)

音频和视频的基础知识的代码demo

 



## 二、[h264、mp4](https://github.com/chensongpoixs/cmp4_box_avi_flv/tree/master/mp4)

音视频基础知识之视频压缩格式保存格式和文件格式分析

## 三、[video_codec](https://chensongpoixs.github.io/cvideo_codec/)

视频的编解码的原理的深度学习

## 四、 音频处理技术 

 
1. 傅里叶变换
2. 语音识别 
3. 语音合成
4. 回声消除
5. 主动降噪
6. 语音增强



# 五、 rockchip


MPP解码和编码demo 

```javascript 

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
```