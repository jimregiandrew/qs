/*
 * Modeller.cpp
 *
 *  Created on: 10/05/2015
 *      Author: jim
 */


#include "Modeller.h"

#include <limits.h>

namespace tnz {

/*
 * Finds the number of bits needed to represent val
 */
Model getMagBitsModel(int val)
{
    int temp = val;
    if (val < 0) {
        if (val == INT_MIN)
            throw std::logic_error("Int min reached");
        val = -val;
        temp--; // assumes two's complement machine
    }
    int nbits = 0;
    while (val) {
      nbits++;
      val >>= 1;
    }
    return Model{nbits, nbits, temp};
}

class MagModeller : public Modeller
{
public:
	virtual ~MagModeller(){}

	virtual const Model& model(int symbol);

private:
	Model mdl;
};

const Model& MagModeller::model(int symbol)
{
	mdl = getMagBitsModel(symbol);
	return mdl;
}

class ZeroRLMagModeller : public Modeller
{
public:
	virtual ~ZeroRLMagModeller(){}

	virtual const Model& model(int symbol);

private:
	Model mdl;
};

const Model& ZeroRLMagModeller::model(int symbol)
{
	return mdl;
}


} // namespace tnz
