
#include "client_manager.h"
#include "protocol_parameter.h"
#include "jt808_packager.h"
#include "jt808_parser.h"
#include "util.h"
#include "bcd.h"
#include "jt808_parser.h"

struct ProtocolParameter parameter_;

/// @brief 设置终端手机号
/// @param phone
void setTerminalPhoneNumber(const char *phone_num, unsigned int phoneSize)
{
    parameter_.msg_head.phone_num = (unsigned char *)phone_num;
}

int packagingAndSendMessage(unsigned int msg_id)
{
    return 0;
}

int packagingMessage(unsigned int msg_id)
{
    //查找当前msgID是否存在于待打包消息ID数组中
    if (0 == findMsgIDFromTerminalPackagerCMD(msg_id))
    {
        printf("[findMsgIDFromTerminalPackagerCMD] no current msg_id in kTerminalPackagerCMD\n");
        return -1;
    }

    printf("[findMsgIDFromTerminalPackagerCMD] OK !\n");

    parameter_.msg_head.msg_id = msg_id; // 设置消息ID.
    if (jt808FramePackage(&parameter_) < 0)
    {
        printf("[jt808FramePackage]: Package message failed !!!\n");
        return -1;
    }
    ++parameter_.msg_head.msg_flow_num; // 每正确生成一条命令, 消息流水号增加1.

    return 0;
}

int findMsgIDFromTerminalPackagerCMD(unsigned int msg_id)
{
    int result = 0, i;
    for (i = 0; i < PACKAGER_NUM; ++i)
    {
        if (kTerminalPackagerCMD[i] == msg_id)
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
        printf("error parsing\n");
        return -1;
    }

    printf("ok parsing\n");
    msg_id = parameter_.parse.msg_head.msg_id;
    printf("%s[%d]: 接受数据解析后的消息ID msg_id = 0x%02x\n", __FUNCTION__, __LINE__, msg_id);
    switch (msg_id)
    {
    // +平台通用应答.
    case kPlatformGeneralResponse:
    {
        printf("%s[%d]: 执行【 平台通用应答 】后的操作\n", __FUNCTION__, __LINE__);
    }
    break;

    //  补传分包请求.
    case kFillPacketRequest:
    {
        printf("%s[%d]: 执行【 补传分包请求 】后的操作\n", __FUNCTION__, __LINE__);
    }
    break;

    // 终端注册应答..
    case kTerminalRegisterResponse:
    {
        printf("%s[%d]: 执行【 终端注册应答 】后的操作\n", __FUNCTION__, __LINE__);
    }
    break;

    // 设置终端参数..
    case kSetTerminalParameters:
    {
        printf("%s[%d]: 执行【 设置终端参数 】后的操作\n", __FUNCTION__, __LINE__);
    }
    break;

    // 查询终端参数..
    case kGetTerminalParameters:
    {
        printf("%s[%d]: 执行【 查询终端参数 】后的操作\n", __FUNCTION__, __LINE__);
    }
    break;

    //查询指定终端参数..
    case kGetSpecificTerminalParameters:
    {
        printf("%s[%d]: 执行【 查询指定终端参数 】后的操作\n", __FUNCTION__, __LINE__);
    }
    break;

    // 终端控制
    case kTerminalControl:
    {
        printf("%s[%d]: 执行【 终端控制 】后的操作\n", __FUNCTION__, __LINE__);
    }
    break;

    // 下发终端升级包.
    case kTerminalUpgrade:
    {
        printf("%s[%d]: 执行【 下发终端升级包 】后的操作\n", __FUNCTION__, __LINE__);
    }
    break;

    //  位置信息查询..
    case kGetLocationInformation:
    {
        printf("%s[%d]: 执行【 位置信息查询 】后的操作\n", __FUNCTION__, __LINE__);
    }
    break;

    default:
        break;
    }
    return 0;
}
