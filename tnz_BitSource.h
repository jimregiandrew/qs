/*
 * tnz_BitSource.h
 *
 *  Created on: 7/05/2015
 *      Author: jim
 */

#ifndef TNZ_BITSOURCE_H_
#define TNZ_BITSOURCE_H_

#include <memory>
#include <vector>

namespace tnz {

class ByteSource;

class BitSource
{
public:
	BitSource(std::shared_ptr<ByteSource> byteSource);

    inline int getAvailableBits() {
        return availableBits;
    }
    inline uint32_t peek(int size) {
        if (size == 0)
            return 0;
        else
            return bitBuf >> (32-size);
    }
	void consume(int numBits);

private:
    inline void updateBitBuf()
    {
        bitBuf =
            (((uint32_t)byteBuf[byteOffset]   << 24) |
             ((uint32_t)byteBuf[byteOffset+1] << 16) |
             ((uint32_t)byteBuf[byteOffset+2] <<  8) |
              (uint32_t)byteBuf[byteOffset+3]) << bitOffset;
    }
    void getBytes();

private:
	std::shared_ptr<ByteSource> byteSource;
	std::vector<uint8_t> byteBuf;
    uint32_t bitBuf;
    int bitOffset;
    int availableBits;
    int byteOffset;  // Start of used part of byteBuf
    int bufferLen;     // Amount of data in the byteBuf. Used length is bufferLen - byteOffset.
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
