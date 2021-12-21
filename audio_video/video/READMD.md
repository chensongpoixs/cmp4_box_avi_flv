录制屏幕
为了实现对于Window桌面录制，有两种方式进行采集：

DirectShow 设备
gdigrab设备
其基本命令行如下：

//Dshow设备
ffmpeg -f dshow -i video="screen-capture-recorder" output.mkv

//gdigrab设备 采集整个桌面
ffmpeg -f gdigrab -framerate 30 -i desktop output.mkv

这两个命令默认都是采用x264 进行编码，在本地CPU不是足够高的情况下，录制的视频画面根本看不清楚，这是因为编码效率太低导致；

为了提高录制效果，我们可以采用无损编码+提高编码速度方式进行录制，具体命令如下：

ffmpeg -framerate 30 -f gdigrab -i desktop -c:v libx264rgb -crf 0 -preset ultrafast output.mkv
或者
ffmpeg -framerate 30 -f gdigrab -i desktop -c:v libx264rgb -preset:v ultrafast -tune:v zerolatency output.mkv

关于FFmpeg屏幕采集可以参考 https://trac.ffmpeg.org/wiki/Capture/Desktop文章
关于H264编码方面的知识可以参考https://trac.ffmpeg.org/wiki/Encode/H.264文章

录制声音
在上面提到过录制屏幕除了采用gdigrab外，还可以采用dshow方式；它们的区别就是：gdigrab设置仅支持截取屏幕信息，对声音的录制是不支持的，而show方式可以支持录制屏幕和声音。

这个dshow软件的下载信息如下：

编译好的下载地址是:
http://sourceforge.net/projects/screencapturer/

源码地址是:
https://github.com/rdp/screen-capture-recorder-to-video-windows-free

为了使系统能识别出dshow设备，我们首先需要进行注册，为了去掉不必要的文件，我们只提取四个dll：

screen-capture-recorder.dll
screen-capture-recorder-x64.dll
audio_sniffer-x64.dll
audio_sniffer.dll
注册命令行如下：

//注册屏幕录制设备（我们采用32位的ffmpeg，可以不用注册带x64的dll）
regsvr32 /s  screen-capture-recorder.dll
//注册虚拟音频设备
regsvr32 /s audio_sniffer.dll
 
注册成功后，可以采用以下命令进行检查是否注册成功

ffmpeg -list_devices true -f dshow -i dummy  
1
系统输出大致如下:

“screen-capture-recorder” 这个就是桌面捕获设备,用于录制屏幕
“virtual-audio-capturer” 这个是音频捕获设备，用于录制声音

-f dshow -i audio="virtual-audio-capturer" 这代表声音从“virtual-audio-capturer”音频设备获取
1
为了能够同时录制声音和画面，我们可以使用以下命令进行采集：

ffmpeg -framerate 30 -f gdigrab -i desktop -f dshow -i audio="virtual-audio-capturer" -c:v libx264rgb -preset:v ultrafast -tune:v zerolatency output.mp4
1
用vlc打开录制文件，可以看书画面显示正常以及声音正常被播放处理，截图如下：


以上就是关于ffmpeg录制window桌面的全部过程了，欢迎大家交流~




H264压缩比

条件： 1. YUV格式为YUV420
     2. 分辨率为640X480
	 3. 帧率为15
	 
公式

```
640*480*1.5 * 15 * 8 	 
```
	 
建议码流：500Kpbs

结果：约 1/100


# 编码帧的分类

## I帧（intraframe frame）， 关键帧，采用帧内压缩技术。IDR帧属于I帧。

## P帧（forward Predicted frame），向前参考帧。压缩时，只参考前面已经处理的帧，采用帧间压缩技术。它占I帧的一半的大小

## B帧（Bidirectionally predicted frame），双向参考帧。压缩时，即参考前面已经处理的帧，也参考后面的帧，帧间压缩技术。它占I帧的1/4大小。

# IDR帧与I帧的区别与联系

I_GOP

## IDR（Instantaneous Decoder Refresh）解码器立即刷新帧

## 每当遇到IDR帧时， 解码器就会清空解码器参考buffer中内容

## 每个GOP中的第一帧就是IDR帧

## IDR帧是一种特殊的I帧




# SPS与PPS

## SPS（Sequence Parameter Set）

序列参数集，作用与一串连续的视频图像。如seq_parameter_set_id、帧数及POC（picture order count）的约束、参考帧数目、解码图像尺寸和帧场编码模式选择标识等。

## PPS（Picture Parameter Set）

图像参数集，作用于视频序列中的图像。如pic_parameter_set_id、熵编码模式选择标识、片组数目、初始量化参数和去方块滤波系数调整标识等


# H264压缩技术

## 帧内压缩、 解决的是空域数据冗余问题

## 帧间压缩、 解决的是时域数据冗余问题

## 整数离散余弦变换(DCT)、 将空间上的相关性变为频域上无关的数据然后进行量化。

## CABAC压缩

# 宏块

## 宏块是视频压缩操作的基本单元

## 无论是帧内压缩还是帧间压缩、他们都以宏块为单位


# 帧内压缩的理论

## 相临橡素差别不大，所以可以进行宏块预测

## 人们对亮度的敏感度超过色度

## YUV很容易将亮度与色度分开

帧内预测：

H264提供9种模式帧内压缩

h264_chiled
h264_x16_x8

h264_frame


h264_frame_picture
h264_en_picture_mode

# 帧间压缩原理

## GOP

## 参考帧 

## 运动估计(宏块匹配 + 运动矢量)

## 运动补偿(解码)


## 宏块查找算法（运动估计）

宏块查找.png
运动矢量与补偿压缩.png

### 三步搜索

### 二维对数搜索

### 四步搜索

### 钻石搜索


## 帧间压缩的帧类型

### P帧

### B帧

视频花屏原因

如果GOP分组中有帧丢失，会造成解码端的图像发生错误、这会出现马赛克(花屏)

视频卡顿原因

为了避免花屏问题的发生，当发现有帧丢失时、就丢弃GOP内的所有帧、直到下一个IDR帧重新刷新图像。

I帧是按照周期来的，需要一个比较长的时间周期，如果在下一个I帧来之前不显示后来的图像、那么视频就静止不动了，这就是出现了所谓的卡顿现象。

GPU解码问题

# 无损压缩

## 1、 DCT变换

## 2、 VLC压缩（MPEG2压缩）
 
 VLC.png

## 3、 CABAC压缩 （H264）

h264_encode_nal.png

h264_decoder_nal.png


H264查考资料：

https://en.wikipedia.org/wiki/Advanced_Video_Coding
