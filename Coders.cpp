/*
 * Coders.cpp
 *
 *  Created on: 10/05/2015
 *      Author: jim
 */

#include "BitSink.h"
#include "Coders.h"
#include "Modeller.h"

#include <math.h>

namespace tnz {

static void predictiveCode(const double* data, int len, double qf, IntPredictor& predictor,
		Modeller& modeller, IntCoder& coder, BitSink& bitSink)
{
    for (int n = 0; n < len; ++n) {
        int x = lround(data[n] * qf);
        Model model = modeller.model(x - predictor.predict());
        if (model.symbol >= 0) {
        	coder.code(model.symbol, bitSink);
        	bitSink.receive(model.bits, model.nBits);
        }
        predictor.update(x);
    }
}

DoublesCoder::DoublesCoder(double qStep, std::shared_ptr<IntPredictor> predictor, std::shared_ptr<Modeller> modeller,
		std::shared_ptr<IntCoder> coder, std::shared_ptr<BitSink> bitSink)
	: qf(1.0/qStep),
	  predictor(predictor),
	  modeller(modeller),
	  coder(coder),
	  bitSink(bitSink)
{
}

void DoublesCoder::code(const double* data, int len)
{
    predictiveCode(data, len, qf, *predictor, *modeller, *coder, *bitSink);
}

}
