/***********************************************************************************************
created: 		2025-06-15

author:			chensong

purpose:		rtmp_push
************************************************************************************************/

#include "cflv.h"
#include "cbytebuffer.h"
#include "librtmp/amf.h"
#include "crtmp-helpers.h"
namespace dsp
{


    static const double VIDEODATA_AVCVIDEOPACKET = 7.0;
    // Additional FLV onMetaData values for Enhanced RTMP/FLV
    static const double VIDEODATA_AV1VIDEOPACKET = 1635135537.0; // FourCC "av01"
 
    static const double VIDEODATA_HEVCVIDEOPACKET = 1752589105.0; // FourCC "hvc1"
 

    void cflv::_write_header()
    {
        unsigned char buffer[4096] = {0};

        unsigned char* ptr = &buffer[0];
        int32_t length = 0;
        // flv header

        length += bytebuffer::writedata(ptr + length, (void *)"FLV", 3);
        //ptr += length;
        //    version
        length += bytebuffer::write8(ptr+length, 1);
        //   audio    video 
        //倒数第一bit是1表示有视频，倒数第三bit是1表示有音频，其他都应该是0（有些软件如flvtool2可能造成倒数第四bit是1，不过也没发现有什么不对）
        //uint8_t   value = 
        length += bytebuffer::write8(ptr + length, FLV_HEADER_FLAG_HASAUDIO * m_has_audio +
            FLV_HEADER_FLAG_HASVIDEO * m_has_audio);
        //整个文件头的长度，一般是9（3+1+1+4），有时候后面还有些别的信息，就不是9了
        length += bytebuffer::write32(ptr + length, 9);
        // 结束位置
        length += bytebuffer::write32(ptr + length, 0);


        // ext  TODO@chensong 2025-08-01 
        /*
        
        length += bytebuffer::write8(ptr + length, 8);     // message type
        length += bytebuffer::write24(ptr + length, 0);   // include flags
        length += bytebuffer::write24(ptr + length, 0);   // time stamp
        length += bytebuffer::write32(ptr + length, 0);   // reserved
        length += bytebuffer::write32(ptr + length, 11);  // size
        */

    }
    void cflv::_write_metadata(uint64_t ts)
    {
        unsigned char buffer[4096] = { 0 };

        unsigned char* ptr = &buffer[0];
        int32_t length = 0;
        int32_t metadata_size_pos = 0;
        
        char flv_metadata[4096] = {0};
        char* p = flv_metadata;
        int32_t meta_data_size = 0;
        _write_flv_metadata(&p, meta_data_size);
 
        /* write meta_tag */
        length += bytebuffer::write8(ptr + length, FLV_TAG_TYPE_META); // tag type META
       
        length += bytebuffer::write24(ptr + length, (uint32_t)meta_data_size);
        length += bytebuffer::write32(ptr + length, 0);
        length += bytebuffer::write24(ptr + length, 0);
        length += bytebuffer::writedata(ptr + length, p, meta_data_size);

        /*
          * From FLV file format specification version 10:
          * Size of previous [current] tag, including its header.
          * For FLV version 1 this value is 11 plus the DataSize of
          * the previous [current] tag.
          */
        length += bytebuffer::write32(ptr + length, (uint32_t)length);
    }
    void cflv::_write_flv_metadata(char** flv_data, int32_t &flv_metadata_size)
    {
        char* enc = *flv_data ;
        char* end = enc + sizeof(enc);
        enc_str(&enc, end, "@setDataFrame");
        enc_str(&enc, end, "onMetaData");

        *enc++ = AMF_ECMA_ARRAY;
        enc = AMF_EncodeInt32(enc, end, 20);

        enc_num_val(&enc, end, "duration", 0.0);
        enc_num_val(&enc, end, "fileSize", 0.0);

        enc_num_val(&enc, end, "width", (double)m_width);
        enc_num_val(&enc, end, "height", (double)m_height);


        double video_code_id = VIDEODATA_AVCVIDEOPACKET;

        enc_num_val(&enc, end, "videocodecid", video_code_id);
        enc_num_val(&enc, end, "videodatarate", m_video_rate);
        enc_num_val(&enc, end, "framerate", m_video_rate);

        enc_num_val(&enc, end, "audiocodecid", AUDIODATA_AAC);
        enc_num_val(&enc, end, "audiodatarate", m_audio_sample_rate);
        enc_num_val(&enc, end, "audiosamplerate", (double)m_audio_sample_rate);
        enc_num_val(&enc, end, "audiosamplesize", 16.0);
        enc_num_val(&enc, end, "audiochannels", (double)m_audio_chnannels);

        enc_bool_val(&enc, end, "stereo", m_audio_chnannels == 2);
        enc_bool_val(&enc, end, "2.1", m_audio_chnannels == 3);
        enc_bool_val(&enc, end, "3.1", m_audio_chnannels == 4);
        enc_bool_val(&enc, end, "4.0", m_audio_chnannels == 4);
        enc_bool_val(&enc, end, "4.1", m_audio_chnannels == 5);
        enc_bool_val(&enc, end, "5.1", m_audio_chnannels == 6);
        enc_bool_val(&enc, end, "7.1", m_audio_chnannels == 8);


        enc_str_val(&enc, end, "encoder", "(chen version 1.0)");


        *enc++ = 0;
        *enc++ = 0;
        *enc++ = AMF_OBJECT_END;

        flv_metadata_size = enc - (char*)*flv_data ;
    }
    void cflv::_write_audio_header(uint64_t ts)
    {
        char buffer[4096] = { 0 };

        char* p = buffer;
        int32_t length = 0;


        length += bytebuffer::write8(p + length, FLV_TAG_TYPE_AUDIO);
        // size patched later
        length += bytebuffer::write24(p + length, 0);

        // timestamp
        length += bytebuffer::write24(p + length, ts & 0xFFFFFF);
        length += bytebuffer::write8(p + length, (ts >> 24) & 0x7F);

        // streamid 
        length += bytebuffer::write24(p + length, 0);

        /* these are the 5 extra bytes mentioned above */
        bool keyframe = true;
        bool is_header = true;

        int32_t ct_offset_ms = 0;

        /* these are the two extra bytes mentioned above */
        length += bytebuffer::write8(p + length, 0xaf);
        length += bytebuffer::write8(p + length, is_header ? 0 : 1);
        //length += bytebuffer::write8(p + length, packet->data, packet->size);

       /* length += bytebuffer::write8(p + length, keyframe ? 0x17 : 0x27);
        length += bytebuffer::write8(p + length, is_header ? 0 : 1);
        length += bytebuffer::write24(p + length, ct_offset_ms);*/
        /*
          * From FLV file format specification version 10:
          * Size of previous [current] tag, including its header.
          * For FLV version 1 this value is 11 plus the DataSize of
          * the previous [current] tag.
          */
        length += bytebuffer::write32(p + length, (uint32_t)length);
    }
    void cflv::_write_video_header(uint64_t ts)
    {

        char buffer[4096] = {0};

        char* p = buffer;
        int32_t length = 0;


        length += bytebuffer::write8(p + length, FLV_TAG_TYPE_VIDEO);
        // size patched later
        length += bytebuffer::write24(p + length, 0);

        // timestamp
        length += bytebuffer::write24(p + length, ts & 0xFFFFFF);
        length += bytebuffer::write8(p + length, (ts >> 24) & 0x7F);

        // streamid 
        length += bytebuffer::write24(p + length, 0);

        /* these are the 5 extra bytes mentioned above */
        bool keyframe = true;
        bool is_header = true;

        int32_t ct_offset_ms = 0;
        length += bytebuffer::write8(p + length,  keyframe ? 0x17 : 0x27);
        length += bytebuffer::write8(p + length, is_header ? 0 : 1);
        length += bytebuffer::write24(p + length, ct_offset_ms);
        /*
          * From FLV file format specification version 10:
          * Size of previous [current] tag, including its header.
          * For FLV version 1 this value is 11 plus the DataSize of
          * the previous [current] tag.
          */
        length += bytebuffer::write32(p + length, (uint32_t)length);

    }
}