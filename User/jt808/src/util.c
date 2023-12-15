#include "util.h"
#include "protocol_parameter.h"
#include "client_manager.h"

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
		printf("[%s] FAILED \r\n",__FUNCTION__);
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
	#ifdef __JT808_DEBUG
		printf("[ReverseEscape_C]:");
	#endif

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
			#ifdef __JT808_DEBUG
				printf("%02x ", out[i - offset_num]);
			#endif
    }
  }
	#ifdef __JT808_DEBUG
		printf("\r\n");
	#endif

  *outlen = inlen - offset_num;

  return 0;
}

// 异或校验.
unsigned char BccCheckSum(const unsigned char *src, unsigned long len)
{
  unsigned char checksum = 0;
	unsigned long i;
  for (i = 0; i < len; ++i)
  {
    checksum = checksum ^ src[i];
  }
	#ifdef __JT808_DEBUG
		printf("[BccCheckSum] OK !\r\n");
	#endif
  return checksum;
	
	
}
//处理字符串，返回子串在母串的第一个字符的位置
int strStr(const char * haystack,  const char * needle)
{
	int len1=strlen(haystack);
	int len2=strlen(needle);
	int i=0,j=0,k=0;
	bool jieguo=true;
	if(len1<len2)
	{
		jieguo=false;
	}
	else
	{
		for(i=0;i<=len1-len2;i++)
	{
	for(j=i,k=0;j<i+len2;j++,k++)
	{
		if(haystack[j]!=needle[k])
		{
			jieguo=false;
			break;
		}
		if(haystack[j]==needle[k])
		{
			jieguo=true;
		}
	}
	if(jieguo==true)
	{
		break;
	}
	}
	}
	if(jieguo==false)
	{
		return -1;
	}
	else
	{
		return i;
	}
}
