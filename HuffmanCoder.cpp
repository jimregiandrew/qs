/*
 * HuffmanCoder.cpp
 *
 *  Created on: 6/05/2015
 *      Author: jim
 */

#include "BitSink.h"
#include "HuffmanCoder.h"
#include "HuffmanTable.h"

#include <stdint.h>

#include <sstream>
#include <stdexcept>

namespace tnz {

/*!
 * Makes values for two input arrays: huffCode and huffCodeLen. huffCode[n]
 * is the code (bit pattern if you like) for symbol n, where the symbols
 * are ordered according to code value (smallest code first). huffCodeLen[n]
 * is the length of this code.
 */
static int makeCodeAndLengthTables(int *huffCode, uint8_t *huffCodeLen, const HuffmanTable& huffTable)
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

HuffmanCoder::HuffmanCoder(const HuffmanTable& huffmanTable)
{
	generateLUTs(huffmanTable);
}

/*
 * generateLuts method
 *
 * Generates the lookup tables (LUT's) used for the Huffman encoding.
 * codeLut[n] is the code for symbol n, while codeLengthLut[n] is the code length,
 * in codeLen (the number of bits in the code) for symbol n. These LUTs are generated from a
 * Canonical Huffman Table in the input HuffmanTable. See HuffmanTable.h for more info.
 */
void HuffmanCoder::generateLUTs(const HuffmanTable& huffmanTable)
{
	/*
	 * Zero coding LUTs, and in particular codeLengthLut. A zero length code is used to
	 * indicate an invalid symbol i.e. not in the Huffman table. An exception is thrown
	 * if a zero length code is attempted to be output.
	 */
    for (int n = 0; n < HUFF_MAX_NUMBER_SYMBOLS; n++) {
        codeLut[n] = 0;
        codeLengthLut[n] = 0;
    }

    /*
     * Generate an array of code lengths and of codes, both in code "value" order.
     */
    int huffCode[HUFF_MAX_NUMBER_SYMBOLS + 1];
    uint8_t codeLen[HUFF_MAX_NUMBER_SYMBOLS + 1];
    int numSymbols = makeCodeAndLengthTables(huffCode, codeLen, huffmanTable);

    /*
     * Currently we have a mapping from n, where n is the nth symbol ordered
     * according to code value, to code and code length ((huffCode[n] and codeLen[n]).
     * Now we create a mapping from symbol s (where s is the symbol (value), symbol = 0,1,...)
     * to code and code length ((codeLut[i]) and codeLengthLut[i])
     */
    int maxsymbol = HUFF_MAX_NUMBER_SYMBOLS - 1;
    for (int p = 0; p < numSymbols; p++) {
        int s = huffmanTable.symbol[p]; // symbol s is the pth symbol according to code value
        if (s < 0 || s > maxsymbol || codeLengthLut[s]) {
            std::ostringstream oss;
            oss<<"HuffmanCoder(generateLuts): symbol "<<s<<" is out of range or code length "
            		<<codeLengthLut[s]<<" is invalid";
            throw std::logic_error(oss.str());
        }
        codeLut[s] = huffCode[p];
        codeLengthLut[s] = codeLen[p];
    }
}

void HuffmanCoder::code(BitSink& bitSink, int symbol)
{
	if (symbol < 0 || symbol > HUFF_MAX_NUMBER_SYMBOLS) {
		throw std::logic_error("HuffmanCoder(encodeSymbol): Invalid symbol");
	}
	if (codeLengthLut[symbol] == 0) {
		std::ostringstream oss;
		oss<<"HuffmanCoder symbol="<<symbol<<" is not in Huffman table";
		throw std::logic_error(oss.str());
	}
	bitSink.receive(codeLut[symbol], codeLengthLut[symbol]);
}


void HuffmanCoder::flush(BitSink& bitSink)
{
	// Fill any remaining bytes with 1's.
	bitSink.receive(0xFF, 7);
}

} // namespace tnz
