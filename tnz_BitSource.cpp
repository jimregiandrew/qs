/*
 * tnz_BitSource.cpp
 *
 *  Created on: 7/05/2015
 *      Author: jim
 */

#include "tnz_BitSource.h"

#include <vector>

using std::vector;

namespace tnz {

BitSource::BitSource(std::shared_ptr<ByteSource> byteSource)
	: byteSource(byteSource),
	  bitBuf(0),
	  bitOffset(0),
	  availableBits(0),
	  byteOffset(0),  // Start of used part of byteBuf
	  bufferLen(0)
{
}

/*!
 */
void BitSource::getBytes()
{
	const vector<uint8_t>& bytes = byteSource->getBytes();
	byteBuf.insert(byteBuf.end(), bytes.begin(), bytes.end());
    if (availableBits < 32)
        updateBitBuf();
    availableBits = (byteBuf.size() << 3) - bitOffset;

    // If this fails, then the caller has probably consumed beyond the available bits.
    if (availableBits < 0)
        throw std::logic_error("availableBits < 0");
}

/*!
 * Consume the specified number of bits, updating the bit byteBuf.
 */
void BitSource::consume(int size)
{
    if (size > availableBits) {
        getBytes();
    }
    bitOffset += size;
    int byteAdvance = bitOffset >> 3;
    if (byteAdvance != 0) {
        bitOffset &= 0x07; // Keep bitOffset in range [0,7]
        byteOffset += byteAdvance;

          // Ensure we are looking ahead at least 4 bytes, so we can fully populate our bit byteBuf.
        int lookingAhead = (int)byteBuf.size() - byteOffset;
        if (lookingAhead < 4) {
            byteOffset = 0;
            bufferLen = 0;
            availableBits = 0;  // ensure updateBitBuf() gets called
        }
        else {
            availableBits -= size;
            // Caller must make sure not consuming more bits than are actually available otherwise this assertion will fail.
            if (availableBits < 0)
                throw std::logic_error("availableBits < 0");
            updateBitBuf();
        }
    }
    else {
        availableBits -= size;
        // Caller must make sure not consuming more bits than are actually available otherwise this assertion will fail.
        if (availableBits < 0)
            throw std::logic_error("availableBits < 0");
        bitBuf <<= size;
    }
}

} // namespace tnz
