/*********BSP headers***********/
#include "./usart/usart.h"
#include "./usart2/usart2.h"
#include "./internal_flash/bsp_internal_flash.h"
#include "./IWDG/iwdg.h"
#include "./key/key.h"
#include "displayLCD.h"
#include "./RTC/rtc.h"

/*********common headers***********/
#include "util.h"
#include "bcd.h"
#include "./delay/delay.h"
#include <stdlib.h>

/*********JT808 headers***********/

#include "client_manager.h"
#include "protocol_parameter.h"
#include "set_terminal_parameter.h"
#include "jt808_parser.h"
#include "jt808_packager.h"

struct ProtocolParameter parameter_;
u8 key_text=0;
u16 time_1ms=0;
u16 up_down_pressed=0;
int weekday;
int text_position = 0;
char str_wakeupTime[6];
const char wakeupCMD[10] = "WAKEUP";
uint32_t wakeupTimeLength = 0;
//uint16_t wakeupTime = 0;
uint32_t i;


void system_reboot(void)
{
    __set_FAULTMASK(1);
    NVIC_SystemReset();
}



void initSystemParameters(int i)
{
    Internal_ReadFlash(FLASH_ADDR, (uint8_t*)&parameter_.parse.terminal_parameters, sizeof(parameter_.parse.terminal_parameters));
    parameter_.parse.terminal_parameters.initFactoryParameters = i;

    printf("initFactoryParameters == %d \r\n",parameter_.parse.terminal_parameters.initFactoryParameters);

    if(parameter_.parse.terminal_parameters.initFactoryParameters == 0)
    {
        FlashWrite();
        Internal_ReadFlash(FLASH_ADDR, (uint8_t*)&parameter_.parse.terminal_parameters, sizeof(parameter_.parse.terminal_parameters));
    }
    printf("-->---------------------------\r\n");
    printf("-->                           \r\n");
    printf("-->       Version = %s     		\r\n", parameter_.parse.terminal_parameters.version);
    printf("-->                           \r\n");
    printf("-->---------------------------\r\n");
    printf("HeartBeatInterval == %d \r\n",parameter_.parse.terminal_parameters.HeartBeatInterval);
    printf("MainServerAddress == \"%s\" \r\n",parameter_.parse.terminal_parameters.MainServerAddress);
    printf("ServerPort == %d \r\n",parameter_.parse.terminal_parameters.ServerPort);
    printf("DefaultTimeReportTimeInterval == %d \r\n",parameter_.parse.terminal_parameters.DefaultTimeReportTimeInterval);
    printf("CornerPointRetransmissionAngle == %d \r\n",parameter_.parse.terminal_parameters.CornerPointRetransmissionAngle);
    printf("MaxSpeed == %d \r\n",parameter_.parse.terminal_parameters.MaxSpeed);
    printf("ProvinceID == %d \r\n",parameter_.parse.terminal_parameters.ProvinceID);
    printf("CityID == %d \r\n",parameter_.parse.terminal_parameters.CityID);
    printf("CarPlateNum == %s \r\n",parameter_.parse.terminal_parameters.CarPlateNum);
    printf("CarPlateColor == %02x \r\n",parameter_.parse.terminal_parameters.CarPlateColor);
    printf("boot_loader_flag  =  0x%lx \r\n", parameter_.parse.terminal_parameters.bootLoaderFlag);
    printf("\r\n");
    printf("initSystemParameters SUCCESS!!!!!!\r\n");
    printf("\r\n");

}

void ReadLocation(void)
{
    Internal_ReadFlash(FLASH_GPS_ADDR, (uint8_t*)&parameter_.location_info, sizeof(parameter_.location_info));

    printf("para->latitude = %d\r\n", parameter_.location_info.latitude);
    printf("para->longitude = %d\r\n", parameter_.location_info.longitude);
    printf("para->altitude = %d\r\n", parameter_.location_info.altitude);
    printf("para->speed = %d\r\n", parameter_.location_info.speed);
    printf("para->bearing = %d\r\n", parameter_.location_info.bearing);
    printf("para->time = %s\r\n", parameter_.location_info.time);
}

void ReadWakeUp(void)
{
	Internal_ReadFlash(FLASH_WakeUp_ADDR, (uint8_t*)&parameter_.parse.WakeUp, sizeof(parameter_.parse.WakeUp));
}

int FlashWrite()
{
    parameter_.parse.terminal_parameters.HeartBeatInterval = 2;

    memset(parameter_.parse.terminal_parameters.MainServerAddress,0,sizeof(parameter_.parse.terminal_parameters.MainServerAddress));

    // 研究院平台
    memcpy(parameter_.parse.terminal_parameters.MainServerAddress,"124.222.183.168", sizeof("124.222.183.168"));
//	memcpy(parameter_.parse.terminal_parameters.MainServerAddress,"http://jt808.gps.ciicp.com", sizeof("http://jt808.gps.ciicp.com"));

//	天瑞平台
//	memcpy(parameter_.parse.terminal_parameters.MainServerAddress,"123.60.47.210", sizeof("123.60.47.210"));

    parameter_.parse.terminal_parameters.ServerPort = 7611;

    parameter_.parse.terminal_parameters.DefaultTimeReportTimeInterval = 5;

    parameter_.parse.terminal_parameters.CornerPointRetransmissionAngle = 10;

    parameter_.parse.terminal_parameters.MaxSpeed = 60;

    parameter_.parse.terminal_parameters.ProvinceID = 0x0029;

    parameter_.parse.terminal_parameters.CityID = 0x0066;

    parameter_.parse.terminal_parameters.CarPlateColor = 0x02;

    parameter_.parse.terminal_parameters.initFactoryParameters = 1;

    memset(parameter_.parse.terminal_parameters.version,0,sizeof(parameter_.parse.terminal_parameters.version));
    memcpy(parameter_.parse.terminal_parameters.version, "v1.0", 5);

    parameter_.parse.terminal_parameters.bootLoaderFlag = 0XFFFFFFFF;


    memset(parameter_.parse.terminal_parameters.PhoneNumber,0, 12);
    memset(parameter_.parse.terminal_parameters.CarPlateNum,0,sizeof(parameter_.parse.terminal_parameters.CarPlateNum));
    memset(parameter_.parse.terminal_parameters.TerminalId,0, 8);


    memcpy(parameter_.parse.terminal_parameters.PhoneNumber, "00000000100211602092", 20);
    memcpy(parameter_.parse.terminal_parameters.TerminalId, "1602092", 8);
//	ff_convert(parameter_.parse.terminal_parameters.CarPlateNum,0);
    memcpy(parameter_.parse.terminal_parameters.CarPlateNum, "豫A02092", 9);

    FLASH_WriteByte(FLASH_ADDR, (uint8_t*)&parameter_.parse.terminal_parameters, sizeof(parameter_.parse.terminal_parameters));
    printf("FLASH_Write SUCCESS!!!!!!\r\n");
    printf("initFactoryParameters == %d \r\n",parameter_.parse.terminal_parameters.initFactoryParameters);

    return 0;
}

int IPFlashWrite()
{
    memset(parameter_.parse.terminal_parameters.MainServerAddress,0,sizeof(parameter_.parse.terminal_parameters.MainServerAddress));
    memcpy(parameter_.parse.terminal_parameters.MainServerAddress,"124.222.183.168", sizeof("124.222.183.168"));
//	memcpy(parameter_.parse.terminal_parameters.MainServerAddress,"123.60.47.210", sizeof("123.60.47.210"));

    parameter_.parse.terminal_parameters.ServerPort = 7611;

    parameter_.parse.terminal_parameters.DefaultTimeReportTimeInterval = 5;

    FLASH_WriteByte(FLASH_ADDR, (uint8_t*)&parameter_.parse.terminal_parameters, sizeof(parameter_.parse.terminal_parameters));
    printf("IPFLASH_Write SUCCESS!!!!!!\r\n");

    return 0;
}

void IWDG_ReBoot_Flag_FlashWrite(void)
{
    parameter_.parse.terminal_parameters.bootLoaderFlag = 0XCCCCCCCC;

    FLASH_WriteByte(FLASH_ADDR, (uint8_t *) &parameter_.parse.terminal_parameters, sizeof(parameter_.parse.terminal_parameters));
    printf("boot_loader_flag write SUCCESS!!!!!!\r\n");
}


void setUUID(void)
{
    Internal_ReadFlash(FLASH_ADDR, (uint8_t *) &parameter_.parse.terminal_parameters, sizeof(parameter_.parse.terminal_parameters));

    setTerminalPhoneNumber(parameter_.parse.terminal_parameters.PhoneNumber, 20);
    setTerminalId(parameter_.parse.terminal_parameters.TerminalId, 8);

}

void boot_loader_flag()
{
    parameter_.parse.terminal_parameters.bootLoaderFlag = 0XAAAAAAAA;

    FLASH_WriteByte(FLASH_ADDR, (uint8_t *) &parameter_.parse.terminal_parameters, sizeof(parameter_.parse.terminal_parameters));
    printf("boot_loader_flag write SUCCESS!!!!!!\r\n");
}


/// @brief 设置终端手机号
/// @param phone
void setTerminalPhoneNumber(unsigned char *phone_num, unsigned int phoneSize)
{
	memset(parameter_.msg_head.phone_num, 0, 20);
    memcpy(parameter_.msg_head.phone_num, phone_num, 20);
    printf("parameter_.msg_head.phone_num = %s\r\n", parameter_.msg_head.phone_num);
}


void setTerminalId(unsigned char *TerminalId,unsigned int lenTerminalId)
{
    //终端ID
//	unsigned int lenTerminalId;
//	lenTerminalId = sizeof(TerminalId);
//	lenTerminalId	=(lenTerminalId>20)?20:lenTerminalId;

    memset(parameter_.register_info.terminal_id, 0, lenTerminalId);
    memcpy(parameter_.register_info.terminal_id, TerminalId, lenTerminalId);
    printf("para->register_info.terminal_id = %s\r\n", parameter_.register_info.terminal_id);
}



int packagingAndSendMessage(unsigned int msg_id)
{
    return 0;
}


void setStatusBit()
{
    parameter_.location_info.status.bit.positioning=1;
}




void initLocationInfo(unsigned int v_alarm_value, unsigned int v_status_value)
{

    printf("\n\r[InitLocationInfo] OK !\r\n");
    //报警标志
    parameter_.location_info.alarm.value = v_alarm_value;
    printf("para->alarm.value = %d\r\n", parameter_.location_info.alarm.value);
    //状态
    parameter_.location_info.status.value = v_status_value;
    printf("para->status.value = %d\r\n", parameter_.location_info.status.value);
}

void updateLocation(double const v_latitude, double const v_longitude, float const v_altitude,
                    float const v_speed, float const v_bearing, unsigned char *v_timestamp)
{
#ifdef __JT808_DEBUG
    printf("\n\r[updateLocationInfo] OK !\r\n");
#endif
//	if (speed >= 10) //默认车速大于等于10公里时为正常行驶状态
//	{
//	 isCarMoving.store(true);
//	}
//	else
//	{
//	 isCarMoving.store(false);
//	}
    parameter_.location_info.latitude = v_latitude * 1e6;
    parameter_.location_info.longitude = v_longitude * 1e6;
    parameter_.location_info.altitude = v_altitude;
    parameter_.location_info.speed = v_speed * 10;
    parameter_.location_info.bearing = v_bearing;
    memcpy(parameter_.location_info.time, v_timestamp, 13);
#ifdef __JT808_DEBUG
    printf("para->latitude = %d\r\n", parameter_.location_info.latitude);
    printf("para->longitude = %d\r\n", parameter_.location_info.longitude);
    printf("para->altitude = %d\r\n", parameter_.location_info.altitude);
    printf("para->speed = %d\r\n", parameter_.location_info.speed);
    printf("para->bearing = %d\r\n", parameter_.location_info.bearing);
    printf("para->time = %s\r\n", parameter_.location_info.time);
#endif
}


int packagingMessage(unsigned int msg_id)
{

    //查找当前msgID是否存在于待打包消息ID数组中
    if (0 == findMsgIDFromTerminalPackagerCMD(msg_id))
    {
        printf("[findMsgIDFromTerminalPackagerCMD] no msg_id \r\n");
        return -1;
    }
#ifdef __JT808_DEBUG
    printf("[findMsgIDFromTerminalPackagerCMD] OK !\r\n");
#endif
    parameter_.msg_head.msg_id = msg_id; // 设置消息ID.
    if (jt808FramePackage(&parameter_) < 0)
    {
        printf("[jt808FramePackage]: FAILED !!!\r\n");
        return -1;
    }
    ++parameter_.msg_head.msg_flow_num; // 每正确生成一条命令, 消息流水号增加1.
    return 0;
}

int findMsgIDFromTerminalPackagerCMD(unsigned int msg_id)
{
    int result = 0;
    int i;
    for (i = 0; i < PACKAGER_NUM; ++i)
    {
        if (kTerminalPackagerCMD[i] == msg_id)
        {
            result = 1;
        }
    }
    return result;
}

int findParameterIDFromArray(unsigned int para_id)
{
    int result = 0;
    int i;
    for (i = 0; i < PARA_SETTING_LIMIT; ++i)
    {
        if (kParameterSettingCMD[i] == para_id)
        {
            result = 1;
        }
    }
    return result;
}


int jt808TerminalRegister(int *isRegistered)
{
    int i=0;
    
    while(i<3)
    {
        packagingMessage(kTerminalRegister);
#ifdef __JT808_DEBUG
		uint8_t j=0;
        for(j=0; j<RealBufferSendSize; j++)
        {
            printf("%02x ",BufferSend[j]);
        }
        printf("\r\n");
#endif
        Usart_SendStr_length(USART2, BufferSend, RealBufferSendSize);
        delay_ms(1000);

        if(USART2_RX_STA&0X8000)    //接收到数据
        {
            USART2_RX_STA = USART2_RX_STA&0x7FFF;//获取到实际字符数量
            parsingMessage(USART2_RX_BUF, USART2_RX_STA);//校验
            if((parameter_.parse.respone_result == kRegisterSuccess)&&(parameter_.parse.msg_head.msg_id==kTerminalRegisterResponse))
            {
                *isRegistered = 1;
                printf("\r\n");
                printf("TerminalRegister SUCCESS!!!!!!!!!!\r\n");
                printf("\r\n");
                USART2_RX_STA=0;
                break;
            }
        }
        USART2_RX_STA=0;
        printf("\r\n");
        printf("TerminalRegister FAILED!!!!!!\r\n");
        printf("\r\n");
        i++;
    }


    return 0;
}


int jt808TerminalAuthentication(int *isAuthenticated)
{
    int i=0;
	
    while(i<3)
    {
        packagingMessage(kTerminalAuthentication);

//#ifdef __JT808_DEBUG
//		uint8_t j=0;
//        for(j=0; j<RealBufferSendSize; j++)
//        {
//            printf("%02x ",BufferSend[j]);
//        }
//        printf("\r\n");
//#endif
		
        Usart_SendStr_length(USART2, BufferSend, RealBufferSendSize);
        delay_ms(1000);

        if(USART2_RX_STA&0X8000)    //接收到数据
        {
            USART2_RX_STA = USART2_RX_STA&0x7FFF;//获取到实际字符数量
            parsingMessage(USART2_RX_BUF, USART2_RX_STA);//校验
            if((parameter_.parse.respone_result	 == kSuccess)&&(parameter_.parse.respone_msg_id==kTerminalAuthentication))
            {
                *isAuthenticated = 1;
                printf("\r\n");
                printf("TerminalAuthentication SUCCESS!!!!!!!!\r\n");
                printf("\r\n");
                USART2_RX_STA=0;
                break;
            }
        }
        USART2_RX_STA=0;
        i++;
        printf("\r\n");
        printf("TerminalAuthentication FAILED RETRY!!!!!!!!\r\n");
        printf("\r\n");
    }
    return 0;
}

int jt808LocationReport()
{
    packagingMessage(kLocationReport);
    Usart_SendStr_length(USART2, BufferSend, RealBufferSendSize);
//	printf("latitude * 1e6 	= %d\r\n", parameter_.location_info.latitude);
//	printf("longitude * 1e6 = %d\r\n", parameter_.location_info.longitude);
//	printf("altitude = %d\r\n", parameter_.location_info.altitude);
//	printf("speed * 10 = %d\r\n", parameter_.location_info.speed);

    return 0;
}

int jt808TerminalHeartBeat()
{
    packagingMessage(kTerminalHeartBeat);
    Usart_SendStr_length(USART2, BufferSend, RealBufferSendSize);

    return 0;
}

int jt808TerminalUpgradeResultReport()
{
    packagingMessage(kTerminalUpgradeResultReport);
    Usart_SendStr_length(USART2, BufferSend, RealBufferSendSize);
    printf("jt808TerminalUpgradeResultReport report SUCCESS!!! \r\n");
    return 0;
}


int jt808TerminalLogOut()
{
    packagingMessage(kTerminalLogOut);
    Usart_SendStr_length(USART2, BufferSend, RealBufferSendSize);
    printf("jt808TerminalLogOut report SUCCESS!!! \r\n");
    return 0;
}



int jt808TerminalGeneralResponse()
{
    packagingMessage(kTerminalGeneralResponse);
    Usart_SendStr_length(USART2, BufferSend, RealBufferSendSize);
    printf("jt808TerminalGeneralResponse report SUCCESS!\r\n");
    delay_ms(1000);
    return 0;
}

void MENU_processing()
{

    key_text=KEY_Scan(1);		//得到键值
	
	/*********按键后重新计时*********/
    if(key_text!=0)time_1ms = 0;	

	/*********当计时大于10秒时重置*********/
    if(time_1ms>=10000)
    {
		TIM_Cmd(TIM5,DISABLE);
		/*********重新计时*********/
		time_1ms = 0;
		up_down_pressed=0;
    }
	
	showMainMenu();
	switch(key_text)
	{
	case KEY_up_down_PRES:
		TIM_Cmd(TIM5,ENABLE);
		printf("up_down_pressed = %d\r\n",up_down_pressed);
		if(up_down_pressed>=3000)
		{
			printf("SOS!!!!!!!!! \r\n");
			if(parameter_.parse.WakeUp.WakeUpMode.bit.conditionWakeUp == 1)
			{
				parameter_.parse.WakeUp.WakeUpConditonType.bit.sosWakeUp = 1;
				parameter_.parse.WakeUp.WakeUpMode_MCU.bit.conditionWakeUp = 1;
				FLASH_WriteByte(FLASH_WakeUp_ADDR, (uint8_t*)&parameter_.parse.WakeUp, sizeof(parameter_.parse.WakeUp));
				printf("sosWakeUp");
				system_reboot();
			}
			up_down_pressed = 0;
			TIM_Cmd(TIM5,DISABLE);
		}
		break;
	default:
		break;
	}
}

int WakeUpIntervalClockDetect(void)
{
	if(calendar.hour == parameter_.parse.WakeUp.WakeUpDay.time1WakeUpTime.HH)
	{
		if(calendar.min == parameter_.parse.WakeUp.WakeUpDay.time1WakeUpTime.MM)
		{
			return 1;
		}
	}
	
	else if(calendar.hour == parameter_.parse.WakeUp.WakeUpDay.time2WakeUpTime.HH)
	{
		if(calendar.min == parameter_.parse.WakeUp.WakeUpDay.time2WakeUpTime.MM)
		{
			return 1;
		}
	}
	
	else if(calendar.hour == parameter_.parse.WakeUp.WakeUpDay.time3WakeUpTime.HH)
	{
		if(calendar.min == parameter_.parse.WakeUp.WakeUpDay.time3WakeUpTime.MM)
		{
			return 1;
		}
	}
	
	else if(calendar.hour == parameter_.parse.WakeUp.WakeUpDay.time4WakeUpTime.HH)
	{
		if(calendar.min == parameter_.parse.WakeUp.WakeUpDay.time4WakeUpTime.MM)
		{
			return 1;
		}
	}
	
	return 0;
}

int WakeUpIntervalDetect(void)
{
	weekday=showMainMenu();
	if(parameter_.parse.WakeUp.setWakeUpDay.bit.Mon==1)
	{
		if(weekday==1)
		{
			return WakeUpIntervalClockDetect();
		}
	}
	
	if(parameter_.parse.WakeUp.setWakeUpDay.bit.Tue==1)
	{
		if(weekday==2)
		{
			return WakeUpIntervalClockDetect();
		}
	}
	
	if(parameter_.parse.WakeUp.setWakeUpDay.bit.Wed==1)
	{
		if(weekday==3)
		{
			return WakeUpIntervalClockDetect();
		}
	}
	
	if(parameter_.parse.WakeUp.setWakeUpDay.bit.Thurs==1)
	{
		if(weekday==4)
		{
			return WakeUpIntervalClockDetect();
		}
	}
	
	if(parameter_.parse.WakeUp.setWakeUpDay.bit.Fri==1)
	{
		if(weekday==5)
		{
			return WakeUpIntervalClockDetect();
		}
	}
	
	if(parameter_.parse.WakeUp.setWakeUpDay.bit.Sat==1)
	{
		if(weekday==6)
		{
			return WakeUpIntervalClockDetect();
		}
	}
	
	if(parameter_.parse.WakeUp.setWakeUpDay.bit.Sun==1)
	{
		if(weekday==7)
		{
			return WakeUpIntervalClockDetect();
		}
	}
	return 0;
}

void File_upload()
{

}

int parsingMessage(const unsigned char *in, unsigned int in_len)
{
    unsigned short msg_id;
    if (jt808FrameParse(in, in_len, &parameter_) < 0)
    {
        printf("jt808FrameParse ERROR\r\n");
        return -1;
    }
    msg_id = parameter_.parse.msg_head.msg_id;

#ifdef __JT808_DEBUG
    printf("ok parsing\r\n");

    printf("%s[%d]: [parameter_.parse.msg_head.msg_id] msg_id = 0x%02x \r\n", __FUNCTION__, __LINE__, msg_id);
    switch (msg_id)
    {
    // +平台通用应答.
    case kPlatformGeneralResponse:
    {
        printf("%s[%d]: [ kPlatformGeneralResponse ] msg_id = 0x%02x parse done \r\n", __FUNCTION__, __LINE__, msg_id);
    }
    break;

    //  补传分包请求.
    case kFillPacketRequest:
    {
        printf("%s[%d]: [ kFillPacketRequest ] msg_id = 0x%02x parse done \r\n", __FUNCTION__, __LINE__, msg_id);
    }
    break;

    // 终端注册应答..
    case kTerminalRegisterResponse:
    {
        printf("%s[%d]: [ kTerminalRegisterResponse ] msg_id = 0x%02x parse done \r\n", __FUNCTION__, __LINE__, msg_id);
    }
    break;

    // 设置终端参数..
    case kSetTerminalParameters:
    {
        printf("%s[%d]: [ kSetTerminalParameters ] msg_id = 0x%02x parse done \r\n", __FUNCTION__, __LINE__, msg_id);
    }
    break;

    // 查询终端参数..
    case kGetTerminalParameters:
    {
        printf("%s[%d]: [ kGetTerminalParameters ] msg_id = 0x%02x parse done \r\n", __FUNCTION__, __LINE__, msg_id);
    }
    break;

    //查询指定终端参数..
    case kGetSpecificTerminalParameters:
    {
        printf("%s[%d]: [ kGetSpecificTerminalParameters ] msg_id = 0x%02x parse done \r\n", __FUNCTION__, __LINE__, msg_id);
    }
    break;

    // 终端控制
    case kTerminalControl:
    {
        printf("%s[%d]: [ kTerminalControl ] msg_id = 0x%02x parse done \r\n", __FUNCTION__, __LINE__, msg_id);
    }
    break;

    // 下发终端升级包.
    case kTerminalUpgrade:
    {
        printf("%s[%d]: [ kTerminalUpgrade ] msg_id = 0x%02x parse done\r\n", __FUNCTION__, __LINE__, msg_id);
    }
    break;

    //  位置信息查询..
    case kGetLocationInformation:
    {
        printf("%s[%d]: [ kGetLocationInformation ] msg_id = 0x%02x parse done\r\n", __FUNCTION__, __LINE__, msg_id);
    }
    break;

    default:
        break;
    }
#endif


    return 0;
}


void text_process(void)
{
	//有短信接入
	if((strstr((const char*)USART2_RX_BUF,"+CMT:"))!=NULL)
	{
		printf((const char*)USART2_RX_BUF,"\r\n");
		text_position =  strStr((const char*)USART2_RX_BUF, wakeupCMD);
		printf("position:%d\r\n",text_position);
		printf("strlen(haystack):%d\r\n",strlen((const char*)USART2_RX_BUF));
		wakeupTimeLength = strlen((const char*)USART2_RX_BUF) - 1 - 6 - text_position;
		printf("wakeupTimeLength:%d\r\n",wakeupTimeLength);
		for(i=0;i<wakeupTimeLength;i++)
		{	
			str_wakeupTime[i] = USART2_RX_BUF[text_position+i+6];
		} 
		
		
		for(i=0;i<6;i++) printf("%02x ",str_wakeupTime[i]);
		printf("\r\n");
		parameter_.parse.WakeUp.WakeUpDuration = atoi(str_wakeupTime);
		if(parameter_.parse.WakeUp.WakeUpDuration>65535) parameter_.parse.WakeUp.WakeUpDuration = 65535;
		printf("wakeupTime=%d\r\n",parameter_.parse.WakeUp.WakeUpDuration);
	}	
}


//定时器5中断服务程序
void TIM5_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源
    {
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源
        time_1ms++;
		if(key_text == KEY_up_down_PRES)
		{
			up_down_pressed++;
		}
    }
}

