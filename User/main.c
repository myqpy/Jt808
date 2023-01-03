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

#include <stdio.h>
#include <stdint.h>
//#define IPSERVER "121.5.140.126"
//#define PORTSERVER 8089

#define IPSERVER "121.5.140.126"
#define PORTSERVER 7611
extern int nmea_decode_test(double *v_latitude, double *v_longitude, float *v_altitude, float  *v_speed, float *v_bearing, unsigned char *v_timestamp);
ErrorStatus ec20_init(void);


//int main(void)
//{
//	int i;

//	
//	unsigned short read_province_id = 0;

//	unsigned short read_city_id = 0;

//	unsigned char read_manufacturer_id[6]={0};

//	unsigned char read_terminal_model[20]={0};

//	unsigned char read_terminal_id[7]={0};

//	unsigned char read_car_plate_color = 0;

//	unsigned char read_car_plate_num[12]={0};
//	
//	unsigned char write_buf[50] = {0};
//	unsigned char read_buf[50] = {0};
//	
//	//struct WriteInfo writeInfo;
//	//struct ReadInfo readInfo;
//	
//	uart_init(115200);
//	
//	writeInfo.province_id = 0x0029;
//	writeInfo.city_id = 0x0066;
//	memcpy(writeInfo.manufacturer_id, "XINDA", 5);
//	memcpy(writeInfo.terminal_model, "ZXIAT-CZ02", 10);
//	memcpy(writeInfo.terminal_id, "0201101", 7);
//	writeInfo.car_plate_color = 0x02;
//	memcpy(writeInfo.car_plate_num, "AAa1", 7);
//	
//	memset(write_buf,0,sizeof(write_buf));
//	memcpy(write_buf, &writeInfo, sizeof(writeInfo));
//	
//	FLASH_WriteByte(((uint32_t)0x08008000) , write_buf , sizeof(write_buf));
//	
//	
//	Internal_ReadFlash(((uint32_t)0x08008000) , read_buf , sizeof(read_buf));
//	
//	memset(&readInfo,0,sizeof(readInfo));
//	memcpy(&readInfo, read_buf, sizeof(read_buf));
//	
//	read_province_id = readInfo.province_id;
//	printf("para->register_info.province_id = 0x%04x\r\n", read_province_id);
//	
//	read_city_id=readInfo.city_id;
//	printf("para->register_info.city_id = 0x%04x\r\n", read_city_id);
//	
//	memcpy(read_manufacturer_id, readInfo.manufacturer_id, 5);
//	printf("ptr_manufacturer_id = %s\r\n", read_manufacturer_id);
//	
//	memcpy(read_terminal_model, readInfo.terminal_model, 10);
//	printf("ptr_terminal_model = %s\r\n", read_terminal_model);
//	
//	memcpy(read_terminal_id, readInfo.terminal_id, 7);
//	printf("ptr_terminal_id = %s\r\n", read_terminal_id);
//	
//	read_car_plate_color = readInfo.car_plate_color;
//	printf("para->register_info.car_plate_color = 0x%02x\r\n", read_car_plate_color);
//	
//	memcpy(read_car_plate_num, readInfo.car_plate_num,7);
//	printf("para->register_info.car_plate_num = %s\r\n", read_car_plate_num);
//	
//	while(1);
//}


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
	memcpy(flashWriteInfo.write_terminal_model, "ZXIAT-CZ05", 10);
	memcpy(flashWriteInfo.write_terminal_id, "221203", 7);
	flashWriteInfo.write_car_plate_color = 0x02;
	memcpy(flashWriteInfo.write_car_plate_num, "A9527", 7);
	
	
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
  setTerminalPhoneNumber("20221203111", 11);

	
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

