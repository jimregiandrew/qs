/*
 * g++ --std=c++0x -o test test.cpp BitSink.cpp
 */
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

//unsigned int Factorial( unsigned int number ) {
//    return number <= 1 ? 1 : Factorial(number-1)*number;
//}
//
//TEST_CASE( "Factorials are computed", "[factorial]" ) {
//    REQUIRE( Factorial(0) == 1 );
//    REQUIRE( Factorial(1) == 1 );
//    REQUIRE( Factorial(2) == 2 );
//    REQUIRE( Factorial(3) == 6 );
//    REQUIRE( Factorial(10) == 3628800 );
//}

#include "BitSink.h"
#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

namespace tnz {

class TestingByteSink : public ByteSink
{
public:
	TestingByteSink() {}
	virtual ~TestingByteSink() {}

	virtual void receive(const uint8_t* data, int len) { buf.insert(buf.end(), data, data+len); }
	virtual void close() {}

	const vector<uint8_t> getBuf() const { return buf; }

private:
	vector<uint8_t> buf;
};

//static uint32_t getBits(const uint8_t* data, int startBit, int numBits)
//{
//	uint32_t out;
//	while (numBits > 0) {
//		int byteBits = 8-startBit;
//	}
//	if (startBit+numBits < 9) {
//	}
//	else {
//
//	}
//}

TEST_CASE( "BitSink", "[bitsink]" ) {
	shared_ptr<TestingByteSink> testingByteSink(new TestingByteSink);
	BitSink bitSink(testingByteSink);

    SECTION( "Simple bit sink test" ) {
    	vector<uint8_t> shouldBe(1, (uint8_t)0xff);
    	bitSink.receive(0xff, 8);
    	bitSink.flush();
    	vector<uint8_t> buf = testingByteSink->getBuf();
    	REQUIRE(buf == shouldBe);
    }

    // Implement this when have done Huffman decoding - want a getBits function
//	SECTION( "Random data, random bit size output" ) {
//		static const int SIZE=1024;
//		vector<uint8_t> data;
//		for (int n = 0; n < SIZE; ++n)
//			data.push_back((uint8_t)(rand()%256));
//		int bitCount = 0;
//		while (bitCount < SIZE*8) {
//			int putBits = rand()%26;
//			if (bitCount + putBits > SIZE*8)
//				putBits = SIZE*8 - bitCount;
//			int byteStart = bitCount/8;
//			bitSink.receive(getBits(&data[0]+byteStart, bitCount%8, putBits), putBits);
//		    bitCount += putBits;
//		}
//		REQUIRE(bitCount == SIZE*8);
//		REQUIRE(testingByteSink->getBuf() == data);
//	}

    SECTION( "Simple bit sink test" ) {
    	// Notes bits are pushed into a byte at byte 0 - i.e. first bit goes into byte 0.
    	vector<uint8_t> shouldBe = {0x00, 0x01, 0x02, 0x5A};
    	bitSink.receive(0x00, 8);
    	bitSink.receive(0x01, 8);
    	bitSink.receive(0x02, 8);
    	bitSink.receive(0x00, 1);
    	bitSink.receive(0x1, 1);
    	bitSink.receive(0x00, 1);
    	bitSink.receive(0x1, 1);
    	bitSink.receive(0x1, 1);
    	bitSink.receive(0x00, 1);
    	bitSink.receive(0x1, 1);
    	bitSink.receive(0x00, 1);
    	bitSink.flush();
    	vector<uint8_t> buf = testingByteSink->getBuf();
    	REQUIRE(buf == shouldBe);
    }
}

} // namespace tnz
