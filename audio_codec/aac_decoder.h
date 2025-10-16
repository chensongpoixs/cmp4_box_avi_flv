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
				   date:  2025-10-16



 ******************************************************************************/



#ifndef _C_AAC_DECODER_H_
#define _C_AAC_DECODER_H_

#include "faad.h"
#include <string>
#include "rtc_base/buffer.h"
namespace libmedia_codec
{
	class AacDecoder
	{
	public:
		AacDecoder();
		~AacDecoder();
		bool Init(const std::string & config);
		rtc::Buffer Decode(unsigned char *aac, size_t aac_size);

	public:

		NeAACDecHandle handle_;
	};
}

#endif // _C_AAC_DECODER_H_
