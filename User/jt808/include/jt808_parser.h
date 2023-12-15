

#ifndef JT808_PARSER_H_
#define JT808_PARSER_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "protocol_parameter.h"
#include "ff.h"

#define PARSER_NUM 9
#define BUFFER_SIZE_RECEIVE 1024 // buffer size 发送缓存大小

extern unsigned short kTerminalParserCMD[PARSER_NUM];

int jt808FrameParse(const unsigned char *in, unsigned int in_len, struct ProtocolParameter *para);

#endif // JT808_PARSER_H_
