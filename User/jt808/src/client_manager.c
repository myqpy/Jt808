#include "./ec20/ec20.h"
#include "client_manager.h"
#include "protocol_parameter.h"
#include "set_terminal_parameter.h"
#include "jt808_packager.h"
#include "jt808_parser.h"
#include "util.h"
#include "./delay/delay.h"
#include "bcd.h"
#include "ff.h"
#include "jt808_parser.h"
#include "./usart/usart.h"
#include "./usart2/usart2.h"
#include "./internal_flash/bsp_internal_flash.h" 

double  v_latitude = 34.824788;
double  v_longitude = 113.558408;
float  v_altitude = 107;
float  v_speed = 15;
float  v_bearing = 132;
unsigned char v_timestamp[] = "221127212855";
extern int nmea_decode_test(double *v_latitude, double *v_longitude, float *v_altitude, float  *v_speed, float *v_bearing, unsigned char *v_timestamp);
struct ProtocolParameter parameter_;

void initSystemParameters(void)
{
	unsigned char read_buf[64] = {0};

	Internal_ReadFlash(((uint32_t)0x08008000) , read_buf , sizeof(read_buf));
	memset(&parameter_.parse.terminal_parameters,0,sizeof(parameter_.parse.terminal_parameters));
	memcpy(&parameter_.parse.terminal_parameters, read_buf, sizeof(read_buf));

	if(parameter_.parse.terminal_parameters.initFactoryParameters == 0)
	{
		FlashWrite();
	}
//	printf("%d \r\n",parameter_.parse.terminal_parameters.initFactoryParameters);
	printf("\r\n");
	printf("系统参数初始化成功！！！!\r\n");
	printf("\r\n");
	
}

int FlashWrite()
{
	unsigned char write_buf[64] = {0};
	parameter_.parse.terminal_parameters.HeartBeatInterval = 1;

	memset(parameter_.parse.terminal_parameters.MainServerAddress,0,sizeof(parameter_.parse.terminal_parameters.MainServerAddress));
	memcpy(parameter_.parse.terminal_parameters.MainServerAddress,"121.5.140.126", sizeof("121.5.140.126"));

	parameter_.parse.terminal_parameters.ServerPort = 7611;

	parameter_.parse.terminal_parameters.DefaultTimeReportTimeInterval = 5;

	parameter_.parse.terminal_parameters.CornerPointRetransmissionAngle = 15;

	parameter_.parse.terminal_parameters.MaxSpeed = 30;

	parameter_.parse.terminal_parameters.ProvinceID = 0x0029;

	parameter_.parse.terminal_parameters.CityID = 0x0066;

	memset(parameter_.parse.terminal_parameters.CarPlateNum,0,sizeof(parameter_.parse.terminal_parameters.CarPlateNum));
	memcpy(parameter_.parse.terminal_parameters.CarPlateNum, "测1222", 7);

	parameter_.parse.terminal_parameters.CarPlateColor = 0x02;
	
	parameter_.parse.terminal_parameters.initFactoryParameters = 1;

	memset(write_buf,0,sizeof(write_buf));
	memcpy(write_buf, &parameter_.parse.terminal_parameters, sizeof(parameter_.parse.terminal_parameters));
	
	
	
	FLASH_WriteByte(((uint32_t)0x08008000) , write_buf , sizeof(write_buf));
	return 0;
}

ErrorStatus ec20_init(void)
{
    u8 data=0,ret=0;
    u8 err=0;
    char atstr[BUFLEN];
    USART2_RX_STA=0;
    if(ec20_send_cmd("AT","OK","NULL","NULL",1000))err|=1<<0;//¼ì²âÊÇ·ñÓ¦´ðATÖ¸Áî
    USART2_RX_STA=0;
    if(ec20_send_cmd("ATE0","OK","NULL","NULL",2000))err|=1<<1;//²»»ØÏÔ
    USART2_RX_STA=0;
    if(ec20_send_cmd("AT+CPIN?","OK","NULL","NULL",2000))err|=1<<3;	//²éÑ¯SIM¿¨ÊÇ·ñÔÚÎ»
    USART2_RX_STA=0;
    data = 0;
    //²éÑ¯GSMÍøÂç×¢²á×´Ì¬£¬È·ÈÏÕÒÍø³É¹¦
    while (ec20_send_cmd("AT+CREG?\r\n","\r\n+CREG: 0,1","NULL","NULL",2000)!= 1 && data < 10)
    {
        USART2_RX_STA=0;
        delay_ms(100);
        data++;
    }
    USART2_RX_STA=0;
    if (data == 10)
    {
        return ERROR;                                                                             //ÕÒÍø²»³É¹¦Ä£¿éÖØÆô
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
    //sprintf(atstr,"AT+QIOPEN=1,0,\"TCP\",\"%s\",%d,0,2\r\n",IPSERVER,PORTSERVER);
		sprintf(atstr,"AT+QIOPEN=1,0,\"TCP\",\"%s\",%d,0,2\r\n",parameter_.parse.terminal_parameters.MainServerAddress,parameter_.parse.terminal_parameters.ServerPort);
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

/// @brief 设置终端手机号
/// @param phone
void setTerminalPhoneNumber(const char *phone_num, unsigned int phoneSize)
{
  memset(parameter_.msg_head.phone_num, 0, 13);
	memcpy(parameter_.msg_head.phone_num, phone_num, phoneSize);  
//	parameter_.msg_head.phone_num = (unsigned char *)phone_num;
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
    printf("\n\r[updateLocationInfo] OK !\r\n");
		
    // if (speed >= 10) //默认车速大于等于10公里时为正常行驶状态
    // {
    //   isCarMoving.store(true);
    // }
    // else
    // {
    //   isCarMoving.store(false);
    // }
    parameter_.location_info.latitude = v_latitude * 1e6;
    printf("para->latitude = %d\r\n", parameter_.location_info.latitude);

    parameter_.location_info.longitude = v_longitude * 1e6;
    printf("para->longitude = %d\r\n", parameter_.location_info.longitude);

    parameter_.location_info.altitude = v_altitude;
    printf("para->altitude = %d\r\n", parameter_.location_info.altitude);

    parameter_.location_info.speed = v_speed * 10;
    printf("para->speed = %d\r\n", parameter_.location_info.speed);

    parameter_.location_info.bearing = v_bearing;
    printf("para->bearing = %d\r\n", parameter_.location_info.bearing);

    //parameter_.location_info.time = v_timestamp;
    memcpy(parameter_.location_info.time, v_timestamp, 13);
		printf("para->time = %s\r\n", parameter_.location_info.time);
}


int packagingMessage(unsigned int msg_id)
{
    //查找当前msgID是否存在于待打包消息ID数组中
    if (0 == findMsgIDFromTerminalPackagerCMD(msg_id))
    {
        printf("[查找当前msgID是否存在于待打包消息ID数组中] 暂无 msg_id \r\n");
        return -1;
    }

    printf("[查找当前msgID是否存在于待打包消息ID数组中] OK !\r\n");

    parameter_.msg_head.msg_id = msg_id; // 设置消息ID.
    if (jt808FramePackage(&parameter_) < 0)
    {
        printf("[jt808消息帧打包]: 失败 !!!\r\n");
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

int jt808TerminalRegister(int isRegistered)
{
	int i=0;
	while(i<3)
	{
		packagingMessage(kTerminalRegister);
		delay_ms(1000);
		Usart_SendStr_length(USART2, BufferSend, RealBufferSendSize);

		delay_ms(100);
		if(USART2_RX_STA&0X8000)    //接收到数据
		{
			USART2_RX_STA = USART2_RX_STA&0x7FFF;//获取到实际字符数量
			parsingMessage(USART2_RX_BUF, USART2_RX_STA);//校验
			if((parameter_.parse.respone_result == kRegisterSuccess)&&(parameter_.parse.msg_head.msg_id==kTerminalRegisterResponse))
			{
				isRegistered = 1;
				printf("\r\n");
				printf("注册成功！！！!\r\n");
				printf("\r\n");
				USART2_RX_STA=0;
				break;
			}
		}
		USART2_RX_STA=0;
		printf("\r\n");
		printf("注册失败 重注册中！！！!\r\n");
		printf("\r\n");		
		i++;	
	}
	
	
	return isRegistered;
}


int jt808TerminalAuthentication(int isAuthenticated)
{
	int i=0;
	while(i<3)
	{
		packagingMessage(kTerminalAuthentication);
		delay_ms(1000);
		Usart_SendStr_length(USART2, BufferSend, RealBufferSendSize);

		delay_ms(100);
		if(USART2_RX_STA&0X8000)    //接收到数据
		{
			USART2_RX_STA = USART2_RX_STA&0x7FFF;//获取到实际字符数量
			parsingMessage(USART2_RX_BUF, USART2_RX_STA);//校验
			if((parameter_.parse.respone_result	 == kSuccess)&&(parameter_.parse.respone_msg_id==kTerminalAuthentication))
			{
				isAuthenticated = 1;
				printf("\r\n");
				printf("终端鉴权成功！！！!\r\n");
				printf("\r\n");
				USART2_RX_STA=0;
				break;
			}
		}
		USART2_RX_STA=0;
		i++;	
		printf("\r\n");
		printf("鉴权失败 重鉴权中！！！!\r\n");
		printf("\r\n");		
	}
	return isAuthenticated;
}

int jt808LocationReport()
{
		nmea_decode_test(&v_latitude, &v_longitude, &v_altitude, &v_speed, &v_bearing, v_timestamp);
		updateLocation(v_latitude, v_longitude, v_altitude, v_speed, v_bearing, v_timestamp);
		packagingMessage(kLocationReport);
		Usart_SendStr_length(USART2, BufferSend, RealBufferSendSize);
		printf("位置上报完成!\r\n");
//		LocationReportCounter++;

	
//		if(USART2_RX_STA&0X8000)    //接收到数据
//		{
//			USART2_RX_STA = USART2_RX_STA&0x7FFF;//获取到实际字符数量
//			parsingMessage(USART2_RX_BUF, USART2_RX_STA);//校验	
//			if((parameter_.parse.respone_result	 == kSuccess)&&(parameter_.parse.respone_msg_id==kLocationReport))
//			{
//				LocationReportCounter = 0;
//				printf("\r\n");
//				printf("位置上报平台应答解析 成功！！！!\r\n");
//				printf("\r\n");
//				USART2_RX_STA=0;
//			}
//			USART2_RX_STA=0;
//		}
//		return LocationReportCounter;
		return 0;
}



int parsingMessage(const unsigned char *in, unsigned int in_len)
{
		unsigned short msg_id;
    if (jt808FrameParse(in, in_len, &parameter_) < 0)
    {
        printf("消息帧解析时出现错误\r\n");
        return -1;
    }

    printf("ok parsing\r\n");
    msg_id = parameter_.parse.msg_head.msg_id;
    printf("%s[%d]: [解析后的信息id] msg_id = 0x%02x \r\n", __FUNCTION__, __LINE__, msg_id);
    switch (msg_id)
    {
    // +平台通用应答.
    case kPlatformGeneralResponse:
    {
        printf("%s[%d]: 【 平台通用应答 】解析完成 \r\n", __FUNCTION__, __LINE__);
    }
    break;

    //  补传分包请求.
    case kFillPacketRequest:
    {
        printf("%s[%d]: 【 补传分包请求 】解析完成 \r\n", __FUNCTION__, __LINE__);
    }
    break;

    // 终端注册应答..
    case kTerminalRegisterResponse:
    {
        printf("%s[%d]: 【 终端注册请求 】解析完成 \r\n", __FUNCTION__, __LINE__);
    }
    break;

    // 设置终端参数..
    case kSetTerminalParameters:
    {
        printf("%s[%d]: 【 设置终端参数 】解析完成 \r\n", __FUNCTION__, __LINE__);
    }
    break;

    // 查询终端参数..
    case kGetTerminalParameters:
    {
        printf("%s[%d]: 【 查询终端参数 】解析完成 \r\n", __FUNCTION__, __LINE__);
    }
    break;

    //查询指定终端参数..
    case kGetSpecificTerminalParameters:
    {
        printf("%s[%d]: 【 查询指定终端参数 】解析完成 \r\n", __FUNCTION__, __LINE__);
    }
    break;

    // 终端控制
    case kTerminalControl:
    {
        printf("%s[%d]: 【 终端控制 】解析完成 \r\n", __FUNCTION__, __LINE__);
    }
    break;

    // 下发终端升级包.
    case kTerminalUpgrade:
    {
        printf("%s[%d]: 【 下发终端升级包 】解析完成\r\n", __FUNCTION__, __LINE__);
    }
    break;

    //  位置信息查询..
    case kGetLocationInformation:
    {
        printf("%s[%d]: 【 位置信息查询 】解析完成\r\n", __FUNCTION__, __LINE__);
    }
    break;

    default:
        break;
    }
    return 0;
}
