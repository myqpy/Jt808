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



extern int nmea_decode_test(double *v_latitude, double *v_longitude, float *v_altitude, float  *v_speed, float *v_bearing, unsigned char *v_timestamp);

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
	unsigned char write_buf[50] = {0};
	struct WriteInfo flashWriteInfo;
	
	
	flashWriteInfo.write_province_id = 0x0029;
	flashWriteInfo.write_city_id = 0x0066;
	memcpy(flashWriteInfo.write_manufacturer_id, "XINDA", 5);
	memcpy(flashWriteInfo.write_terminal_model, "ZXIAT-CZ12", 10);
	memcpy(flashWriteInfo.write_terminal_id, "221212", 7);
	flashWriteInfo.write_car_plate_color = 0x02;
	memcpy(flashWriteInfo.write_car_plate_num, "测试12", 7);
	
	
	memset(write_buf,0,sizeof(write_buf));
	memcpy(write_buf, &flashWriteInfo, sizeof(flashWriteInfo));
	
	FLASH_WriteByte(((uint32_t)0x08008000) , write_buf , sizeof(write_buf));

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
  setTerminalPhoneNumber("10000001212", 11);

	
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

