
/***********************************************************************************************
created: 		2025-06-15

author:			chensong

purpose:		_C_ROCKCHIP_DECDER_H_
************************************************************************************************/

#ifndef _C_RAW_PACKET_H_
#define _C_RAW_PACKET_H_

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
namespace dsp
{
	class craw_packet
	{
	public:
		craw_packet(const unsigned char* _data, const size_t _size = 0, int64_t _pts = 0, int64_t _dts = 0)
			:data_(_data, _data + _size), pts_(_pts), dts_(_dts)  {};
		const unsigned char* data() const noexcept { return data_.data(); }
		size_t size() const noexcept { return data_.size(); }
		int64_t  pts() const noexcept { return pts_; }
		int64_t  dts() const noexcept { return dts_; }
	private:
		std::vector<unsigned char> data_;
		int64_t		pts_; 
		int64_t     dts_;
	};
}


#endif // 