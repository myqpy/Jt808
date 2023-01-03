
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./USART2/usart2.h"
#include "./led/bsp_led.h"
#include "./gps/gps_config.h"
#include "client_manager.h"
#include "jt808_packager.h"
#include "./EC20/ec20.h"
	
#define PORTSERVER 7611
#define IPSERVER "121.5.140.126"

#define MAXDATASIZE (1024 * 5)

#define TIME_DIFF(t1, t2) (((t1).tv_sec - (t2).tv_sec) * 1000 + ((t1).tv_usec - (t2).tv_usec) / 1000)
ErrorStatus ec20_init(void);
extern int nmea_decode_test(void);
void Usart_SendString( USART_TypeDef * pUSARTx, unsigned char *str);
//static void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch )
int main(void)
{
	int sockfd, sendbytes, recvbytes;
	int i;
  char buf[MAXDATASIZE];
	/* LED 端口初始化 */
	LED_GPIO_Config();	 
  
  GPIO_SetBits(GPIOD,GPIO_Pin_2);
  
	/*串口初始化*/
	USART_Config();
  USART2_Init(115200);
  GPS_Config();

  /* GPS解码测试 */
  //nmea_decode_test();
	while(1){
			if(ec20_init() == ERROR)continue;
		
			sendbytes = 0;
			recvbytes = 0;

			//gettimeofday(&timestamp, NULL);

			setTerminalPhoneNumber("17737702736", 11);
			printf("will send !\n");

			printf("RealBufferSendSize = %d \n", RealBufferSendSize);
			// sleep(1);
			packagingMessage(kTerminalRegister);
			// sleep(1);
			printf("RealBufferSendSize = %d \n", RealBufferSendSize);

			for (i = 0; i < RealBufferSendSize; ++i)
			{
				printf("%02x ", BufferSend[i]);
			}
			
			printf("\r\n");
			//sleep(1);
			
			Usart_SendString(USART2, BufferSend);
			printf("send ok!\n");
			

			printf("send done !\n");
			//gettimeofday(&timestamp_end, NULL);
			printf("sendbytes: %d\n", sendbytes);

			printf("will recv !\n");
			//if ((recvbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1)
			if(USART2_RX_STA&0X8000){
				printf("recv ok!\n");
				for (i = 0; i < recvbytes; ++i)
				{
					printf("%02x ", buf[i]);
				}
				printf("\r\n");
				
				parsingMessage(buf, recvbytes);
			}

			printf("recv done!\n");
			printf("Client receive bytes: %d\n", recvbytes);

			sendbytes = 0;
			recvbytes = 0;

			//gettimeofday(&timestamp, NULL);

			setTerminalPhoneNumber("17737702736", 11);
			printf("will send !\n");

			printf("RealBufferSendSize = %d \n", RealBufferSendSize);
			// sleep(1);
			packagingMessage(kLocationReport);
			// sleep(1);
			printf("RealBufferSendSize = %d \n", RealBufferSendSize);

			for (i = 0; i < RealBufferSendSize; ++i)
			{
				printf("%02x ", BufferSend[i]);
			}
			printf("\r\n");
			//Delay(1000);

			Usart_SendString(USART2, BufferSend);
			printf("send ok!\n");
			

			printf("send done !\n");
			//gettimeofday(&timestamp_end, NULL);
			printf("sendbytes: %d\n", sendbytes);

			printf("will recv !\n");
			//if ((recvbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1)
			if(USART2_RX_STA&0X8000){
				printf("recv ok!\n");
				for (i = 0; i < recvbytes; ++i)
				{
					// if ((i > 0) && (buf[i] == 0x00) && (buf[i - 1] == 0x7e))
					// {
					//   break;
					// }
					printf("%02x ", buf[i]);
				}
				printf("\r\n");
				
				parsingMessage(buf, recvbytes);
			}

			printf("recv done!\n");
			printf("Client receive bytes: %d\n", recvbytes);
			
	}
}

void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}

ErrorStatus ec20_init(void)
{
    u8 data=0,ret=0;
    u8 err=0;
    char atstr[200];
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
        Delay(100);
        data++;
    }
    USART2_RX_STA=0;
    if (data == 10)
    {
        return ERROR;                                                                             //找网不成功模块重启
    }
    ec20_send_cmd("AT+CGATT?\r\n","+CGATT: 1","OK","NULL",2000);
    USART2_RX_STA=0;
    Delay(200);
    ec20_send_cmd("AT+QIACT?\r\n","OK","NULL","NULL",2000);

    USART2_RX_STA=0;
    Delay(200);
    ec20_send_cmd("AT+QICLOSE=0\r\n","OK","NULL","NULL",2000);
    USART2_RX_STA=0;
    Delay(200);
    memset(atstr,0,200);
    sprintf(atstr,"AT+QIOPEN=1,0,\"TCP\",\"%s\",%d,0,2\r\n",IPSERVER,PORTSERVER);
    data=ec20_send_cmd((u8*)atstr,"CONNECT","OK","NULL",2000);
    USART2_RX_STA=0;
    Delay(200);
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

static void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch )
{
    /* 发送一个字节数据到USART1 */
    USART_SendData(pUSARTx,ch);

    /* 等待发送完毕 */
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

void Usart_SendString( USART_TypeDef * pUSARTx, unsigned char *str)
{
    unsigned int k=0;
    do 
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(*(str + k)!='\0');
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
