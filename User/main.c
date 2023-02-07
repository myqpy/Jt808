#include "./usart/usart.h"
#include "./usart2/usart2.h"
#include "./led/bsp_led.h"
#include "./gps/gps_config.h"
#include "./delay/delay.h"
#include "./sys/sys.h"
#include "math.h"
#include "client_manager.h"
#include "jt808_packager.h"
#include "ff.h"
extern int nmea_decode_test(double *v_latitude, double *v_longitude, float *v_altitude, 
										float  *v_speed, float *v_bearing, unsigned char *v_timestamp,
											nmeaINFO info, uint8_t new_parse);
void Tim3_Int_Init(u16 arr,u16 psc);
void TIM3_IRQHandler(void);
int time_1s = 0;		
										
int main(void)
{
	
	int 					isTCPconnected=0;
	int 					isRegistered=0;
	int 					isAuthenticated=0;
	int 					LocationReportCounter=0;
	int 					HeartBeatCounter=0;
	int 					CornerPointRetransmission=0;
	int						isNewLocationParse=0;
	unsigned int 	v_alarm_value = 0;
	unsigned int 	v_status_value = 0;
		

	double 				v_latitude ;
	double 				v_longitude ;
	float 				v_altitude ;
	float 				v_speed ;
	float 				v_bearing ;
	float 				m_bearing ;
	unsigned char v_timestamp[13] = "700101000000"; // 1970-01-01-00-00-00.
	
	GPIO_InitTypeDef  GPIO_InitStructure;
  nmeaPARSER parser;      //解码时使用的数据结构  
	nmeaINFO info;          //GPS解码后得到的信息
  uint8_t new_parse=0;    //是否有新的解码数据标志
	/* 初始化GPS数据结构 */
	nmea_parser_init(&parser);
	
	NVIC_Configuration(); 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	
	delay_init();	 					//延时函数初始化
	
	PC1_Config(&GPIO_InitStructure);
	delay_ms(1000);
   	 		
	uart_init(115200); //串口初始化
  USART2_Init(115200);
  GPS_Config();
	LED_GPIO_Config(&GPIO_InitStructure);	//LED 端口初始化

	printf("\r\n");
	printf("SYSTEM INIT SUCCESS\r\n");
	printf("\r\n");
	
	GPIO_SetBits(GPIOC, GPIO_Pin_5);	
	delay_ms(500);
	GPIO_ResetBits(GPIOC, GPIO_Pin_5);	

	while(1)
	{
		HeartBeatCounter = 0;
		LocationReportCounter = 0;
		CornerPointRetransmission = 0;
		time_1s = 0;
		initSystemParameters(1); //0 烧写出厂参数 1 不烧写出厂参数
		//设置手机号（唯一识别id）
		setUUID();
		
//		setTerminalPhoneNumber("100221000206", 12);
//		setTerminalId("1000206", 8);
		//连接服务器
		if(isTCPconnected == 0)
		{
			if(ec20_init() == SUCCESS)
			{	
				printf("server connected\r\n");
				isTCPconnected=1;
				delay_ms(2000);
			}
			
			else
			{
				IPFlashWrite();
				isTCPconnected = 0;
				system_reboot();
				continue;
			}
		}



		//终端注册
		if(isRegistered == 0)	
		{
			jt808TerminalRegister(&isRegistered);
			if(isRegistered==0)
			{
				//isTCPconnected=0;
				system_reboot();
				continue;
			}
		}

		//终端鉴权
		if(isAuthenticated == 0)
		{
			jt808TerminalAuthentication(&isAuthenticated);			
			if(isAuthenticated==0)
			{
//				isRegistered=0;
//				isTCPconnected=0;
//				parameter_.msg_head.msg_flow_num=0;
//				parameter_.respone_flow_num=0;
				system_reboot();
				continue;
			}
		}
		
		//设置位置上报警报位、状态位
		initLocationInfo(v_alarm_value, v_status_value);
		setStatusBit();
		
		Tim3_Int_Init(10000-1,7199);
		while(1)
		{

			if(GPS_HalfTransferEnd)     /* 接收到GPS_RBUFF_SIZE一半的数据 */
			{
				/* 进行nmea格式解码 */
				nmea_parse(&parser, (const char*)&gps_rbuff[0], HALF_GPS_RBUFF_SIZE, &info);
				
				GPS_HalfTransferEnd = 0;   //清空标志位
				new_parse = 1;             //设置解码消息标志
			}
			else if(GPS_TransferEnd)    /* 接收到另一半数据 */
			{
				/* 进行nmea格式解码 */
				nmea_parse(&parser, (const char*)&gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &info);
			 
				GPS_TransferEnd = 0;
				new_parse = 1;
			}
		


			if(new_parse == 1)
			{
				//位置数据更新
				isNewLocationParse = nmea_decode_test(&v_latitude, &v_longitude, &v_altitude, &v_speed, &v_bearing, v_timestamp, info, new_parse);
				updateLocation(v_latitude, v_longitude, v_altitude, v_speed, v_bearing, v_timestamp);
				new_parse = 0;
			}
			
			//拐弯时上报位置数据
			//需修改 协议为连续3s角度大于15上报
			if((fabs(v_bearing - m_bearing)) >= parameter_.parse.terminal_parameters.CornerPointRetransmissionAngle)
			{
				m_bearing = v_bearing;
				

				printf("fabs(v_bearing - m_bearing)) > %d trigger LocationReport SUCCESS\r\n",parameter_.parse.terminal_parameters.CornerPointRetransmissionAngle);
				jt808LocationReport();
//				LocationReportCounter++;
//				printf("m_bearing ===== %f  \r\n", m_bearing);				
			}

			
			
//			if((fabs(v_bearing - m_bearing)) >= parameter_.parse.terminal_parameters.CornerPointRetransmissionAngle)
//			{
//				m_bearing = v_bearing;
//				CornerPointRetransmission++;
////				printf("m_bearing ===== %f  \r\n", m_bearing);				
//			}

//			if(CornerPointRetransmission>=3)
//			{
//				printf("fabs(v_bearing - m_bearing)) > %d trigger LocationReport SUCCESS\r\n",parameter_.parse.terminal_parameters.CornerPointRetransmissionAngle);
//				jt808LocationReport();
//				CornerPointRetransmission = 0;
//			} 
			
			//当计时器达到缺省时间上报间隔时上报位置数据
			if(time_1s >= parameter_.parse.terminal_parameters.DefaultTimeReportTimeInterval )
			{
				
				if(isNewLocationParse == 1)
				{
					printf("locationReport!!!!!!!!!!!!!!!!! SUCCESS\r\n");
					jt808LocationReport();
					time_1s = 0;
					LocationReportCounter++; 
					GPIO_SetBits(GPIOA,GPIO_Pin_8);
				}
				else
				{							
					printf("HeartBeat!!!!!!!!!!!!!!!!! SUCCESS\r\n");
					jt808TerminalHeartBeat();
					time_1s = 0;
					HeartBeatCounter++; 
					GPIO_ResetBits(GPIOA,GPIO_Pin_8);
				}
			}
			
			if(USART2_RX_STA&0X8000)    //接收到数据
			{
				USART2_RX_STA = USART2_RX_STA&0x7FFF;//获取到实际字符数量
				if((USART2_RX_BUF[0]==0x7e)&&(USART2_RX_BUF[USART2_RX_STA-1]==0x7e))
				{
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
						HeartBeatCounter = 0;
						printf("\r\n");
						printf("jt808TerminalHeartBeat report parse SUCCESS!!!! \r\n ");
						printf("\r\n");
						USART2_RX_STA=0;
					}
					
					if(parameter_.parse.msg_head.msg_id==kSetTerminalParameters)
					{
						printf("\r\n");
						printf("SetTerminalParameters parse SUCCESS!!!!\r\n ");
						printf("\r\n");
//						isRegistered=0;
//						isTCPconnected=0;
//						isAuthenticated=0;
//						USART2_RX_STA=0;
//						LocationReportCounter = 0;	
//						boot_loader_flag();		
						jt808TerminalLogOut();
						
						break;
					}
					
//					if(parameter_.parse.msg_head.msg_id==kTerminalUpgrade)
//					{
//						printf("\r\n");
//						printf("kTerminalUpgrade parse SUCCESS!!!!\r\n ");
//						printf("\r\n");
//						
//						jt808TerminalLogOut();
//						break;
//					}
					
					
										
					if((parameter_.parse.respone_result	 == kSuccess)&&(parameter_.parse.respone_msg_id==kTerminalLogOut))
					{
						printf("\r\n");
						printf("jt808TerminalLogOut parse SUCCESS!!!! \r\n ");
						printf("\r\n");
						USART2_RX_STA=0;
						system_reboot();
					}
					
				}
				
				USART2_RX_STA=0;
			}
			
			//现行逻辑位如果上报5次未收到平台响应消息则重新连接服务器

			if(LocationReportCounter>=5||HeartBeatCounter>=5)
			{
				printf("LocationReportCounter == %d \r\n",LocationReportCounter);
				printf("HeartBeatCounter == %d \r\n",HeartBeatCounter);
//				isRegistered=0;
//				isTCPconnected=0;
//				isAuthenticated=0;
//				HeartBeatCounter = 0;
//				LocationReportCounter = 0;
//				time_1s = 0;
				system_reboot();
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
		time_1s += 1;
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
	
}

