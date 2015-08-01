/*
 * g++ --std=c++0x -o test test.cpp test_Huffman.cpp BitSink.cpp qs_BitSource.cpp HuffmanCoder.cpp HuffmanDecoder.cpp HuffmanTable.cpp
 */
#include "catch.hpp"

#include "BitSink.h"
#include "qs_BitSource.h"
#include "Coders.h"
#include "Decoders.h"
#include "HuffmanTable.h"
#include "HuffmanCoder.h"
#include "HuffmanDecoder.h"
#include "Modeller.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

namespace qs {

TEST_CASE( "Huffman coding", "[huffman]" ) {

	shared_ptr<ByteBufferSink> byteSink(new ByteBufferSink());

	SECTION( "Simple huffman coding test" ) {
    	// Huffman code with 1 code of length 1 bit and 4 codes of length 3.
    	// Symbols are 0,1,..4 (i.e. symbols (also) ordered according to code word length and order
    	qs::HuffmanTable huffTable = { {0, 1, 0, 3}, {0, 1, 2, 3} };
    	BitSink bitSink(byteSink);
    	HuffmanCoder huffCoder(huffTable);
    	huffCoder.code(bitSink, 0); // Code 0
    	huffCoder.code(bitSink, 1); // Code 100
    	huffCoder.code(bitSink, 2); // Code 101
    	huffCoder.code(bitSink, 3); // Code 110
    	// Output is then 01001011 | 10. Padding with 1's 01001011 | 10111111 = 0x4B 0xBF
    	bitSink.close();

    	const std::vector<uint8_t> code = byteSink->getBuf();
    	REQUIRE(code.size() == 2);
    	REQUIRE(code[0] == 0x4B);
    	REQUIRE(code[1] == 0xBF);

    	shared_ptr<qs::ByteBuffer> byteSource(new qs::ByteBuffer(byteSink->getBuf()));
    	qs::BitSource bitSource(byteSource);
    	qs::HuffmanDecoder huffDecoder(huffTable);
    	int sym = 0;
    	sym = huffDecoder.decode(bitSource);
    	REQUIRE(sym == 0);
    	sym = huffDecoder.decode(bitSource);
    	REQUIRE(sym == 1);
    	sym = huffDecoder.decode(bitSource);
    	REQUIRE(sym == 2);
    	sym = huffDecoder.decode(bitSource);
    	REQUIRE(sym == 3);
    	int availBits = bitSource.getAvailableBits();
    	REQUIRE(availBits == 6);
    }

	SECTION( "Kraft inequality" ) {
		{
			qs::HuffmanTable huffTable = { {0, 1, 0, 3}, {0, 1, 2, 3} };
			int maxCodeLen = 3;
			uint32_t ksum = kraftSum(huffTable.numCodes, maxCodeLen);
			REQUIRE(ksum == (1 << maxCodeLen) - 1);
		}
		{
			qs::HuffmanTable huffTable = { {0, 1, 0, 4}, {0, 1, 2, 3, 4} };
			int maxCodeLen = 3;
			uint32_t ksum = kraftSum(huffTable.numCodes, maxCodeLen);
			REQUIRE(ksum == (1 << maxCodeLen));
		}
	}

	SECTION( "magnitude bits" ) {
		{
			qs::Model model = getMagBitsModel(0);
			qs::Model shouldBe = Model{0,0,0};
			REQUIRE(model == shouldBe);
		}
		{
			qs::Model model = getMagBitsModel(1);
			qs::Model shouldBe = Model{1,1,1};
			REQUIRE(model == shouldBe);
		}
		{
			qs::Model model = getMagBitsModel(-1);
			qs::Model shouldBe = Model{1,1,-2}; // bits = 0b
			REQUIRE(model == shouldBe);
		}
		{
			qs::Model model = getMagBitsModel(2);
			qs::Model shouldBe = Model{2,2,2}; // bits = 10b = 2
			REQUIRE(model == shouldBe);
		}
		{
			qs::Model model = getMagBitsModel(3);
			qs::Model shouldBe = Model{2,2,3}; // bits = 11b = 3
			REQUIRE(model == shouldBe);
		}
		{
			qs::Model model = getMagBitsModel(-2);
			qs::Model shouldBe = Model{2,2,-3}; // bits = 01b = 1
			REQUIRE(model == shouldBe);
		}
		{
			qs::Model model = getMagBitsModel(-3);
			qs::Model shouldBe = Model{2,2,-4}; // bits = -100 = 2
			REQUIRE(model == shouldBe);
		}
		{
			qs::Model model = getMagBitsModel(0xabcd);
			qs::Model shouldBe = Model{16,16,0xabcd}; // bits = 0xabcd
			REQUIRE(model == shouldBe);
		}
	}

	SECTION( "predictor" ) {
		{
			std::shared_ptr<IntPredictor> predictor(getIntPredictor(0));
			REQUIRE(predictor->predict() == 0);
			predictor->update(1);
			REQUIRE(predictor->predict() == 0);
			predictor->update(-20);
			REQUIRE(predictor->predict() == 0);
		}
		{
			std::shared_ptr<IntPredictor> predictor(getIntPredictor(1));
			REQUIRE(predictor->predict() == 0);
			predictor->update(1);
			REQUIRE(predictor->predict() == 1);
			predictor->update(-20);
			REQUIRE(predictor->predict() == -20);
		}
		{
			std::shared_ptr<IntPredictor> predictor(getIntPredictor(2));
			REQUIRE(predictor->predict() == 0);
			predictor->update(1);
			REQUIRE(predictor->predict() == 2);
			predictor->update(-20);
			REQUIRE(predictor->predict() == -41);
		}
	}
}

} // namespace qs
