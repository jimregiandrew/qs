/*
 * HuffmanTable.h
 *
 *  Created on: 6/05/2015
 *      Author: jim
 */

#ifndef HUFFMANTABLE_H_
#define HUFFMANTABLE_H_

#include <stdint.h>

#include <ostream>

namespace tnz {

#define HUFF_MAX_CODE_LENGTH 16
#define HUFF_MAX_NUMBER_SYMBOLS 256

/*!
 * HUFFMAN_RESERVED_SYMBOL is used in the construction of a Huffman symbol. (It
 * is a dummy symbol with minimum weight (probability or count) that gets
 * assigned a codeword of all 1's. This means no other (real) symbol is assigned
 * this symbol)
 */
#define HUFFMAN_RESERVED_SYMBOL 256

/*
 * HuffmanTable
 *
 * We use a Canonical Huffman symbol representation to describe the Huffman symbol.
 * See e.g. http://en.wikipedia.org/wiki/Canonical_Huffman_code.
 * This has the advantage that the Huffman table can be stored compactly
 * (the same that is used in JPEG). Specifically, numCodes[i] stores the
 * number of codes of length i (i=1,2,..,HUFF_MAX_CODE_LENGTH), and symbol[n] is the nth symbol
 * (n=0,1,..,HUFF_MAX_NUMBER_SYMBOLS-1), where the symbols are ordered according to
 * symbols code "value". A symbol's code "value" is the unsigned binary number defined by its
 * code's bit pattern. (Note that shorter codes have a lower value than longer codes).
 */
struct HuffmanTable
{
    uint8_t numCodes[HUFF_MAX_CODE_LENGTH + 1];
    uint8_t symbol[HUFF_MAX_NUMBER_SYMBOLS];
    //ToDo: look for type errors because I had symbol as array of int (and not uint8_t).
};

HuffmanTable getDefaultHuffmanTable();

int makeCodeAndLengthTables(int *huffCode, uint8_t *huffCodeLen, const HuffmanTable& huffTable);

// Returns Kraft sum of code lengths, shifted left by maxCodeLen
// kraftSum <= 1 << maxCodeLen is then the Kraft inequality.
uint32_t kraftSum(const uint8_t* numCodes, int maxCodeLen);

inline bool operator == (const HuffmanTable& lhs,  const HuffmanTable& rhs)
{
    for (unsigned int n = 1; n <= HUFF_MAX_CODE_LENGTH; n++) {
        if (lhs.numCodes[n] != rhs.numCodes[n])
            return false;
    }
    for (unsigned int n = 0; n < HUFF_MAX_NUMBER_SYMBOLS; n++) {
        if (lhs.symbol[n] != rhs.symbol[n])
            return false;
    }
    return true;
}

inline std::ostream& operator << (std::ostream& os, const HuffmanTable& table)
{
    unsigned int numCodes = 0;
    os << "numCodes: ";
    for (unsigned int n = 1; n < HUFF_MAX_CODE_LENGTH + 1; n++) {
        os << (int)table.numCodes[n] << " ";
        numCodes += table.numCodes[n];
    }
    os << "symbol: ";
    for (unsigned int n = 0; n < numCodes; n++)
        os << (int)table.symbol[n] << " ";
    return os;
}

} // namespace tnz

#endif /* HUFFMANTABLE_H_ */
