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
	  byteBuf(vector<uint8_t>(4,0)), // Needs to be at least 4 bytes long so that updateBitBuf doens't access invalid memory.
	  bitBuf(0),
	  bitOffset(0),
	  availableBits(0),
	  byteOffset(0),  // Start of used part of byteBuf
	  endOffset(0)
{
	getBytes();
}

// Put any remaining bytes in byteBuf at the start
void BitSource::rotateRemainingBytes()
{
	int remainingBytes = endOffset - byteOffset;
	int n = 0;
	while(remainingBytes > 0) {
		byteBuf[n] = byteBuf[endOffset - remainingBytes];
		n++;
		remainingBytes--;
	}
	byteOffset = 0;
	endOffset = n;
}

/*!
 */
void BitSource::getBytes()
{
	rotateRemainingBytes();
	const vector<uint8_t>& bytes = byteSource->getBytes();
	byteBuf.insert(byteBuf.begin() + endOffset, bytes.begin(), bytes.end());
	endOffset += bytes.size();
    if (availableBits < 32)
        updateBitBuf();
    availableBits = ((endOffset - byteOffset) << 3) - bitOffset;

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
    if (size > 25) {
    	int extra = size - 25;
    	consume(extra);
    	size = 25;
    }
    bitOffset += size;
	availableBits -= size;
    int byteAdvance = bitOffset >> 3;
    if (byteAdvance != 0) {
        bitOffset &= 0x07; // Keep bitOffset in range [0,7]
        byteOffset += byteAdvance;
        // Ensure we are looking ahead at least 4 bytes, if possible, so we can fully populate our bit byteBuf.
        int lookingAhead = (int)endOffset - byteOffset;
        if (lookingAhead < 4) {
        	getBytes(); // pull model
        }
		updateBitBuf();
    }
    else {
        bitBuf <<= size;
    }
	// Caller must make sure not consuming more bits than are actually available otherwise this assertion will fail.
	if (availableBits < 0)
		throw std::logic_error("availableBits < 0");
}

} // namespace tnz
