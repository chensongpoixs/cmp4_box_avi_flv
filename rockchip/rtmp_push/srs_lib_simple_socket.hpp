 

#ifndef SRS_LIB_SIMPLE_SOCKET_HPP
#define SRS_LIB_SIMPLE_SOCKET_HPP

/*
#include <srs_lib_simple_socket.hpp>
*/

#include "srs_librtmp.h"
#include "srs_rtmp_io.hpp"

// for srs-librtmp, @see https://github.com/ossrs/srs/issues/213
#ifndef _WIN32
    #define SOCKET int
#endif

#ifndef _WIN32
#include <sys/uio.h>
#endif

/**
* simple socket stream,
* use tcp socket, sync block mode, for client like srs-librtmp.
*/
class SimpleSocketStream : public ISrsProtocolReaderWriter
{
public:
	SimpleSocketStream(){};
	virtual ~SimpleSocketStream(){};
public:
    virtual srs_hijack_io_t hijack_io() = 0;
	virtual int create_socket() = 0;
	virtual int connect(const char* server, int port) = 0;
	virtual int disconnect() = 0;
// ISrsBufferReader
public:
	virtual int read(void* buf, size_t size, ssize_t* nread) = 0;
// ISrsProtocolReader
public:
	virtual void set_recv_timeout(int64_t timeout_us) = 0;
	virtual int64_t get_recv_timeout() = 0;
	virtual int64_t get_recv_bytes() = 0;
// ISrsProtocolWriter
public:
	virtual void set_send_timeout(int64_t timeout_us) = 0;
	virtual int64_t get_send_timeout() = 0;
	virtual int64_t get_send_bytes() = 0;
	virtual int writev(const iovec *iov, int iov_size, ssize_t* nwrite) = 0;
// ISrsProtocolReaderWriter
public:
	virtual bool is_never_timeout(int64_t timeout_us) = 0;
	virtual int read_fully(void* buf, size_t size, ssize_t* nread) = 0;
	virtual int write(void* buf, size_t size, ssize_t* nwrite) = 0;
};

#endif

