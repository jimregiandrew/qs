/*
 * Sink.h
 *
 *  Created on: 16/04/2015
 *      Author: jim
 */

#ifndef BITSINK_H_
#define BITSINK_H_

#include "stdint.h"

#include <memory>

namespace tnz {

class ByteSink
{
public:
	virtual ~ByteSink() {}
	virtual void receive(const uint8_t* data, int len) = 0;
	virtual void close() = 0;
};

class BitSink
{
public:
	BitSink(std::shared_ptr<ByteSink> byteSink);
	~BitSink();

	void receive(uint32_t code, int size);
	void flush();
	void close();

private:
    void emitByte(uint8_t c);

private:
    std::shared_ptr<ByteSink> out;
    uint32_t bitBuf;
    int queuedBits;
	static const int BITSTREAM_BYTE_BUFFER_SIZE=1024;
    uint8_t byteBuf[BITSTREAM_BYTE_BUFFER_SIZE];
    int queuedBytes;
    unsigned int byteTally;
};

}

#endif /* BITSINK_H_ */
