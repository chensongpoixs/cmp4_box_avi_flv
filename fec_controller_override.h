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
				   date:  2025-09-25



 ******************************************************************************/


// This is an EXPERIMENTAL interface.

#ifndef _C_API_FEC_CONTROLLER_OVERRIDE_H_
#define _C_API_FEC_CONTROLLER_OVERRIDE_H_

namespace libmedia_codec {

// Interface for temporarily overriding FecController's bitrate allocation.
class FecControllerOverride {
 public:
  virtual void SetFecAllowed(bool fec_allowed) = 0;

 protected:
  virtual ~FecControllerOverride() = default;
};

}  // namespace webrtc

#endif  // API_FEC_CONTROLLER_OVERRIDE_H_
