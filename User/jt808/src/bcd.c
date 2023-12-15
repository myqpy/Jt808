#include "bcd.h"

  unsigned char  HexToBcd(unsigned char  src)
  {
    unsigned char  temp;
    temp = ((src / 10) << 4) + (src % 10);
    return temp;
  }

   unsigned char  BcdToHex(unsigned char  src)
  {
    unsigned char  temp;
    temp = (src >> 4) * 10 + (src & 0x0f);
    return temp;
  }

  unsigned char  *jt808StringToBcdCompress(const unsigned char  *src, unsigned char  *dst, int srclen)
  {
    unsigned char  *ptr = dst;
    unsigned char  temp;
    if (srclen % 2 != 0)
    {
      *ptr++ = HexToBcd(*src++ - '0');
    }
    while (*src)
    {
      temp = *src++ - '0';
      temp *= 10;
      temp += *src++ - '0';
      *ptr++ = HexToBcd(temp);
    }
    *ptr = 0;
    return dst;
  }

  unsigned char  *jt808BcdToStringCompress(const unsigned char  *src, unsigned char  *dst, int srclen)
  {
    unsigned char  *ptr = dst;
    unsigned char  temp;
    int cnt = srclen;
    while (cnt--)
    {
      temp = BcdToHex(*src);
      *ptr++ = temp / 10 + '0';
      if (dst[0] == '0')
      {
        ptr = dst;
      }
      *ptr++ = temp % 10 + '0';
      ++src;
    }
    return dst;
  }


  unsigned char  *jt808BcdToStringCompressFillingZero(const unsigned char  *src, unsigned char  *dst, int srclen)
  {
    unsigned char  *ptr = dst;
    unsigned char  temp;
    int cnt = srclen;
    while (cnt--)
    {
      temp = BcdToHex(*src);
      *ptr++ = temp / 10 + '0';
      *ptr++ = temp % 10 + '0';
      ++src;
    }
    return dst;
  }
