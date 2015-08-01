/*
 * Decoders.h
 *
 *  Created on: 21/05/2015
 *      Author: jim
 */

#ifndef DECODERS_H_
#define DECODERS_H_

namespace qs {

class BitSource;

class IntDecoder
{
    public:
        struct Run {
            int run;
            int val;
            Run(int run=-1, int val=0) : run(run), val(val) {}
        };
    public:
        virtual ~IntDecoder() {}

        virtual int decode(BitSource& bitSource, Run& out) = 0;
};

class HuffmanTable;
IntDecoder* getSizeIntDecoder(const HuffmanTable& table);

}

#endif /* DECODERS_H_ */
