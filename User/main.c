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
  nmeaPARSER parser;      //����ʱʹ�õ����ݽṹ  
	nmeaINFO info;          //GPS�����õ�����Ϣ
  uint8_t new_parse=0;    //�Ƿ����µĽ������ݱ�־
	/* ��ʼ��GPS���ݽṹ */
	nmea_parser_init(&parser);
	
	NVIC_Configuration(); 	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	delay_init();	 					//��ʱ������ʼ��
	
	PC1_Config(&GPIO_InitStructure);
	delay_ms(1000);
   	 		
	uart_init(115200); //���ڳ�ʼ��
  USART2_Init(115200);
  GPS_Config();
	LED_GPIO_Config(&GPIO_InitStructure);	//LED �˿ڳ�ʼ��

	printf("\r\n");
	printf("SYSTEM INIT SUCCESS\r\n");
	printf("\r\n");


	while(1)
	{
		HeartBeatCounter = 0;
		LocationReportCounter = 0;
		CornerPointRetransmission = 0;
		time_1s = 0;
		initSystemParameters(1); //0 ��д�������� 1 ����д��������
		//�����ֻ��ţ�Ψһʶ��id��
		setTerminalPhoneNumber("100221000206", 12);
		setTerminalId("1000206", 8);
		//���ӷ�����
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
				continue;
			}
		}



		//�ն�ע��
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

		//�ն˼�Ȩ
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
		
		//����λ���ϱ�����λ��״̬λ
		initLocationInfo(v_alarm_value, v_status_value);
		setStatusBit();
		
		Tim3_Int_Init(10000-1,7199);
		while(1)
		{

			if(GPS_HalfTransferEnd)     /* ���յ�GPS_RBUFF_SIZEһ������� */
			{
				/* ����nmea��ʽ���� */
				nmea_parse(&parser, (const char*)&gps_rbuff[0], HALF_GPS_RBUFF_SIZE, &info);
				
				GPS_HalfTransferEnd = 0;   //��ձ�־λ
				new_parse = 1;             //���ý�����Ϣ��־
			}
			else if(GPS_TransferEnd)    /* ���յ���һ������ */
			{
				/* ����nmea��ʽ���� */
				nmea_parse(&parser, (const char*)&gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &info);
			 
				GPS_TransferEnd = 0;
				new_parse = 1;
			}
		


			if(new_parse == 1)
			{
				//λ�����ݸ���
				isNewLocationParse = nmea_decode_test(&v_latitude, &v_longitude, &v_altitude, &v_speed, &v_bearing, v_timestamp, info, new_parse);
				updateLocation(v_latitude, v_longitude, v_altitude, v_speed, v_bearing, v_timestamp);
				new_parse = 0;
			}
			
			//����ʱ�ϱ�λ������
			//���޸� Э��Ϊ����3s�Ƕȴ���15�ϱ�
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
			
			//����ʱ���ﵽȱʡʱ���ϱ����ʱ�ϱ�λ������
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
			
			if(USART2_RX_STA&0X8000)    //���յ�����
			{
				USART2_RX_STA = USART2_RX_STA&0x7FFF;//��ȡ��ʵ���ַ�����
				if((USART2_RX_BUF[0]==0x7e)&&(USART2_RX_BUF[USART2_RX_STA-1]==0x7e))
				{
					parsingMessage(USART2_RX_BUF, USART2_RX_STA);//��ʼУ��
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
						
						jt808TerminalLogOut();
						
						break;
					}
					
					if(parameter_.parse.msg_head.msg_id==kTerminalUpgrade)
					{
						printf("\r\n");
						printf("kTerminalUpgrade parse SUCCESS!!!!\r\n ");
						printf("\r\n");
						
						jt808TerminalLogOut();
						break;
					}
					
					
										
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
			
			//�����߼�λ����ϱ�5��δ�յ�ƽ̨��Ӧ��Ϣ���������ӷ�����

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

