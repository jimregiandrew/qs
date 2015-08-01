/*
 * Decoders.cpp
 *
 *  Created on: 21/05/2015
 *      Author: jim
 */

#include "qs_BitSource.h"
#include "Decoders.h"
#include "HuffmanDecoder.h"
#include "HuffmanTable.h"

#include <limits.h>

#include <memory>
#include <sstream>

namespace qs {

class SizeIntDecoder : public IntDecoder
{
public:
	SizeIntDecoder(std::shared_ptr<HuffmanDecoder>);
	virtual ~SizeIntDecoder() {}

    virtual int decode(BitSource& bitSource, Run& out);

private:
    std::shared_ptr<HuffmanDecoder> huffDecoder;
    static const int SIZE_NOT_SAVED = -1;
    int savedSize;
};


SizeIntDecoder::SizeIntDecoder(std::shared_ptr<HuffmanDecoder> huffDecoder)
	: huffDecoder(huffDecoder),
	  savedSize(SIZE_NOT_SAVED)
{
}

int SizeIntDecoder::decode(BitSource& bitSource, Run& val)
{
	/*
	 * Decode number of bits needed for the size (num bits) of the coefficient. Then
	 * get that number of bits and work out actual value
	 */
	int size;
	if (savedSize >= 0)
		size = savedSize;
	else
		size = huffDecoder->decode(bitSource);
	if (size == HuffmanDecoder::HUFF_NEED_MORE_BITS)
		return HuffmanDecoder::HUFF_NEED_MORE_BITS;

	if (size == sizeof(int) * 8) { // Decode special case of 32 bit INT_MIN
		val = INT_MIN;
		savedSize = SIZE_NOT_SAVED;
		return HuffmanDecoder::HUFF_DECODING_OK;
	}
	//Check if we have enough bits to get the remaining bits for amp. If not fill
	//bitBuf (hopefully leaving no bytes in byte buffer), save size and return.
	if (bitSource.getAvailableBits() < size) {
		savedSize = size;
		return HuffmanDecoder::HUFF_NEED_MORE_BITS;
	}

	// o.k. we have enough bits to decode amp. The remaining bits store the
	// value of amp
	int nlsbs = size;
	if (size > 31) { // Caught INT32_MIN case (size = 32) above
		std::ostringstream oss;
		oss<<"SizeIntDecoder::decode: size="<<size<<" is more than the maximum of 31 bits";
		throw std::logic_error(oss.str());
	}
	int temp = 0x0;
	if (size > 16) { // Decode in two chunks
		temp = bitSource.peek(size - 16) << 16;
		bitSource.consume(size - 16);
		nlsbs = 16;
	}
	temp |= bitSource.peek(nlsbs);
	bitSource.consume(nlsbs);
	int thresh = 1 << (size - 1);
	if (temp < thresh)
		temp = ((-1) << size) + temp + 1;

	savedSize = SIZE_NOT_SAVED;
	val = Run(0, temp);
	return HuffmanDecoder::HUFF_DECODING_OK;
}

IntDecoder* getSizeIntDecoder(const HuffmanTable& table)
{
    std::shared_ptr<HuffmanDecoder> huffDecoder(new HuffmanDecoder(table));
    return new SizeIntDecoder(huffDecoder);
}

}

// huffDecoder(new HuffmanDecoder(table))
