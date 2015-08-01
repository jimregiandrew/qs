/*
 * HuffmanCoder.h
 *
 *  Created on: 6/05/2015
 *      Author: jim
 */

#ifndef TNZ_HUFFMANCODER_H_
#define TNZ_HUFFMANCODER_H_

#include "HuffmanTable.h"

namespace qs {

class BitSink;

class HuffmanCoder
{
public:
	HuffmanCoder(const HuffmanTable& huffmanTable);
	void code(BitSink& bitSink, int symbol);
	// We could put flush outside of HuffmanCoder - particularly if we use the codeword that is all 1's
	void flush(BitSink& bitSink);

private:
	void generateLUTs(const HuffmanTable& huffmanTable);

private:
	unsigned int codeLut[HUFF_MAX_NUMBER_SYMBOLS]; // LUT mapping coding symbols to (Huffman) codes
	char codeLengthLut[HUFF_MAX_NUMBER_SYMBOLS]; // LUT mapping symbols to code length (number bits)
};

} // namespace qs

#endif /* TNZ_HUFFMANCODER_H_ */
