
#include "location_report.h"

#include <string.h>

#include "util.h"

// double const v_latitude = 23.123456;
// double const v_longitude = 123.123456;
// float const v_altitude=10;
// float const v_speed=10;
// float const v_bearing=100;
// unsigned char *v_timestamp = "211221213045";

// void initGPSInfo(struct ProtocolParameter *para, unsigned int v_alarm_value,
//                  unsigned int v_status_value, double const v_latitude,
//                  double const v_longitude, float const v_altitude,
//                  float const v_speed, float const v_bearing,
//                  unsigned char *v_timestamp)
// {
//   printf("\n\r[initGPSInfo] OK !\n");
//   //报警标志
//   para->location_info.alarm.value = v_alarm_value;
//   printf("para->alarm.value = %d\n", para->location_info.alarm.value);
//   //状态
//   para->location_info.status.value = v_status_value;
//   printf("para->status.value = %d\n", para->location_info.status.value);

//   // if (speed >= 10) //默认车速大于等于10公里时为正常行驶状态
//   // {
//   //   isCarMoving.store(true);
//   // }
//   // else
//   // {
//   //   isCarMoving.store(false);
//   // }
//   para->location_info.latitude = v_latitude * 1e6;
//   printf("para->latitude = %d\n", para->location_info.latitude);

//   para->location_info.longitude = v_longitude * 1e6;
//   printf("para->longitude = %d\n", para->location_info.longitude);

//   para->location_info.altitude = v_altitude;
//   printf("para->altitude = %d\n", para->location_info.altitude);

//   para->location_info.speed = v_speed * 10;
//   printf("para->speed = %d\n", para->location_info.speed);

//   para->location_info.bearing = v_bearing;
//   printf("para->bearing = %d\n", para->location_info.bearing);

//   para->location_info.time = v_timestamp;
//   printf("para->time = %s\n", para->location_info.time);
// }

// void UpdateLocation(double const latitude, double const longitude,
//                     float const altitude, float const speed,
//                     float const bearing, unsigned char *timestamp)
// {
//   // if (speed >= 10) //默认车速大于等于10公里时为正常行驶状态
//   // {
//   //   isCarMoving.store(true);
//   // }
//   // else
//   // {
//   //   isCarMoving.store(false);
//   // }
//   para->location_info.latitude = static_cast<uint32_t>(latitude * 1e6);
//   para->location_info.longitude = static_cast<uint32_t>(longitude * 1e6);
//   para->location_info.altitude = static_cast<uint16_t>(altitude);
//   para->location_info.speed = static_cast<uint16_t>(speed * 10);
//   para->location_info.bearing = static_cast<uint16_t>(bearing);
//   para->location_info.time.assign(timestamp.begin(), timestamp.end());

//   // spdlog::info("[{}] [{}] 更新上报位置信息 ", libjt808::getCurrentFileName(__FILE__), __LINE__);
// }
