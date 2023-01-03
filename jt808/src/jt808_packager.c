#include "jt808_packager.h"
//#include <memory.h>
#include "util.h"
#include "string.h"
#include "terminal_register.h"

// 所有终端数据打包命令.
unsigned short kTerminalPackagerCMD[PACKAGER_NUM] = {
    kTerminalGeneralResponse,       // 终端通用应答.
    kTerminalHeartBeat,             // 终端心跳.
    kTerminalRegister,              // 终端注册.
    kTerminalLogOut,                // 终端注销.
    kTerminalAuthentication,        // 终端鉴权.
    kGetTerminalParametersResponse, // 查询终端参数应答.
    kTerminalUpgradeResultReport,   // 终端升级结果通知.
    kLocationReport,                // 位置信息汇报.
    kGetLocationInformationResponse // 位置信息查询应答.
};

/// @brief 发送缓存
unsigned char BufferSend[BUFFER_SIZE_SEND] = {0};

/// @brief 实际打包发送数据的长度
unsigned int RealBufferSendSize = 0;
// int RealBufferSendSize = 0;

void clearBufferSend()
{
    memset(BufferSend, 0, BUFFER_SIZE_SEND);
    RealBufferSendSize = 0;
}

void bufferSendPushByte(unsigned char byte)
{
    BufferSend[RealBufferSendSize] = byte;
    ++RealBufferSendSize;
}

int bufferSendPushBytes(const unsigned char *bytes, unsigned int size)
{
		unsigned int i;
    if (bytes == NULL)
    {
        return 0;
    }
    for (i = 0; i < size; ++i)
    {
        bufferSendPushByte(*(bytes + i));
    }

    return 1;
}

int bufferSendChangeByte(unsigned char byte, unsigned int index)
{
    if (index > RealBufferSendSize)
    {
        return 0;
    }
    else if (index == RealBufferSendSize)
    {
        BufferSend[index] = byte;
        ++RealBufferSendSize;
    }
    else
    {
        BufferSend[index] = byte;
    }
    return 1;
}

int copyU16ToU8ArrayToBufferSend(const unsigned char *u8array)
{
    return bufferSendPushBytes(u8array, 2);
}

int copyU32ToU8ArrayToBufferSend(const unsigned char *u8array)
{
    return bufferSendPushBytes(u8array, 4);
}

// 终端通用应答.
int handle_kTerminalGeneralResponse(struct ProtocolParameter *para)
{
    

    int msg_len = 5;
    union U16ToU8Array u16converter;
		printf("[%s] 终端通用应答 msg_id = 0x%04x\n", __FUNCTION__, kTerminalGeneralResponse);
    // 应答消息流水号.
    u16converter.u16val = EndianSwap16(para->parse.msg_head.msg_flow_num);
    copyU16ToU8ArrayToBufferSend(u16converter.u8array);

    // 应答消息ID.
    u16converter.u16val = EndianSwap16(para->parse.msg_head.msg_id);
    copyU16ToU8ArrayToBufferSend(u16converter.u8array);

    // 应答结果.
    bufferSendPushByte(para->respone_result);

    return msg_len;
}

// 终端心跳.
int handle_kTerminalHeartBeat(struct ProtocolParameter *para)
{
    printf("[%s] 终端心跳 <消息体为空>  msg_id = 0x%04x\n", __FUNCTION__, kTerminalHeartBeat);

    return 0;
}

// 终端注册.
int handle_kTerminalRegister(struct ProtocolParameter *para)
{
    int msg_len = 37;

    union U16ToU8Array u16converter;
		unsigned int len_car_num;
		printf("[%s] 终端注册 msg_id = 0x%04x\n", __FUNCTION__, kTerminalRegister);
		initRegisterInfo(para); //初始化注册参数
		
    // 省域ID.
    u16converter.u16val = EndianSwap16(para->register_info.province_id);
    copyU16ToU8ArrayToBufferSend(u16converter.u8array);

    // 市县域ID.
    u16converter.u16val = EndianSwap16(para->register_info.city_id);
    copyU16ToU8ArrayToBufferSend(u16converter.u8array);

    // 制造商ID.
    bufferSendPushBytes(para->register_info.manufacturer_id, sizeof(para->register_info.manufacturer_id));

    // 终端型号.
    bufferSendPushBytes(para->register_info.terminal_model, sizeof(para->register_info.terminal_model));

    // 终端ID.
    bufferSendPushBytes(para->register_info.terminal_id, sizeof(para->register_info.terminal_id));

    // 车牌颜色.
    bufferSendPushByte(para->register_info.car_plate_color);

    // 车牌标识，即车牌号
    if (para->register_info.car_plate_color != 0x00)
    {
        len_car_num = strlen(para->register_info.car_plate_num);
        bufferSendPushBytes(para->register_info.car_plate_num, len_car_num);
        msg_len += len_car_num;
    }

    return msg_len;
}

// 终端注销.
int handle_kTerminalLogOut(struct ProtocolParameter *para)
{
    printf("[%s] 终端注销 <消息体为空> msg_id = 0x%04x\n", __FUNCTION__, kTerminalLogOut);

    // 空消息体.
    return 0;
}

// 终端鉴权.
int handle_kTerminalAuthentication(struct ProtocolParameter *para)
{
    int msg_len = strlen(para->parse.authentication_code);
		printf("[%s] msg_id = 0x%04x\n", __FUNCTION__, kTerminalAuthentication);

    
    // 鉴权码.
    bufferSendPushBytes(para->parse.authentication_code, msg_len);

    return msg_len;
}

// 查询终端参数应答.
int handle_kGetTerminalParametersResponse(struct ProtocolParameter *para)
{
    printf("[%s] msg_id = 0x%04x\n", __FUNCTION__, kGetTerminalParametersResponse);
		return 0;
}

// 终端升级结果通知.
int handle_kTerminalUpgradeResultReport(struct ProtocolParameter *para)
{
    printf("[%s] msg_id = 0x%04x\n", __FUNCTION__, kTerminalUpgradeResultReport);
		return 0;
}

// 位置信息汇报.
int handle_kLocationReport(struct ProtocolParameter *para)
{
    

    unsigned int v_alarm_value = 0;
    unsigned int v_status_value = 0;
    double const v_latitude = 23.123456;
    double const v_longitude = 123.123456;
    float const v_altitude = 10;
    float const v_speed = 10;
    float const v_bearing = 100;
    unsigned char *v_timestamp = "211221213045";
		unsigned char time_bcd[6] = {0};
		int msg_len;
		union U32ToU8Array u32converter;
		union U16ToU8Array u16converter;
		printf("[%s] 位置上报 msg_id = 0x%04x\n", __FUNCTION__, kLocationReport);
    updateLocation(para, v_alarm_value, v_status_value, v_latitude, v_longitude, v_altitude, v_speed, v_bearing, v_timestamp);

    msg_len = 28;
    // auto &basic_info = para->location_info;
    // auto &extension_info = para->location_extension;
    //union U32ToU8Array u32converter;
    // 报警标志.
    u32converter.u32val = EndianSwap32(para->location_info.alarm.value);
    copyU32ToU8ArrayToBufferSend(u32converter.u8array);
    // 状态.
    u32converter.u32val = EndianSwap32(para->location_info.status.value);
    copyU32ToU8ArrayToBufferSend(u32converter.u8array);
    // 纬度.
    u32converter.u32val = EndianSwap32(para->location_info.latitude);
    copyU32ToU8ArrayToBufferSend(u32converter.u8array);
    // 经度.
    u32converter.u32val = EndianSwap32(para->location_info.longitude);
    copyU32ToU8ArrayToBufferSend(u32converter.u8array);
    //union U16ToU8Array u16converter;
    // 海拔高程.
    u16converter.u16val = EndianSwap16(para->location_info.altitude);
    copyU16ToU8ArrayToBufferSend(u16converter.u8array);
    // 速度.
    u16converter.u16val = EndianSwap16(para->location_info.speed);
    copyU16ToU8ArrayToBufferSend(u16converter.u8array);
    // 方向.
    u16converter.u16val = EndianSwap16(para->location_info.bearing);
    copyU16ToU8ArrayToBufferSend(u16converter.u8array);

    
    jt808StringToBcdCompress(para->location_info.time, time_bcd, strlen(para->location_info.time));
    bufferSendPushBytes(time_bcd, 6);
		return msg_len;
}

// 位置信息查询应答.
int handle_kGetLocationInformationResponse(struct ProtocolParameter *para)
{
    printf("[%s] msg_id = 0x%04x\n", __FUNCTION__, kGetLocationInformationResponse);
		return 0;
}

int jt808FrameBodyPackage(struct ProtocolParameter *para)
{
    unsigned short msg_id = para->msg_head.msg_id;
		int result = -1;
    printf("[jt808FrameBodyPackage] current msg_id: 0x%04x\n", para->msg_head.msg_id);
    

    switch (msg_id)
    {
        // 终端通用应答.
    case kTerminalGeneralResponse:
    {
        result = handle_kGetTerminalParametersResponse(para);
    }
    break;

        // 终端心跳.
    case kTerminalHeartBeat:
    {
        result = handle_kTerminalHeartBeat(para);
    }
    break;

        // 终端注册.
    case kTerminalRegister:
    {
        result = handle_kTerminalRegister(para);
    }
    break;

        // 终端注销.
    case kTerminalLogOut:
    {
        result = handle_kTerminalLogOut(para);
    }
    break;

    // 终端鉴权.
    case kTerminalAuthentication:
    {
        result = handle_kTerminalAuthentication(para);
    }
    break;

    // 查询终端参数应答.
    case kGetTerminalParametersResponse:
    {
        result = handle_kGetTerminalParametersResponse(para);
    }
    break;

    // 终端升级结果通知.
    case kTerminalUpgradeResultReport:
    {
        result = handle_kTerminalUpgradeResultReport(para);
    }
    break;

    // 位置信息汇报
    case kLocationReport:
    {
        result = handle_kLocationReport(para);
    }
    break;

    // 位置信息查询应答.
    case kGetLocationInformationResponse:
    {
        result = handle_kGetLocationInformationResponse(para);
    }
    break;

    default:
        break;
    }

    return result;
}

// 消息内容长度修正.
int jt808MsgBodyLengthFix(struct MsgHead *msg_head, unsigned int msgBody_len)
{
		union MsgBodyAttribute msgbody_attr;
		union U16ToU8Array u16converter;
    if (RealBufferSendSize < 12)
        return -1;
    msgbody_attr = msg_head->msgbody_attr;
    msgbody_attr.bit.msglen = msgBody_len;
    //union U16ToU8Array u16converter;
    u16converter.u16val = EndianSwap16(msgbody_attr.u16val);
    BufferSend[3] = u16converter.u8array[0];
    BufferSend[4] = u16converter.u8array[1];
    return 0;
}

// JT808协议转义.
int jt808MsgEscape()
{
		unsigned int outBufferSize;
		unsigned char *outBuffer;
    BufferSend[0] = 0x00;
    BufferSend[RealBufferSendSize - 1] = 0x00;

    outBufferSize = RealBufferSendSize * 2;
    outBuffer = (unsigned char *)malloc(outBufferSize);

    if (Escape_C(BufferSend, RealBufferSendSize, outBuffer, &outBufferSize) < 0)
        return -1;

    *(outBuffer + 0) = PROTOCOL_SIGN;
    *(outBuffer + (outBufferSize - 1)) = PROTOCOL_SIGN;

    memcpy(BufferSend, outBuffer, outBufferSize);
    RealBufferSendSize = outBufferSize;

    if (outBuffer != NULL)
    {
        free(outBuffer);
        outBuffer = NULL;
    }

    return 0;
}

void jt808SetFrameFlagHeader()
{
    // BufferSend[0] = PROTOCOL_SIGN; //设置头标识
    // ++RealBufferSendSize; //发送buf长度自加
    // bufferSendPushByte(PROTOCOL_SIGN);
    bufferSendChangeByte(PROTOCOL_SIGN, 0);
}

int jt808FrameHeadPackage(struct MsgHead *msg_head)
{
    union U16ToU8Array u16converter;
		unsigned char phone_num_bcd[6] = {0};

    // 1消息ID.
    u16converter.u16val = EndianSwap16(msg_head->msg_id);
    copyU16ToU8ArrayToBufferSend(u16converter.u8array);

    // 2消息体属性.
    u16converter.u16val = EndianSwap16(msg_head->msgbody_attr.u16val);
    copyU16ToU8ArrayToBufferSend(u16converter.u8array);

    // 3终端手机号(BCD码).
    // msg_head->phone_num = "17737702736"; //测试用2022.10.25
    //phone_num_bcd[6] = {0};
    jt808StringToBcdCompress(msg_head->phone_num, phone_num_bcd, strlen(msg_head->phone_num));
    bufferSendPushBytes(phone_num_bcd, 6);

    // 4消息流水号.
    u16converter.u16val = EndianSwap16(msg_head->msg_flow_num);
    bufferSendPushBytes(u16converter.u8array, 2);

    // 5封包项.
    if ((msg_head->msgbody_attr.bit.packet == 1) &&
        (msg_head->total_packet > 1))
    {
        u16converter.u16val = EndianSwap16(msg_head->total_packet);

        bufferSendPushBytes(u16converter.u8array, 2);

        u16converter.u16val = EndianSwap16(msg_head->packet_seq);
        bufferSendPushBytes(u16converter.u8array, 2);
    }
    return 0;
}

int jt808FramePackage(struct ProtocolParameter *para)
{
		int ret;
		unsigned char valueCheck;
    // 清空发送缓存，避免缓存中会有前一次的数据
    clearBufferSend();
    // 0、设置头标志位
    jt808SetFrameFlagHeader();
    printf("[jt808SetFrameFlagHeader] OK !\n");

    // 1、生成消息头
    if (jt808FrameHeadPackage(&(para->msg_head)) < 0)
        return -1;

    printf("[jt808FrameHeadPackage] OK !\n");

    // 2、封装消息内容.
		ret = jt808FrameBodyPackage(para);

    if (ret >= 0)
    {
        // 3、修正消息长度.
        if (jt808MsgBodyLengthFix(&(para->msg_head), ret) < 0)

            return -1;

        // 4、获取校验码，并将其写入发送缓存.
        valueCheck = BccCheckSum(BufferSend, RealBufferSendSize);
        bufferSendPushByte(valueCheck);

        // 5、写入发送缓存结束标识位.
        bufferSendPushByte(PROTOCOL_SIGN);

        // 6、处理转义.
        if (jt808MsgEscape() < 0)
            return -1;
        return 0;
    }

    return -1;
}

void updateLocation(struct ProtocolParameter *para, unsigned int v_alarm_value,
                      unsigned int v_status_value, double const v_latitude,
                      double const v_longitude, float const v_altitude,
                      float const v_speed, float const v_bearing,
                      unsigned char *v_timestamp)
{
    printf("\n\r[initGPSInfo] OK !\n");
    //报警标志
    para->location_info.alarm.value = v_alarm_value;
    printf("para->alarm.value = %d\n", para->location_info.alarm.value);
    //状态
    para->location_info.status.value = v_status_value;
    printf("para->status.value = %d\n", para->location_info.status.value);

    // if (speed >= 10) //默认车速大于等于10公里时为正常行驶状态
    // {
    //   isCarMoving.store(true);
    // }
    // else
    // {
    //   isCarMoving.store(false);
    // }
    para->location_info.latitude = v_latitude * 1e6;
    printf("para->latitude = %d\n", para->location_info.latitude);

    para->location_info.longitude = v_longitude * 1e6;
    printf("para->longitude = %d\n", para->location_info.longitude);

    para->location_info.altitude = v_altitude;
    printf("para->altitude = %d\n", para->location_info.altitude);

    para->location_info.speed = v_speed * 10;
    printf("para->speed = %d\n", para->location_info.speed);

    para->location_info.bearing = v_bearing;
    printf("para->bearing = %d\n", para->location_info.bearing);

    para->location_info.time = v_timestamp;
    printf("para->time = %s\n", para->location_info.time);
}
