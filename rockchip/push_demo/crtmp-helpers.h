/***********************************************************************************************
created: 		2025-06-15

author:			chensong

purpose:		rtmp_push
************************************************************************************************/

#ifndef _C_RTMP_HELPER_H_
#define _C_RTMP_HELPER_H_

#include "librtmp/rtmp.h"
#include <stdio.h>
#include <string.h>
namespace dsp
{

static inline AVal *flv_str(AVal *out, const char *str)
{
	out->av_val = (char *)str;
	out->av_len = (int)strlen(str);
	return out;
}

static inline void enc_num_val(char **enc, char *end, const char *name, double val)
{
	AVal s;
	*enc = AMF_EncodeNamedNumber(*enc, end, flv_str(&s, name), val);
}

static inline void enc_bool_val(char **enc, char *end, const char *name, bool val)
{
	AVal s;
	*enc = AMF_EncodeNamedBoolean(*enc, end, flv_str(&s, name), val);
}

static inline void enc_str_val(char **enc, char *end, const char *name, const char *val)
{
	AVal s1, s2;
	*enc = AMF_EncodeNamedString(*enc, end, flv_str(&s1, name), flv_str(&s2, val));
}

static inline void enc_str(char **enc, char *end, const char *str)
{
	AVal s;
	*enc = AMF_EncodeString(*enc, end, flv_str(&s, str));
}


}
#endif // _C_RTMP_HELPER_H_