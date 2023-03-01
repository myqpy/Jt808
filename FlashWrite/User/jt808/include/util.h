
#ifndef JT808_UTIL_H_
#define JT808_UTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// #include <vector>

// namespace libjt808
// {

// 无符号64位整型转无符号字节数组.
union U64ToU8Array
{
  unsigned long u64val;
  unsigned char u8array[8];
};

// 无符号32位整型转无符号字节数组.
union U32ToU8Array
{
  unsigned int u32val;
  unsigned char u8array[4];
};

// 无符号16位整型转无符号字节数组.
union U16ToU8Array
{
  unsigned short u16val;
  unsigned char u8array[2];
};

// 大小端互换.
unsigned short EndianSwap16(unsigned short u16val);

// 大小端互换.
unsigned int EndianSwap32(unsigned int u32val);


// 转义函数.
int Escape_C(unsigned char *in, unsigned int inlen, unsigned char *out, unsigned int *outlen);
int ReverseEscape_C(unsigned char *in, unsigned int inlen, unsigned char *out, unsigned int *outlen);

// 异或校验.
unsigned char BccCheckSum(const unsigned char *src, unsigned long len);

// } // namespace libjt808

#endif // JT808_UTIL_H_
