/*
 * BitSink.cpp
 *
 *  Created on: 16/04/2015
 *      Author: jim
 */

#include "BitSink.h"

#include <cassert>
#include <memory>

namespace tnz {

BitSink::BitSink(std::shared_ptr<ByteSink> byteSink)
	: out(byteSink),
	  bitBuf(0),
	  queuedBits(0),
	  queuedBytes(0),
	  byteTally(0)
{
}

BitSink::~BitSink()
{
	flush();
}

void BitSink::receive(uint32_t code, int size)
{
	assert(size > 0 && size <= 32);

	code &= (((uint32_t) 1) << size) - 1; /* mask off any extra bits in code */
	int putBits = queuedBits + size;     /* new number of bits in buffer */
	code <<= 32 - putBits; /* align incoming bits */
	code |= bitBuf; /* and merge with old put buffer contents */
	while (putBits >= 8) {
		uint8_t c = (uint8_t) (code >> 24);
		emitByte(c);
		code <<= 8;
		putBits -= 8;
	}
	bitBuf = code; /* update state variables */
	queuedBits = putBits;
}

void BitSink::flush()
{
	if (queuedBytes > 0) {
		byteTally += queuedBytes;
		out->receive((const uint8_t*)byteBuf, queuedBytes);
		queuedBytes = 0;
	}
}

void BitSink::emitByte(uint8_t c)
{
    byteBuf[queuedBytes++] = c;
    if (queuedBytes == sizeof(byteBuf))
        flush();
}

void BitSink::close()
{
	receive(0xFF, 8 - queuedBits);
	flush();
}

}
