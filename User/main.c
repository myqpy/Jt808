/**
  ******************************************************************************
  * @file    main.c
  * @author  WJSHM
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   对GPS模块传输的数据进行解码，获取定位信息。
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

//#include "stm32f10x.h"
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

//#define IPSERVER "121.5.140.126"
//#define PORTSERVER 8089

#define IPSERVER "121.5.140.126"
#define PORTSERVER 7611
extern int nmea_decode_test(void);
ErrorStatus ec20_init(void);

int main(void)
{
	int i;
		
	unsigned int v_alarm_value = 0;
	unsigned int v_status_value = 0;
	double const v_latitude = 34.824788;
	double const v_longitude = 113.558408;
	float const v_altitude = 107;
	float const v_speed = 15;
	float const v_bearing = 132;
	unsigned char *v_timestamp = "221127212855";
	
	
	LED_GPIO_Config();	//LED 端口初始化
  
  GPIO_SetBits(GPIOD,GPIO_Pin_2);

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
  setTerminalPhoneNumber("15637142115", 11);
	printf("will send !\r\n");
	
	printf("RealBufferSendSize = %d \r\n", RealBufferSendSize);
	delay_ms(1000);
	packagingMessage(kTerminalRegister);
	delay_ms(1000);
	printf("RealBufferSendSize = %d \r\n", RealBufferSendSize);
	
	for (i = 0; i < RealBufferSendSize; i++)
	{
		printf("%02x ",BufferSend[i]);
	}
	printf("\r\n");
	Usart_SendStr_length(USART2, BufferSend, RealBufferSendSize);
	
	printf("send ok!\r\n");			
	printf("sendbytes: %d\r\n", RealBufferSendSize);
	printf("send done !\r\n");

	printf("will recv !\r\n");
	
	while(1)
	{
		//printf("1");
		delay_ms(100);
		if(USART2_RX_STA&0X8000)    //接收到数据
		{
			USART2_RX_STA = USART2_RX_STA&0x7FFF;//获取到实际字符数量
			//开始校验
			parsingMessage(USART2_RX_BUF, USART2_RX_STA);
			printf("recv done!\r\n");
			printf("Client receive bytes: %d\r\n", USART2_RX_STA);
			USART2_RX_STA=0;
			break;
		}
	}
	
	
  packagingMessage(kTerminalAuthentication);

  printf("RealBufferSendSize = %d \n", RealBufferSendSize);
  delay_ms(1000);
  for (i = 0; i < RealBufferSendSize; ++i)
  {
    printf("%02X ", BufferSend[i]);
  }
  printf("\r\n");
  // sleep(1);
	Usart_SendStr_length(USART2, BufferSend, RealBufferSendSize);
	printf("send ok!\r\n");
	printf("sendbytes: %d\r\n", RealBufferSendSize);
  printf("send done !\r\n");

  printf("will recv !\r\n");
	
	while(1)
	{
		delay_ms(100);
		if(USART2_RX_STA&0X8000)    //接收到数据
		{
			USART2_RX_STA = USART2_RX_STA&0x7FFF;//获取到实际字符数量
			//开始校验
			parsingMessage(USART2_RX_BUF, USART2_RX_STA);
			printf("recv done!\r\n");
			printf("Client receive bytes: %d\r\n", USART2_RX_STA);
			USART2_RX_STA=0;
			break;
		}
	}
	
	initLocationInfo(v_alarm_value, v_status_value);
	setStatusBit();
	
	
	while(1)
	{
		delay_ms(100);
		updateLocation(v_latitude, v_longitude, v_altitude, v_speed, v_bearing, v_timestamp);
		printf("will send !\r\n");
		printf("RealBufferSendSize = %d \r\n", RealBufferSendSize);
		delay_ms(1000);
		packagingMessage(kLocationReport);
		delay_ms(1000);
		printf("RealBufferSendSize = %d \r\n", RealBufferSendSize);

		for (i = 0; i < RealBufferSendSize; i++)
		{
			printf("%02x ",BufferSend[i]);
		}
		printf("\r\n");
		Usart_SendStr_length(USART2, BufferSend, RealBufferSendSize);
		
		printf("send ok!\r\n");			
		printf("sendbytes: %d\r\n", RealBufferSendSize);
		printf("send done !\r\n");
		while(1)
		{
			delay_ms(100);
			if(USART2_RX_STA&0X8000)    //接收到数据
			{
				USART2_RX_STA = USART2_RX_STA&0x7FFF;//获取到实际字符数量
				//开始校验
				parsingMessage(USART2_RX_BUF, USART2_RX_STA);
				printf("recv done!\r\n");
				printf("Client receive bytes: %d\r\n", USART2_RX_STA);
				USART2_RX_STA=0;
				nmea_decode_test();
				break;
			}
		}
	}
  //nmea_decode_test(); //GPS解码测试
  
  while(1);


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
/*
void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
*/
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
