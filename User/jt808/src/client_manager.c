
#include "client_manager.h"
#include "protocol_parameter.h"
#include "set_terminal_parameter.h"
#include "jt808_packager.h"
#include "jt808_parser.h"
#include "util.h"
#include "bcd.h"
#include "ff.h"
#include "jt808_parser.h"

struct ProtocolParameter parameter_;

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
        printf("[jt808消息头打包]: 失败 !!!\r\n");
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


int parsingMessage(const unsigned char *in, unsigned int in_len)
{
		unsigned short msg_id;
    if (jt808FrameParse(in, in_len, &parameter_) < 0)
    {
        printf("解析时出现错误\r\n");
        return -1;
    }

//    printf("ok parsing\r\n");
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
