/*
 * Coders.cpp
 *
 *  Created on: 10/05/2015
 *      Author: jim
 */

#include "BitSink.h"
#include "Coders.h"
#include "HuffmanCoder.h"

#include <limits.h>
#include <math.h>

#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

namespace tnz {

/*
 *  Do we use a 'generic' Model here e.g.
 *  struct VarLenWord {
 *    int numBits;
 *    int word;
 *  }
 *  struct Model {
 *    int symbol;
 *    vector<VarLenWord> words;
 *  }
 *  And coding all VarLenWords appropriately?
 *
 *  Or do we just use a generic IntCoder as below?
 */
static void predictiveCode(const double* data, int len, double qf, IntPredictor& predictor,
		IntCoder& coder, BitSink& bitSink)
{
    for (int n = 0; n < len; ++n) {
        int x = lround(data[n] * qf);
        coder.code(bitSink, x - predictor.predict());
        predictor.update(x);
    }
}

DoublesCoder::DoublesCoder(double qStep, std::shared_ptr<IntPredictor> predictor,
		std::shared_ptr<IntCoder> coder, std::shared_ptr<BitSink> bitSink)
	: qf(1.0/qStep),
	  predictor(predictor),
	  coder(coder),
	  bitSink(bitSink)
{
}

void DoublesCoder::code(const double* data, int len)
{
    predictiveCode(data, len, qf, *predictor, *coder, *bitSink);
}

namespace {

class ZeroOrderPredictor : public IntPredictor
{
public:
	ZeroOrderPredictor(int initialVal) : initialVal(initialVal) {}

	virtual int predict() { return initialVal; }
	virtual void update(int) { }

private:
	int initialVal;
};

class FirstOrderPredictor : public IntPredictor
{
public:
	FirstOrderPredictor(int initialVal) : prev(initialVal) {}

	virtual int predict() { return prev; }
	virtual void update(int val) { prev = val; }

private:
	int prev;
};

class SecondOrderPredictor : public IntPredictor
{
public:
	SecondOrderPredictor(int prev1, int prev2) : prev1(prev1), prev2(prev2) {}

	virtual int predict() { return 2*prev1 - prev2; }
	virtual void update(int val) { prev2 = prev1; prev1 = val; }

private:
	int prev1;
	int prev2;
};

}  // anonymous namespace

IntPredictor* getIntPredictor(int order, int initial1, int initial2)
{
	if (order == 0)
		return new ZeroOrderPredictor(initial1);
	if (order == 1)
		return new FirstOrderPredictor(initial1);
	else if (order == 2)
		return new SecondOrderPredictor(initial1, initial2);
	else
		throw std::logic_error("Predictor order must be 0, 1 or 2");
}

struct SizeAmp {
	int size;
	int amp;
};

SizeAmp getSizeAmp(int val)
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
    return SizeAmp{nbits, temp};
}


/*******************************************************************************
 *
 *
 *
 *
 *
 * Magnitude huffman coding
 *
 *
 *
 *
 *
 ******************************************************************************/
#define UNUSED(x) (void)(x)
class SizeIntCoder : public IntCoder
{
    public:
        SizeIntCoder(std::shared_ptr<HuffmanCoder> huffCoder);
        virtual ~SizeIntCoder();

    	virtual void code(BitSink& bitSink, int val);
        virtual const std::vector<int>& getCounts() const { return counts; }
        virtual void flush(BitSink& bitSink) { UNUSED(bitSink); }

    private:
        std::shared_ptr<HuffmanCoder> huffCoder;
        std::vector<int> counts;
        int nlsbs;
};

SizeIntCoder::SizeIntCoder(std::shared_ptr<HuffmanCoder> huffCoder)
    : huffCoder(huffCoder),
	  counts(vector<int>(HUFF_MAX_NUMBER_SYMBOLS, 0)),
      nlsbs(0)
{
}

SizeIntCoder::~SizeIntCoder()
{
}

void SizeIntCoder::code(BitSink& bitSink, int val)
{
	SizeAmp sa = getSizeAmp(val);
    huffCoder->code(bitSink, sa.size);
    counts[sa.size]++;
    if (sa.size) {
        bitSink.receive(sa.amp, sa.size);
    }
}

IntCoder* getSizeIntCoder(const HuffmanTable& table)
{
	shared_ptr<HuffmanCoder> huffCoder(new HuffmanCoder(table));
    return new SizeIntCoder(huffCoder);
}


}
