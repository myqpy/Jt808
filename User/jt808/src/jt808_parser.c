#include "jt808_parser.h"

#include "util.h"
//#include <memory.h>
#include "bcd.h"

// 所有终端解析命令.
unsigned short kTerminalParserCMD[PARSER_NUM] = {
    kPlatformGeneralResponse,       //平台通用应答
    kFillPacketRequest,             // 补传分包请求.
    kTerminalRegisterResponse,      // 终端注册应答.
    kSetTerminalParameters,         // 设置终端参数.
    kGetTerminalParameters,         // 查询终端参数.
    kGetSpecificTerminalParameters, // 查询指定终端参数.
    kTerminalControl,               //终端控制
    kTerminalUpgrade,               // 下发终端升级包.
    kGetLocationInformation,        // 位置信息查询.
};

/// @brief 接收缓存
unsigned char BufferReceive[BUFFER_SIZE_RECEIVE] = {0};

/// @brief 实际接收的数据长度
unsigned int RealBufferReceiveSize = 0;

// 解析消息头.
int jt808FrameHeadParse(const unsigned char *in, unsigned int in_len, struct MsgHead *msg_head)
{
		static int isPhoneMalloc;
    if (msg_head == NULL || in_len < 15)
        return -1;
    // 消息ID.
    // msg_head->msg_id = in[1] * 256 + in[2];
    msg_head->msg_id = (in[1] << 8) + in[2];
    printf("[jt808FrameHeadParse] msg_head->msg_id = 0x%02x\r\n", msg_head->msg_id);

    // 消息体属性.
    msg_head->msgbody_attr.u16val = (in[3] << 8) + in[4];
    printf("[jt808FrameHeadParse] msg_head->msgbody_attr.u16val = 0x%02x\r\n", msg_head->msgbody_attr.u16val);

    // 终端手机号.
    isPhoneMalloc = 0;
    if (isPhoneMalloc == 0)
    {
        msg_head->phone_num = (unsigned char *)malloc(12 * sizeof(unsigned char));
    }
    memset(msg_head->phone_num, 0, 12);

    if (jt808BcdToStringCompress((&(in[5])), msg_head->phone_num, 6) == NULL)
    {
        return -1;
    }
    printf("[jt808FrameHeadParse] msg_head->phone_num = %s !!!\r\n", msg_head->phone_num);

    // 消息流水号.
    msg_head->msg_flow_num = (in[11] << 8) + in[12];
    printf("[jt808FrameHeadParse] msg_head->msg_flow_num = 0x%02x !!!\r\n", msg_head->msg_flow_num);

    // 出现封包.
    if ((msg_head->msgbody_attr.bit.packet == 1) &&
        ((in_len - 15 - msg_head->msgbody_attr.bit.msglen) == 4))
    {
        msg_head->total_packet = (in[13] << 8) + in[14];
        msg_head->packet_seq = (in[15] << 8) + in[16];
    }
    else
    {
        msg_head->total_packet = 0;
        msg_head->packet_seq = 0;
    }
    return 0;
}

//平台通用应答
int handle_kPlatformGeneralResponse(struct ProtocolParameter *para)
{
		uint16_t pos;
    printf("[%s] 平台通用应答 msg_id = 0x%04x\r\n", __FUNCTION__, kPlatformGeneralResponse);
    if (para == NULL)
        return -1;
    pos = MSGBODY_NOPACKET_POS;
    if (para->msg_head.msgbody_attr.bit.packet == 1)
        pos = MSGBODY_PACKET_POS;
    // 应答流水号.
    para->parse.respone_flow_num = BufferReceive[pos] << 8 + BufferReceive[pos + 1];
    printf("[%s] 平台通用应答 应答流水号 = 0x%04x\r\n", __FUNCTION__, para->parse.respone_flow_num);

    // 应答消息ID.
    para->parse.respone_msg_id = BufferReceive[pos + 2] << 8 + BufferReceive[pos + 3];
    printf("[%s] 平台通用应答 应答流水号 = 0x%04x\r\n", __FUNCTION__, para->parse.respone_msg_id);

    // 应答结果.
    para->parse.respone_result = BufferReceive[pos + 4];
    printf("[%s] 平台通用应答 应答流水号 = 0x%04x\r\n", __FUNCTION__, para->parse.respone_result);

    return 0;
}

//  补传分包请求.
int handle_kFillPacketRequest(struct ProtocolParameter *para)
{
    printf("[%s] msg_id = 0x%04x\r\n", __FUNCTION__, kPlatformGeneralResponse);

    return 0;
}

// 终端注册应答..
int handle_kTerminalRegisterResponse(struct ProtocolParameter *para)
{
		unsigned short pos;
		unsigned short len_code;
	
    printf("[%s] TerminalRegisterResponse msg_id = 0x%04x\r\n", __FUNCTION__, kPlatformGeneralResponse);

    if (para == NULL)
        return -1;
    pos = MSGBODY_NOPACKET_POS;
    if (para->parse.msg_head.msgbody_attr.bit.packet == 1)
        pos = MSGBODY_PACKET_POS;
    // 应答流水号.
    para->parse.respone_flow_num = (BufferReceive[pos] << 8) + BufferReceive[pos + 1];
    printf("[%s] TerminalRegisterResponseFlow#  = 0x%04x\r\n", __FUNCTION__, para->parse.respone_flow_num);
    // 应答结果.
    para->parse.respone_result = BufferReceive[pos + 2];
    printf("[%s] TerminalRegisterResponseResult  = 0x%02x\r\n", __FUNCTION__, para->parse.respone_result);
    // 应答结果为0(成功)时解析出附加的鉴权码.
    if (para->parse.respone_result == 0)
    {
        len_code = para->parse.msg_head.msgbody_attr.bit.msglen - 3;
        para->parse.authentication_code = (unsigned char *)malloc((len_code + 1) * sizeof(unsigned char));
        memcpy(para->parse.authentication_code, &(BufferReceive[pos + 3]), len_code);

        printf("[%s] TerminalRegisterResponseAuthenticationCode = %s\r\n", __FUNCTION__, para->parse.authentication_code);
    }

    return 0;
}

// 设置终端参数..
int handle_kSetTerminalParameters(struct ProtocolParameter *para)
{
		uint16_t pos;
		unsigned short msg_len;
		unsigned char cnt;
		union U32ToU8Array u32converter;
		unsigned int id;
			
    printf("[%s] 设置终端参数 msg_id = 0x%04x\r\n", __FUNCTION__, kSetTerminalParameters);
    if (para == NULL)
        return -1;
    pos = MSGBODY_NOPACKET_POS;
    if (para->parse.msg_head.msgbody_attr.bit.packet == 1)
        pos = MSGBODY_PACKET_POS;
    msg_len = para->parse.msg_head.msgbody_attr.bit.msglen;
    if (msg_len < 1)
        return -1;
    // 设置的参数总个数.
    cnt = BufferReceive[pos];
    ++pos;

    
    // 设置的参数项.
    id = 0;

    // std::vector<uint8_t> value;
    // auto &paras = para->parse.terminal_parameters;
    // paras.clear();

    // for (int i = 0; i < cnt; ++i)
    // {
    //     // 参数ID.
    //     memcpy(u32converter.u8array, &(in[pos]), 4);
    //     id = EndianSwap32(u32converter.u32val);
    //     pos += 4;
    //     // 参数值.
    //     value.assign(in.begin() + pos + 1, in.begin() + pos + 1 + in[pos]);
    //     paras.insert({id, value});
    //     pos += 1 + in[pos];
    // }

    return 0;
}

// 查询终端参数..
int handle_kGetTerminalParameters(struct ProtocolParameter *para)
{
    printf("[%s] 查询终端参数 msg_id = 0x%04x\r\n", __FUNCTION__, kGetTerminalParameters);

    return 0;
}

//查询指定终端参数..
int handle_kGetSpecificTerminalParameters(struct ProtocolParameter *para)
{
    printf("[%s] 查询指定终端参数 msg_id = 0x%04x\r\n", __FUNCTION__, kGetSpecificTerminalParameters);

    return 0;
}

// 终端控制
int handle_kTerminalControl(struct ProtocolParameter *para)
{
    printf("[%s] 终端控制 msg_id = 0x%04x\r\n", __FUNCTION__, kTerminalControl);

    return 0;
}

// 下发终端升级包.
int handle_kTerminalUpgrade(struct ProtocolParameter *para)
{
    printf("[%s] 下发终端升级包 msg_id = 0x%04x\r\n", __FUNCTION__, kTerminalUpgrade);

    return 0;
}

//  位置信息查询..
int handle_kGetLocationInformation(struct ProtocolParameter *para)
{
    printf("[%s] 位置信息查询 msg_id = 0x%04x\r\n", __FUNCTION__, kGetLocationInformation);

    return 0;
}

int jt808FrameBodyParse(struct ProtocolParameter *para)
{
    unsigned short msg_id = para->parse.msg_head.msg_id;
    
    int result = -1;
		printf("[jt808FrameBodyParse] current msg_id: 0x%04x\r\n", msg_id);
	
    switch (msg_id)
    {
    // +平台通用应答.
    case kPlatformGeneralResponse:
    {
        result = handle_kPlatformGeneralResponse(para);
    }
    break;

    //  补传分包请求.
    case kFillPacketRequest:
    {
        result = handle_kFillPacketRequest(para);
    }
    break;

    // 终端注册应答..
    case kTerminalRegisterResponse:
    {
        result = handle_kTerminalRegisterResponse(para);
    }
    break;

    // 设置终端参数..
    case kSetTerminalParameters:
    {
        result = handle_kSetTerminalParameters(para);
    }
    break;

    // 查询终端参数..
    case kGetTerminalParameters:
    {
        result = handle_kGetTerminalParameters(para);
    }
    break;

    //查询指定终端参数..
    case kGetSpecificTerminalParameters:
    {
        result = handle_kGetSpecificTerminalParameters(para);
    }
    break;

    // 终端控制
    case kTerminalControl:
    {
        result = handle_kTerminalControl(para);
    }
    break;

    // 下发终端升级包.
    case kTerminalUpgrade:
    {
        result = handle_kTerminalUpgrade(para);
    }
    break;

    //  位置信息查询..
    case kGetLocationInformation:
    {
        result = handle_kGetLocationInformation(para);
    }
    break;

    default:
        break;
    }

    return result;
}

int jt808FrameParse(const unsigned char *in, unsigned int in_len, struct ProtocolParameter *para)
{
		int ret;
		unsigned int outBufferSize;
		unsigned char *outBuffer;
	
    printf("%s[%d]: JT808FrameParse -->1 !!!\r\n", __FUNCTION__, __LINE__);
    if (para == NULL)
        return -1;
    memcpy(BufferReceive, in, in_len);
    RealBufferReceiveSize = in_len;
    outBufferSize = RealBufferReceiveSize;
    outBuffer = (unsigned char *)malloc(outBufferSize * sizeof(unsigned char));
    memset(outBuffer, 0, outBufferSize);

    printf("%s[%d]: outBufferSize = %d \r\n", __FUNCTION__, __LINE__, outBufferSize);

    // 逆转义.
    if (ReverseEscape_C(BufferReceive, RealBufferReceiveSize, outBuffer, &outBufferSize) < 0)
        return -1;
    RealBufferReceiveSize = outBufferSize;
    printf("%s[%d]: ReverseEscape_C.  outBufferSize = %d  !!!\r\n", __FUNCTION__, __LINE__, outBufferSize);

    // 异或校验检查.
    if (BccCheckSum(&(outBuffer[1]), (outBufferSize - 3)) != *(outBuffer + outBufferSize - 2))
        return -1;
    printf("%s[%d]: BccCheckSum. -->3 !!!\r\n", __FUNCTION__, __LINE__);

    // 解析消息头.
    if (jt808FrameHeadParse(outBuffer, outBufferSize, &(para->parse.msg_head)) != 0)
        return -1;
    printf("%s[%d]: FrameHeadParse. -->4 !!!\r\n", __FUNCTION__, __LINE__);
    para->msg_head.phone_num = para->parse.msg_head.phone_num;

    // 解析消息内容.
    ret = jt808FrameBodyParse(para);

    //释放缓存
    if (outBuffer != NULL)
    {
        free(outBuffer);
        outBuffer = NULL;
    }

    return ret;
}