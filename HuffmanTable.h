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
 * This has an advantage in that the Huffman table can be stored compactly
 * (the same that is used in JPEG). Specifically, numCodes[i] stores the
 * number of codes of length i, and symbol[n] is the nth symbol, where the symbols are
 * ordered according to symbol "value". A symbol's "value" is the unsigned binary number
 * defined by its bit pattern. (Note that shorter codes have a lower
 * value than longer codes).
 */
struct HuffmanTable
{
    uint8_t numCodes[HUFF_MAX_CODE_LENGTH + 1];
    int     symbol[HUFF_MAX_NUMBER_SYMBOLS];
};

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
