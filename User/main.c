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
//#include <stdio.h>
//#include <stdlib.h>
//#define IPSERVER "121.5.140.126"
//#define PORTSERVER 8089

#define IPSERVER "121.5.140.126"
#define PORTSERVER 7611
extern int nmea_decode_test(double *v_latitude, double *v_longitude, float *v_altitude, float  *v_speed, float *v_bearing, unsigned char *v_timestamp);
ErrorStatus ec20_init(void);


int main(void)
{
	
	struct RegisterInfo
	{
		unsigned short province_id;

		unsigned short city_id;

		unsigned char manufacturer_id[5];

		unsigned char terminal_model[20];

		unsigned char terminal_id[7];

		unsigned char car_plate_color;

		unsigned char car_plate_num[12];
	};
	unsigned char buf_info[37] = {0};
	
	
	struct RegisterInfo *info;
	info->province_id = 0x0029;
	info->city_id = 0x0066;
	uart_init(115200);
	//info->manufacturer_id = "XINDA";
	memcpy(info->manufacturer_id, "XINDA", 5);
	//info->terminal_model = "ZXIAT-CZ02";
	memcpy(info->terminal_model, "ZXIAT-CZ02", 10);
	//info->terminal_id = "0200001";
	memcpy(info->terminal_id, "0200001", 7);
	info->car_plate_color = 0x02;
	//info->car_plate_num = "AAab111";
	memcpy(info->car_plate_num, "AAab111", 7);
	
	memcpy(buf_info, info, 37);
	
	uart_init(115200);
	if(InternalFlash_Test((uint32_t)(buf_info))== PASSED)
	{
		printf("读写内部FLASH测试成功\r\n");

	}
	else
	{
		printf("读写内部FLASH测试失败\r\n");
	}
	while(1);
}

/*
int main(void)
{
	int i;
	unsigned int v_alarm_value = 0;
	unsigned int v_status_value = 0;
//  double  v_latitude = 34.741348;
//	double  v_longitude = 113.701872;
	double  v_latitude = 34.824788;
	double  v_longitude = 113.558408;
	float  v_altitude = 107;
	float  v_speed = 15;
	float  v_bearing = 132;
	unsigned char v_timestamp[] = "221127212855";
	

//	LED_GPIO_Config();	//LED 端口初始化
//  
//  GPIO_SetBits(GPIOD,GPIO_Pin_2);

	NVIC_Configuration(); 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	delay_init();	    	 		//延时函数初始化
	uart_init(115200); //串口初始化
  USART2_Init(115200);
  GPS_Config();
	
	//Connecting Server;
	while(1)
	{
		if(ec20_init() == SUCCESS)
		{	
			printf("server connected\r\n");
			break;
		}
		delay_ms(2000);
	}
  setTerminalPhoneNumber("20220200001", 11);
	packagingMessage(kTerminalRegister);
	delay_ms(1000);
	Usart_SendStr_length(USART2, BufferSend, RealBufferSendSize);

	while(1)
	{
		delay_ms(100);
		if(USART2_RX_STA&0X8000)    //接收到数据
		{
			USART2_RX_STA = USART2_RX_STA&0x7FFF;//获取到实际字符数量
			//开始校验
			parsingMessage(USART2_RX_BUF, USART2_RX_STA);
			printf("Register done!\r\n");
			printf("Client receive bytes: %d\r\n", USART2_RX_STA);
			USART2_RX_STA=0;
			break;
			
		}
	}
  packagingMessage(kTerminalAuthentication);

//  printf("RealBufferSendSize = %d \n", RealBufferSendSize);
//  delay_ms(1000);
//  for (i = 0; i < RealBufferSendSize; ++i)
//  {
//    printf("%02X ", BufferSend[i]);
//  }
//  printf("\r\n");
	Usart_SendStr_length(USART2, BufferSend, RealBufferSendSize);
	while(1)
	{
		delay_ms(100);
		if(USART2_RX_STA&0X8000)    //接收到数据
		{
			USART2_RX_STA = USART2_RX_STA&0x7FFF;//获取到实际字符数量
			//开始校验
			parsingMessage(USART2_RX_BUF, USART2_RX_STA);
			printf("Authentication done!\r\n");
			printf("Client receive bytes: %d\r\n", USART2_RX_STA);
			USART2_RX_STA=0;
			break;
		}
	}
	
	initLocationInfo(v_alarm_value, v_status_value);
	setStatusBit();
	
	updateLocation(v_latitude, v_longitude, v_altitude, v_speed, v_bearing, v_timestamp);

	packagingMessage(kLocationReport);
	Usart_SendStr_length(USART2, BufferSend, RealBufferSendSize);
	while(1)
	{
		delay_ms(100);
		if(USART2_RX_STA&0X8000)    //接收到数据
		{
			USART2_RX_STA = USART2_RX_STA&0x7FFF;//获取到实际字符数量
			//开始校验
			parsingMessage(USART2_RX_BUF, USART2_RX_STA);
			printf("updata location done!\r\n");
			printf("Client receive bytes: %d\r\n", USART2_RX_STA);
			USART2_RX_STA=0;
			break;
		}
	}
	
	while(1)
	{
		nmea_decode_test(&v_latitude, &v_longitude, &v_altitude, &v_speed, &v_bearing, v_timestamp);
		
		updateLocation(v_latitude, v_longitude, v_altitude, v_speed, v_bearing, v_timestamp);
		packagingMessage(kLocationReport);
		printf("RealBufferSendSize = %d \r\n", RealBufferSendSize);

		Usart_SendStr_length(USART2, BufferSend, RealBufferSendSize);
		while(1)
		{
			delay_ms(100);
			if(USART2_RX_STA&0X8000)    //接收到数据
			{
				USART2_RX_STA = USART2_RX_STA&0x7FFF;//获取到实际字符数量
				//开始校验
				parsingMessage(USART2_RX_BUF, USART2_RX_STA);
				USART2_RX_STA=0;
				break;
			}
		}
	}
}

ErrorStatus ec20_init(void)
{
    u8 data=0,ret=0;
    u8 err=0;
    char atstr[BUFLEN];
    USART2_RX_STA=0;
    if(ec20_send_cmd("AT","OK","NULL","NULL",1000))err|=1<<0;//检测是否应答AT指令
    USART2_RX_STA=0;
    if(ec20_send_cmd("ATE0","OK","NULL","NULL",2000))err|=1<<1;//不回显
    USART2_RX_STA=0;
    if(ec20_send_cmd("AT+CPIN?","OK","NULL","NULL",2000))err|=1<<3;	//查询SIM卡是否在位
    USART2_RX_STA=0;
    data = 0;
    //查询GSM网络注册状态，确认找网成功
    while (ec20_send_cmd("AT+CREG?\r\n","\r\n+CREG: 0,1","NULL","NULL",2000)!= 1 && data < 10)
    {
        USART2_RX_STA=0;
        delay_ms(100);
        data++;
    }
    USART2_RX_STA=0;
    if (data == 10)
    {
        return ERROR;                                                                             //找网不成功模块重启
    }
    ec20_send_cmd("AT+CGATT?\r\n","+CGATT: 1","OK","NULL",2000);
    USART2_RX_STA=0;
    delay_ms(200);
    ec20_send_cmd("AT+QIACT?\r\n","OK","NULL","NULL",2000);

    USART2_RX_STA=0;
    delay_ms(200);
    ec20_send_cmd("AT+QICLOSE=0\r\n","OK","NULL","NULL",2000);
    USART2_RX_STA=0;
    delay_ms(200);
    memset(atstr,0,BUFLEN);
    sprintf(atstr,"AT+QIOPEN=1,0,\"TCP\",\"%s\",%d,0,2\r\n",IPSERVER,PORTSERVER);
    data=ec20_send_cmd((u8*)atstr,"CONNECT","OK","NULL",2000);
    USART2_RX_STA=0;
    delay_ms(200);
    USART2_RX_STA=0;
    if (data == 1 || data == 2 || data == 3 || ret==1)
    {
        printf("data=%d\r\n",data);
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
} 
*/
/*
void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
*/
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
