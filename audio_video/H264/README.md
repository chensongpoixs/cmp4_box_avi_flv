
# 一、H264码流分层

## 1、NAL层

Network Abstraction Layer,视频数据网络抽样层。

## 2、 VCL层

VIdeo Coding Layer, 视频数据编码层。

## 3、 码流基本概念

### ①、 SODB（String Of Data Bits）

原始数据比特流，长度不一定是8的倍数，故需要补齐。它是由VCL层产生的。

### ②、 RBSP（Raw Byte Sequence Payload）

SODB+trailing bits

算法是如果SODB最后一个字节不对齐，则补1和多个0.

### ③、 NALU 单元

NAL Header(1byte) + RBSP 

NALUnit.png

h264_slice_block.png

h264_slice.png

h264_rtp_annexb.png


# 1、SPS/PPS/Slice Header 

# 2、 常见分析工具

# 3、 ffmpeg视频编码


#  二, SPS中两个重要的参数分别是 Profile 与 Level

## 1、 H264 Profile


h264_profile.png

h264_profile_main_high.png

对视频压缩特性的描述，Profile越高，就说明采用了越高级的压缩特性

## 2、 H264 Level

h264_level.png

Level是对视频的描述，Level越高，视频的码率、分辨率、fps越高


## 3、 分辨率 

|||
|:---:|:---:|
|pic_width_in_mbs_minues1|图像宽度包括的宏块个数-1|
|pic_heigh_in_mbs_minus1| 图像高度包括的宏块个数-1|
|frame_mbs_only_flag| 帧编码还是场编码 （场是隔行扫描、产生两张图）|
|frame_cropping_flag| 图像算法需要裁剪|
|frame_crop_left_offset|减去左侧的偏移量|
|frame_crop_right_offset|减去右侧的偏移量|
|frame_crop_top_offset|减去顶部的偏移量|
|frame_crop_bottom_offset|减去底部的偏移量|

## 4、 帧相关的

### ①、 帧数 log2_max_frame_num_minus4

### ②、 参考帧数 max_num_ref_frames

解码设置缓冲区大小的

### ③、 显示帧序号 pic_order_cnt_type

解码 

## 5、 帧率的计算

```
framerate = (float)(sps->vui.vui_time_scale)/(float)(sps->vui.vui_num_units_in_tick)/2;
```



# 三、 PPS与 Slice Header

|||
|:---:|:---:|
|entropy_coding_mode_flag|熵编码，1表示使用CABAC编码|
|num_slice_groups_minus1|分片数量|
|weighted_pred_flag|在P/SP Slice中开启权重预测|
|weighted_bipred_idc|在B Slice中加权预测的方法|
|pic_init_qp_minus26/pic_init_qs_minus26|初始化量参数,实际参数在Slice header|
|chroma_qp_index_offset|用于计算色度分量的量化参数|
|deblocking_filter_control_present_flag|表示Slice header中是否存在用于去块滤波器控制的信息|
|constrainged_intra_pred_flag|若该标识为1，表示I宏块在进行帧内预测时只能使用来自I和SI类型宏块的信息|
|redundant_pic_cnt_preset_flag|用于表示Slice Header中是否存在redundant_pic_cnt语法元素|

## Slice Header

### ①、 帧类型

### ②、 GOP中解码帧序号

### ③、 预测权重

### ④、 滤波


工具

Elecard Stream Eye

CodecVisa

40 * 16 = 640





