/*
 * Modeller.h
 *
 *  Created on: 10/05/2015
 *      Author: jim
 */

#ifndef MODELLER_H_
#define MODELLER_H_

#include "Coders.h"

#include <memory>

#include <iostream>

namespace qs {

class BitStream;

struct Model {
	int symbol;
	int nBits;
	int bits;
};

class Modeller
{
public:
	virtual ~Modeller(){}

	virtual const Model& model(int symbol) = 0;
};

Model getMagBitsModel(int val);
Modeller* getMagModeller();
Modeller* getZeroRLMagModeller();
//Modeller* getRLMagModeller(); ?

inline bool operator==(const Model& lhs, const Model& rhs)
{
	return  lhs.symbol == rhs.symbol &&
			lhs.nBits  == rhs.nBits &&
			lhs.bits   == rhs.bits;
}

inline std::ostream& operator<<(std::ostream& os, const Model& m)
{
	os<<"symbol="<<(int)(int8_t)m.symbol<<", nBits="<<m.nBits<<", bits="<<m.bits<<"(0x"<<std::hex<<m.bits<<")";
	return os;
}

} // namespace qs



#endif /* MODELLER_H_ */
