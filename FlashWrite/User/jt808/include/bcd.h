

#ifndef JT808_BCD_H_
#define JT808_BCD_H_

#include <stdio.h>
#include <stdint.h>

#include <stdlib.h>



unsigned char HexToBcd(unsigned char src);
unsigned char BcdToHex(unsigned char src);
unsigned char *jt808StringToBcdCompress(const unsigned char *src, unsigned char *dst, int srclen);
unsigned char *jt808BcdToStringCompress(const unsigned char *src, unsigned char *dst, int srclen);
unsigned char *jt808BcdToStringCompressFillingZero(const unsigned char *src, unsigned char *dst, int srclen);

// uint8_t HexToBcd(uint8_t const  &src);
// uint8_t BcdToHex(uint8_t const  &src);
// uint8_t *StringToBcdCompress(const uint8_t *src, uint8_t *dst, const int & srclen);
// uint8_t *BcdToStringCompress(const uint8_t *src, uint8_t *dst, const int & srclen);
// uint8_t *BcdToStringCompressFillingZero(const uint8_t *src, uint8_t *dst, const int &srclen);
// int StringToBcd(std::string const &in, std::vector<uint8_t> *out);
// int BcdToString(std::vector<uint8_t> const &in, std::string *out);
// int BcdToStringFillZero(std::vector<uint8_t> const &in, std::string *out);


#endif // JT808_BCD_H_
