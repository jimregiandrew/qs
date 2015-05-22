/*
 * g++ --std=c++0x -o test test.cpp test_Huffman.cpp BitSink.cpp tnz_BitSource.cpp HuffmanCoder.cpp HuffmanDecoder.cpp HuffmanTable.cpp
 */
#include "catch.hpp"

#include "BitSink.h"
#include "tnz_BitSource.h"
#include "Coders.h"
#include "Decoders.h"
#include "HuffmanTable.h"
#include "HuffmanCoder.h"
#include "HuffmanDecoder.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

namespace tnz {

TEST_CASE( "IntCoder tests", "[intcoder]" ) {

	shared_ptr<ByteBufferSink> byteSink(new ByteBufferSink());
	BitSink bitSink(byteSink);

	SECTION( "SizeIntCoder" ) {
		HuffmanTable table = getDefaultHuffmanTable();
		shared_ptr<IntCoder> intCoder(getSizeIntCoder(table));
		vector<int> seq = {1,1<<30};
		for (auto val : seq) {
			intCoder->code(bitSink, val);
		}
    	bitSink.close();

    	const std::vector<uint8_t> code = byteSink->getBuf();
    	shared_ptr<tnz::ByteBuffer> byteSource(new tnz::ByteBuffer(byteSink->getBuf()));
    	tnz::BitSource bitSource(byteSource);
    	shared_ptr<IntDecoder> intDecoder(getSizeIntDecoder(table));
    	for (auto val: seq) {
    		IntDecoder::Run run;
    		int err = intDecoder->decode(bitSource, run);
    		REQUIRE(run.val == val);
    	}
	}
}

} // namespace tnz
