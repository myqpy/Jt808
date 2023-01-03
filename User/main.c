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

void Tim3_Int_Init(u16 arr,u16 psc);
void TIM3_IRQHandler(void);
int TIM3_IT_FLAG = 0;

int main(void)
{
	
	int isTCPconnected=0;
	int isRegistered=0;
	int isAuthenticated=0;
	int LocationReportCounter=0;
	unsigned int v_alarm_value = 0;
	unsigned int v_status_value = 0;


	
//	LED_GPIO_Config();	//LED 端口初始化
//  
//  GPIO_SetBits(GPIOD,GPIO_Pin_2);

	NVIC_Configuration(); 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	delay_init();	    	 		//延时函数初始化
	uart_init(115200); //串口初始化
  USART2_Init(115200);
  GPS_Config();
	
	
	
	while(1)
	{
		initSystemParameters();
		//连接服务器
		if(isTCPconnected == 0)
		{
			if(ec20_init() == SUCCESS)
			{	
				printf("server connected\r\n");
				isTCPconnected=1;
				delay_ms(2000);
				continue;
			}
		}

		//设置手机号（唯一识别id）
		setTerminalPhoneNumber("19149421105", 11);

		//终端注册
		if(isRegistered == 0)	
		{
			isRegistered = jt808TerminalRegister(isRegistered);
			
			if(isRegistered==0)
			{
				isTCPconnected=0;
				continue;
			}
			continue;
		}

		//终端鉴权
		if(isAuthenticated == 0)
		{
			isAuthenticated =jt808TerminalAuthentication(isAuthenticated);		
			
			if(isAuthenticated==0)
			{
				isRegistered=0;
				isTCPconnected=0;
				continue;
			}
			continue;
		}
		
		//设置位置上报警报位、状态位
		initLocationInfo(v_alarm_value, v_status_value);
		setStatusBit();
		
		
		Tim3_Int_Init(parameter_.parse.terminal_parameters.DefaultTimeReportTimeInterval*10000-1,7199);
		while(1)
		{
			//位置上报 现行逻辑位如果上报5次未收到平台响应消息则重新连接服务器
			if(TIM3_IT_FLAG == 1)
			{
				jt808LocationReport();
				LocationReportCounter++; 
			}

//			jt808TerminalHeartBeat();
			
			
			if(USART2_RX_STA&0X8000)    //接收到数据
			{
				USART2_RX_STA = USART2_RX_STA&0x7FFF;//获取到实际字符数量
				parsingMessage(USART2_RX_BUF, USART2_RX_STA);//开始校验
				if((parameter_.parse.respone_result	 == kSuccess)&&(parameter_.parse.respone_msg_id==kLocationReport))
				{
					LocationReportCounter = 0;
					printf("\r\n");
					printf("Platform general response location report parse SUCCESS!!!!\r\n ");
					printf("\r\n");
					USART2_RX_STA=0;
				}
				
				if((parameter_.parse.respone_result	 == kSuccess)&&(parameter_.parse.respone_msg_id==kTerminalHeartBeat))
				{
					printf("\r\n");
					printf("jt808TerminalHeartBeat SEND COMPLETE!!!! \r\n ");
					printf("\r\n");
					USART2_RX_STA=0;
				}
				
				
				if(parameter_.parse.msg_head.msg_id==kSetTerminalParameters)
				{
					printf("\r\n");
					printf("SetTerminalParameters parse COMPLETE!!!!\r\n ");
					printf("\r\n");
					isRegistered=0;
					isTCPconnected=0;
					isAuthenticated=0;
					USART2_RX_STA=0;
					LocationReportCounter = 0;
					break;
				}
				
				USART2_RX_STA=0;
			}
			
			
			printf("%d \r\n",LocationReportCounter);
			if(LocationReportCounter>=5)
			{
				isRegistered=0;
				isTCPconnected=0;
				isAuthenticated=0;
				LocationReportCounter = 0;
				break;
			}


		
		}
	}
}




void Tim3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = arr;
	TIM_TimeBaseInitStruct.TIM_Prescaler = psc;
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM3,ENABLE);
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == 1)
	{
		TIM3_IT_FLAG = 1;
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
	
}

