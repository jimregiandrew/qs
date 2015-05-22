/*
 * tnz_BitSource.h
 *
 *  Created on: 7/05/2015
 *      Author: jim
 */

#ifndef TNZ_BITSOURCE_H_
#define TNZ_BITSOURCE_H_

#include <assert.h>

#include <memory>
#include <vector>

namespace tnz {

class ByteSource;

/*
 * BitSource
 *
 * A cache of the next bytes in the bitstream are stored in byteBuf. A cache of the next (up to 32)
 * bits in the bitstream are (also) stored in bitBuf. The bits are stored
 * in order from the msb in bitBuf: that is the 0th (next) bit in the bitstream
 * is in the msb of bifBuf, the 1st bit in msb-1, the 2nd bit in msb-2 etc.
 * byteOffset is a pointer into byteBuf to the first (remaining) byte in byteBuf and endOffset
 * one past the last byte. bitOffset is a pointer to the first (remaining) bit in the first byte
 * (byteOffset in byteBuf) - noting some bits in this byte may be stale - that is already consumed.
 */
class BitSource
{
public:
	BitSource(std::shared_ptr<ByteSource> byteSource);

    inline int getAvailableBits() {
        return availableBits;
    }
    void consume(int numBits);
    // peek up to 25 bits
    inline uint32_t peek(int size) {
    	assert(size <= 25); // bitBuf may not hold more than 25 valid bits - see updateBitBuf
        if (size == 0)
            return 0;
        else
            return bitBuf >> (32-size); // Next bit is bit 31 (the msb) in bitBuf
    }
    inline uint32_t pop25(int size)
    {
        assert(size <= 25);
        uint32_t value = peek(size);
        consume(size);
        return value;
    }

    uint32_t pop(int size)
    {
        if (size <= 25)
            return pop25(size);
        else {
            int extra = size - 25;
            uint32_t value = pop25(25) << extra;
            return value | pop25(extra);
        }
    }


private:
	// updateBitBuf fills at least 25 valid bits into bitBuff (assuming bitOffset <= 7)
    inline void updateBitBuf()
    {
        bitBuf =
            (((uint32_t)byteBuf[byteOffset]   << 24) |
             ((uint32_t)byteBuf[byteOffset+1] << 16) |
             ((uint32_t)byteBuf[byteOffset+2] <<  8) |
              (uint32_t)byteBuf[byteOffset+3] <<  0) << bitOffset;
    }
    void getBytes();
    void rotateRemainingBytes();

private:
	std::shared_ptr<ByteSource> byteSource;
	std::vector<uint8_t> byteBuf; // byte buffer
    uint32_t bitBuf;   // 4 byte cache of next bits
    int bitOffset;     // Offset of the next bit in the first byte in byteBuf
    int availableBits; // Number of bits left in bitstream
    int byteOffset;    // Pointer to first (current) byte in byteBuf
    int endOffset;     // Pointer to one past the last byte in byteBuf
    // (i.e. byteBuf[byteOffset], byteBuf[byteOffset+1],.., byteBuf[endOffset-1] are the current bytes in byteBuf
};

class ByteSource
{
public:
	virtual ~ByteSource() {}

    virtual const std::vector<uint8_t>& getBytes() = 0;
};

class ByteBuffer : public ByteSource
{
public:
	ByteBuffer(const std::vector<uint8_t> buf) : buf (buf), accessCount(0) {}
    virtual const std::vector<uint8_t>& getBytes() { return accessCount++ > 0 ? emptyBuf : buf; }

private:
    std::vector<uint8_t> buf;
    std::vector<uint8_t> emptyBuf;
    int accessCount;
};

} // namespace tnz

#endif /* TNZ_BITSOURCE_H_ */
