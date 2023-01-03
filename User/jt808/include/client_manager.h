

#ifndef JT808_CLIENT_MANAGER_H_
#define JT808_CLIENT_MANAGER_H_



#include <stdio.h>
#include <stdint.h>
#include "./sys/sys.h"
#include <stdlib.h>
#include "ff.h"



//#define __JT808_DEBUG
#define FLASH_ADDR (uint32_t)0x0800f000



extern struct ProtocolParameter parameter_;

void setTerminalPhoneNumber(const char *phone_num, unsigned int phoneSize);

/******************************************************************************
 * @description: 数据打包并发送接口
 * @param {unsigned int} msg_id  消息ID
 * @return {*}
 * @author: ZTL
 *******************************************************************************/
int packagingAndSendMessage(unsigned int msg_id);

/******************************************************************************
 * @description: 数据打包接口
 * @param {unsigned int} msg_id  JT808消息ID
 * @param { unsigned int} *realBufSize   用于记录实际打包待发送的buffer长度
 * @return {0}成功   {-1}失败
 * @author: ZTL
 *******************************************************************************/
int packagingMessage(unsigned int msg_id);

/******************************************************************************
 * @description: 从终端打包器命令数组中查找是否有当前相应的消息ID，以便根据该命令数组中的消息ID调用相应的接口
 * @param {unsigned int} msg_id  消息ID
 * @return {0}失败 {1}成功
 * @author: ZTL
 *******************************************************************************/
int findMsgIDFromTerminalPackagerCMD(unsigned int msg_id);
int findParameterIDFromArray(unsigned int para_id);
int parsingMessage(const unsigned char *in, unsigned int in_len);
int jt808TerminalRegister(int *isRegistered);
int jt808TerminalAuthentication(int *isAuthenticated);
int jt808LocationReport(void);
int jt808TerminalLogOut(void);
int jt808TerminalHeartBeat(void);
int jt808TerminalGeneralResponse(void);
void setStatusBit(void);
int FlashWrite(void);
int IPFlashWrite(void);
void system_reboot(void);
ErrorStatus ec20_init(void);
void initSystemParameters(void);
void initLocationInfo(unsigned int v_alarm_value, unsigned int v_status_value);
void updateLocation(double const v_latitude, double const v_longitude, float const v_altitude,
                    float const v_speed, float const v_bearing, unsigned char *v_timestamp);

#endif // JT808_CLIENT_MANAGER_H_
