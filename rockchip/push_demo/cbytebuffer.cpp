/***********************************************************************************************
created: 		2025-06-15

author:			chensong

purpose:		_C_ROCKCHIP_DECDER_H_
************************************************************************************************/
#include "cbytebuffer.h"

namespace dsp
{

	namespace bytebuffer
	{
		int32_t writedata(void* buffer, void* value, uint32_t size)
		{
			memcpy(buffer, value, size);
			return size;
		}
		int32_t write8(void* buffer, uint8_t value)
		{
			writedata(buffer, (void*)&value, sizeof(uint8_t));
			return 1;
		}
		int32_t write16(void* buffer, uint16_t value)
		{
			
			write8(buffer, (uint8_t)(value>>8));
			write8(buffer, (uint8_t)value);
			return 2;

		}
		int32_t write24(void* buffer, uint32_t value)
		{
			
			write16(buffer, (uint16_t)(value >> 8));
			write8(buffer, (uint8_t)value);
			return 3;
		}
		int32_t write32(void* buffer, uint32_t value)
		{
			write16(buffer, (uint16_t)(value >> 16));
			write16(buffer, (uint16_t)value);
			return 4;
		}
		int32_t write64(void* buffer, uint64_t value)
		{
			write32(buffer, (uint32_t)(value >> 32));
			write32(buffer, (uint32_t)value);
			return 8;
		}
		int32_t writefloat(void* buffer, float value)
		{
			write32(buffer, *(uint32_t*)&value);
			return 4;
		}
		int32_t writedouble(void* buffer, double value)
		{
			write64(buffer, *(uint64_t*)&value);
			return 8;
		}

	}


}