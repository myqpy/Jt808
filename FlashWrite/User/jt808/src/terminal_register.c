#include "terminal_register.h"  
#include "./internal_flash/bsp_internal_flash.h" 
//#include <stdint.h>
#include "ff.h"
#include "string.h"

struct RegisterInfo registerInfo_;

void initRegisterInfo(struct ProtocolParameter *para)
{
		int lenManufacturer, lenTModel, lenTerminalId;
    printf("\r\n[initRegisterInfo] OK !\r\n");
    //省域ID
	  //para->register_info.province_id = readFlashInfo.read_province_id;
    para->register_info.province_id = para->parse.terminal_parameters.ProvinceID;
		printf("para->register_info.province_id = %d\r\n", para->register_info.province_id);

    //市域ID
		para->register_info.city_id = para->parse.terminal_parameters.CityID;
    printf("para->register_info.city_id = %04d\r\n", para->register_info.city_id);

//    //制造商ID
//		lenManufacturer=sizeof(readFlashInfo.read_manufacturer_id);
//		lenManufacturer=(lenManufacturer>5)?5:lenManufacturer;
		lenManufacturer=sizeof("XINDA");
		lenManufacturer=(lenManufacturer>5)?5:lenManufacturer;
	
		memset(para->register_info.manufacturer_id, 0, lenManufacturer);
		memcpy(para->register_info.manufacturer_id, "XINDA", lenManufacturer);
			//memcpy(para->register_info.manufacturer_id, readFlashInfo.read_manufacturer_id, lenManufacturer);

//    printf("para->register_info.manufacturer_id : ");
//    for (i = 0; i < lenManufacturer; ++i)
//    {
//        printf("0x%02x ", para->register_info.manufacturer_id[i]);
//    }
//    printf("\r\n");

//    //终端型号
//		lenTModel=sizeof(readFlashInfo.read_terminal_model);
//		lenTModel=(lenTModel>20)?20:lenTModel;
			lenTModel=sizeof("ZXIAT-CZ05");
			lenTModel=(lenTModel>20)?20:lenTModel;
			
			memset(para->register_info.terminal_model, 0, lenTModel);
			memcpy(para->register_info.terminal_model, "ZXIAT-CZ05", lenTModel);	
//		memcpy(para->register_info.terminal_model, readFlashInfo.read_terminal_model, lenTModel);

//    printf("para->register_info.terminal_model : ");
//    for (i = 0; i < lenTModel; ++i)
//    {
//        printf("0x%02x ", para->register_info.terminal_model[i]);
//    }
//    printf("\r\n");

//    //终端ID
//		lenTerminalId = sizeof(readFlashInfo.read_terminal_id);
//		lenTerminalId=(lenTerminalId>20)?20:lenTerminalId;
//		
			lenTerminalId = sizeof("221219");
			lenTerminalId=(lenTerminalId>20)?20:lenTerminalId;

			memset(para->register_info.terminal_id, 0, lenTerminalId);
			memcpy(para->register_info.terminal_id, "221219", lenTerminalId);
//		memcpy(para->register_info.terminal_id, readFlashInfo.read_terminal_id, lenTerminalId);

//    printf("para->register_info.terminal_id : ");
//    for (i = 0; i < lenTerminalId; ++i)
//    {
//        printf("0x%02x ", para->register_info.terminal_id[i]);
//    }
//    printf("\r\n");




    //车牌颜色
		para->register_info.car_plate_color = para->parse.terminal_parameters.CarPlateColor;
    printf("para->register_info.car_plate_color = 0x%02x\r\n", para->register_info.car_plate_color);

    //车牌号
    if (para->register_info.car_plate_color != 0x00)
    {
        memcpy(para->register_info.car_plate_num, para->parse.terminal_parameters.CarPlateNum, 12);
        printf("para->register_info.car_plate_num = %s\r\n", para->register_info.car_plate_num);
    }
    printf("\r\n");
		printf("注册信息更新完成\r\n");
		printf("\r\n");

		
}
