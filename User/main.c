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
extern nmeaPARSER parser;      //解码时使用的数据结构
extern nmeaINFO info;          //GPS解码后得到的信息
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

//	uint8_t new_parse = 0; // 是否有新的解码数据标志
//	/* 初始化GPS数据结构 */
//	nmea_zero_INFO(&info);
//	nmea_parser_init(&parser);

    NVIC_Configuration(); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级

    delay_init(); //延时函数初始化
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
	RTC_Init(2000,1,1,0,0,0);	  			//RTC初始化
//	IWDG_Init(6,4095);
    Tim3_Int_Init(10000 - 1, 7199);
	Tim5_Int_Init(9, 7199);	//定时计数器，一毫秒
	KEY_Init();
    ReadLocation(); 						//读取3399发来的最后一条位置数据
	ReadWakeUp();
	
//	memset((uint8_t*)&parameter_.parse.WakeUp,0,sizeof(parameter_.parse.WakeUp));
//	FLASH_WriteByte(FLASH_WakeUp_ADDR, (uint8_t*)&parameter_.parse.WakeUp, sizeof(parameter_.parse.WakeUp));
	
    while (1)
    {
//        HeartBeatCounter = 0;
        LocationReportCounter = 0;
        time_1s = 0;
        initSystemParameters(1); // //0 烧写出厂参数 1 不烧写出厂参数
        //设置手机号（唯一识别id）
        setUUID();

        //连接服务器
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
		

		
        //终端注册
        if (isRegistered == 0)
        {
			parameter_.msg_head.Protocolversion = 1;
            jt808TerminalRegister(&isRegistered);
			
            if (isRegistered == 0) system_reboot();
        }

        //终端鉴权
        if (isAuthenticated == 0)
        {
			memset(parameter_.IMEI,0,15);
			memcpy(parameter_.IMEI,"100000000000000",15);
			memset(parameter_.softwareVersion,0,20);
			memcpy(parameter_.softwareVersion,"10000000000000000000",20);
			
            jt808TerminalAuthentication(&isAuthenticated);
            if (isAuthenticated == 0) system_reboot();           
        }

        //设置位置上报警报位、状态位
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
			
			/*车辆熄火*/
			if(ACC == 0) parameter_.location_info.alarm.bit.power_cut = 1;
			else parameter_.location_info.alarm.bit.power_cut = 0;
			
			if((VoltageAD>1.7)&& (ACC == 0)) 
			{
				printf("voltage && ACC");
				system_reboot();
			}
			
			/*车门打开*/
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
			
			
			
            //当计时器达到缺省时间上报间隔时上报位置数据
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
					parsingMessage(Non_transliterated_receive,msgRecvLength);//校验
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

            //现行逻辑位如果上报5次未收到平台响应消息则重新连接服务器

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
