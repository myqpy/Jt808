#ifndef JT808_TERMINAL_REGISTER_H_
#define JT808_TERMINAL_REGISTER_H_


#include <stdio.h>
#include <stdlib.h>
//#include <memory.h>
#include <string.h>
#include "protocol_parameter.h"

#include "./internal_flash/bsp_internal_flash.h" 
/// @brief 设置注册省域ID
/// @param provinceId
void setRegisterProvinceId(unsigned short provinceId);

/// @brief 获取省域ID
/// @return
unsigned short getRegisterProvinceId(void);
 
/// @brief 设置市县域ID
/// @param cityId
void setRegisterCityId(unsigned short cityId);

/// @brief 获取市县域ID
/// @return
unsigned short getRegisterCityId(void);

// 制造商ID, 固定5个字节. 
void setRegister_manufacturer_id(const char *manufacturerId, unsigned int manufacturer_id_size);

const char *getRegister_manufacturer_id(void);

// 终端型号, 固定20个字节, 位数不足后补0x00. 
void setRegister_terminal_model(const char *terminalModel, unsigned int terminalModel_size);

/// @brief 获取终端型号
/// @return
const char *getRegister_terminal_model(void);

// 终端ID, 固定7个字节, 位数不足后补0x00. 
void setRegister_terminal_id(const char *terminal_id, unsigned int terminal_id_size);

/// @brief 获取终端型号
/// @return
const char *getRegister_terminal_id(void);

// 车牌颜色, 0表示未上牌. 
void setRegister_car_plate_color(unsigned char car_plate_color);

/// @brief 获取市县域ID
/// @return
unsigned char getRegister_car_plate_color(void);

// 车辆标识, 仅在上牌时使用.
void setRegister_car_plate_num(const char *car_plate_num, unsigned int car_plate_num_size);

/// @brief 获取终端型号
/// @return
//const char *getRegister_car_plate_num(void);

//void initRegisterInfo(struct ProtocolParameter *para);
void initRegisterInfo(struct ProtocolParameter *para, struct ReadInfo readFlashInfo);


#endif  //JT808_TERMINAL_REGISTER_H_

