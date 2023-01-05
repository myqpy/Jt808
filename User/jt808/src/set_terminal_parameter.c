#include "set_terminal_parameter.h"
#include "terminal_parameter.h"
#include "client_manager.h"
#include "ff.h"

unsigned short kParameterSettingCMD[PARA_SETTING_LIMIT] = {
	kTerminalHeartBeatInterval, // DWORD, 终端心跳发送间隔(s).
	kMainServerAddress,//STRING, 主服务器地址,IP 或域名
	kServerPort,//DWORD, 服务器 TCP 端口		
	kDefaultTimeReportTimeInterval, // DWORD, 缺省时间汇报间隔
	kCornerPointRetransmissionAngle, // DWORD, 拐点补传角度, < 180°.
	kMaxSpeed,// DWORD, 最高速度, km/h.
	kProvinceID,// WORD, 车辆所在的省域 ID
	kCityID,// WORD, 车辆所在的市域 ID	
	kCarPlateNum,//STRING, 公安交通管理部门颁发的机动车号牌
	kCarPlateColor,//车牌颜色，按照 JT/T415-2006 的 5.4.12
};

void jt808ParameterSettingParse(unsigned int id,unsigned char *buf,unsigned char buf_len, struct ProtocolParameter *para)
{
	switch (id)
    {
			case kTerminalHeartBeatInterval:
			{
				handle_HeartBeatInterval(buf,buf_len,para);
			}
			break;

			case kMainServerAddress:
			{
				handle_MainServerAddress(buf,buf_len,para);
			}
			break;

			case kServerPort:
			{
				handle_ServerPort(buf,buf_len,para);
			}
			break;

			case kDefaultTimeReportTimeInterval:
			{
				handle_DefaultTimeReportTimeInterval(buf,buf_len,para);
			}
			break;

			case kCornerPointRetransmissionAngle:
			{
				handle_CornerPointRetransmissionAngle(buf,buf_len,para);
			}
			break;

			case kMaxSpeed:
			{
				handle_MaxSpeed(buf,buf_len,para);
			}
			break;

			case kProvinceID:
			{
				handle_ProvinceID(buf,buf_len,para);
			}
			break;		
			
			case kCityID:
			{
				handle_CityID(buf,buf_len,para);
			}
			break;		
			
			case kCarPlateNum:
			{
				handle_CarPlateNum(buf,buf_len,para);
			}
			break;	

			case kCarPlateColor:
			{
				handle_CarPlateColor(buf,buf_len,para);
			}
			break;
			
			default:
			break;
    }
}



void handle_HeartBeatInterval(unsigned char *buf,unsigned char buf_len,struct ProtocolParameter *para)
{
	union U32ToU8Array u32converter;//注意大小端转换
	unsigned char *p=NULL;
	unsigned int heartBeatInterval;
	unsigned char write_buf[64] = {0};
	if((buf==NULL)||(buf_len==0))
		return ;

	p = (unsigned char *)malloc(sizeof(unsigned char)*buf_len);
	memcpy(p,buf,buf_len);
	
	memcpy(u32converter.u8array,p,buf_len);
	heartBeatInterval=EndianSwap32(u32converter.u32val);
	
	para->parse.terminal_parameters.HeartBeatInterval = heartBeatInterval;
	memset(write_buf,0,sizeof(write_buf));
	memcpy(write_buf, &para->parse.terminal_parameters, sizeof(para->parse.terminal_parameters));
	printf("handle_HeartBeatInterval ==== %d \r\n",para->parse.terminal_parameters.HeartBeatInterval);
	FLASH_WriteByte(FLASH_ADDR , write_buf , sizeof(write_buf));
	
	free(p);
	
	return ;
}

void handle_MainServerAddress(unsigned char *buf,unsigned char buf_len,struct ProtocolParameter *para)
{
	unsigned char *p=NULL;
	unsigned char write_buf[64] = {0};
	if((buf==NULL)||(buf_len==0))
		return ;


	p = (unsigned char *)malloc(sizeof(unsigned char)*(buf_len+1));
	memcpy(p,buf,buf_len);
	
	//字符串注意GBK转码
	memset(para->parse.terminal_parameters.MainServerAddress,0,sizeof(para->parse.terminal_parameters.MainServerAddress));
	memcpy(para->parse.terminal_parameters.MainServerAddress,p,buf_len);
	memset(write_buf,0,sizeof(write_buf));
	memcpy(write_buf, &para->parse.terminal_parameters, sizeof(para->parse.terminal_parameters));
	FLASH_WriteByte(FLASH_ADDR , write_buf , sizeof(write_buf));	
	printf("handle_MainServerAddress ==== %s \r\n",para->parse.terminal_parameters.MainServerAddress);
	free(p);
	
	return ;
}

void handle_ServerPort(unsigned char *buf,unsigned char buf_len,struct ProtocolParameter *para)
{
	union U32ToU8Array u32converter;//注意大小端转换
	unsigned char *p=NULL;
	unsigned int serverPort;
	unsigned char write_buf[64] = {0};
	
	if((buf==NULL)||(buf_len==0))
		return ;

	p = (unsigned char *)malloc(sizeof(unsigned char)*buf_len);
	memcpy(p,buf,buf_len);
	
	
	memcpy(u32converter.u8array,p,buf_len);
	serverPort=EndianSwap32(u32converter.u32val);
	
	para->parse.terminal_parameters.ServerPort = serverPort;
	memset(write_buf,0,sizeof(write_buf));
	memcpy(write_buf, &para->parse.terminal_parameters, sizeof(para->parse.terminal_parameters));
	FLASH_WriteByte(FLASH_ADDR , write_buf , sizeof(write_buf));
	printf("handle_ServerPort ==== %d \r\n",para->parse.terminal_parameters.ServerPort);
	free(p);
	
	return ;
}

void handle_DefaultTimeReportTimeInterval(unsigned char *buf, unsigned char buf_len,struct ProtocolParameter *para)
{
	union U32ToU8Array u32converter;//注意大小端转换
	unsigned char *p=NULL;
	unsigned int DefaultTimeReportTimeInterval;
	unsigned char write_buf[64] = {0};
	if((buf==NULL)||(buf_len==0))
		return ;

	p = (unsigned char *)malloc(sizeof(unsigned char)*buf_len);
	memcpy(p,buf,buf_len);
	
	
	memcpy(u32converter.u8array,p,buf_len);
	DefaultTimeReportTimeInterval=EndianSwap32(u32converter.u32val);
	if(DefaultTimeReportTimeInterval >= 30)
	{
		DefaultTimeReportTimeInterval = 30;
	}
	if(DefaultTimeReportTimeInterval <= 2)
	{
		DefaultTimeReportTimeInterval = 2;
	}
	para->parse.terminal_parameters.DefaultTimeReportTimeInterval = DefaultTimeReportTimeInterval;
	memset(write_buf,0,sizeof(write_buf));
	memcpy(write_buf, &para->parse.terminal_parameters, sizeof(para->parse.terminal_parameters));
	FLASH_WriteByte(FLASH_ADDR , write_buf , sizeof(write_buf));
	printf("handle_DefaultTimeReportTimeInterval ==== %d \r\n",para->parse.terminal_parameters.DefaultTimeReportTimeInterval);
	free(p);
	
	return ;
}

void handle_CornerPointRetransmissionAngle(unsigned char *buf, unsigned char buf_len,struct ProtocolParameter *para)
{
	union U32ToU8Array u32converter;//注意大小端转换
	unsigned char *p=NULL;
	unsigned int CornerPointRetransmissionAngle;
	unsigned char write_buf[64] = {0};
	if((buf==NULL)||(buf_len==0))
		return ;

	p = (unsigned char *)malloc(sizeof(unsigned char)*buf_len);
	memcpy(p,buf,buf_len);
	
	
	memcpy(u32converter.u8array,p,buf_len);
	CornerPointRetransmissionAngle=EndianSwap32(u32converter.u32val);

	para->parse.terminal_parameters.CornerPointRetransmissionAngle = CornerPointRetransmissionAngle;
	memset(write_buf,0,sizeof(write_buf));
	memcpy(write_buf, &para->parse.terminal_parameters, sizeof(para->parse.terminal_parameters));
	FLASH_WriteByte(FLASH_ADDR , write_buf , sizeof(write_buf));
	printf("handle_DefaultTimeReportTimeInterval ==== %d \r\n",para->parse.terminal_parameters.DefaultTimeReportTimeInterval);
	free(p);
	
	return ;
}

void handle_MaxSpeed(unsigned char *buf,unsigned char buf_len,struct ProtocolParameter *para)
{
	union U32ToU8Array u32converter;//注意大小端转换
	unsigned char *p=NULL;
	unsigned int MaxSpeed;
	unsigned char write_buf[64] = {0};
	if((buf==NULL)||(buf_len==0))
		return ;

	p = (unsigned char *)malloc(sizeof(unsigned char)*buf_len);
	memcpy(p,buf,buf_len);
	
	
	memcpy(u32converter.u8array,p,buf_len);
	MaxSpeed=EndianSwap32(u32converter.u32val);
	
	para->parse.terminal_parameters.MaxSpeed = MaxSpeed;
	memset(write_buf,0,sizeof(write_buf));
	memcpy(write_buf, &para->parse.terminal_parameters, sizeof(para->parse.terminal_parameters));
	FLASH_WriteByte(FLASH_ADDR, write_buf , sizeof(write_buf));
	printf("handle_MaxSpeed ==== %d \r\n",para->parse.terminal_parameters.MaxSpeed);
	free(p);
	
	return ;
}

void handle_ProvinceID(unsigned char *buf,unsigned char buf_len,struct ProtocolParameter *para)
{
	union U16ToU8Array u16converter;//注意大小端转换
	unsigned char *p=NULL;
	unsigned short ProvinceID;
	unsigned char write_buf[64] = {0};
	if((buf==NULL)||(buf_len==0))
		return ;

	p = (unsigned char *)malloc(sizeof(unsigned char)*buf_len);
	memcpy(p,buf,buf_len);
	
	
	memcpy(u16converter.u8array,p,buf_len);
	ProvinceID=EndianSwap16(u16converter.u16val);
	
	para->parse.terminal_parameters.ProvinceID = ProvinceID;
	memset(write_buf,0,sizeof(write_buf));
	memcpy(write_buf, &para->parse.terminal_parameters, sizeof(para->parse.terminal_parameters));
	FLASH_WriteByte(FLASH_ADDR , write_buf , sizeof(write_buf));
	printf("handle_ProvinceID ==== %d \r\n",para->parse.terminal_parameters.ProvinceID);
	free(p);
	
	return ;
}

void handle_CityID(unsigned char *buf,unsigned char buf_len,struct ProtocolParameter *para)
{
	union U16ToU8Array u16converter;//注意大小端转换
	unsigned char *p=NULL;
	unsigned short CityID;
	unsigned char write_buf[64] = {0};
	if((buf==NULL)||(buf_len==0))
		return ;

	p = (unsigned char *)malloc(sizeof(unsigned char)*buf_len);
	memcpy(p,buf,buf_len);
	
	
	memcpy(u16converter.u8array,p,buf_len);
	CityID=EndianSwap16(u16converter.u16val);
	
	para->parse.terminal_parameters.CityID = CityID;
	memset(write_buf,0,sizeof(write_buf));
	memcpy(write_buf, &para->parse.terminal_parameters, sizeof(para->parse.terminal_parameters));
	FLASH_WriteByte(FLASH_ADDR, write_buf , sizeof(write_buf));
	printf("handle_CityID ==== %d \r\n",para->parse.terminal_parameters.CityID);
	free(p);
	
	return ;
}

void handle_CarPlateNum(unsigned char *buf,unsigned char buf_len,struct ProtocolParameter *para)
{
	//字符串注意GBK转码
	unsigned char *p=NULL;
	unsigned char write_buf[64] = {0};
	if((buf==NULL)||(buf_len==0))
		return ;

	p = (unsigned char *)malloc(sizeof(unsigned char)*buf_len+1);
	memcpy(p,buf,buf_len);
	
	memset(para->parse.terminal_parameters.CarPlateNum,0,sizeof(para->parse.terminal_parameters.CarPlateNum));
	memcpy(para->parse.terminal_parameters.CarPlateNum,p,buf_len);
	memset(write_buf,0,sizeof(write_buf));
	memcpy(write_buf, &para->parse.terminal_parameters, sizeof(para->parse.terminal_parameters));
	FLASH_WriteByte(FLASH_ADDR , write_buf , sizeof(write_buf));
	printf("handle_CarPlateNum ==== %s \r\n",para->parse.terminal_parameters.CarPlateNum);
	free(p);
	
	return ;
}

void handle_CarPlateColor(unsigned char *buf,unsigned char buf_len,struct ProtocolParameter *para)
{
	unsigned char *p=NULL;
	unsigned char write_buf[64] = {0};
	if((buf==NULL)||(buf_len==0))
		return ;
	p = (unsigned char *)malloc(sizeof(unsigned char)*buf_len+1);
	memcpy(p,buf,buf_len);

	para->parse.terminal_parameters.CarPlateColor = *p;
	memset(write_buf,0,sizeof(write_buf));
	memcpy(write_buf, &para->parse.terminal_parameters, sizeof(para->parse.terminal_parameters));
	FLASH_WriteByte(FLASH_ADDR , write_buf , sizeof(write_buf));
	printf("handle_CarPlateColor ==== 0x%02x \r\n",para->parse.terminal_parameters.CarPlateColor);
	free(p);
	
	return ;
}

