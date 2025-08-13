/***********************************************************************************************
created: 		2025-06-15

author:			chensong

purpose:		_C_ROCKCHIP_DECDER_H_
************************************************************************************************/

#ifndef _C_BYTE_BUFFER_H_
#define _C_BYTE_BUFFER_H_
#include <cstdio>
#include <cstdlib>
#include <iostream>

 
namespace dsp
{

	namespace bytebuffer
	{
		int32_t writedata(void* buffer, void * value, uint32_t size);
		int32_t write8(void* buffer, uint8_t value);
		int32_t write16(void* buffer, uint16_t value);
		int32_t write24(void* buffer, uint32_t value);
		int32_t write32(void* buffer, uint32_t value);
		int32_t write64(void* buffer, uint64_t value);
		int32_t writefloat(void* buffer, float value);
		int32_t writedouble(void* buffer, double value);
		
	}


}


#endif // _C_BYTE_BUFFER_H_