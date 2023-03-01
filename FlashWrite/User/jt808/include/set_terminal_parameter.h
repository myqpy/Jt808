#include "protocol_parameter.h"
#include "bcd.h"
#include "ff.h"
#include "util.h"
#include "./internal_flash/bsp_internal_flash.h"   
#define PARA_SETTING_LIMIT 10        //终端打包器命令相应消息ID数量

// 所有终端数据打包命令.
extern unsigned short kParameterSettingCMD[PARA_SETTING_LIMIT];

void jt808ParameterSettingParse(unsigned int id,unsigned char *buf,unsigned char buf_len, struct ProtocolParameter *para);
void handle_HeartBeatInterval(unsigned char *buf,unsigned char buf_len,struct ProtocolParameter *para);
void handle_MainServerAddress(unsigned char *buf,unsigned char buf_len,struct ProtocolParameter *para);
void handle_ServerPort(unsigned char *buf,unsigned char buf_len,struct ProtocolParameter *para);
void handle_DefaultTimeReportTimeInterval(unsigned char *buf,unsigned char buf_len,struct ProtocolParameter *para);
void handle_CornerPointRetransmissionAngle(unsigned char *buf,unsigned char buf_len,struct ProtocolParameter *para);
void handle_MaxSpeed(unsigned char *buf,unsigned char buf_len,struct ProtocolParameter *para);
void handle_ProvinceID(unsigned char *buf,unsigned char buf_len,struct ProtocolParameter *para);
void handle_CityID(unsigned char *buf,unsigned char buf_len,struct ProtocolParameter *para);
void handle_CarPlateNum(unsigned char *buf,unsigned char buf_len,struct ProtocolParameter *para);
void handle_CarPlateColor(unsigned char *buf,unsigned char buf_len,struct ProtocolParameter *para);
