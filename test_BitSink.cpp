/*
 * g++ --std=c++0x -o test test.cpp BitSink.cpp tnz_BitSource.cpp HuffmanCoder.cpp HuffmanDecoder.cpp HuffmanTable.cpp
 */
#include "catch.hpp"

#include "BitSink.h"
#include "tnz_BitSource.h"

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

TEST_CASE( "BitSink", "[bitsink]" ) {
	shared_ptr<TestingByteSink> testingByteSink(new TestingByteSink);
	BitSink bitSink(testingByteSink);
/*
    SECTION( "Simple bit sink test 1" ) {
    	vector<uint8_t> shouldBe(1, (uint8_t)0xff);
    	bitSink.receive(0xff, 8);
    	bitSink.flush();
    	vector<uint8_t> buf = testingByteSink->getBuf();
    	REQUIRE(buf == shouldBe);
    }

    SECTION( "Simple bit sink test 2" ) {
    	// Note the first bit pushed into a byte becomes that bytes msb, the next bit the next msb and so on.
    	vector<uint8_t> shouldBe = {0x00, 0x01, 0xA2, 0x5A};
    	bitSink.receive(0x00, 8);
    	bitSink.receive(0x01, 8);
    	bitSink.receive(0xA2, 8);
    	bitSink.receive(0x0, 1);
    	bitSink.receive(0x1, 1);
    	bitSink.receive(0x0, 1);
    	bitSink.receive(0x1, 1);
    	bitSink.receive(0x1, 1);
    	bitSink.receive(0x0, 1);
    	bitSink.receive(0x1, 1);
    	bitSink.receive(0x0, 1);
    	bitSink.flush();
    	vector<uint8_t> buf = testingByteSink->getBuf();
    	REQUIRE(buf == shouldBe);
    }
*/
    SECTION( "Something a bit harder for bit sink" ) {
    	for (int n = 31; n <= 32; ++n) {
    		bitSink.receive(0xABCD, n);
    	}
    	bitSink.receive(0xFF, 7); // make sure last byte will be output
    	bitSink.flush();
    	shared_ptr<ByteSource> byteSource(new ByteBuffer(testingByteSink->getBuf()));
    	BitSource bitSource(byteSource);
    	for (int n = 31; n <= 32; ++n) {
    		uint32_t word = bitSource.pop(n);
    		uint32_t mask = n < 32 ? ((1<<n) - 1) : 0xFFFFFFFF;
    		uint32_t shouldBe = 0xABCD & mask;
    		REQUIRE(word == shouldBe);
    	}
    }
}

} // namespace tnz
