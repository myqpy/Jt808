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


/*********SYSTEM headers***********/
#include "./delay/delay.h"
#include "./sys/sys.h"
#include "math.h"
// #include "ff.h"

/*********JT808 headers***********/
#include "client_manager.h"
#include "protocol_parameter.h"
#include "displayLCD.h"

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
    ReadLocation(); 						//��ȡ3399���������һ��λ������
	
	
	while(1)
	{
		showMainMenu();
		VoltageAD = (float) (Get_Adc_Average(ADC_Channel_6,10) * 3.3 /4096) ;
		if (VoltageAD>1.7) system_reboot();
	}
	
	
	
//    while (1)
//    {
////        showMainMenu();
//        HeartBeatCounter = 0;
//        LocationReportCounter = 0;
//        time_1s = 0;
//        initSystemParameters(1); // //0 ��д�������� 1 ����д��������
//        //�����ֻ��ţ�Ψһʶ��id��
//        setUUID();

//        //���ӷ�����
//        if (isTCPconnected == 0)
//        {
//            if (ec20_init(parameter_.parse.terminal_parameters.MainServerAddress, parameter_.parse.terminal_parameters.ServerPort) == SUCCESS)
//            {
//                printf("server connected\r\n");
//                isTCPconnected = 1;
//                delay_ms(2000);
//            }

//            else
//            {
//                IPFlashWrite();
//                isTCPconnected = 0;
//                system_reboot();
//                continue;
//            }
//        }

//        //�ն�ע��
//        if (isRegistered == 0)
//        {
//			parameter_.msg_head.Protocolversion = 1;
//            jt808TerminalRegister(&isRegistered);
//            if (isRegistered == 0) system_reboot();
//        }

//        //�ն˼�Ȩ
//        if (isAuthenticated == 0)
//        {
//			memset(parameter_.IMEI,0,15);
//			memcpy(parameter_.IMEI,"100000000000000",15);
//			memset(parameter_.softwareVersion,0,20);
//			memcpy(parameter_.softwareVersion,"10000000000000000000",20);
//			
//            jt808TerminalAuthentication(&isAuthenticated);
//            if (isAuthenticated == 0) system_reboot();           
//        }

//        //����λ���ϱ�����λ��״̬λ
//        initLocationInfo(v_alarm_value, v_status_value);
//        setStatusBit();


//        while (1)
//        {
////			IWDG_Feed();
//            VoltageAD = (float) (Get_Adc_Average(ADC_Channel_6,10) * 3.3 /4096) ;
//            showMainMenu();

//            if (VoltageAD>1.7) system_reboot();
//            
//            //����ʱ���ﵽȱʡʱ���ϱ����ʱ�ϱ�λ������
//            if (time_1s >= parameter_.parse.terminal_parameters.DefaultTimeReportTimeInterval)
//            {
//                    printf("locationReport!!!!!!!!!!!!!!!!! SUCCESS\r\n");
//                    jt808LocationReport();
//                    time_1s = 0;
//                    LocationReportCounter++;
//            }

//            if (USART2_RX_STA & 0X8000)
//            {
//                USART2_RX_STA = USART2_RX_STA & 0x7FFF;
//                if ((USART2_RX_BUF[0] == 0x7e) && (USART2_RX_BUF[USART2_RX_STA - 1] == 0x7e))
//                {
//                    parsingMessage(USART2_RX_BUF, USART2_RX_STA);
//                    if ((parameter_.parse.respone_result == kSuccess) && (parameter_.parse.respone_msg_id == kLocationReport))
//                    {
//                        LocationReportCounter = 0;
//                        printf("Platform general response location report parse SUCCESS!!!!\r\n ");
//                        printf("\r\n");
//                        USART2_RX_STA = 0;
//                    }
//                }

//                USART2_RX_STA = 0;
//            }

//            //�����߼�λ����ϱ�5��δ�յ�ƽ̨��Ӧ��Ϣ���������ӷ�����

//            if (LocationReportCounter >= 5 || HeartBeatCounter >= 5)
//            {
//                printf("LocationReportCounter == %d \r\n", LocationReportCounter);
//                printf("HeartBeatCounter == %d \r\n", HeartBeatCounter);
//                printf("\r\n");
//                system_reboot();
//                break;
//            }
//        }
//    }
}

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == 1)
    {
        time_1s += 1;
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}
