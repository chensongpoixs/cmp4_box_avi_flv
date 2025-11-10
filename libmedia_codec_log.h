

/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Please visit https://chensongpoixs.github.io for detail
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 ******************************************************************************/
 /*****************************************************************************
				   Author: chensong
				   date:  2025-11-09



 ******************************************************************************/

#ifndef _LIBMEDIA_CODEC_LOG_H_
#define _LIBMEDIA_CODEC_LOG_H_
#include "rtc_base/logging.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// libmedia_codec mobule
#define  LIBMEIDA_CODEC_LOG(sev)  RTC_LOG(sev)			<< "[libmedia_codec]"
#define  LIBMEIDA_CODEC_LOG_F(sev)  RTC_LOG_F(sev)		<< "[libmedia_codec]"
#define  LIBMEIDA_CODEC_LOG_T_F(sev)  RTC_LOG_T_F(sev)	<< "[libmedia_codec]"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _LIBMEDIA_CODEC_LOG_H_