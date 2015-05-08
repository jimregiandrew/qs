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

} // namespace tnz
