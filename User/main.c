/*********BSP headers***********/
#include "./usart/usart.h"
#include "./usart2/usart2.h"
#include "./gps/gps_config.h"
#include "./gpio/gpio.h"
#include "./ec20/ec20.h"
#include "./IWDG/iwdg.h"
#include "./timer/timer.h"

/*********SYSTEM headers***********/
#include "./delay/delay.h"
#include "./sys/sys.h"
#include "math.h"
// #include "ff.h"

/*********JT808 headers***********/
#include "client_manager.h"
#include "protocol_parameter.h"

extern int nmea_decode_test(double *v_latitude, double *v_longitude, float *v_altitude,
							float *v_speed, float *v_bearing, unsigned char *v_timestamp,
							uint8_t new_parse);
							
extern uint8_t gpsData_Receive(uint8_t *new_parse);
extern nmeaPARSER parser;      //����ʱʹ�õ����ݽṹ  
extern nmeaINFO info;          //GPS�����õ�����Ϣ

int time_1s = 0;

int main(void)
{
	int isTCPconnected = 0;
	int isRegistered = 0;
	int isAuthenticated = 0;
	int LocationReportCounter = 0;
	int HeartBeatCounter = 0;
	//	int CornerPointRetransmission=0;
	int isNewLocationParse = 0;
	unsigned int v_alarm_value = 0;
	unsigned int v_status_value = 0;

	double v_latitude;
	double v_longitude;
	float v_altitude;
	float v_speed;
	float v_bearing;
	float m_bearing;
	unsigned char v_timestamp[13] = "700101000000"; // 1970-01-01-00-00-00.

	GPIO_InitTypeDef GPIO_InitStructure;

	uint8_t new_parse = 0; // �Ƿ����µĽ������ݱ�־
	/* ��ʼ��GPS���ݽṹ */
	nmea_zero_INFO(&info);
	nmea_parser_init(&parser);

	NVIC_Configuration(); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

	delay_init(); //��ʱ������ʼ��	

	PC_Config(&GPIO_InitStructure);

	uart_init(115200); 
	USART2_Init(115200);
	GPS_Config();
	LED_GPIO_Config(&GPIO_InitStructure); 

	printf("\r\n");
	printf("SYSTEM INIT SUCCESS\r\n");
	printf("\r\n");

	//	IWDG_Init(6,4095); 
	while (1)
	{
		HeartBeatCounter = 0;
		LocationReportCounter = 0;
		//		CornerPointRetransmission = 0;
		time_1s = 0;
		initSystemParameters(1); // //0 ��д�������� 1 ����д��������
		//�����ֻ��ţ�Ψһʶ��id��
		setUUID();

		//���ӷ�����
		if (isTCPconnected == 0)
		{
			if (ec20_init(parameter_.parse.terminal_parameters.MainServerAddress, parameter_.parse.terminal_parameters.ServerPort) == SUCCESS)
			{
				printf("server connected\r\n");
				isTCPconnected = 1;
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

		//�ն�ע��
		if (isRegistered == 0)
		{
			jt808TerminalRegister(&isRegistered);
			if (isRegistered == 0)
			{
				// isTCPconnected=0;
				system_reboot();
				continue;
			}
		}

		//�ն˼�Ȩ
		if (isAuthenticated == 0)
		{
			jt808TerminalAuthentication(&isAuthenticated);
			if (isAuthenticated == 0)
			{
//				isRegistered=0;
//				isTCPconnected=0;
				system_reboot();
				continue;
			}
		}

		//����λ���ϱ�����λ��״̬λ
		initLocationInfo(v_alarm_value, v_status_value);
		setStatusBit();

		Tim3_Int_Init(10000 - 1, 7199);
		while (1)
		{
			IWDG_Feed();
			new_parse = gpsData_Receive(&new_parse);

			if (new_parse == 1)
			{
				/* ����nmea��ʽ���� */
				isNewLocationParse = nmea_decode_test(&v_latitude, &v_longitude, &v_altitude, &v_speed, &v_bearing, v_timestamp, new_parse);
				updateLocation(v_latitude, v_longitude, v_altitude, v_speed, v_bearing, v_timestamp);
				new_parse = 0;
			}

			//����ʱ�ϱ�λ������
			//���޸� Э��Ϊ����3s�Ƕȴ���15�ϱ�
			if ((fabs(v_bearing - m_bearing)) >= parameter_.parse.terminal_parameters.CornerPointRetransmissionAngle)
			{
				m_bearing = v_bearing;

				printf("fabs(v_bearing - m_bearing)) > %d trigger LocationReport SUCCESS\r\n", parameter_.parse.terminal_parameters.CornerPointRetransmissionAngle);
				jt808LocationReport();
//				LocationReportCounter++;
//				printf("m_bearing ===== %f  \r\n", m_bearing);
			}

//			if((fabs(v_bearing - m_bearing)) >= parameter_.parse.terminal_parameters.CornerPointRetransmissionAngle)
//			{
//				m_bearing = v_bearing;
//				CornerPointRetransmission++;
//				//				printf("m_bearing ===== %f  \r\n", m_bearing);
//			}

//			if(CornerPointRetransmission>=3)
//			{
//				printf("fabs(v_bearing - m_bearing)) > %d trigger LocationReport SUCCESS\r\n",parameter_.parse.terminal_parameters.CornerPointRetransmissionAngle);
//				jt808LocationReport();
//				CornerPointRetransmission = 0;
//			}

			//����ʱ���ﵽȱʡʱ���ϱ����ʱ�ϱ�λ������
			if (time_1s >= parameter_.parse.terminal_parameters.DefaultTimeReportTimeInterval)
			{

				if (isNewLocationParse == 1)
				{
					printf("locationReport!!!!!!!!!!!!!!!!! SUCCESS\r\n");
					jt808LocationReport();
					time_1s = 0;
					LocationReportCounter++;
					GPIO_SetBits(GPIOA, GPIO_Pin_8);
				}
				else
				{
					printf("HeartBeat!!!!!!!!!!!!!!!!! SUCCESS\r\n");
					jt808TerminalHeartBeat();
					time_1s = 0;
					HeartBeatCounter++;
					GPIO_ResetBits(GPIOA, GPIO_Pin_8);
				}
			}

			if (USART2_RX_STA & 0X8000) 
			{
				USART2_RX_STA = USART2_RX_STA & 0x7FFF; 
				if ((USART2_RX_BUF[0] == 0x7e) && (USART2_RX_BUF[USART2_RX_STA - 1] == 0x7e))
				{
					parsingMessage(USART2_RX_BUF, USART2_RX_STA); 
					if ((parameter_.parse.respone_result == kSuccess) && (parameter_.parse.respone_msg_id == kLocationReport))
					{
						LocationReportCounter = 0;
						printf("\r\n");
//						printf("Platform general response location report parse SUCCESS!!!!\r\n ");
//						printf("\r\n");
						USART2_RX_STA = 0;
					}

					if ((parameter_.parse.respone_result == kSuccess) && (parameter_.parse.respone_msg_id == kTerminalHeartBeat))
					{
						HeartBeatCounter = 0;
						printf("\r\n");
//						printf("jt808TerminalHeartBeat report parse SUCCESS!!!! \r\n ");
//						printf("\r\n");
						USART2_RX_STA = 0;
					}

					if (parameter_.parse.msg_head.msg_id == kSetTerminalParameters)
					{
						printf("\r\n");
//						printf("SetTerminalParameters parse SUCCESS!!!!\r\n ");
//						printf("\r\n");
						jt808TerminalLogOut();

						break;
					}

					if(parameter_.parse.msg_head.msg_id==kTerminalUpgrade)
					{
						printf("\r\n");
//						printf("kTerminalUpgrade parse SUCCESS!!!!\r\n ");
//						printf("\r\n");

//						jt808TerminalLogOut();
//						break;
					}

					if ((parameter_.parse.respone_result == kSuccess) && (parameter_.parse.respone_msg_id == kTerminalLogOut))
					{
						printf("\r\n");
//						printf("jt808TerminalLogOut parse SUCCESS!!!! \r\n ");
//						printf("\r\n");
						USART2_RX_STA = 0;
						system_reboot();
					}
				}

				USART2_RX_STA = 0;
			}

			//�����߼�λ����ϱ�5��δ�յ�ƽ̨��Ӧ��Ϣ���������ӷ�����

			if (LocationReportCounter >= 5 || HeartBeatCounter >= 5)
			{
				printf("LocationReportCounter == %d \r\n", LocationReportCounter);
				printf("HeartBeatCounter == %d \r\n", HeartBeatCounter);
				system_reboot();
				break;
			}
		}
	}
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == 1)
	{
		time_1s += 1;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}
