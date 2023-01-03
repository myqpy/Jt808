#include "terminal_register.h"  
#include "./internal_flash/bsp_internal_flash.h" 
#include <stdint.h>
#include "string.h"

struct RegisterInfo registerInfo_;

////struct ReadInfo readFlashInfo;

///*
//unsigned short provinceId_ = 0x0029; // 2022.10.26,暂存
//// 省域ID.
///// @brief 设置注册省域ID
///// @param provinceId
//void setRegisterProvinceId(unsigned short provinceId)
//{
//    provinceId_ = provinceId;
//}
//*/
///// @brief 获取省域ID
///// @return
//unsigned short getRegisterProvinceId()
//{
//    //return provinceId_;
//	return readFlashInfo.read_province_id;
//}
///*
//// 市县域ID.
//unsigned short cityId_ = 0x0066; // 2022.10.26,暂存

///// @brief 设置市县域ID
///// @param cityId
//void setRegisterCityId(unsigned short cityId)
//{
//    cityId_ = cityId;
//}
//*/
///// @brief 获取市县域ID
///// @return
//unsigned short getRegisterCityId()
//{
//    //return cityId_;
//	return readFlashInfo.read_city_id;
//}


////// 制造商ID, 固定5个字节.
//////const char *manufacturer_id_ = "XINDA";
////char manufacturer_id_[20] = "XINDA";
////void setRegister_manufacturer_id(const char *manufacturerId, unsigned int manufacturer_id_size)
////{
////    unsigned int len_limit = 5;
////		unsigned int len;
////    memset(manufacturer_id_, 0, strlen(manufacturer_id_));
////    len = (manufacturer_id_size >= len_limit) ? len_limit : manufacturer_id_size;
////    memcpy(manufacturer_id_, manufacturerId, len);
////}

//const char *getRegister_manufacturer_id()
//{
//    //return manufacturer_id_;
//	return readFlashInfo.read_manufacturer_id;
//}


////// 终端型号, 固定20个字节, 位数不足后补0x00.
//////const char *terminal_model_ = "ZXIAT-CZ01";
////unsigned char terminal_model_ [40] = "ZXIAT-CZ02";
////void setRegister_terminal_model(const char *terminalModel, unsigned int terminalModel_size)
////{
////    unsigned int len_limit = 20;
////		unsigned int len;
////    memset(terminal_model_, 0, strlen(terminal_model_));
////    len = (terminalModel_size >= len_limit) ? len_limit : terminalModel_size;
////    memcpy(terminal_model_, terminalModel, len);
////}

///// @brief 获取终端型号
///// @return
//const char *getRegister_terminal_model()
//{
//    return readFlashInfo.read_terminal_model;
//}


////// 终端ID, 固定7个字节, 位数不足后补0x00.
////char terminal_id_ [20]= "0200001";
////void setRegister_terminal_id(const char *terminal_id, unsigned int terminal_id_size)
////{
////    unsigned int len_limit = 7;
////		unsigned int len;
////    memset(terminal_id_, 0, strlen(terminal_id_));
////    len = (terminal_id_size >= len_limit) ? len_limit : terminal_id_size;
////    memcpy(terminal_id_, terminal_id, len);
////}

///// @brief 获取终端型号
///// @return
//const char *getRegister_terminal_id()
//{
//    return readFlashInfo.read_terminal_id;
//}

//// 车牌颜色, 0表示未上牌.
////unsigned char car_plate_color_ = 0x02;
////void setRegister_car_plate_color(unsigned char car_plate_color)
////{
////    car_plate_color_ = car_plate_color;
////}

///// @brief 获取市县域ID
///// @return
//unsigned char getRegister_car_plate_color()
//{
//    return readFlashInfo.read_car_plate_color;
//}

////// 车辆标识, 仅在上牌时使用.
//////const char *car_plate_num_ = "豫778899";
////unsigned char car_plate_num_ [12] = "豫Aab111";

////void setRegister_car_plate_num(const char *car_plate_num, unsigned int car_plate_num_size)
////{
////    unsigned int len_limit = 20;
////		unsigned int len;
////    memset(car_plate_num_, 0, strlen(car_plate_num_));
////    len = (car_plate_num_size >= len_limit) ? len_limit : car_plate_num_size;
////    memcpy(car_plate_num_, car_plate_num, len);
////}

///// @brief 获取终端型号
///// @return
//const char *getRegister_car_plate_num()
//{
//    return readFlashInfo.read_car_plate_num;
//}

void initRegisterInfo(struct ProtocolParameter *para, struct ReadInfo readFlashInfo)
{
		int i, max_len, real_len;
		int lenManufacturer, lenTModel, lenTerminalId;
//		char *ptr_manufacturer_id;
//		char *ptr_terminal_model;
//		char *ptr_terminal_id;
    printf("\n\r[initRegisterInfo] OK !\n");
    //省域ID
    //para->register_info.province_id = getRegisterProvinceId();
	  para->register_info.province_id = readFlashInfo.read_province_id;
    printf("para->register_info.province_id = %d\r\n", para->register_info.province_id);

    //市域ID
    //para->register_info.city_id = getRegisterCityId();
		para->register_info.city_id = readFlashInfo.read_city_id;
    printf("para->register_info.city_id = %04d\r\n", para->register_info.city_id);

    //制造商ID
    //ptr_manufacturer_id = getRegister_manufacturer_id();
    //memcpy(para->register_info.manufacturer_id, readFlashInfo.read_manufacturer_id, 5);
		//printf("ptr_manufacturer_id = %s\r\n", ptr_manufacturer_id);
		lenManufacturer=sizeof(readFlashInfo.read_manufacturer_id);
		lenManufacturer=(lenManufacturer>5)?5:lenManufacturer;

    memset(para->register_info.manufacturer_id, 0, lenManufacturer);
		memcpy(para->register_info.manufacturer_id, readFlashInfo.read_manufacturer_id, lenManufacturer);
//    if (real_len >= max_len)
//    {
//        memcpy(para->register_info.manufacturer_id, readFlashInfo.read_manufacturer_id, max_len);
//    }
//    else
//    {
//        memcpy(para->register_info.manufacturer_id, ptr_manufacturer_id, real_len);
//        for (i = real_len; i < (max_len - real_len); ++i)
//        {
//            para->register_info.manufacturer_id[i] = 0x00;
//        }
//    }

    printf("para->register_info.manufacturer_id : ");
    for (i = 0; i < max_len; ++i)
    {
        printf("0x%02x ", para->register_info.manufacturer_id[i]);
    }
    printf("\r\n");

    //终端型号
    //ptr_terminal_model = getRegister_terminal_model();
		lenTModel=sizeof(readFlashInfo.read_terminal_model);
		lenTModel=(lenTModel>20)?20:lenTModel;
		
		memcpy(para->register_info.terminal_model, readFlashInfo.read_terminal_model, lenTModel);
//    printf("ptr_terminal_model = %s\r\n", ptr_terminal_model);
//    max_len = 20;
//    real_len = strlen(ptr_terminal_model);
//    memset(para->register_info.terminal_model, 0, sizeof(para->register_info.terminal_model));
//    if (real_len >= max_len)
//    {
//        memcpy(para->register_info.terminal_model, ptr_terminal_model, max_len);
//    }
//    else
//    {
//        memcpy(para->register_info.terminal_model, ptr_terminal_model, real_len);
//        for (i = real_len; i < (max_len - real_len); ++i)
//        {
//            para->register_info.terminal_model[i] = 0x00;
//        }
//    }
    printf("para->register_info.terminal_model : ");
    for (i = 0; i < max_len; ++i)
    {
        printf("0x%02x ", para->register_info.terminal_model[i]);
    }
    printf("\r\n");

    //终端ID
//    ptr_terminal_id = getRegister_terminal_id();
		lenTerminalId = sizeof(readFlashInfo.read_terminal_id);
		lenTerminalId=(lenTerminalId>20)?20:lenTerminalId;
		
//		memcpy(ptr_terminal_id, readFlashInfo.read_terminal_id, sizeof(readFlashInfo.read_terminal_id));
//    printf("ptr_terminal_id = %s\r\n", ptr_terminal_id);
//    max_len = 20;
//    real_len = strlen(ptr_terminal_id);
    memset(para->register_info.terminal_id, 0, lenTerminalId);
		memcpy(para->register_info.terminal_id, readFlashInfo.read_terminal_id, lenTerminalId);
//    if (real_len >= max_len)
//    {
//        memcpy(para->register_info.terminal_id, ptr_terminal_id, max_len);
//    }
//    else
//    {
//        memcpy(para->register_info.terminal_id, ptr_terminal_id, real_len);
//        for (i = real_len; i < (max_len - real_len); ++i)
//        {
//            para->register_info.terminal_id[i] = 0x00;
//        }
//    }
    printf("para->register_info.terminal_id : ");
    for (i = 0; i < max_len; ++i)
    {
        printf("0x%02x ", para->register_info.terminal_id[i]);
    }
    printf("\r\n");

    //车牌颜色
//    para->register_info.car_plate_color = getRegister_car_plate_color();
		para->register_info.car_plate_color = readFlashInfo.read_car_plate_color;
    printf("para->register_info.car_plate_color = 0x%02x\r\n", para->register_info.car_plate_color);

    //车牌号
    if (para->register_info.car_plate_color != 0x00)
    {
        memcpy(para->register_info.car_plate_num, readFlashInfo.read_car_plate_num, 12);
        printf("para->register_info.car_plate_num = %s\r\n", para->register_info.car_plate_num);
    }
    printf("\r\n ");
		
//		memcpy(buf_info, para->register_info, 37);
}
