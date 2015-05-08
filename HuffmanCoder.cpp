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
