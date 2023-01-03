#include "terminal_register.h"  
#include "./internal_flash/bsp_internal_flash.h" 
//#include <stdint.h>
#include "ff.h"
#include "string.h"

struct RegisterInfo registerInfo_;

void initRegisterInfo(struct ProtocolParameter *para, struct ReadInfo readFlashInfo)
{
		int i, lenManufacturer, lenTModel, lenTerminalId;
    printf("\n\r[initRegisterInfo] OK !\n");
    //省域ID
	  para->register_info.province_id = readFlashInfo.read_province_id;
    printf("para->register_info.province_id = %d\r\n", para->register_info.province_id);

    //市域ID
		para->register_info.city_id = readFlashInfo.read_city_id;
    printf("para->register_info.city_id = %04d\r\n", para->register_info.city_id);

    //制造商ID
		lenManufacturer=sizeof(readFlashInfo.read_manufacturer_id);
		lenManufacturer=(lenManufacturer>5)?5:lenManufacturer;

    memset(para->register_info.manufacturer_id, 0, lenManufacturer);
		memcpy(para->register_info.manufacturer_id, readFlashInfo.read_manufacturer_id, lenManufacturer);

    printf("para->register_info.manufacturer_id : ");
    for (i = 0; i < lenManufacturer; ++i)
    {
        printf("0x%02x ", para->register_info.manufacturer_id[i]);
    }
    printf("\r\n");

    //终端型号
		lenTModel=sizeof(readFlashInfo.read_terminal_model);
		lenTModel=(lenTModel>20)?20:lenTModel;
		
		memcpy(para->register_info.terminal_model, readFlashInfo.read_terminal_model, lenTModel);

    printf("para->register_info.terminal_model : ");
    for (i = 0; i < lenTModel; ++i)
    {
        printf("0x%02x ", para->register_info.terminal_model[i]);
    }
    printf("\r\n");

    //终端ID
		lenTerminalId = sizeof(readFlashInfo.read_terminal_id);
		lenTerminalId=(lenTerminalId>20)?20:lenTerminalId;
		
    memset(para->register_info.terminal_id, 0, lenTerminalId);
		memcpy(para->register_info.terminal_id, readFlashInfo.read_terminal_id, lenTerminalId);

    printf("para->register_info.terminal_id : ");
    for (i = 0; i < lenTerminalId; ++i)
    {
        printf("0x%02x ", para->register_info.terminal_id[i]);
    }
    printf("\r\n");

    //车牌颜色
		para->register_info.car_plate_color = readFlashInfo.read_car_plate_color;
    printf("para->register_info.car_plate_color = 0x%02x\r\n", para->register_info.car_plate_color);

    //车牌号
    if (para->register_info.car_plate_color != 0x00)
    {
        memcpy(para->register_info.car_plate_num, readFlashInfo.read_car_plate_num, 12);
        printf("para->register_info.car_plate_num = %s\r\n", para->register_info.car_plate_num);
    }
    printf("\r\n ");
}
