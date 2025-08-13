/***********************************************************************************************
created: 		2025-06-15

author:			chensong

purpose:		_C_ROCKCHIP_DECDER_H_
************************************************************************************************/
#include "crockchip_encoder.h"

namespace dsp
{
#define MPP_ALIGN(x, a)         (((x)+(a)-1)&~((a)-1))
	bool crockchip_encoder::init(MppCodingType coding_type, video_info_param param)
	{
		MPP_RET  ret = MPP_SUCCESS;
		MppPollType timeout = MPP_POLL_NON_BLOCK;
		m_coding_type = coding_type;

		m_video_info_param = param;
		ret = mpp_create(&m_ctx, &m_mpi);
		if (ret != MPP_SUCCESS)
		{
			printf("mpp_create failed ret = %u !!! \n", ret);
			return false;
		}
		printf("mpp_create OK !!!\n");

		ret = m_mpi->control(m_ctx, MPP_SET_INPUT_TIMEOUT, &timeout);
		if (ret != MPP_SUCCESS) 
		{
			printf("mpi control set input timeout %d ret %d\n", timeout, ret);
			return false;
		}

		timeout = MPP_POLL_BLOCK;

		ret = m_mpi->control(m_ctx, MPP_SET_OUTPUT_TIMEOUT, &timeout);
		if (ret != MPP_SUCCESS) 
		{
			printf("mpi control set output timeout %d ret %d\n", timeout, ret);
			return false;
		}

		ret = mpp_init(m_ctx, MPP_CTX_ENC, coding_type);
		if (ret != MPP_SUCCESS) 
		{
			printf("mpp_init failed ret %d\n", ret);
			return false;
		}
		MppEncCfg   enc_cfg;
		ret = mpp_enc_cfg_init(&enc_cfg);
		if (ret != MPP_SUCCESS)
		{
			printf("[%s][%d]encoder init config failed !!! [ret = %u]", __FUNCTION__, __LINE__, ret);
			return false;
		}

		ret = m_mpi->control(m_ctx, MPP_ENC_GET_CFG, enc_cfg);
		if (ret != MPP_SUCCESS)
		{
			printf("[%s][%d] get encoder config failed !!! [ret = %u]", __FUNCTION__, __LINE__, ret);
			return false;
		}


		// set enc cfg 
		if (!_set_enc_cfg(&enc_cfg))
		{
			printf("[%s][%d]set enc  config failed !!!\n", __FUNCTION__, __LINE__);
			return false;
		}

		ret = m_mpi->control(m_ctx, MPP_ENC_SET_CFG, enc_cfg);
		if (ret != MPP_SUCCESS)
		{
			printf("[%s][%d] set encoder config failed !!! [ret = %u]", __FUNCTION__, __LINE__, ret);
			return false;
		}
		RK_U32   sei_mode = MPP_ENC_SEI_MODE_ONE_FRAME;
		ret = m_mpi->control(m_ctx, MPP_ENC_SET_SEI_CFG, &sei_mode);
		if (ret != MPP_SUCCESS)
		{
			printf("[%s][%d]mpi control enc set sei cfg failed ret %d\n", __FUNCTION__, __LINE__, ret);
			return false;
		}

		if (coding_type == MPP_VIDEO_CodingAVC || coding_type == MPP_VIDEO_CodingHEVC) 
		{
			RK_U32 header_mode = MPP_ENC_HEADER_MODE_EACH_IDR;
			ret = m_mpi->control(m_ctx, MPP_ENC_SET_HEADER_MODE, &header_mode);
			if (ret != MPP_SUCCESS)
			{
				printf("[%s][%d]mpi control enc set header mode failed ret %d\n", __FUNCTION__, __LINE__, ret);
				return false;
			}
		}


		m_stoped = false;
		m_thread = std::thread(&crockchip_encoder::_work_thread, this);
		return true;
	}
	bool crockchip_encoder::push(MppFrame &frame)
	{
		if (m_stoped)
		{
			return false;
		}

		int32_t ret = m_mpi->encode_put_frame(m_ctx, frame);

		return false;
	}
	bool crockchip_encoder::_set_enc_cfg(MppEncCfg * enc_cfg)
	{
		MppEncCfg cfg = *enc_cfg;
		mpp_enc_cfg_set_s32(cfg, "prep:width", m_video_info_param.width);
		mpp_enc_cfg_set_s32(cfg, "prep:height", m_video_info_param.height);
		mpp_enc_cfg_set_s32(cfg, "prep:hor_stride", MPP_ALIGN(m_video_info_param.width, 16));
		mpp_enc_cfg_set_s32(cfg, "prep:ver_stride", MPP_ALIGN(m_video_info_param.height, 16));
		mpp_enc_cfg_set_s32(cfg, "prep:format", m_video_info_param.fmt);

		mpp_enc_cfg_set_s32(cfg, "rc:mode", m_video_info_param.rc_mode);

		/* fix input / output frame rate */
		mpp_enc_cfg_set_s32(cfg, "rc:fps_in_flex", m_fps_in_flex);
		mpp_enc_cfg_set_s32(cfg, "rc:fps_in_num", m_video_info_param.input_fps);
		mpp_enc_cfg_set_s32(cfg, "rc:fps_in_denorm", 1);
		mpp_enc_cfg_set_s32(cfg, "rc:fps_out_flex", m_fps_out_flex);
		mpp_enc_cfg_set_s32(cfg, "rc:fps_out_num", m_video_info_param.output_fps);
		mpp_enc_cfg_set_s32(cfg, "rc:fps_out_denorm", 1);
		mpp_enc_cfg_set_s32(cfg, "rc:gop", m_video_info_param.gop_size);

		/* drop frame or not when bitrate overflow */
		mpp_enc_cfg_set_u32(cfg, "rc:drop_mode", MPP_ENC_RC_DROP_FRM_DISABLED);
		mpp_enc_cfg_set_u32(cfg, "rc:drop_thd", 20);        /* 20% of max bps */
		mpp_enc_cfg_set_u32(cfg, "rc:drop_gap", 1);         /* Do not continuous drop frame */

		/* setup bitrate for different rc_mode */
		mpp_enc_cfg_set_s32(cfg, "rc:bps_target", m_video_info_param.avg_bitrate);

		switch (m_rc_mode)
		{
		case  MPP_ENC_RC_MODE_FIXQP:
		{
			/* do not setup bitrate on FIXQP mode */
			break;
		}
		case MPP_ENC_RC_MODE_CBR:
		{
			/* CBR mode has narrow bound */
			mpp_enc_cfg_set_s32(cfg, "rc:bps_max", m_video_info_param.max_bitrate);
			mpp_enc_cfg_set_s32(cfg, "rc:bps_min", m_video_info_param.min_bitrate);
			break;
		}
		case MPP_ENC_RC_MODE_VBR:
		case MPP_ENC_RC_MODE_AVBR: {
			/* VBR mode has wide bound */
			//???
			mpp_enc_cfg_set_s32(cfg, "rc:bps_max", m_video_info_param.max_bitrate);
			mpp_enc_cfg_set_s32(cfg, "rc:bps_min", m_video_info_param.min_bitrate);
			break;
		}   
		default:
			{
			mpp_enc_cfg_set_s32(cfg, "rc:bps_max", m_video_info_param.max_bitrate);
			mpp_enc_cfg_set_s32(cfg, "rc:bps_min", m_video_info_param.min_bitrate);
			break;
			}
		}


		/* setup qp for different codec and rc_mode */
		switch (m_coding_type) 
		{
			case MPP_VIDEO_CodingAVC:
			case MPP_VIDEO_CodingHEVC: 
			{
				switch (m_video_info_param.rc_mode)
				{
					case MPP_ENC_RC_MODE_FIXQP: {
						RK_S32 fix_qp = 30;// cmd->qp_init;

						mpp_enc_cfg_set_s32(cfg, "rc:qp_init", fix_qp);
						mpp_enc_cfg_set_s32(cfg, "rc:qp_max", fix_qp);
						mpp_enc_cfg_set_s32(cfg, "rc:qp_min", fix_qp);
						mpp_enc_cfg_set_s32(cfg, "rc:qp_max_i", fix_qp);
						mpp_enc_cfg_set_s32(cfg, "rc:qp_min_i", fix_qp);
						mpp_enc_cfg_set_s32(cfg, "rc:qp_ip", 0);
						break;
					} 
					case MPP_ENC_RC_MODE_CBR:
					case MPP_ENC_RC_MODE_VBR:
					case MPP_ENC_RC_MODE_AVBR: {
						mpp_enc_cfg_set_s32(cfg, "rc:qp_init", -1);
						mpp_enc_cfg_set_s32(cfg, "rc:qp_max", 51);
						mpp_enc_cfg_set_s32(cfg, "rc:qp_min", 10);
						mpp_enc_cfg_set_s32(cfg, "rc:qp_max_i", 51);
						mpp_enc_cfg_set_s32(cfg, "rc:qp_min_i", 10);
						mpp_enc_cfg_set_s32(cfg, "rc:qp_ip", 2);
						break;
					} 
					default: 
					{
						printf("unsupport encoder rc mode %d\n", m_rc_mode);
						break;
					} 
				}
				break;
			}  
			default: 
			{
				break;
			} 
		}



		/* setup codec  */
		mpp_enc_cfg_set_s32(cfg, "codec:type", m_coding_type);
		switch (m_coding_type) 
		{
			case MPP_VIDEO_CodingAVC: 
			{
				/*
				 * H.264 profile_idc parameter
				 * 66  - Baseline profile
				 * 77  - Main profile
				 * 100 - High profile
				 */
				mpp_enc_cfg_set_s32(cfg, "h264:profile", 100);
				/*
				 * H.264 level_idc parameter
				 * 10 / 11 / 12 / 13    - qcif@15fps / cif@7.5fps / cif@15fps / cif@30fps
				 * 20 / 21 / 22         - cif@30fps / half-D1@@25fps / D1@12.5fps
				 * 30 / 31 / 32         - D1@25fps / 720p@30fps / 720p@60fps
				 * 40 / 41 / 42         - 1080p@30fps / 1080p@30fps / 1080p@60fps
				 * 50 / 51 / 52         - 4K@30fps
				 */
				mpp_enc_cfg_set_s32(cfg, "h264:level", 40);
				mpp_enc_cfg_set_s32(cfg, "h264:cabac_en", 1);
				mpp_enc_cfg_set_s32(cfg, "h264:cabac_idc", 0);
				mpp_enc_cfg_set_s32(cfg, "h264:trans8x8", 1);
				break;
			} 
			case MPP_VIDEO_CodingHEVC:
			case MPP_VIDEO_CodingMJPEG:
			case MPP_VIDEO_CodingVP8: 
			{
				break;
			} 
			default: 
			{
				printf("unsupport encoder coding type %d\n", m_coding_type);
				break;
			} 
		}
		RK_U32 split_mode = MPP_ENC_SPLIT_NONE;
		RK_U32 split_arg = 0;
		RK_U32 split_out = 0;

		if (split_mode) 
		{
			printf( " split mode %d arg %d out %d\n" ,split_mode, split_arg, split_out);
			mpp_enc_cfg_set_s32(cfg, "split:mode", split_mode);
			mpp_enc_cfg_set_s32(cfg, "split:arg", split_arg);
			mpp_enc_cfg_set_s32(cfg, "split:out", split_out);
		}

		// config gop_len and ref cfg
		mpp_enc_cfg_set_s32(cfg, "rc:gop", m_video_info_param.gop_size/*p->gop_len ? p->gop_len : p->fps_out_num * 2*/);

		

		/*
		mpp_env_get_u32("gop_mode", &gop_mode, gop_mode);
		if (gop_mode) {
			mpp_enc_ref_cfg_init(&ref);

			if (p->gop_mode < 4)
				mpi_enc_gen_ref_cfg(ref, gop_mode);
			else
				mpi_enc_gen_smart_gop_ref_cfg(ref, p->gop_len, p->vi_len);

			mpp_enc_cfg_set_ptr(cfg, "rc:ref_cfg", ref);
		}
		*/
		return true;
	}
	void crockchip_encoder::_encoder_thread()
	{
		MppFrame   frame = NULL;
		while (!m_stoped)
		{
			//mpp_frame_init();
			{
				std::unique_lock<std::mutex> lock(m_queue_lock);
				m_condition.wait(lock, [this]() { return m_queue_list.size() > 0 || m_stoped; });
			}


			int32_t ret = mpp_frame_init(&frame);
			if (ret != 0)
			{
				printf("[%s][%d]\n", __FUNCTION__, __LINE__);
				continue;
			}


			mpp_frame_set_width(frame, m_width);
			mpp_frame_set_height(frame, m_height);
			mpp_frame_set_hor_stride(frame, m_hor_stride);
			mpp_frame_set_ver_stride(frame, m_ver_stride);
			//mpp_frame_set_fmt(frame, m_fmt);
			//mpp_frame_set_eos(frame, m_frm_eos);


			//mpp_frame_set_buffer(frame, buffer);


			ret = m_mpi->encode_put_frame(m_ctx, frame);
			//if (ret)
				//msleep(1);
		}
	}
	void crockchip_encoder::_work_thread()
	{
		MppPacket packet = NULL;
		void *ptr;
		size_t len;


		static FILE *out_file_ptr = NULL;
		if (!out_file_ptr)
		{
			out_file_ptr = fopen("chensong.h264", "wb+");
		}
		int32_t   frame_count = 0;
		int32_t    frm_pkt_cnt = 0;
		while (!m_stoped)
		{
			char log_buf[1024] = {0};
			RK_S32 log_size = sizeof(log_buf) - 1;
			RK_S32 log_len = 0;
			int32_t ret = m_mpi->encode_get_packet(m_ctx, &packet);
			if (ret || NULL == packet) 
			{
				//msleep(1);
				continue;
			}

			// write packet to file here
			ptr = mpp_packet_get_pos(packet);
			len = mpp_packet_get_length(packet);
			if (out_file_ptr)
			{
				::fwrite(ptr, 1, len, out_file_ptr);
				::fflush(out_file_ptr);
			}
			log_len += snprintf(log_buf + log_len, log_size - log_len,
				"encoded frame %-4d", ++frame_count);

			/* for low delay partition encoding */
			if (mpp_packet_is_partition(packet)) 
			{
				auto eoi = mpp_packet_is_eoi(packet);

				log_len += snprintf(log_buf + log_len, log_size - log_len,
					" pkt %d", frm_pkt_cnt);
				frm_pkt_cnt = (eoi) ? (0) : (frm_pkt_cnt + 1);
			}

			log_len += snprintf(log_buf + log_len, log_size - log_len,
				" size %-7zu", len);


			/*if (mpp_packet_has_meta(packet))
			{
				MppMeta meta = mpp_packet_get_meta(packet);
				MppFrame frm = NULL;
				RK_S32 temporal_id = 0;
				RK_S32 lt_idx = -1;
				RK_S32 avg_qp = -1;

				if (MPP_OK == mpp_meta_get_s32(meta, KEY_TEMPORAL_ID, &temporal_id))
					log_len += snprintf(log_buf + log_len, log_size - log_len,
						" tid %d", temporal_id);

				if (MPP_OK == mpp_meta_get_s32(meta, KEY_LONG_REF_IDX, &lt_idx))
					log_len += snprintf(log_buf + log_len, log_size - log_len,
						" lt %d", lt_idx);

				if (MPP_OK == mpp_meta_get_s32(meta, KEY_ENC_AVERAGE_QP, &avg_qp))
					log_len += snprintf(log_buf + log_len, log_size - log_len,
						" qp %d", avg_qp);

				if (MPP_OK == mpp_meta_get_frame(meta, KEY_INPUT_FRAME, &frm)) {
					MppBuffer frm_buf = NULL;

					mpp_assert(frm);
					frm_buf = mpp_frame_get_buffer(frm);

					if (frm_buf) {
						mpp_mutex_cond_lock(&list_buf->cond_lock);
						mpp_list_add_at_tail(list_buf, &frm_buf, sizeof(frm_buf));
						mpp_list_signal(list_buf);
						mpp_mutex_cond_unlock(&list_buf->cond_lock);
					}

					mpp_frame_deinit(&frm);
				}
			}


*/

			printf( "%s\n", log_buf);

			mpp_packet_deinit(&packet);
		}
	}
}