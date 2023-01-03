#include <stdio.h>
#include <stdint.h>

#include <stdlib.h>

#define PACKAGER_NUM 9        //终端打包器命令相应消息ID数量
#define BUFFER_SIZE_SEND 4096 // buffer size 发送缓存大小

#include "protocol_parameter.h"
#include "bcd.h"

extern unsigned short kTerminalPackagerCMD[PACKAGER_NUM];
extern unsigned char BufferSend[BUFFER_SIZE_SEND]; //发送缓存
extern unsigned int RealBufferSendSize;
// extern  int RealBufferSendSize;

int jt808FramePackage(struct ProtocolParameter *para);
void updateLocation(struct ProtocolParameter *para, unsigned int v_alarm_value,
                    unsigned int v_status_value, double const v_latitude,
                    double const v_longitude, float const v_altitude,
                    float const v_speed, float const v_bearing,
                    unsigned char *v_timestamp);
