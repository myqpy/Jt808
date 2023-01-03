#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define PACKAGER_NUM 9        //终端打包器命令相应消息ID数量
#define BUFFER_SIZE_SEND 1024 // buffer size 发送缓存大小

#include "protocol_parameter.h"
#include "terminal_register.h"
#include "bcd.h"
#include "ff.h"

extern unsigned short kTerminalPackagerCMD[PACKAGER_NUM];
extern unsigned char BufferSend[BUFFER_SIZE_SEND]; //发送缓存s
extern unsigned int RealBufferSendSize;

int jt808FramePackage(struct ProtocolParameter *para);

