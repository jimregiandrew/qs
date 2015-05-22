/*
 * HuffmanTable.cpp
 *
 *  Created on: 7/05/2015
 *      Author: jim
 */

#include "HuffmanTable.h"

#include <stdint.h>

#include <stdexcept>

namespace tnz {

/*
 * Makes a default Huffman table (currently hack from the JPEG
 * default (example) tables.
 */
HuffmanTable getDefaultHuffmanTable()
{
    static const uint8_t bits[HUFF_MAX_CODE_LENGTH + 1] =
        { /* 0-base */ 0, 0, 2, 1, 3, 3, 2, 4, 3, 4, 2, 10, 0, 0, 32, 1, 0xbd };
    static const uint8_t huffval[HUFF_MAX_NUMBER_SYMBOLS] =
        { 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
          0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
          0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
          0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
          0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
          0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
          0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
          0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
          0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
          0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
          0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
          0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
          0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
          0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
          0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
          0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
          0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
          0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
          0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
          0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
          0xf9, 0xfa,

          /* Add in unused symbols from JPEG ac Huffman table */
          0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0,
          0x0b, 0x1b, 0x2b, 0x3b, 0x4b, 0x5b, 0x6b, 0x7b, 0x8b, 0x9b, 0xab, 0xbb, 0xcb, 0xdb, 0xeb, 0xfb,
          0x0c, 0x1c, 0x2c, 0x3c, 0x4c, 0x5c, 0x6c, 0x7c, 0x8c, 0x9c, 0xac, 0xbc, 0xcc, 0xdc, 0xec, 0xfc,
          0x0d, 0x1d, 0x2d, 0x3d, 0x4d, 0x5d, 0x6d, 0x7d, 0x8d, 0x9d, 0xad, 0xbd, 0xcd, 0xdd, 0xed, 0xfd,
          0x0e, 0x1e, 0x2e, 0x3e, 0x4e, 0x5e, 0x6e, 0x7e, 0x8e, 0x9e, 0xae, 0xbe, 0xce, 0xde, 0xee, 0xfe,
          0x0f, 0x1f, 0x2f, 0x3f, 0x4f, 0x5f, 0x6f, 0x7f, 0x8f, 0x9f, 0xaf, 0xbf, 0xcf, 0xdf, 0xef, 0xff
        };

    HuffmanTable table;
    for (int n = 0; n < HUFF_MAX_CODE_LENGTH + 1; n++) {
        table.numCodes[n] = bits[n];
    }
    for (int n = 0; n < HUFF_MAX_NUMBER_SYMBOLS; n++) {
        table.symbol[n] = huffval[n];
    }
    return table;
}

/*!
 * Makes values for two input arrays: huffCode and huffCodeLen. huffCode[n]
 * is the code (bit pattern if you like) for symbol n, where the symbols
 * are ordered according to code value (smallest code first). huffCodeLen[n]
 * is the length of this code.
 */
int makeCodeAndLengthTables(int *huffCode, uint8_t *huffCodeLen, const HuffmanTable& huffTable)
{
    /*
     * Figure C.1 in JPEG standard: make table of Huffman code lengths for
     * each symbol, in code value order
     */
    int p = 0;
    for (int len = 1; len <= HUFF_MAX_CODE_LENGTH; len++) {
        int i = (int)huffTable.numCodes[len];
        if (i < 0 || p + i > HUFF_MAX_NUMBER_SYMBOLS)	/* protect against table overrun */
            throw std::logic_error("HuffmanCoder: Huffman table numCodes values out of range");
        while (i--)
          huffCodeLen[p++] = (char) len;
    }
    if (p < 1)
        throw std::logic_error("HuffmanCoder: Huffman table with no symbols");
    // Do we want maxCodeLen (lMax) for e.g. making an optimum Huffman Table?
    //int maxCodeLen = huffCodeLen[p - 1];
    int numSymbols = p;
    huffCodeLen[numSymbols] = 0; // Terminate huffCodeLen array with a zero value

    /*
     * Generate the Huffman codes for each symbol in code value order - using
     * that lovely property of the JPEG Canonical Huffman code. (See Figure C.2
     * of the JPEG standard). Also validate that the counts represent a legal
     * Huffman code tree.
     */
    int code = 0; //First code is all zeros!
    int len = huffCodeLen[0]; // current code length
    p = 0;
    while (huffCodeLen[p] != 0) {
        while (((int) huffCodeLen[p]) == len) {
            huffCode[p++] = code;
            code++;
        }
        /*
         * code is now 1 more (in value) than the last code used for codes of
         * length len; but it must still fit in len numCodes, since no code is
         * allowed to be all ones. Note how it can't be a prefix of any previous code!
         */
        if (code >=  1 << len)
            throw std::logic_error("HuffmanCoder: Huffman Table does not specify a valid Huffman code "
            		"(not a prefix code,  or there is a code  word of all ones)");
        // Increase code length by one, and shift code to left accordingly
        // (Note how this provides the prefix free quality)
        len++;
        code <<= 1;
    }
    return numSymbols;
}

uint32_t kraftSum(const uint8_t* numCodes, int maxCodeLen)
{
	uint32_t sum = 0;
	for (int n = 1; n <= maxCodeLen; ++n) {
		sum += numCodes[n] * (1 << (maxCodeLen - n));
	}
	return sum;
}

} // namespace tnz
