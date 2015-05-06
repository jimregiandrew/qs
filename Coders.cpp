/*
 * Coders.cpp
 *
 *  Created on: 6/05/2015
 *      Author: jim
 */

/*
 * Int32Coder.cpp
 *
 *  Created on: 17/10/2014
 *      Author: jim
 */

#include <string>
#include <vector>

#include "BitSink.h"
#include "HuffmanTable.h"

class HuffmanCoder;

using std::shared_ptr;
using std::string;
using std::vector;

namespace tnz {

/*******************************************************************************
 *
 *
 *
 *
 *
 * Magnitude huffman coding
 *
 *
 *
 *
 *
 ******************************************************************************/
class MagInt32Coder : public Int32CoderCounter
{
    public:
        MagInt32Coder(shared_ptr<BitSink> bitSink, HuffmanTable* table);
        virtual ~MagInt32Coder();

        virtual void code(int32_t val);
        virtual const std::vector<int>& getCounts() { return counts; }
        virtual void flush() { /* nothing to do*/}


    private:
        std::vector<int> counts;
        shared_ptr<BitSink> bitSink;
        shared_ptr<HuffmanCoder> huffCoder;
        int nlsbs;
};

MagInt32Coder::MagInt32Coder(shared_ptr<BitSink> bitSink, HuffmanTable* table)
    : counts(vector<int>(HUFF_MAX_NUMBER_SYMBOLS, 0)),
      bitSink(bitSink),
      huffCoder(new HuffmanCoder(table)),
      nlsbs(0)
{
}

MagInt32Coder::~MagInt32Coder()
{
    if (tracer.tracing(DIAG))
        tracer(DIAG)<<"Number lsbs bytes="<<nlsbs/8<<endl;
}

void MagInt32Coder::code(int32_t val)
{
    MagBits v = getMagBits(val);
    huffCoder->encodeSymbol(bitSink, v.mag);
    counts[v.mag]++;
    if (v.mag) {
        bitSink->receiveBitsLong(v.bits, v.mag);
        if (tracer.tracing(DIAG))
            nlsbs += v.mag;
    }
}
Int32CoderCounter* newMagInt32Coder(shared_ptr<BitSink> bitSink, HuffmanTable* table)
{
    return new MagInt32Coder(bitSink, table);
}

/*******************************************************************************
 *
 *
 *
 *
 *
 * Run-length magnitude/bits coding
 *
 *
 *
 *
 *
 ******************************************************************************/
struct RunLengthTwosComplement {
    int run;   // Number of preceding zero elements
    int nbits; // Number of bits in the two's complement representation
    int bits;  // Two's complement
    RunLengthTwosComplement(int run=0, int nbits=0, int bits=0) : run(run), nbits(nbits), bits(bits) {}
};

class RLMagInt32Coder : public virtual Int32CoderCounter
{
    public:
        RLMagInt32Coder(shared_ptr<BitSink> bitSink, HuffmanTable* table);
        virtual ~RLMagInt32Coder() {}

        virtual void code(int32_t val);
        virtual const std::vector<int>& getCounts() { return counts; }
        virtual void flush();

    private:
        void codeRl2c(int32_t val);

    private:
        std::vector<int> counts;
        shared_ptr<BitSink> bitSink;
        shared_ptr<HuffmanCoder> huffCoder;
        int runLength;
        RunLengthTwosComplement rl2c;
};

RLMagInt32Coder::RLMagInt32Coder(shared_ptr<BitSink> bitSink, HuffmanTable* table)
    : counts(vector<int>(HUFF_MAX_NUMBER_SYMBOLS, 0)),
      bitSink(bitSink),
      huffCoder(new HuffmanCoder(table)),
      runLength(0)
{
}

static RunLengthTwosComplement getRL2C(const RunLengthTwosComplement& in, int32_t val)
{
    RunLengthTwosComplement rl2c = in;
    if (val == 0) {
        rl2c.run++;
        if (rl2c.run >= 65536)
            throw IOException("Can't handle run-length >= 25536 for now. ToDo: handle this :-)");
    }
    else {
        MagBits magBits = getMagBits(val);
        rl2c.nbits = magBits.mag;
        rl2c.bits  = magBits.bits;
    }
    return rl2c;
}

void RLMagInt32Coder::code(int32_t val)
{
    rl2c = getRL2C(rl2c, val);
    if (rl2c.nbits <= 0)
        return;
    codeRl2c(val);
}

void RLMagInt32Coder::codeRl2c(int32_t val)
{
    MagBits rl = getMagBits(rl2c.run);
    if (rl.mag > 15)
        throw IOException("runLength > 15! This should be impossible :-)");
    int vNBits = rl2c.nbits;
    if (vNBits > 15)
        vNBits = 15;
    uint8_t rlMagCode = (rl.mag<<4) | vNBits;
    huffCoder->encodeSymbol(bitSink, rlMagCode);
    counts[rlMagCode]++;
    if (rl.mag > 1)
        bitSink->receiveBits(rl.bits, rl.mag - 1); // Unsigned means we need bottom bits only
    if (vNBits > 0) {
        if (vNBits < 15)
            bitSink->receiveBits(rl2c.bits, vNBits);
        else
            bitSink->receiveBitsLong(val, 32);
    }
    rl2c = RunLengthTwosComplement(); // reset run-state for next input
}


void RLMagInt32Coder::flush()
{
    if (rl2c.run > 0) {
        tracer(INFO)<<"Flushing zero run: run="<<rl2c.run<<endl;
        rl2c.run--;
        rl2c.nbits = 0;
        rl2c.bits = 0;
        codeRl2c(0);
    }
}

Int32CoderCounter* newRLMagInt32Coder(shared_ptr<BitSink> bitSink, HuffmanTable* table)
{
    return new RLMagInt32Coder(bitSink, table);
}

/*******************************************************************************
 *
 *
 *
 *
 *
 * Decoding
 *
 *
 *
 *
 *
 ******************************************************************************/
class MagInt32Decoder : public Int32Decoder
{
    public:
        MagInt32Decoder(shared_ptr<BitSource> bitSource, shared_ptr<HuffmanDecoder> huffmanDecoder);
        virtual ~MagInt32Decoder() {}

        virtual int decode(Run& out);

    private:
        shared_ptr<BitSource> bitSource;
        shared_ptr<HuffmanDecoder> huffmanDecoder;
        int nbitsSaved;
};

MagInt32Decoder::MagInt32Decoder(shared_ptr<BitSource> bitSource, shared_ptr<HuffmanDecoder> huffmanDecoder)
	: bitSource(bitSource),
	  huffmanDecoder(huffmanDecoder),
	  nbitsSaved(-1)
{
}

int MagInt32Decoder::decode(Run& val)
{
	/*
	 * Decode number of bits needed for the magnitude of the coefficient. Then
	 * get that number of bits and work out actual value
	 */
	int nbits;
	if (nbitsSaved >= 0) {
		nbits = nbitsSaved;
	}
	else
		nbits = huffmanDecoder->decodeSymbol(bitSource);
	if (nbits == HUFF_NEED_MORE_BITS)
		return HUFF_NEED_MORE_BITS;

	if (nbits == sizeof(int) * 8) { // Decode special case of 32 bit INT_MIN
		val = INT_MIN;
		nbitsSaved = -1;
		return HUFF_DECODING_OK;
	}
	// Check if we have enough bits to get the remaining bits for val. If not
	// fill bitBuf (hopefully leaving no bytes in byte buffer), save nbits
	//and return.
	if (bitSource->getAvailableBits() < nbits) {
		nbitsSaved = nbits;
		return HUFF_NEED_MORE_BITS;
	}

	// o.k. we have enough bits to decode val. The remaining bits store the
	// value of val in a 2's compliment type of form.
	int nlsbs = nbits;
	if (nbits > 31) { // Caught INT32_MIN case (nbits = 32) above
		char error_msg[60];
		sprintf(error_msg, "decodeMagBits: num bits (%d) too large\n", nbits);
		throw HuffmanException(error_msg);
	}
	int temp2 = 0x0;
	if (nbits > 16) { // Decode in two chunks
		temp2 = bitSource->peekBits(nbits - 16) << 16;
		bitSource->consumeBits(nbits - 16);
		nlsbs = 16;
	}
	temp2 |= bitSource->peekBits(nlsbs);
	bitSource->consumeBits(nlsbs);
	int thresh = 1 << (nbits - 1);
	if (temp2 < thresh)
		temp2 = ((-1) << nbits) + temp2 + 1;

	nbitsSaved = -1; // Indicate the nbits not saved
	val = Run(0, temp2);
	return HUFF_DECODING_OK;
}

Int32Decoder* newMagInt32Decoder(shared_ptr<BitSource> bitSource, HuffmanTable* table)
{
    shared_ptr<HuffmanDecoder> huffDecoder = new HuffmanDecoder(table);
    return new MagInt32Decoder(bitSource, huffDecoder);
}

class RLMagInt32Decoder : public Int32Decoder
{
    public:
        RLMagInt32Decoder(shared_ptr<BitSource> bitSource, shared_ptr<HuffmanDecoder> huffmanDecoder);
        virtual ~RLMagInt32Decoder() {}

        virtual int decode(Run& out);

    private:
        shared_ptr<BitSource> bitSource;
        shared_ptr<HuffmanDecoder> huffmanDecoder;
        int rlMagCodeSaved;
        int rNBitsSaved;
        int vNBitsSaved;
};

RLMagInt32Decoder::RLMagInt32Decoder(shared_ptr<BitSource> bitSource, shared_ptr<HuffmanDecoder> huffmanDecoder)
    : bitSource(bitSource),
      huffmanDecoder(huffmanDecoder),
      rlMagCodeSaved(-1),
      rNBitsSaved(-1),
      vNBitsSaved(-1)
{
}

int RLMagInt32Decoder::decode(Run& outRun)
{
    /*
     * Decode number of bits needed for the magnitude of the coefficient. Then
     * get that number of bits and work out actual value
     */
    int rlMagCode;
    if (rlMagCodeSaved >= 0) {
        rlMagCode = rlMagCodeSaved;
    }
    else {
        rlMagCode = huffmanDecoder->decodeSymbol(bitSource);
        if (rlMagCode == HUFF_NEED_MORE_BITS)
            return HUFF_NEED_MORE_BITS;
    }

    int rNBits = (rlMagCode >> 4) & 0x0F;
    int vNBits = (rlMagCode >> 0) & 0x0F;
    // Check if we have enough bits to get the remaining bits
    if (bitSource->getAvailableBits() < (rNBits - 1) + vNBits) {
        rlMagCodeSaved = rlMagCode;
        return HUFF_NEED_MORE_BITS;
    }

    // o.k. we have enough bits to decode the run-length bits and value bits
    int run = rNBits == 1 ? 1 : 0;
    if (rNBits > 1)
        run = bitSource->popBits(rNBits - 1) + (1 << (rNBits - 1));
    int32_t val;
    if (vNBits < 15) {
        val = bitSource->popBits(vNBits);
        int thresh = 1 << (vNBits - 1);
        if (val < thresh)
            val = ((-1) << vNBits) + val + 1;
    }
    else {
        val = (int32_t)bitSource->popBitsLong(32);
    }
    outRun = Run(run, val);
    return HUFF_DECODING_OK;
}

Int32Decoder* newRLMagInt32Decoder(shared_ptr<BitSource> bitSource, HuffmanTable* table)
{
    shared_ptr<HuffmanDecoder> huffDecoder = new HuffmanDecoder(table);
    return new RLMagInt32Decoder(bitSource, huffDecoder);
}

} // namespace tnz
