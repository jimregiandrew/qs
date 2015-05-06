/*
 * Coders.h
 *
 *  Created on: 6/05/2015
 *      Author: jim
 */

#ifndef CODERS_H_
#define CODERS_H_

#include <stdint.h>

#include <vector>

class BitSink;
class BitSource;
class HuffmanTable;

namespace tnz {

class Int32Coder
{
    public:
        virtual ~Int32Coder() {}

        virtual void code(int32_t val) = 0;
        virtual void flush() = 0;
};

class Int32CoderCounter : public Int32Coder
{
    public:
        virtual ~Int32CoderCounter() {}

        virtual void code(int32_t val) = 0;
        virtual void flush() = 0;
        virtual const std::vector<int>& getCounts() = 0;
};

Int32CoderCounter* newMagInt32Coder(std::shared_ptr<BitSink> bitSink, HuffmanTable* table);
Int32CoderCounter* newRLMagInt32Coder(std::shared_ptr<BitSink> bitSink, HuffmanTable* table);
Int32CoderCounter* newMagInt32Optimizer();

class Int32Decoder
{
    public:
        struct Run {
            int run;
            int32_t val;
            Run(int run=-1, int32_t val=0) : run(run), val(val) {}
        };
    public:
        virtual ~Int32Decoder() {}

        virtual int decode(Run& out) = 0;
};

Int32Decoder* newMagInt32Decoder(std::shared_ptr<BitSource> bitSource, HuffmanTable* table);
Int32Decoder* newRLMagInt32Decoder(std::shared_ptr<BitSource> bitSource, HuffmanTable* table);

} // namespace tnz


#endif /* CODERS_H_ */
