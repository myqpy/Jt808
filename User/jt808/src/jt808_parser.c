#include "jt808_parser.h"
#include "set_terminal_parameter.h"
#include "client_manager.h"
#include "util.h"
//#include "ff.h"
#include "bcd.h"
#include "protocol_parameter.h"

//struct ProtocolParameter parameter_;

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
    if (msg_head == NULL || in_len < 15)
	{
		printf("msg_head == NULL || in_len < 15");
		return -1;
	}
	
    // 消息ID.	
    msg_head->msg_id = (in[1] << 8) + in[2];
		
    

    // 消息体属性.
    msg_head->msgbody_attr.u16val = (in[3] << 8) + in[4];
    

    // 终端手机号.

    memset(msg_head->phone_num, 0, 12);

    if (jt808BcdToStringCompress((&(in[5])), msg_head->phone_num, 6) == NULL)
    {
		printf("jt808BcdToStringCompress error \r\n");
		return -1;
    }
    

    // 消息流水号.
    msg_head->msg_flow_num = (in[11] << 8) + in[12];
    

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
		
		#ifdef __JT808_DEBUG
			printf("[jt808FrameHeadParse] msg_head->msg_id = 0x%04x\r\n", msg_head->msg_id);
			printf("[jt808FrameHeadParse] msg_head->msgbody_attr.u16val = 0x%04x\r\n", msg_head->msgbody_attr.u16val);
			printf("[jt808FrameHeadParse] msg_head->phone_num = %s !!!\r\n", msg_head->phone_num);
			printf("[jt808FrameHeadParse] msg_head->msg_flow_num = 0x%04x !!!\r\n", msg_head->msg_flow_num);
		#endif
		
		
    return 0;
}

//平台通用应答
int handle_kPlatformGeneralResponse(struct ProtocolParameter *para)
{
		uint16_t pos;
		#ifdef __JT808_DEBUG
			printf("[%s] msg_id = 0x%04x\r\n", __FUNCTION__, kPlatformGeneralResponse);
	  #endif
    if (para == NULL)
        return -1;
    pos = MSGBODY_NOPACKET_POS;
    if (para->msg_head.msgbody_attr.bit.packet == 1)
        pos = MSGBODY_PACKET_POS;
    // 应答流水号.
    para->parse.respone_flow_num = (BufferReceive[pos] << 8) + BufferReceive[pos + 1];
    

    // 应答消息ID.
    para->parse.respone_msg_id = (BufferReceive[pos + 2] << 8) + BufferReceive[pos + 3];
    

    // 应答结果.
    para->parse.respone_result = BufferReceive[pos + 4];
		
		#ifdef __JT808_DEBUG
			printf("[%s] respone_flow_num = 0x%04x\r\n", __FUNCTION__, para->parse.respone_flow_num);
			printf("[%s] respone_msg_id = 0x%04x\r\n", __FUNCTION__, para->parse.respone_msg_id);
			printf("[%s] respone_result = 0x%04x\r\n", __FUNCTION__, para->parse.respone_result);
		#endif

    return 0;
}

//  补传分包请求.
int handle_kFillPacketRequest(struct ProtocolParameter *para)
{
//	uint16_t pos;
//	unsigned short cnt;
//	unsigned char i;
//	unsigned short id;
//	
//	printf("[%s] msg_id = 0x%04x\r\n", __FUNCTION__, kFillPacketRequest);
//	
//	if (para == NULL)
//	{
//		return -1;
//	}
//	pos = MSGBODY_NOPACKET_POS;
//	if (para->parse.msg_head.msgbody_attr.bit.packet == 1){
//			pos = MSGBODY_PACKET_POS;
//	}
//	
//  para->fill_packet.first_packet_msg_flow_num = (BufferReceive[pos] << 8) + BufferReceive[pos + 1];
//	pos += 2;
//	
//	cnt = BufferReceive[pos+2];
//	++pos;
//	
////	if(para->msg_head.msgbody_attr.bit.msglen -3 != (cnt * 2))
////	{
////		return -1;
////	}
////	
//////	memset(para->fill_packet.packet_id, 0, sizeof(para->fill_packet.packet_id));
////	for(i=0;i<cnt;++i)
////	{
////		id = BufferReceive[pos + i * 2] + BufferReceive[pos + 1 + i * 2];
////		memncpy(para->fill_packet.packet_id[pos], id, 2);
////		pos += 2;
////	}

//	File_upload();
//	
	return 0;
}

// 终端注册应答..
int handle_kTerminalRegisterResponse(struct ProtocolParameter *para)
{
		unsigned short pos;
		unsigned short len_code;
	
    printf("[%s] msg_id = 0x%04x\r\n", __FUNCTION__, kTerminalRegisterResponse);

    if (para == NULL)
        return -1;
    pos = MSGBODY_NOPACKET_POS;
    if (para->parse.msg_head.msgbody_attr.bit.packet == 1)
        pos = MSGBODY_PACKET_POS;
    // 应答流水号.
    para->parse.respone_flow_num = (BufferReceive[pos] << 8) + BufferReceive[pos + 1];
    printf("[%s] respone_flow_num  = 0x%04x\r\n", __FUNCTION__, para->parse.respone_flow_num);
    // 应答结果.
    para->parse.respone_result = BufferReceive[pos + 2];
    printf("[%s] respone_result  = 0x%02x\r\n", __FUNCTION__, para->parse.respone_result);
    // 应答结果为0(成功)时解析出附加的鉴权码.
    if (para->parse.respone_result == kRegisterSuccess)
    {
        len_code = para->parse.msg_head.msgbody_attr.bit.msglen - 3;
        para->parse.authentication_code = (unsigned char *)malloc((len_code + 1) * sizeof(unsigned char));
        memcpy(para->parse.authentication_code, &(BufferReceive[pos + 3]), len_code);
		
        printf("[%s] authentication_code = %s\r\n", __FUNCTION__, para->parse.authentication_code);
    }
//	free(para->parse.authentication_code);
    return 0;
}

// 设置终端参数..
int handle_kSetTerminalParameters(struct ProtocolParameter *para)
{
	uint16_t pos;
	unsigned short msg_len;
	unsigned int p_id;
	unsigned char cnt;
	union U32ToU8Array u32converter;
	int isFind=0, i;
	unsigned len;
	
	jt808TerminalGeneralResponse();
	
	printf("[%s] msg_id = 0x%04x\r\n", __FUNCTION__, kSetTerminalParameters);
	if (para == NULL){
			return -1;
	}
	pos = MSGBODY_NOPACKET_POS;
	if (para->parse.msg_head.msgbody_attr.bit.packet == 1){
			pos = MSGBODY_PACKET_POS;
	}
	msg_len = para->parse.msg_head.msgbody_attr.bit.msglen;
	if (msg_len < 1){
			return -1;
	}
	// 解析设置的参数总个数.
	cnt = BufferReceive[pos];
	pos++;

	if(cnt<=0)
	{
		return -1;
	}

	for(i=0;i<cnt;++i)
	{
		//查找参数项的参数ID
		memcpy(u32converter.u8array,(BufferReceive+pos),4);
		p_id=EndianSwap32(u32converter.u32val);
		pos+=4;
		//从已支持的参数项数组中查找是否有当前参数ID
		isFind=findParameterIDFromArray(p_id);
		len=BufferReceive[pos];
		pos++;
		if(isFind==1)
		{
			jt808ParameterSettingParse(p_id,(BufferReceive+pos),len,para);
		}
		pos+=len;
	}
	
	return 0;
}

// 查询终端参数..
int handle_kGetTerminalParameters(struct ProtocolParameter *para)
{
	printf("[%s] msg_id = 0x%04x\r\n", __FUNCTION__, kGetTerminalParameters);

	return 0;
}

//查询指定终端参数..
int handle_kGetSpecificTerminalParameters(struct ProtocolParameter *para)
{
	printf("[%s] msg_id = 0x%04x\r\n", __FUNCTION__, kGetSpecificTerminalParameters);

	return 0;
}

// 终端控制
int handle_kTerminalControl(struct ProtocolParameter *para)
{
	printf("[%s] msg_id = 0x%04x\r\n", __FUNCTION__, kTerminalControl);

	return 0;
}

// 下发终端升级包.
int handle_kTerminalUpgrade(struct ProtocolParameter *para)
{
	union U32ToU8Array u32converter;
	uint16_t pos;
	int i;
	printf("[%s] msg_id = 0x%04x\r\n", __FUNCTION__, kTerminalUpgrade);
	
	if (para == NULL)
	{
		return -1;
	}
	
	pos = MSGBODY_NOPACKET_POS;
	if (para->parse.msg_head.msgbody_attr.bit.packet == 1)
	{
		pos = MSGBODY_PACKET_POS;
	}
	
	jt808TerminalGeneralResponse();
	
	// 升级类型.
	para->upgrade_info.upgrade_type = BufferReceive[pos];
	pos++;
	printf("para->upgrade_info.upgrade_type: %02x \r\n", para->upgrade_info.upgrade_type);

	// 制造商ID, 固定5个字节.
	memset(para->upgrade_info.manufacturer_id, 0, sizeof(para->upgrade_info.manufacturer_id));
	for(i=0;i<5;i++)
	{
		para->upgrade_info.manufacturer_id[i] = BufferReceive[pos];
		pos++;
	}
	printf("para->upgrade_info.manufacturer_id: %s \r\n", para->upgrade_info.manufacturer_id);
	
	//版本号长度
	para->upgrade_info.version_id_len = BufferReceive[pos];
	pos++;
	printf("para->upgrade_info.version_id_len: %02x \r\n", para->upgrade_info.version_id_len);
	
	
	// 升级版本号.
	para->upgrade_info.version_id = (unsigned char *)malloc(sizeof(unsigned char)*(para->upgrade_info.version_id_len)+1);
	memset(para->upgrade_info.version_id,0,para->upgrade_info.version_id_len);
	memcpy(para->upgrade_info.version_id, &BufferReceive[pos], para->upgrade_info.version_id_len);
	pos+=(para->upgrade_info.version_id_len);
	printf("para->upgrade_info.version_id: %s \r\n", para->upgrade_info.version_id);	
	//free(para->upgrade_info.version_id);
	
	// 升级包总长度.
	for(i=0;i<4;i++)
	{
		u32converter.u8array[i] = BufferReceive[pos];
//		printf(("%02x "),u32converter.u8array[i]);
		pos++;
	}
	para->upgrade_info.upgrade_data_total_len = EndianSwap32(u32converter.u32val);
	printf("para->upgrade_info.upgrade_data_total_len: %d \r\n", para->upgrade_info.upgrade_data_total_len);

	// 升级数据包.
	para->upgrade_info.upgrade_data = (unsigned char *)malloc(sizeof(unsigned char)*(para->upgrade_info.upgrade_data_total_len)+1);
	memset(para->upgrade_info.upgrade_data,0,para->upgrade_info.upgrade_data_total_len);
	memcpy(para->upgrade_info.upgrade_data, &BufferReceive[pos], para->upgrade_info.upgrade_data_total_len);
	printf("para->upgrade_info.upgrade_data:");
	for(i=0;i<para->upgrade_info.upgrade_data_total_len;i++)
	{
		printf(("%02x "),para->upgrade_info.upgrade_data[i]);
	}
	printf("\r\n");
//	free(para->upgrade_info.upgrade_data);
	
//	jt808TerminalUpgradeResultReport();
//	boot_loader_flag();					
	return 0;
}

//  位置信息查询..
int handle_kGetLocationInformation(struct ProtocolParameter *para)
{
	printf("[%s] msg_id = 0x%04x\r\n", __FUNCTION__, kGetLocationInformation);

	return 0;
}

int jt808FrameBodyParse(struct ProtocolParameter *para)
{
	unsigned short msg_id = para->parse.msg_head.msg_id;
	int result = -1;

	#ifdef __JT808_DEBUG
		printf("[%s] current msg_id: 0x%04x\r\n",__FUNCTION__, msg_id);
	#endif

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

	//	查询指定终端参数..
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
	
	#ifdef __JT808_DEBUG
		printf("%s[%d]: jt808FrameParse -->1 !!! \r\n", __FUNCTION__, __LINE__);
	#endif
    if (para == NULL)
	{
		printf("para == NULL \r\n");
		return -1;
	}
    memcpy(BufferReceive, in, in_len);
    RealBufferReceiveSize = in_len;
    outBufferSize = RealBufferReceiveSize;
    outBuffer = (unsigned char *)malloc(outBufferSize * sizeof(unsigned char));
    memset(outBuffer, 0, outBufferSize);

	#ifdef __JT808_DEBUG
		printf("%s[%d]: outBufferSize = %d \r\n", __FUNCTION__, __LINE__, outBufferSize);
	#endif
		
    // 逆转义.
    if (ReverseEscape_C(BufferReceive, RealBufferReceiveSize, outBuffer, &outBufferSize) < 0)
	{
		printf("ReverseEscape_C ERROR\r\n");
		return -1;
	}
        
    RealBufferReceiveSize = outBufferSize;
		
	#ifdef __JT808_DEBUG
		printf("%s[%d]: ReverseEscape_C.  outBufferSize = %d  !!!\r\n", __FUNCTION__, __LINE__, outBufferSize);
	#endif 
	
    // 异或校验检查.
    if (BccCheckSum(&(outBuffer[1]), (outBufferSize - 3)) != *(outBuffer + outBufferSize - 2))
	{
		printf("BccCheckSum ERROR\r\n");
		return -1;
	}
		
	#ifdef __JT808_DEBUG
		printf("%s[%d]: BccCheckSum. -->3 !!!\r\n", __FUNCTION__, __LINE__);
	#endif
    // 解析消息头.
    if (jt808FrameHeadParse(outBuffer, outBufferSize, &(para->parse.msg_head)) != 0)
    {
		printf("jt808FrameHeadParse ERROR\r\n");
		return -1;
	}
	#ifdef __JT808_DEBUG
		printf("%s[%d]:  jt808FrameHeadParse. -->4 !!!\r\n", __FUNCTION__, __LINE__);
	#endif 
    memcpy(para->msg_head.phone_num, para->parse.msg_head.phone_num, 11);

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
