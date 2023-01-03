#ifndef JT808_PROTOCOL_PARAMETER_H_
#define JT808_PROTOCOL_PARAMETER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "location_report.h"
#include "terminal_parameter.h"
#include "terminal_control.h"

// #define PACKAGER_NUM 9
// #define PARSER_NUM 9

// 已支持的协议命令.
enum SupportedCommands
{
  kTerminalGeneralResponse = 0x0001,        // 终端通用应答.
  kPlatformGeneralResponse = 0x8001,        // 平台通用应答.
  kTerminalHeartBeat = 0x0002,              // 终端心跳.
  kFillPacketRequest = 0x8003,              // 补传分包请求.
  kTerminalRegister = 0x0100,               // 终端注册.
  kTerminalRegisterResponse = 0x8100,       // 终端注册应答.
  kTerminalLogOut = 0x0003,                 // 终端注销.
  kTerminalAuthentication = 0x0102,         // 终端鉴权.
  kSetTerminalParameters = 0x8103,          // 设置终端参数.
  kGetTerminalParameters = 0x8104,          // 查询终端参数.
  kGetSpecificTerminalParameters = 0x8106,  // 查询指定终端参数.
  kGetTerminalParametersResponse = 0x0104,  // 查询终端参数应答.
  kTerminalControl = 0x8105,                //终端控制
  kTerminalUpgrade = 0x8108,                // 下发终端升级包.
  kTerminalUpgradeResultReport = 0x0108,    // 终端升级结果通知.
  kLocationReport = 0x0200,                 // 位置信息汇报.
  kGetLocationInformation = 0x8201,         // 位置信息查询.
  kGetLocationInformationResponse = 0x0201, // 位置信息查询应答.
  kLocationTrackingControl = 0x8202,        // 临时位置跟踪控制.

};

// // 所有应答命令.
// unsigned short kResponseCommand[] = {
//     kTerminalGeneralResponse,        //终端通用应答
//     kPlatformGeneralResponse,        //平台通用应答
//     kTerminalRegisterResponse,       //终端注册应答
//     kGetTerminalParametersResponse,  //查询终端参数应答
//     kGetLocationInformationResponse, //位置信息查询应答
// };

// // 所有终端解析命令.
// unsigned short kTerminalParserCMD[PARSER_NUM] = {
//     kPlatformGeneralResponse,       //平台通用应答
//     kFillPacketRequest,             // 补传分包请求.
//     kTerminalRegisterResponse,      // 终端注册应答.
//     kSetTerminalParameters,         // 设置终端参数.
//     kGetTerminalParameters,         // 查询终端参数.
//     kGetSpecificTerminalParameters, // 查询指定终端参数.
//     kTerminalControl,               //终端控制
//     kTerminalUpgrade,               // 下发终端升级包.
//     kGetLocationInformation,        // 位置信息查询.
// };

// // 所有终端数据打包命令.
// unsigned short kTerminalPackagerCMD[PACKAGER_NUM] = {
//     kTerminalGeneralResponse,        // 终端通用应答.
//     kTerminalHeartBeat,              // 终端心跳.
//     kTerminalRegister,               // 终端注册.
//     kTerminalLogOut,                 // 终端注销.
//     kTerminalAuthentication,         // 终端鉴权.
//     kGetTerminalParametersResponse,  // 查询终端参数应答.
//     kTerminalUpgradeResultReport,    // 终端升级结果通知.
//     kLocationReport,                 // 位置信息汇报.
//     kGetLocationInformationResponse, // 位置信息查询应答.
// };

// 车牌颜色.
enum VehiclePlateColor
{
  kVin = 0x0, // 车辆未上牌.
  kBlue,
  kYellow,
  kBlack,
  kWhite,
  kOther
};

// 通用应答结果.
enum GeneralResponseResult
{
  kSuccess = 0x0,             // 成功/确认.
  kFailure,                   // 失败.
  kMessageHasWrong,           // 消息有误.
  kNotSupport,                // 不支持.
  kAlarmHandlingConfirmation, // 报警处理确认, 仅平台应答使用.
};

// 注册应答结果.
enum RegisterResponseResult
{
  kRegisterSuccess = 0x0,       // 成功.
  kVehiclesHaveBeenRegistered,  // 车辆已被注册.
  kNoSuchVehicleInTheDatabase,  // 数据库中无该车辆.
  kTerminalHaveBeenRegistered,  // 终端已被注册.
  kNoSuchTerminalInTheDatabase, // 数据库中无该终端.
};

// 消息体属性.
union MsgBodyAttribute
{
  struct
  {
    // 消息体长度, 占用10bit.
    unsigned short msglen : 10;
    // 数据加密方式, 当此三位都为0, 表示消息体不加密,
    // 当第10位为1, 表示消息体经过RSA算法加密.
    unsigned short encrypt : 3;
    // 分包标记.
    unsigned short packet : 1;
    // 保留2位.
    unsigned short retain : 2;
  } bit;
  unsigned short u16val;
};

// 消息内容起始位置.
enum MsgBodyPos
{
  MSGBODY_NOPACKET_POS = 13, // 短消息体消息内容起始位置.SMS
  MSGBODY_PACKET_POS = 17,   // 长消息体消息内容起始位置.
};

// 转义相关标识.
enum ProtocolEscapeFlag
{
  PROTOCOL_SIGN = 0x7E,          // 标识位.
  PROTOCOL_ESCAPE = 0x7D,        // 转义标识.
  PROTOCOL_ESCAPE_SIGN = 0x02,   // 0x7E<-->0x7D后紧跟一个0x02.
  PROTOCOL_ESCAPE_ESCAPE = 0x01, // 0x7D<-->0x7D后紧跟一个0x01.
};

// 消息头.
struct MsgHead
{
  // 消息ID.
  unsigned short msg_id;
  // 消息体属性.
  union MsgBodyAttribute msgbody_attr;
  // 终端手机号.
  unsigned char *phone_num;
  // 消息流水号.
  unsigned short msg_flow_num;
  // 总包数, 分包情况下使用.
  unsigned short total_packet;
  // 当前包编号, 分包情况下使用.
  unsigned short packet_seq;
};

// 注册信息.
struct RegisterInfo
{
  // 省域ID.
  unsigned short province_id;

  // 市县域ID.
  unsigned short city_id;

  // 制造商ID, 固定5个字节.
  unsigned char manufacturer_id[5];

  // 终端型号, 固定20个字节, 位数不足后补0x00.
  unsigned char terminal_model[20];

  // 终端ID, 固定7个字节, 位数不足后补0x00.
  unsigned char terminal_id[7];

  // 车牌颜色, 0表示未上牌.
  unsigned char car_plate_color;

  // 车辆标识, 仅在上牌时使用.
  const char *car_plate_num;
};

// 升级类型.
enum kTerminalUpgradeType
{
  // 终端.
  kTerminal = 0x0,
  // 道路运输证 IC 卡读卡器
  kICCardReader = 0xc,
  // 北斗卫星定位模块.
  kGNSS = 0x34,
};

// 升级结果.
enum kTerminalUpgradeResultType
{
  // 终端升级成功.
  kTerminalUpgradeSuccess = 0x0,
  // 终端升级失败.
  kTerminalUpgradeFailed,
  // 终端升级取消.
  kTerminalUpgradeCancel
};

// 升级信息.
struct UpgradeInfo
{
  // 升级类型.
  unsigned char upgrade_type;
  // 升级结果.
  unsigned char upgrade_result;
  // 制造商ID, 固定5个字节.
  unsigned char manufacturer_id[5];
  // 升级版本号.
  const char *version_id;
  // 升级包总长度.
  unsigned int upgrade_data_total_len;
  // 升级数据包.
  char *upgrade_data;
};

// 补传分包信息.
struct FillPacket
{
  // 分包数据首包的消息流水号.
  unsigned short first_packet_msg_flow_num;
  // 需要重传包的ID.
  const char *packet_id;
};

// 协议格式、各消息ID等相关参数.
struct ProtocolParameter
{
  unsigned char respone_result;
  unsigned short respone_msg_id;
  unsigned short respone_flow_num;
  // 消息头.
  struct MsgHead msg_head;
  // 终端注册时需填充注册信息.
  struct RegisterInfo register_info;
  // 平台随机生成鉴权码.
  unsigned char *authentication_code;
  // 设置终端参数项.
  // TerminalParameters terminal_parameters;
  // 查询终端参数ID列表.
  // std::vector<unsigned  int> terminal_parameter_ids;
  // 位置上报时填充位置基本信息, 必须项.
  struct LocationBasicInformation location_info;
  // 位置上报时填充位置附加信息, 可选项.
  // LocationExtensions location_extension;
  // 临时位置跟踪控制信息.
  struct LocationTrackingControl location_tracking_control;

  // 升级信息.
  struct UpgradeInfo upgrade_info;
  // 补传分包信息.
  struct FillPacket fill_packet;

  //终端控制
  struct TerminalControl terminal_control;

  // // 保留字段.
  // std::vector<unsigned  char > retain;

  // 用于解析消息.
  struct
  {
    unsigned char respone_result;
    unsigned short respone_msg_id;
    unsigned short respone_flow_num;
    // 解析出的消息头.
    struct MsgHead msg_head;
    // 解析出的注册信息.
    struct RegisterInfo register_info;
    // 解析出的鉴权码.
    // 平台随机生成鉴权码.
    unsigned char *authentication_code;
    // 解析出的设置终端参数项.
    // TerminalParameters terminal_parameters;
    // 解析出的查询终端参数ID列表.
    // std::vector<unsigned  int> terminal_parameter_ids;
    // 解析出的位置基本信息.
    struct LocationBasicInformation location_info;
    // 解析出的位置附加信息.
    // LocationExtensions location_extension;
    // 解析出的临时位置跟踪控制信息.
    struct LocationTrackingControl location_tracking_control;

    // 解析出的升级信息.
    struct UpgradeInfo upgrade_info;
    // 解析出的补传分包信息.
    struct FillPacket fill_packet;

    //终端控制
    struct TerminalControl terminal_control;

    // 解析出的保留字段.
    // std::vector<unsigned  char > retain;

  } parse;
};


#endif // JT808_PROTOCOL_PARAMETER_H_
