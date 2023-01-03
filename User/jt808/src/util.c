

// #include "jt808/util.h"
// #include "jt808/protocol_parameter.h"

#include "util.h"
#include "protocol_parameter.h"
//#include <memory.h>

union U16ToU8Array u16converter;
union U32ToU8Array u32converter;

// 双字节大小段互换
unsigned short EndianSwap16(unsigned short u16val)
{
  return (((u16val & 0x00FF) << 8) +
          ((u16val & 0xFF00) >> 8));
}

// 四字节大小端互换.
unsigned int EndianSwap32(unsigned int u32val)
{
  return (((u32val & 0x000000FF) << 24) +
          ((u32val & 0x0000FF00) << 8) +
          ((u32val & 0x00FF0000) >> 8) +
          ((u32val & 0xFF000000) >> 24));
}

// 转义函数.   outlen应大于inlen   建议outlen >= 2*inlen；
int Escape_C(unsigned char *in, unsigned int inlen, unsigned char *out, unsigned int *outlen)
{
	unsigned int i;
	int offset_num;
  if ((out == NULL) || ((*outlen) < 2 * inlen))
	{
		printf("转义函数出现错误 \r\n");
		return -1;
		
	}
    
  memset(out, 0, *outlen);

  offset_num = 0;
  for (i = 0; i < inlen; ++i)
  {
    if (in[i] == PROTOCOL_SIGN)
    {
      out[i + offset_num] = PROTOCOL_ESCAPE;
      out[i + 1 + offset_num] = PROTOCOL_ESCAPE_SIGN;
      offset_num++;
    }
    else if (in[i] == PROTOCOL_ESCAPE)
    {
      out[i + offset_num] = PROTOCOL_ESCAPE;
      out[i + 1 + offset_num] = PROTOCOL_ESCAPE_ESCAPE;
      offset_num++;
    }
    else
    {
      out[i + offset_num] = in[i];
    }
  }
  *outlen = inlen + offset_num;

  return 0;
}

// 逆转义函数.   outlen >= (inlen/2)
int ReverseEscape_C(unsigned char *in, unsigned int inlen, unsigned char *out, unsigned int *outlen)
{
  // if ((out == NULL) || (inlen == 0))
  //   return -1;
  // memset(out, 0, *outlen);
	unsigned int i;
	int offset_num;
  printf("[逆转义ReverseEscape_C]:");

  offset_num = 0;
  for (i = 0; i < inlen; ++i)
  {
    if ((in[i] == PROTOCOL_ESCAPE) && (in[i + 1] == PROTOCOL_ESCAPE_SIGN))
    {
      out[i - offset_num] = PROTOCOL_SIGN;
      ++i;
      offset_num++;
    }
    else if ((in[i] == PROTOCOL_ESCAPE) && (in[i + 1] == PROTOCOL_ESCAPE_ESCAPE))
    {
      out[i - offset_num] = PROTOCOL_ESCAPE;
      ++i;
      offset_num++;
    }
    else
    {
      out[i - offset_num] = in[i];
      printf("%02x ", out[i - offset_num]);
    }
  }
  printf("\r\n");

  *outlen = inlen - offset_num;

  return 0;
}

// 奇偶校验.
unsigned char BccCheckSum(const unsigned char *src, unsigned long len)
{
  unsigned char checksum = 0;
	unsigned long i;
  for (i = 0; i < len; ++i)
  {
    checksum = checksum ^ src[i];
  }
	printf("[奇偶校验] OK !\r\n");
  return checksum;
	
	
}

// } // namespace libjt808
