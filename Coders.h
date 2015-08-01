/*
 * Coders.h
 *
 *  Created on: 10/05/2015
 *      Author: jim
 */

#ifndef CODERS_H_
#define CODERS_H_

#include <memory>
#include <vector>

namespace qs {

class Modeller;
class BitSink;

class IntCoder
{
public:
	virtual ~IntCoder() {}
	virtual void code(BitSink& bitSink, int val) = 0;
    virtual void flush(BitSink& bitSink) = 0;
    virtual const std::vector<int>& getCounts() const = 0;
};

class IntPredictor
{
public:
	virtual ~IntPredictor() {}

	/*!
	 * Separate interface into predict and update so that can call predict multiple times.
	 */
	virtual int predict() = 0;
	virtual void update(int) = 0;
};

class DoublesCoder
{
public:
	DoublesCoder(double qStep, std::shared_ptr<IntPredictor> predictor,
			std::shared_ptr<IntCoder> coder, std::shared_ptr<BitSink> bitSink);

	void code(const double* data, int dataLen);

private:
	double qf;
	std::shared_ptr<IntPredictor> predictor;
	std::shared_ptr<IntCoder> coder;
	std::shared_ptr<BitSink> bitSink;
};

IntPredictor* getIntPredictor(int order, int initial1=0, int initial2=0);

class HuffmanTable;
IntCoder* getSizeIntCoder(const HuffmanTable& table);

}

#endif /* CODERS_H_ */
