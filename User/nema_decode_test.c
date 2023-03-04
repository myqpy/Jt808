#include "stm32f10x.h"
#include "./usart/usart.h"
#include "./gps/gps_config.h"
#include "ff.h"
#include "nmea/nmea.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

nmeaPARSER parser;      //解码时使用的数据结构  
nmeaINFO info;          //GPS解码后得到的信息
nmea_parser_init(&parser);
char bufTime[13]={0};

uint8_t gpsData_Receive(uint8_t *new_parse)
{
	if(GPS_HalfTransferEnd)     /* 接收到GPS_RBUFF_SIZE一半的数据 */
	{
		/* 进行nmea格式解码 */
		nmea_parse(&parser, (const char*)&gps_rbuff[0], HALF_GPS_RBUFF_SIZE, &info);
		
		GPS_HalfTransferEnd = 0;   //清空标志位
		*new_parse = 1;             //设置解码消息标志
	}
	else if(GPS_TransferEnd)    /* 接收到另一半数据 */
	{
		/* 进行nmea格式解码 */
		nmea_parse(&parser, (const char*)&gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &info);
	 
		GPS_TransferEnd = 0;
		*new_parse = 1;
	}
	return 0;
}

int nmea_decode_test(double *v_latitude, double *v_longitude, float *v_altitude, 
										float  *v_speed, float *v_bearing, unsigned char *v_timestamp,
										//nmeaINFO info, 
										uint8_t new_parse)
{
		double deg_lat;//转换成[degree].[degree]格式的纬度
		double deg_lon;//转换成[degree].[degree]格式的经度
	
    nmeaTIME beiJingTime;    //北京时间 

    /* 设置用于输出调试信息的函数 */
    nmea_property()->trace_func = &trace;
    nmea_property()->error_func = &error;
    nmea_property()->info_func = &gps_info;

		if(new_parse )                //有新的解码消息   
		{    
			/* 对解码后的时间进行转换，转换成北京时间 */
			GMTconvert(&info.utc,&beiJingTime,8,1);
					
			//info.lat lon中的格式为[degree][min].[sec/60]，使用以下函数转换成[degree].[degree]格式
			deg_lat = nmea_ndeg2degree(info.lat);
			deg_lon = nmea_ndeg2degree(info.lon);
			
			if(deg_lat!=0)
			{
				*v_latitude = deg_lat;
				*v_longitude = deg_lon;
				*v_altitude = info.elv;
				*v_speed = info.speed;
				*v_bearing = info.direction;
				
				sprintf(bufTime,"%02d%02d%02d%02d%02d%02d \r\n",((beiJingTime.year+1900)%2000), beiJingTime.mon,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
				memset(v_timestamp, 0, 13);
				memcpy(v_timestamp, bufTime, 12);
				memset(bufTime,0,12);
				printf("v_timestamp = %s \r\n",v_timestamp);
//				sprintf(v_timestamp,"%02d%02d%02d%02d%02d%02d \r\n",((beiJingTime.year+1900)%2000), beiJingTime.mon,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);

				return 1;
			}

			memset(bufTime,0,12);
				
		}
		
//		nmea_parser_destroy(&parser);
		return 0;
}







/**************************************************end of file****************************************/

