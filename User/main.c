/*********BSP headers***********/
#include "./usart/usart.h"
#include "./usart2/usart2.h"
#include "./gps/gps_config.h"
#include "./gpio/gpio.h"
#include "./ec20/ec20.h"
#include "./IWDG/iwdg.h"
#include "./timer/timer.h"
#include "./ADC/adc.h"
#include "./LCD/ST7567a.h"
#include "./RTC/rtc.h" 		    
#include "./internal_flash/bsp_internal_flash.h"
#include "./key/key.h"

/*********SYSTEM headers***********/
#include "./delay/delay.h"
#include "./sys/sys.h"
#include "math.h"
// #include "ff.h"

/*********JT808 headers***********/
#include "client_manager.h"
#include "protocol_parameter.h"
#include "displayLCD.h"
#include "jt808_packager.h"

extern int nmea_decode_test(double *v_latitude, double *v_longitude, float *v_altitude,
                            float *v_speed, float *v_bearing, unsigned char *v_timestamp,
                            uint8_t new_parse);

extern uint8_t gpsData_Receive(uint8_t *new_parse);
extern nmeaPARSER parser;      //����ʱʹ�õ����ݽṹ
extern nmeaINFO info;          //GPS�����õ�����Ϣ
//extern u8 uart2_cmd[1024];
int time_1s = 0;
extern uint16_t msgRecvLength;
//int test = 0;
							
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
	uint8_t ACC = 0;
	uint8_t doorOpen = 0;
	uint8_t messageReceived = 0;
	
//    double v_latitude;
//    double v_longitude;
//    float v_altitude;
//    float v_speed;
//    float v_bearing;
//    float m_bearing;
//    unsigned char v_timestamp[13] = "700101000000"; // 1970-01-01-00-00-00.
    u8 VoltageAD = 0;

    GPIO_InitTypeDef GPIO_InitStructure;

//	uint8_t new_parse = 0; // �Ƿ����µĽ������ݱ�־
//	/* ��ʼ��GPS���ݽṹ */
//	nmea_zero_INFO(&info);
//	nmea_parser_init(&parser);

    NVIC_Configuration(); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

    delay_init(); //��ʱ������ʼ��
    GPIO_Config(&GPIO_InitStructure);

    GPIO_SetBits(GPIOC, GPIO_Pin_1);
    delay_ms(1000);

    uart_init(115200);
    USART2_Init(115200);
    GPS_Config();
    Adc_Init();
    LcdInitial();
//	GPIO_SetBits(GPIOC, GPIO_Pin_5);
//	delay_ms(500);
//	GPIO_ResetBits(GPIOC, GPIO_Pin_5);



    GPIO_SetBits(GPIOA, GPIO_Pin_0);

    GPIO_SetBits(GPIOA, GPIO_Pin_1);
    delay_ms(500);
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);

    printf("\r\n");
    printf("SYSTEM INIT SUCCESS\r\n");
    printf("\r\n");
	RTC_Init(2000,1,1,0,0,0);	  			//RTC��ʼ��
//	IWDG_Init(6,4095);
    Tim3_Int_Init(10000 - 1, 7199);
	Tim5_Int_Init(9, 7199);	//��ʱ��������һ����
	KEY_Init();
    ReadLocation(); 						//��ȡ3399���������һ��λ������
	ReadWakeUp();
	
//	memset((uint8_t*)&parameter_.parse.WakeUp,0,sizeof(parameter_.parse.WakeUp));
//	FLASH_WriteByte(FLASH_WakeUp_ADDR, (uint8_t*)&parameter_.parse.WakeUp, sizeof(parameter_.parse.WakeUp));
	
    while (1)
    {
//        HeartBeatCounter = 0;
        LocationReportCounter = 0;
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
                delay_ms(4000);
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
			parameter_.msg_head.Protocolversion = 1;
            jt808TerminalRegister(&isRegistered);
			
            if (isRegistered == 0) system_reboot();
        }

        //�ն˼�Ȩ
        if (isAuthenticated == 0)
        {
			memset(parameter_.IMEI,0,15);
			memcpy(parameter_.IMEI,"100000000000000",15);
			memset(parameter_.softwareVersion,0,20);
			memcpy(parameter_.softwareVersion,"10000000000000000000",20);
			
            jt808TerminalAuthentication(&isAuthenticated);
            if (isAuthenticated == 0) system_reboot();           
        }

        //����λ���ϱ�����λ��״̬λ
        initLocationInfo(v_alarm_value, v_status_value);
        setStatusBit();


        while (1)
        {
//			IWDG_Feed();
            VoltageAD = (float) (Get_Adc_Average(ADC_Channel_6,10) * 3.3 /4096) ;
			ACC = (GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0));
            MENU_processing();

            if (VoltageAD>1.7) parameter_.location_info.alarm.bit.power_low =1;
			else parameter_.location_info.alarm.bit.power_low = 1;
			
			/*����Ϩ��*/
			if(ACC == 0) parameter_.location_info.alarm.bit.power_cut = 1;
			else parameter_.location_info.alarm.bit.power_cut = 0;
			
			if((VoltageAD>1.7)&& (ACC == 0)) 
			{
				printf("voltage && ACC");
				system_reboot();
			}
			
			/*���Ŵ�*/
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)) doorOpen = 0;
			else	doorOpen = 1;
            
			if(doorOpen == 0)
			{
				if(parameter_.parse.WakeUp.WakeUpMode.bit.conditionWakeUp == 1)
				{
					parameter_.parse.WakeUp.WakeUpConditonType.bit.doorOpen = 1;
					parameter_.parse.WakeUp.WakeUpMode_MCU.bit.conditionWakeUp = 1;
					FLASH_WriteByte(FLASH_WakeUp_ADDR, (uint8_t*)&parameter_.parse.WakeUp, sizeof(parameter_.parse.WakeUp));
					printf("doorOpen");
					system_reboot();
				}
			}
			
			if(messageReceived == 1)
			{
				if(parameter_.parse.WakeUp.WakeUpMode.bit.manualWakeUp == 1)
				{
					parameter_.parse.WakeUp.WakeUpMode_MCU.bit.manualWakeUp = 1;
					FLASH_WriteByte(FLASH_WakeUp_ADDR, (uint8_t*)&parameter_.parse.WakeUp, sizeof(parameter_.parse.WakeUp));
					printf("messageReceived");
					system_reboot();
				}
			}
			
			if(WakeUpIntervalDetect())
			{
				if(parameter_.parse.WakeUp.WakeUpMode.bit.timeWakeUp == 1)
				{
					parameter_.parse.WakeUp.WakeUpMode_MCU.bit.timeWakeUp = 1;
					FLASH_WriteByte(FLASH_WakeUp_ADDR, (uint8_t*)&parameter_.parse.WakeUp, sizeof(parameter_.parse.WakeUp));
					printf("WakeUpIntervalDetect");
					system_reboot();
				}
			}
			
			
			
            //����ʱ���ﵽȱʡʱ���ϱ����ʱ�ϱ�λ������
            if (time_1s >= parameter_.parse.terminal_parameters.DefaultTimeReportTimeInterval)
            {
				printf("locationReport!!!!!!!!!!!!!!!!! SUCCESS\r\n");
				jt808LocationReport();
				time_1s = 0;
				LocationReportCounter++;
//				printf("%x%x\r\n", VoltageAD, ACC);
            }

            if (USART2_RX_STA & 0X8000)
            {
                USART2_RX_STA = USART2_RX_STA & 0x7FFF;
				
				text_process();
				
				msgRecvLength = UartRecv_Non_transliterated();
				if(msgRecvLength!=0)
				{
					parsingMessage(Non_transliterated_receive,msgRecvLength);//У��
					if ((parameter_.parse.respone_result == kSuccess) && (parameter_.parse.respone_msg_id == kLocationReport))
                    {
                        LocationReportCounter = 0;
                        printf("Platform general response location report parse SUCCESS!!!!\r\n ");
                        printf("\r\n");
                        
                    }
					if(parameter_.parse.msg_head.msg_id==kSetTerminalParameters)
					{
						printf("\r\n");
						printf("SetTerminalParameters parse SUCCESS!!!!\r\n ");
						printf("\r\n");
					}
					USART2_RX_STA = 0;
				}

                USART2_RX_STA = 0;
            }

            //�����߼�λ����ϱ�5��δ�յ�ƽ̨��Ӧ��Ϣ���������ӷ�����

            if (LocationReportCounter >= 5 || HeartBeatCounter >= 5)
            {
                printf("LocationReportCounter == %d \r\n", LocationReportCounter);
                printf("HeartBeatCounter == %d \r\n", HeartBeatCounter);
                printf("\r\n");
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
