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
  
  int BCD2String(char *pcBCDData, int iBCDDataLen, char *strDigitBuf, int iBufLen)
{
    int  iPosFlag = 0;
    char cBCD     = 0;

    if (pcBCDData==NULL || strDigitBuf==NULL)
    {
        printf("BCD2String: pcBCDData==NULL || strDigitBuf==NULL!\n");
        return -1;
    }
	
    if (2 * iBCDDataLen > iBufLen)
    {
        printf("BCD2String: 2 * iBCDDataLen(%d) > iBufLen(%d)!\n", iBCDDataLen, iBufLen);
        return -1;
    }
	
    for (iPosFlag = 0; iPosFlag < iBCDDataLen; iPosFlag++)
    {
        cBCD = (pcBCDData[iPosFlag] >> 4);
        if (0x0f == cBCD)
        {
            break;
        }
        strDigitBuf[2 * iPosFlag] = cBCD + '0';
        if (!isdigit(strDigitBuf[2 * iPosFlag]))
        {
            printf("BCD2String: strDigitBuf[2 * %d] is not digit!\n", iPosFlag);
            return -2;
        }

        cBCD = (pcBCDData[iPosFlag] & 0x0f);
        if (0x0f == cBCD)
        {
            break;
        }
        strDigitBuf[2 * iPosFlag + 1] = cBCD + '0';
        if (!isdigit(strDigitBuf[2 * iPosFlag + 1]))
        {
            printf("BCD2String: strDigitBuf[2 * %d + 1] is not digit!\n", iPosFlag);
            return -2;
        }
    }
    
    return 0;
}

