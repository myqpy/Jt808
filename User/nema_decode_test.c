/**
  ******************************************************************************
  * @file    nmea_decode_test.c
  * @author  WJSHM
  * @version V1.0
  * @date    2016-07-xx
  * @brief   测试NEMA解码库
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */ 
  
#include "stm32f10x.h"
#include "./usart/usart.h"
#include "./gps/gps_config.h"
#include "ff.h"
#include "nmea/nmea.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char bufTime[12]={0};
int nmea_decode_test(double *v_latitude, double *v_longitude, float *v_altitude, 
										float  *v_speed, float *v_bearing, unsigned char *v_timestamp,
											nmeaINFO info, uint8_t new_parse)
{
		double deg_lat;//转换成[degree].[degree]格式的纬度
		double deg_lon;//转换成[degree].[degree]格式的经度
	  long m_lat=0;
		long m_lon=0;
		int m_alt=0;
		int m_speed=0;
		int m_bearing=0;
	
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

			m_lat=(long)(deg_lat*(1e6));
			m_lon=(long)(deg_lon*1000000);
			m_alt=(int)(info.elv);
			m_speed=(int)(info.speed*10);
			m_bearing=(int)(info.direction);
			
			if(m_lat!=0)
			{
				*v_latitude = deg_lat;
				*v_longitude = deg_lon;
				*v_altitude = info.elv;
				*v_speed = info.speed;
				*v_bearing = info.direction;

				return 1;
			}

			memset(bufTime,0,12);
				
		}
		
//		nmea_parser_destroy(&parser);
		return 0;
}







/**************************************************end of file****************************************/

