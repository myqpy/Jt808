#include <stdint.h>
// 报警标志位
union AlarmBit
{
  struct
  {
    // bit_0 紧急报瞥触动报警开关后触发.收到应答后清零
    unsigned int sos : 1;
    // bit_1 超速报警.
    unsigned int overspeed : 1;
    // bit_2 疲劳驾驶.
    unsigned int fatigue : 1;
    // bit_3 危险预警.
    unsigned int early_warning : 1;
    // bit_4 GNSS模块发生故障.
    unsigned int gnss_fault : 1;
    // bit_5 GNSS天线未接或被剪断.
    unsigned int gnss_antenna_cut : 1;
    // bit_6 GNSS天线短路.
    unsigned int gnss_antenna_shortcircuit : 1;
    // bit_7 终端主电源欠压.
    unsigned int power_low : 1;
    // bit_8 终端主电源掉电.
    unsigned int power_cut : 1;
    // bit_9 终端LCD或显示器故障.
    unsigned int lcd_fault : 1;
    // bit_10 TTS模块故障.
    unsigned int tts_fault : 1;
    // bit_11 摄像头故障.
    unsigned int camera_fault : 1;

    // // OBD故障码.
    // unsigned  int  obd_fault_code : 1;
    // // 保留5位.
    // unsigned  int  retain1 : 5;

    // bit_12 道路运输证 IC 卡模块故障.
    unsigned int transport_license_IC_card_fault : 1;

    // bit_1 超速预警.
    unsigned int overspeed_notice : 1;
    // bit_2 疲劳驾驶预警.
    unsigned int fatigue_notice : 1;

    // 保留3位.
    unsigned int retain1 : 3;

    // bit_18 当天累计驾驶超时.
    unsigned int day_drive_overtime : 1;
    // bit_19 超时停车.
    unsigned int stop_driving_overtime : 1;
    // bit_20 进出区域.收到应答后清零
    unsigned int in_out_area : 1;
    // bit_21 进出路线.收到应答后清零
    unsigned int in_out_road : 1;
    // bit_22 路段行驶时间不足/过长.收到应答后清零
    unsigned int road_drive_time : 1;
    // bit_23 路线偏离报警.
    unsigned int road_deviate : 1;
    // bit_24 车辆VSS故障.
    unsigned int vss_fault : 1;
    // bit_25 车辆油量异常.
    unsigned int oil_fault : 1;
    // bit_26 车辆被盗(通过车辆防盗器).
    unsigned int car_alarm : 1;
    // bit_27 车辆非法点火.收到应答后清零
    unsigned int car_acc_alarm : 1;
    // bit_28 车辆非法位移.收到应答后清零
    unsigned int car_move : 1;
    // 碰撞侧翻报警.
    unsigned int collision : 1;
    // 保留2位.
    unsigned int retain2 : 2;
  } bit;
  unsigned int value;
};

// 状态位
union StatusBit
{
  struct
  {
    // ACC开关, 0:ACC关; 1:ACC开.
    unsigned int acc : 1;
    // 定位标志, 0:未定位; 1:定位.
    unsigned int positioning : 1;
    // 纬度半球, 0:北纬: 1:南纬.
    unsigned int sn_latitude : 1;
    // 经度半球, 0:东经; 1:西经.
    unsigned int ew_longitude : 1;
    // 0:运营状态; 1:停运状态.
    unsigned int operation : 1; // 0:运营状态; 1:停运状态.
    // 0:经纬度未经保密插件加密; 1:经纬度已经保密插件加密.
    unsigned int gps_encrypt : 1;
    // 保留2位.
    unsigned int retain1 : 2;
    // 00: 空车; 01: 半载; 10: 保留; 11: 满载.
    unsigned int trip_status : 2;
    // 0:车辆油路正常; 1:车辆油路断开.
    unsigned int oil_cut : 1;
    // 0:车辆电路正常; 1:车辆电路断开.
    unsigned int circuit_cut : 1;
    // 0:车门解锁; 1: 车门加锁.
    unsigned int door_lock : 1;
    // 0:门1 关; 1: 门1 开; (前门).
    unsigned int door1_status : 1;
    // 0:门2 关; 1: 门2 开; (中门).
    unsigned int door2_status : 1;
    // 0:门 3 关; 1: 门 3 开; (后门).
    unsigned int door3_status : 1;
    // 0:门 4 关; 1: 门 4 开; (驾驶席门).
    unsigned int door4_status : 1;
    // 0:门 5 关; 1: 门 5 开; (自定义).
    unsigned int door5_status : 1;
    // 0: 未使用 GPS 卫星进行定位; 1: 使用 GPS 卫星进行定位.
    unsigned int gps_en : 1;
    // 0: 未使用北斗卫星进行定位; 1: 使用北斗卫星进行定位.
    unsigned int beidou_en : 1;
    // 0: 未使用 GLONASS 卫星进行定位; 1: 使用 GLONASS 卫星进行定位.
    unsigned int glonass_en : 1;
    // 0: 未使用 Galileo 卫星进行定位; 1: 使用 Galileo 卫星进行定位.
    unsigned int galileo_en : 1;
    // 保留10位.
    unsigned int retain2 : 10;
  } bit;
  unsigned int value;
};

// 位置基本信息数据.
struct LocationBasicInformation
{
  // 报警标志 4B
  union AlarmBit alarm;
  // 状态位定义 4B
  union StatusBit status;
  // 纬度(以度为单位的纬度值乘以10的6次方, 精确到百万分之一度) 4B
  unsigned int latitude;
  // 经度(以度为单位的纬度值乘以10的6次方, 精确到百万分之一度) 4B
  unsigned int longitude;
  // 海拔高度, 单位为米(m) 2B
  unsigned short altitude;
  // 速度 1/10km/h 2B
  unsigned short speed;
  // 方向 0-359,正北为0, 顺时针 2B
  unsigned short bearing;
  // 时间, "YYMMDDhhmmss"(GMT+8时间, 本标准之后涉及的时间均采用此时区).12B
  // std::string time;
  const char *time;
};

// 扩展车辆信号状态位
union ExtendedVehicleSignalBit
{
  struct
  {
    // 近光灯信号
    unsigned int near_lamp : 1;
    // 远光灯信号
    unsigned int farl_amp : 1;
    // 右转向灯信号
    unsigned int right_turn_lamp : 1;
    // 左转向灯信号
    unsigned int left_turn_lamp : 1;
    // 制动信号
    unsigned int breaking : 1;
    // 倒档信号
    unsigned int reversing : 1;
    // 雾灯信号
    unsigned int fog_lamp : 1;
    // 示廓灯
    unsigned int outline_lamp : 1;
    // 喇叭信号
    unsigned int horn : 1;
    // 空调状态
    unsigned int air_conditioner : 1;
    // 空挡信号
    unsigned int neutral : 1;
    // 缓速器工作
    unsigned int retarder : 1;
    // ABS 工作
    unsigned int abs : 1;
    // 加热器工作
    unsigned int heater : 1;
    // 离合器状态
    unsigned int clutch : 1;
    // 保留17位.
    unsigned int retain : 17;
  } bit;
  unsigned int value;
};

// 位置信息上报附加项ID.
enum LocationExtensionId
{
  // 里程, 1/10km, 对应车上里程表读数, DWORD
  kMileage = 0x01,
  // 油量, 1/10L, 对应车上油量表读数, WORD
  kOilMass = 0x02,
  // 行驶记录功能获取的速度, 1/10km/h, WORD
  kTachographSpeed = 0x03,
  // 需要人工确认报警事件的 ID, 从 1 开始计数, WORD
  kAlarmCount = 0x04,
  // 超速报警附加信息, BYTE or BYTE+DWORD
  kOverSpeedAlarm = 0x11,
  // 进出区域/路线报警附加信息, BYTE+DWORD+BYTE
  kAccessAreaAlarm = 0x12,
  // 路段行驶时间不足/过长报警附加信息, DWORD+WORD+BYTE
  kDrivingTimeAlarm = 0x13,
  // 扩展车辆信号状态位, DWORD
  kVehicleSignalStatus = 0x25,
  // IO 状态位, WORD
  kIoStatus = 0x2A,
  // 模拟量, DWORD
  kAnalogQuantity = 0x2B,
  // 无线通信网络信号强度, BYTE
  kNetworkQuantity = 0x30,
  // GNSS 定位卫星数, BYTE
  kGnssSatellites = 0x31,
  // 后续自定义信息长度, BYTE
  kCustomInformationLength = 0xE0,
  // 定位解状态, BYTE
  kPositioningStatus = 0xEE,

  //称重信息ID
  kWeightInfo = 0xF3,

  // 视频报警上报
  kAlarm_video = 0x14,                   // 视频相关报警，附加信息长度 4B
  kAlarm_video_Signal_Lost = 0x15,       // 视频信号丢失报警状态，附加信息长度 4B
  kAlarm_video_Signal_Occlusion = 0x16,  // 视频信号遮挡报警状态，附加信息长度 4B
  kAlarm_Memory_Bad = 0x17,              // 存储器故障报警状态，附加信息长度 2B
  kAlarm_Abnormal_Drive_Behavior = 0x18, // 异常驾驶行为报警详细描述，附加信息长度 2B

};

// 位置信息附加项存储定义: key: itemid, value: itemvalue.
// using LocationExtensions = std::map<unsigned  char , std::vector<unsigned  char >>;

//  超速报警附加信息位置类型, BYTE.
enum kOverSpeedAlarmLocationType
{
  // 0:无特定位置.
  kOverSpeedAlarmNoSpecificLocation = 0x0,
  // 1:圆形区域.
  kOverSpeedAlarmCircularArea,
  // 2:矩形区域.
  kOverSpeedAlarmRectangleArea,
  // 3:多边形区域.
  kOverSpeedAlarmPolygonArea,
  // 4:路段.
  kOverSpeedAlarmRoadSection
};

// 进出区域/路线报警附加信息消息体位置类型, BYTE.
enum kAccessAreaAlarmLocationType
{
  // 1：圆形区域.
  kAccessAreaAlarmCircularArea,
  // 2：矩形区域.
  kAccessAreaAlarmRectangleArea,
  // 3：多边形区域.
  kAccessAreaAlarmPolygonArea,
  // 4：路线.
  kOverSpeedAlarmRoute
};

// 进出区域/路线报警附加信息消息体方向类型, BYTE.
enum kAccessAreaAlarmDirectionType
{
  // 进入区域.
  kAccessAreaAlarmInArea = 0x0,
  // 离开区域.
  kAccessAreaAlarmOutArea
};

// IO 状态位
union IoStatusBit
{
  struct
  {
    // 深度休眠状态
    unsigned short deep_rmancy : 1;
    // 休眠状态
    unsigned short dormancy : 1;
    // 保留14位.
    unsigned short retain : 14;
  } bit;
  unsigned short value;
};

// 临时位置跟踪控制信息.
struct LocationTrackingControl
{
  // 时间间隔.
  unsigned short interval;
  // 单位为秒(s), 有效时间.
  unsigned int tracking_time;
};
/*
  // 设置超速报警附加信息消息体.
  int SetOverSpeedAlarmBody(unsigned  char  const &location_type,
                            unsigned  int  const &area_route_id,
                            std::vector<unsigned  char > *out);

  // 获得超速报警报警附加信息消息体.
  int GetOverSpeedAlarmBody(std::vector<unsigned  char > const &out,
                            unsigned  char  *location_type,
                            unsigned  int  *area_route_id);

  // 设置进出区域/路线报警附加信息消息体.
  int SetAccessAreaAlarmBody(unsigned  char  const &location_type,
                             unsigned  int  const &area_route_id,
                             unsigned  char  const &direction,
                             std::vector<unsigned  char > *out);

  // 获得进出区域/路线报警附加信息消息体.
  int GetAccessAreaAlarmBody(std::vector<unsigned  char > const &out,
                             unsigned  char  *location_type,
                             unsigned  int  *area_route_id,
                             unsigned  char  *direction);
*/

// void initGPSInfo(struct ProtocolParameter *para);
// void initGPSInfo(struct LocationBasicInformation *para);
// void UpdateLocation(double const latitude, double const longitude, float const altitude, float const speed,
//                     float const bearing, unsigned char *timestamp);

// void initGPSInfo(struct ProtocolParameter *para, unsigned int v_alarm_value,
//                  unsigned int v_status_value, double const v_latitude,
//                  double const v_longitude, float const v_altitude,
//                  float const v_speed, float const v_bearing,
//                  unsigned char *v_timestamp);

