/*
 * HuffmanDecoder.h
 *
 *  Created on: 7/05/2015
 *      Author: jim
 */

#ifndef HUFFMANDECODER_H_
#define HUFFMANDECODER_H_

#include "HuffmanTable.h"

namespace tnz {

class BitSource;

class HuffmanDecoder
{
    public:
        HuffmanDecoder(const HuffmanTable& huffTable);

        static const int HUFF_NEED_MORE_BITS = -1;
        int decodeSymbol(BitSource& bitSource);

      private:
        int decodeLongCode(BitSource& bit_source);
        void generateLuts(const HuffmanTable& huffTable);
        void fillBitBuffer(BitSource& bitSource);

      private:
        HuffmanTable huffTable;
        static const int HUFF_LOOKAHEAD=8;
        int numBitsLut[1 << HUFF_LOOKAHEAD]; /* # bits, or 0 if too long */
        uint8_t symbolLut[1 << HUFF_LOOKAHEAD]; /* symbol, or unused */
        int32_t symOffset[HUFF_MAX_CODE_LENGTH + 2];
        int32_t maxCode[HUFF_MAX_CODE_LENGTH + 2];
};

} // namespace tnz

#endif /* HUFFMANDECODER_H_ */
