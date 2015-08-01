/*
 * utils.h
 *
 *  Created on: 7/06/2015
 *      Author: jim
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <iomanip>
#include <iosfwd>
#include <sstream>

namespace qs {

// http://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream
struct HexCharStruct
{
    char c;
    HexCharStruct(char _c) : c(_c) {}
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
{
  o << std::hex << "0x" << std::setw(2) << std::setfill('0');
  o << std::uppercase << (int)(uint8_t)hs.c;
  o << std::nouppercase << std::dec;
  return o;
}

inline HexCharStruct hex(char _c)
{
  return HexCharStruct(_c);
}

inline HexCharStruct hex(unsigned char _c)
{
  return HexCharStruct((char)_c);
}

struct HexInt32Struct
{
    uint32_t n;
    HexInt32Struct(uint32_t n) : n(n) {}
};

inline std::ostream& operator<<(std::ostream& o, const HexInt32Struct& hs)
{
  o << std::hex << "0x" << std::setw(2) << std::setfill('0') << std::uppercase;
  o << (int)(uint8_t)((hs.n >> 24) & 0xFF) << std::setw(2) << std::setfill('0');
  o << (int)(uint8_t)((hs.n >> 16) & 0xFF) << std::setw(2) << std::setfill('0');
  o << (int)(uint8_t)((hs.n >> 8)  & 0xFF) << std::setw(2) << std::setfill('0');
  o << (int)(uint8_t)(hs.n         & 0xFF);
  o << std::dec << std::nouppercase;
  return o;
}

inline HexInt32Struct hex(uint32_t n)
{
  return HexInt32Struct(n);
}

inline HexInt32Struct hex(int32_t n)
{
    return HexInt32Struct((uint32_t)n);
}

struct HexInt16Struct
{
    uint16_t n;
    HexInt16Struct(uint16_t n) : n(n) {}
};

inline std::ostream& operator<<(std::ostream& o, const HexInt16Struct& hs)
{
  o << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0');
  o << (int)(uint8_t)((hs.n >> 8)  & 0xFF) << std::setw(2) << std::setfill('0');
  o << (int)(uint8_t)(hs.n         & 0xFF);
  o << std::dec << std::nouppercase;
  return o;
}

inline HexInt16Struct hex(uint16_t n)
{
  return HexInt16Struct(n);
}

struct HexUint8pStruct
{
    const uint8_t* data;
    int numBytes;
    HexUint8pStruct(const uint8_t* data, int numBytes) : data(data), numBytes(numBytes) {}
};

inline std::ostream& operator<<(std::ostream& o, const HexUint8pStruct& hs)
{
  o << "0x" << std::hex << std::uppercase;
  for (int n = 0; n < hs.numBytes; ++n) {
      if (n > 0)
          o<<" ";
      o<<std::setw(2)<<std::setfill('0')<<(int)(uint8_t)(hs.data[n]);
  }
  o << std::dec << std::nouppercase;
  return o;
}

inline struct HexUint8pStruct hex(const uint8_t* data, int numBytes)
{
    return HexUint8pStruct(data, numBytes);
}

} // namespace qs

#endif /* UTILS_H_ */
