#include "HuffmanDecoder.h"
#include "tnz_BitSource.h"

#include <cstring>

#include <stdexcept>

namespace tnz {

HuffmanDecoder::HuffmanDecoder(const HuffmanTable& huffTable)
	: huffTable(huffTable)
{
    generateLuts(huffTable);
}

/*
 * decodeSymbol
 *
 * Decodes a symbol from bitSource, removing the decoded bits.
 */
int HuffmanDecoder::decodeSymbol(BitSource& bitSource)
{
    register int numBits;
    register int look;

    /*
     * Try to do a lookup decoding, and if this fails use the slower (but
     * complete) decoding method.
     */
    register int avail = bitSource.getAvailableBits();
    if (avail == 0)
        return HUFF_NEED_MORE_BITS;

    if (avail >= HUFF_LOOKAHEAD) {
        look = bitSource.peek(HUFF_LOOKAHEAD);
        numBits = numBitsLut[look];
    }
    if (numBits == 0) // avail < HUFF_LOOKAHEAD or numBitsLut[look] = 0
        return decodeLongCode(bitSource);

    // We have a valid short code.
    bitSource.consume(numBits);
    return symbolLut[look];
}

/*
 * decodeLongCode method
 *
 * Decodes a symbol corresponding to a long code (whose code is longer than
 * HUFF_LOOKAHEAD) from bitSource. Throws an exception when an unknown code is found.
 */
int HuffmanDecoder::decodeLongCode(BitSource& bitSource)
{
    int len = HUFF_LOOKAHEAD + 1;
    if (bitSource.getAvailableBits() < len)
        return HUFF_NEED_MORE_BITS;  // Need more data
    int code = bitSource.peek(len);
    while (code > maxCode[len]) {
        len++;
        if (len > HUFF_MAX_CODE_LENGTH) {
            throw std::logic_error("HuffmanDecoder: Invalid code");
        }
        if (bitSource.getAvailableBits() < len)
            return HUFF_NEED_MORE_BITS; // Need more data
        code = bitSource.peek(len);
    }

    /*
     * o.k. we have found a valid code. Remove bits and return symbol
     */
    bitSource.consume(len);
    return huffTable.symbol[(int)(code + symOffset[len])];
}

/*!
 * Generates the LUTs used for decoding (numBitsLut, symbolLut for codes that
 * are 8 numCodes or less, and symOffset and maxCode for decoding longer codes)
 * huffCodeLen[] and huffCode[] are filled in code-length order.
 */
void HuffmanDecoder::generateLuts(const HuffmanTable& huffTable)
{
    uint8_t huffCodeLen[HUFF_MAX_NUMBER_SYMBOLS + 1];
    int huffCode[HUFF_MAX_NUMBER_SYMBOLS + 1];
    int numSymbols = makeCodeAndLengthTables(huffCode, huffCodeLen, huffTable);

   /*
    * Figure F.15 of JPEG standard: generate decoding tables for bit-sequential
    * decoding
    */
    const uint8_t* numCodes = huffTable.numCodes;
    int p, i, len; // si, numsymbols;
    p = 0;
    for (len = 1; len <= HUFF_MAX_CODE_LENGTH; len++) {
        if (numCodes[len]) {
            // symOffset[len] = symbol[] index of 1st symbol of code length len minus the minimum code of length len
            symOffset[len] = (int32_t) p - (int32_t) huffCode[p];
            p += numCodes[len];
            maxCode[len] = huffCode[p-1]; /* maximum code of length len */
        }
        else {
            maxCode[len] = -1;	/* -1 if no codes of this length */
        }
    }
    maxCode[HUFF_MAX_CODE_LENGTH + 2] = 0xFFFFF; /* ensures decoding terminates - ToDo, handle codeword of all ones*/

    /* Compute lookahead tables to speed up decoding.
     * First we set all the table entries to 0, indicating "too long for lookup
     * decoding"; then we iterate through the Huffman codes that are short
     * enough and fill in all the entries that correspond to bit sequences
     * starting with that code.
     */
    memset(numBitsLut, 0, sizeof(numBitsLut));
    p = 0;
    for (len = 1; len <= HUFF_LOOKAHEAD; len++) {
        for (i = 1; i <= (int) numCodes[len]; i++, p++) {
            /* len = current code's length, p = its index in huffCode[] & symbol[]. */
            /* Generate left-justified code followed by all possible bit sequences */
            int lutBits = huffCode[p] << (HUFF_LOOKAHEAD - len);
            for (int ctr = 1 << (HUFF_LOOKAHEAD - len); ctr > 0; ctr--) {
              	numBitsLut[lutBits] = len;
              	symbolLut[lutBits] = huffTable.symbol[p];
              	lutBits++;
            }
        }
    }
    // ToDo: We should validate symbols as being reasonable (as in IJG code) ...
}

} //namespace tnz
