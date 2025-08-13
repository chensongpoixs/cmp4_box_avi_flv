/***********************************************************************************************
created: 		2025-06-15

author:			chensong

purpose:		_C_ROCKCHIP_DECDER_H_
************************************************************************************************/
#include "crockchip_decoder.h"
//#include "mpp_mem.h"
namespace dsp
{
    bool crockchip_decoder::init(MppCodingType type)
    {
      
        RK_U32 need_split = 1;
        MPP_RET  ret = mpp_create(&m_ctx, &m_mpi);
        if (ret != MPP_SUCCESS)
        {
            printf("mpp_create failed ret = %u !!! \n", ret);
            return false;
        }
        printf("mpp_create OK !!!\n");
        ret = mpp_init(m_ctx, MPP_CTX_DEC, type);

        if (ret != MPP_SUCCESS)
        {
            printf("mpp_init failed ret = %u !!!\n", ret);
            return false;
        }

        // init packet
        ret = mpp_packet_init(&m_packet, NULL, 0);
        if (ret != MPP_SUCCESS)
        {
            printf("mpp_packet_init failed [ret = %u] !!!\n", ret);
            return false;
        }

        //
        {
            MppPollType timeout = MPP_POLL_BLOCK;
            MppParam  param = &timeout;
            ret = m_mpi->control(m_ctx, MPP_SET_OUTPUT_BLOCK, param);
            if (ret != MPP_SUCCESS)
            {
                printf("set output timeout [timeout = %u][ret = %u] failed !!!\n", timeout, ret);

                return false;
            }
        }


        // init defalut config 
        ret = mpp_dec_cfg_init(&m_cfg);

        if (ret != MPP_SUCCESS)
        {
            printf("get decder default config [ret = %u]\n", ret);

           // return false;
        }
        /* get default config from decoder context */
        ret = m_mpi->control(m_ctx, MPP_DEC_GET_CFG, m_cfg);
        if (ret != MPP_SUCCESS) 
        {
            printf("  to get decoder cfg [ret %d ] failed!!!\n",  ret);
            return false;
        }

        /*
         * split_parse is to enable mpp internal frame spliter when the input
         * packet is not aplited into frames.
         */
        ret = mpp_dec_cfg_set_u32(m_cfg, "base:split_parse", need_split);
        if (ret != MPP_SUCCESS)
        {
            printf("   to set split_parse [ret= %d]failed !!!\n", ret);
            return false;
        }

        ret = m_mpi->control(m_ctx, MPP_DEC_SET_CFG, m_cfg);
        if (ret != MPP_SUCCESS)
        {
            printf(" to set cfg  [ ret = %d] failed !!!\n",  ret);
            return false;
        }


        m_stoped.store(false);
        m_decder_thread = std::thread(&crockchip_decoder::_decder_thread, this);
        m_thread = std::thread(&crockchip_decoder::_work_thread, this);

        return false;
    }
    void crockchip_decoder::destroy()
    {
        stop();
        printf("[%s][%u] decoder jointable .... ", __FUNCTION__, __LINE__);
        if (m_decder_thread.joinable())
        {
            m_decder_thread.join();
        }
        printf("[%s][%u] decoder jointable OK !!! ", __FUNCTION__, __LINE__);
        printf("[%s][%u] deocker work thread  jointable .... ", __FUNCTION__, __LINE__);
        if (m_decder_thread.joinable())
        {
            m_decder_thread.join();
        }
        printf("[%s][%u] decoder jointable OK !!! ", __FUNCTION__, __LINE__);


        if (m_mpi)
        {
            m_mpi->reset(m_ctx);
        }
        if (m_packet)
        {
            mpp_packet_deinit(&m_packet);
            m_packet = NULL;
        }


        if (m_ctx) 
        {
            mpp_destroy(m_ctx);
            m_ctx = NULL;
        }

        _deinit_dec_mem();

        if (m_cfg)
        {
            mpp_dec_cfg_deinit(m_cfg);
            m_cfg = NULL;
        }

    }
    void crockchip_decoder::stop()
    {
        m_stoped.store(true);
        {
            m_condition.notify_all();
        }
    }
    void crockchip_decoder::push_packet(void* data, uint32_t size, int64_t pts, int64_t dts)
    {
        {
            clock_guard    lock(m_quene_lock);
            m_packet_queue.emplace_back(craw_packet((const unsigned char *)data, size, pts, dts));

        }
        m_condition.notify_one();
    }
    void crockchip_decoder::_decder_thread()
    {

        // m_packet_queue

        craw_packet* packet_ptr = NULL;
        while (!m_stoped)
        {
            {
                std::unique_lock<std::mutex> lock(m_quene_lock);
                m_condition.wait(lock, [this]() { return m_packet_queue.size() > 0 || m_stoped; });
            }


            while (!m_packet_queue.empty() && !m_stoped) {
                {
                    std::lock_guard<std::mutex> lock{ m_quene_lock };
                    packet_ptr = &m_packet_queue.front();
                    // m_packet_queue.pop_front();
                }

                if (packet_ptr) 
                {
                    if (!_handler_packet(packet_ptr))
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(5));
                        printf("handler packet failed !!!\n");
                        continue;
                    } 
                }
                 

                
                packet_ptr = NULL;
                {
                    std::lock_guard<std::mutex> lock{ m_quene_lock };
                    //packet_ptr = &m_packet_queue.front();
                    m_packet_queue.pop_front();
                }

            }
        }

    }

    static void dump_mpp_frame_to_file(MppFrame frame, FILE* fp)
    {
        RK_U32 width = 0;
        RK_U32 height = 0;
        RK_U32 h_stride = 0;
        RK_U32 v_stride = 0;
        MppFrameFormat fmt = MPP_FMT_YUV420SP;
        MppBuffer buffer = NULL;
        RK_U8* base = NULL;

        if (NULL == fp || NULL == frame)
            return;

        width = mpp_frame_get_width(frame);
        height = mpp_frame_get_height(frame);
        h_stride = mpp_frame_get_hor_stride(frame);
        v_stride = mpp_frame_get_ver_stride(frame);
        fmt = mpp_frame_get_fmt(frame);
        buffer = mpp_frame_get_buffer(frame);

        if (NULL == buffer)
            return;

        base = (RK_U8*)mpp_buffer_get_ptr(buffer);

        if (MPP_FRAME_FMT_IS_RGB(fmt) && MPP_FRAME_FMT_IS_LE(fmt)) 
        {
           int32_t new_fmt  =  fmt &  MPP_FRAME_FMT_MASK;
           fmt = (MppFrameFormat)new_fmt;
        }
        switch (fmt) {
        case MPP_FMT_YUV422SP: {
            /* YUV422SP -> YUV422P for better display */
            RK_U32 i, j;
            RK_U8* base_y = base;
            RK_U8* base_c = base + h_stride * v_stride;
            RK_U8* tmp = (RK_U8*)malloc(  h_stride * height * 2);
            RK_U8* tmp_u = tmp;
            RK_U8* tmp_v = tmp + width * height / 2;

            for (i = 0; i < height; i++, base_y += h_stride)
                fwrite(base_y, 1, width, fp);

            for (i = 0; i < height; i++, base_c += h_stride) {
                for (j = 0; j < width / 2; j++) {
                    tmp_u[j] = base_c[2 * j + 0];
                    tmp_v[j] = base_c[2 * j + 1];
                }
                tmp_u += width / 2;
                tmp_v += width / 2;
            }

            fwrite(tmp, 1, width * height, fp);
            free(tmp);
        } break;
        case MPP_FMT_YUV420SP_VU:
        case MPP_FMT_YUV420SP: {
            RK_U32 i;
            RK_U8* base_y = base;
            RK_U8* base_c = base + h_stride * v_stride;

            for (i = 0; i < height; i++, base_y += h_stride) {
                fwrite(base_y, 1, width, fp);
            }
            for (i = 0; i < height / 2; i++, base_c += h_stride) {
                fwrite(base_c, 1, width, fp);
            }
        } break;
        case MPP_FMT_YUV420P: {
            RK_U32 i;
            RK_U8* base_y = base;
            RK_U8* base_c = base + h_stride * v_stride;

            for (i = 0; i < height; i++, base_y += h_stride) {
                fwrite(base_y, 1, width, fp);
            }
            for (i = 0; i < height / 2; i++, base_c += h_stride / 2) {
                fwrite(base_c, 1, width / 2, fp);
            }
            for (i = 0; i < height / 2; i++, base_c += h_stride / 2) {
                fwrite(base_c, 1, width / 2, fp);
            }
        } break;
        case MPP_FMT_YUV444SP: {
            /* YUV444SP -> YUV444P for better display */
            RK_U32 i, j;
            RK_U8* base_y = base;
            RK_U8* base_c = base + h_stride * v_stride;
            RK_U8* tmp = (RK_U8*)malloc(  h_stride * height * 2);
            RK_U8* tmp_u = tmp;
            RK_U8* tmp_v = tmp + width * height;

            for (i = 0; i < height; i++, base_y += h_stride)
                fwrite(base_y, 1, width, fp);

            for (i = 0; i < height; i++, base_c += h_stride * 2) {
                for (j = 0; j < width; j++) {
                    tmp_u[j] = base_c[2 * j + 0];
                    tmp_v[j] = base_c[2 * j + 1];
                }
                tmp_u += width;
                tmp_v += width;
            }

            fwrite(tmp, 1, width * height * 2, fp);
            free(tmp);
        } break;
        case MPP_FMT_YUV400: {
            RK_U32 i;
            RK_U8* base_y = base;
            RK_U8* tmp = (RK_U8*)malloc( h_stride * height);

            for (i = 0; i < height; i++, base_y += h_stride)
                fwrite(base_y, 1, width, fp);

            free(tmp);
        } break;
        case MPP_FMT_ARGB8888:
        case MPP_FMT_ABGR8888:
        case MPP_FMT_BGRA8888:
        case MPP_FMT_RGBA8888: {
            RK_U32 i;
            RK_U8* base_y = base;
            RK_U8* tmp = (RK_U8*)malloc( width * height * 4);

            for (i = 0; i < height; i++, base_y += h_stride)
                fwrite(base_y, 1, width * 4, fp);

            free(tmp);
        } break;
        case MPP_FMT_RGB565:
        case MPP_FMT_BGR565:
        case MPP_FMT_RGB555:
        case MPP_FMT_BGR555:
        case MPP_FMT_RGB444:
        case MPP_FMT_BGR444: {
            RK_U32 i;
            RK_U8* base_y = base;
            RK_U8* tmp = (RK_U8*)malloc(  width * height * 2);

            for (i = 0; i < height; i++, base_y += h_stride)
                fwrite(base_y, 1, width * 2, fp);

            free(tmp);
        } break;
        default: {
            printf("not supported format %d\n", fmt);
        } break;
        }
    }
    //static FILE* out_file_ptr = NULL;
    void crockchip_decoder::_work_thread()
    {

        MppFrame frame = NULL;
        /*if (!out_file_ptr)
        {
            out_file_ptr = fopen("chensong.yuv", "wb+");
        }*/

		MppCodingType   coding_type = MPP_VIDEO_CodingAVC;
		video_info_param   video_param;
		
        RK_U32 fmt;
        while (!m_stoped)
        {
            MPP_RET  ret = m_mpi->decode_get_frame(m_ctx, &frame);
            if (ret != MPP_SUCCESS)
            {
                printf("decoder get frame failed (ret = %u) !!!\n", ret);
                continue;
            }
            if (frame == NULL)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                continue;
            }
            if (mpp_frame_get_info_change(frame))
            {
                // found info change and create buffer group for decoding
                RK_U32 width = mpp_frame_get_width(frame);
                RK_U32 height = mpp_frame_get_height(frame);
                RK_U32 hor_stride = mpp_frame_get_hor_stride(frame);
                RK_U32 ver_stride = mpp_frame_get_ver_stride(frame);
                RK_U32 buf_size = mpp_frame_get_buf_size(frame);
				  fmt = mpp_frame_get_fmt(frame);
                //MppBufferGroup grp = NULL;


                printf( "decode_get_frame get info changed found\n");
                printf(  "decoder require buffer w:h [%d:%d] stride [%d:%d] size %d\n",
                    width, height, hor_stride, ver_stride, buf_size);
                _deinit_dec_mem();
                bool init_mem =  _init_dec_mem(buf_size, 32 );
                if (!init_mem)
                {
                    printf("[%s][%d]\n", __FUNCTION__, __LINE__);
                }
                else
                {
                    /* Set buffer to mpp decoder */
                    ret = m_mpi->control(m_ctx, MPP_DEC_SET_EXT_BUF_GROUP, m_buffer_group);
                    if (ret != MPP_SUCCESS) 
                    {
                        printf(" [%s][%u] set buffer group failed ret %d\n", __FUNCTION__, __LINE__,  ret);
                         
                    }
                     

                    ret = m_mpi->control(m_ctx, MPP_DEC_SET_INFO_CHANGE_READY, NULL);
                    if (ret != MPP_SUCCESS)
                    {
                        printf("[%s][%u]info change ready failed ret %d\n",  __FUNCTION__, __LINE__, ret);
                        
                    }
                }

				//m_encoder.destroy();
				video_param.width = width;
				video_param.height = height;
				static const int32_t   default_bitrate = 1000;
				video_param.avg_bitrate = 500 * default_bitrate;
				video_param.min_bitrate = 500 * default_bitrate;
				video_param.max_bitrate = 500 * default_bitrate;
				video_param.gop_size = 250;
				video_param.fmt = fmt;
                //printf("")
				video_param.input_fps = 25;
				video_param.output_fps = 25;
				video_param.rc_mode = MPP_ENC_RC_MODE_CBR;
				m_encoder.init(coding_type, video_param);
               // grp = dec_buf_mgr_setup(data->buf_mgr, buf_size, 24, cmd->buf_mode);
            }
            else
            {
                char log_buf[256];
                RK_S32 log_size = sizeof(log_buf) - 1;
                RK_S32 log_len = 0;
                RK_U32 err_info = mpp_frame_get_errinfo(frame);
                RK_U32 discard = mpp_frame_get_discard(frame);
                RK_U32 width = mpp_frame_get_width(frame);
                RK_U32 height = mpp_frame_get_height(frame);

                log_len += snprintf(log_buf + log_len, log_size - log_len,
                    "decode get new frame");

                if (mpp_frame_has_meta(frame)) 
                {
                    MppMeta meta = mpp_frame_get_meta(frame);
                    RK_S32 temporal_id = 0;

                    mpp_meta_get_s32(meta, KEY_TEMPORAL_ID, &temporal_id);

                    log_len += snprintf(log_buf + log_len, log_size - log_len,
                        " tid %d", temporal_id);
                }

                if (err_info || discard)
                {
                    log_len += snprintf(log_buf + log_len, log_size - log_len,
                        " err %x discard %x", err_info, discard);
                }
                //mpp_log_q(quiet, "%p %s\n", ctx, log_buf);
                printf("[%s][%d][%s] [width = %u][height = %u][fmt = %u] \n", __FUNCTION__, __LINE__, log_buf, width, height, fmt);

				m_encoder.push(frame);

                /*dump_mpp_frame_to_file(frame, out_file_ptr);
                if (out_file_ptr)
                {
                    fflush(out_file_ptr);
                }*/
               // printf("[%s][%d] [width = %u][height = %u] %s\n", __FUNCTION__, __LINE__, log_buf, width, height);
                /*data->frame_count++;
                if (data->fp_output && !err_info)
                    dump_mpp_frame_to_file(frame, data->fp_output);*/
            }


            RK_U32 frm_eos = mpp_frame_get_eos(frame);
            mpp_frame_deinit(&frame);
        }

    }
    bool crockchip_decoder::_handler_packet(const craw_packet* packet_ptr)
    {
        if (!packet_ptr)
        {
            printf("[%s][%d]packet == null !!!\n", __FUNCTION__, __LINE__);

            return false;
        }


        mpp_packet_set_data(m_packet, (void *)packet_ptr->data());
        mpp_packet_set_size(m_packet, packet_ptr->size());
        mpp_packet_set_pos(m_packet, (void *)packet_ptr->data());
        mpp_packet_set_length(m_packet, packet_ptr->size());
        mpp_packet_set_pts(m_packet, packet_ptr->pts());
        mpp_packet_set_dts(m_packet, packet_ptr->dts());



        MPP_RET ret = m_mpi->decode_put_packet(m_ctx, m_packet);
        if (ret != MPP_SUCCESS)
        {
            printf("[%s][%d] decode_put_packet packet failed [ret = %u]  !!!\n", __FUNCTION__, __LINE__, ret);

            return false;
        }
        return true;
    }
    bool crockchip_decoder::_init_dec_mem(int32_t size, int32_t count)
    {

        MPP_RET ret = MPP_NOK;
        switch (m_decbuf_mode)
        {
        case MPP_DEC_BUF_HALF_INT:
        {

            ret = mpp_buffer_group_get_internal(&m_buffer_group, MPP_BUFFER_TYPE_ION);
            if (ret != MPP_SUCCESS) 
            {
                printf("[%s][%d]get mpp internal buffer group failed ret %d\n", __FUNCTION__, __LINE__, ret);
                return false;
            }

            /* Use limit config to limit buffer count and buffer size */
            ret = mpp_buffer_group_limit_config(m_buffer_group, size, count);
            if (ret != MPP_SUCCESS) 
            {
                printf("[%s][%d]limit buffer group failed ret %d\n", __FUNCTION__, __LINE__, ret);
                return false;
            }
            break;
        }
        case MPP_DEC_BUF_INTERNAL:
        {
            /* do nothing juse keep buffer group empty */
          //  mpp_assert(NULL == impl->group);
            ret = MPP_OK;
            break;
        }
        case MPP_DEC_BUF_EXTERNAL:
        {

            MppBufferInfo commit;
            m_buf_count = count;
            m_buf_size = size;
            m_bufs = new MppBuffer[count]; //mpp_calloc(MppBuffer, count);
            if (!m_bufs)
            {
                printf("create %d external buffer record failed\n", count);
                return false;
            }
            ret = mpp_buffer_group_get_external(&m_buffer_group, MPP_BUFFER_TYPE_ION);
            if (ret != MPP_SUCCESS)
            {
                printf("get mpp external buffer group failed ret %d\n", ret);
                return false;
            }
            commit.type = MPP_BUFFER_TYPE_ION;
            commit.size = size;
            for (int32_t i = 0; i < count; i++) 
            {
              //  ret = mpp_buffer_get(NULL, &m_bufs[i], size);
                ret = mpp_buffer_get_with_tag(NULL, &m_bufs[i], size, "MPP_BUFFER", __FUNCTION__);
                if (ret || NULL == m_bufs[i]) {
                    printf("[%s][%d]get misc buffer failed ret %d\n", __FUNCTION__, __LINE__,  ret);
                    break;
                }

                commit.index = i;
                commit.ptr = mpp_buffer_get_ptr_with_caller(m_bufs[i], NULL);
                commit.fd = mpp_buffer_get_fd_with_caller(m_bufs[i], NULL);

                ret = mpp_buffer_commit(m_buffer_group, &commit);
                if (ret != MPP_SUCCESS) 
                {
                    printf("[%s][%d]external buffer commit failed ret %d\n", __FUNCTION__, __LINE__,  ret);
                    break;
                }
            }
            break;
        }
        case MPP_DEC_BUF_MODE_BUTT:
        {
            printf("[%s][%d]unsupport buffer mode %d\n", __FUNCTION__, __LINE__, m_decbuf_mode);
            return false;
            break;
        }
        default:
            printf("[%s][%d]unsupport buffer mode %d\n", __FUNCTION__, __LINE__, m_decbuf_mode);
            return false;
            break;
        }


        return true;
    }
    void crockchip_decoder::_deinit_dec_mem()
    {
        if (!m_buffer_group)
        {
            return;
        }


        mpp_buffer_group_put(m_buffer_group);
        m_buffer_group = NULL;
        if (m_buf_count > 0 && m_bufs)
        {
            for (int32_t i = 0; i < m_buf_count; ++i)
            {
                if (m_bufs[i]) 
                {
                    mpp_buffer_put_with_caller(m_bufs[i], __FUNCTION__);
                    m_bufs[i] = NULL;
                }
            }
            delete [] m_bufs;
            //MPP_FREE(m_bufs);
            m_bufs = NULL;
        }


    }
}
