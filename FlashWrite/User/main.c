#include "./usart/usart.h"
#include "./usart2/usart2.h"
#include "./led/bsp_led.h"
#include "./gps/gps_config.h"
#include "./delay/delay.h"
#include "./ec20/ec20.h"
#include "./sys/sys.h"
#include "string.h"
#include "client_manager.h"
#include "jt808_packager.h"
#include "./internal_flash/bsp_internal_flash.h"  
#include "ff.h"
#include <stdio.h>
#include <stdint.h>
#include <terminal_parameter.h>

int main(void)
{
	parameter_.parse.terminal_parameters.HeartBeatInterval = 2;

	memset(parameter_.parse.terminal_parameters.MainServerAddress,0,sizeof(parameter_.parse.terminal_parameters.MainServerAddress));
	
// 研究院平台
//	memcpy(parameter_.parse.terminal_parameters.MainServerAddress,"121.5.140.126", sizeof("121.5.140.126"));
//	memcpy(parameter_.parse.terminal_parameters.MainServerAddress,"http://jt808.gps.ciicp.com", sizeof("http://jt808.gps.ciicp.com"));
	
//	天瑞平台
	memcpy(parameter_.parse.terminal_parameters.MainServerAddress,"123.60.47.210", sizeof("123.60.47.210"));

	parameter_.parse.terminal_parameters.ServerPort = 7611;

	parameter_.parse.terminal_parameters.DefaultTimeReportTimeInterval = 5;

	parameter_.parse.terminal_parameters.CornerPointRetransmissionAngle = 10;

	parameter_.parse.terminal_parameters.MaxSpeed = 60;

	parameter_.parse.terminal_parameters.ProvinceID = 0x0029;

	parameter_.parse.terminal_parameters.CityID = 0x0066;
	
	parameter_.parse.terminal_parameters.CarPlateColor = 0x02;
	
	parameter_.parse.terminal_parameters.initFactoryParameters = 1;
	
	memset(parameter_.parse.terminal_parameters.version,0,sizeof(parameter_.parse.terminal_parameters.version));
	memcpy(parameter_.parse.terminal_parameters.version, "v1.2", 5);
	
	parameter_.parse.terminal_parameters.bootLoaderFlag = 0XFFFFFFFF;
	
	
	memset(parameter_.parse.terminal_parameters.PhoneNumber,0, 12);
	memset(parameter_.parse.terminal_parameters.CarPlateNum,0,sizeof(parameter_.parse.terminal_parameters.CarPlateNum));
	memset(parameter_.parse.terminal_parameters.TerminalId,0, 8);
	
	memcpy(parameter_.parse.terminal_parameters.PhoneNumber, "100221000019" , 12);
	memcpy(parameter_.parse.terminal_parameters.TerminalId, "1000019" , 8);
	memcpy(parameter_.parse.terminal_parameters.CarPlateNum, "豫A00019", 9);
	
	
	FLASH_WriteByte((uint32_t)0x0803c000 , (uint8_t *)&parameter_.parse.terminal_parameters , sizeof(parameter_.parse.terminal_parameters));
	
	return 0;
}

