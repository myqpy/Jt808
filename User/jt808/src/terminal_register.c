#include "terminal_register.h"  
#include "string.h"

struct RegisterInfo registerInfo_;

void initRegisterInfo(struct ProtocolParameter *para)
{
	int lenManufacturer, lenTModel;
	printf("\r\n[ initRegisterInfo ] OK !\r\n");
	//省域ID
	para->register_info.province_id = para->parse.terminal_parameters.ProvinceID;
	printf("para->register_info.province_id = %d\r\n", para->register_info.province_id);

	//市域ID
	para->register_info.city_id = para->parse.terminal_parameters.CityID;
	printf("para->register_info.city_id = %04d\r\n", para->register_info.city_id);

	//制造商ID
	
//	lenManufacturer=sizeof("410102XINDA");
	lenManufacturer=(lenManufacturer>11)?11:lenManufacturer;
//	sprintf(para->register_info.manufacturer_id,"%03d%03dXINDA",para->register_info.province_id,para->register_info.city_id);
	memset(para->register_info.manufacturer_id, 0, lenManufacturer);
	memcpy(para->register_info.manufacturer_id, "410102XINDA", lenManufacturer);
	printf("para->register_info.manufacturer_id = %s\r\n", para->register_info.manufacturer_id);

	//终端型号
	lenTModel=sizeof("XINDA-CZ03");
	lenTModel=(lenTModel>30)?30:lenTModel;
	
	memset(para->register_info.terminal_model, 0, lenTModel);
	memcpy(para->register_info.terminal_model, "XINDA-CZ03", lenTModel);	
	printf("para->register_info.terminal_model = %s\r\n", para->register_info.terminal_model);

//	//终端ID
//	lenTerminalId = sizeof("1000000");
//	lenTerminalId=(lenTerminalId>20)?20:lenTerminalId;

//	memset(para->register_info.terminal_id, 0, lenTerminalId);
//	memcpy(para->register_info.terminal_id, "1000000", lenTerminalId);
//	printf("para->register_info.terminal_id = %s\r\n", para->register_info.terminal_id);

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
