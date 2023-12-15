/**
  ******************************************************************************
  * @file    nmea_decode_test.c
  * @author  WJSHM
  * @version V1.0
  * @date    2016-07-xx
  * @brief   ����NEMA�����
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

char bufTime[13]={0};
int nmea_decode_test(double *v_latitude, double *v_longitude, float *v_altitude, 
										float  *v_speed, float *v_bearing, unsigned char *v_timestamp,
											nmeaINFO info, uint8_t new_parse)
{
		double deg_lat;//ת����[degree].[degree]��ʽ��γ��
		double deg_lon;//ת����[degree].[degree]��ʽ�ľ���
//	  long m_lat=0;
//		long m_lon=0;
//		int m_alt=0;
//		int m_speed=0;
//		int m_bearing=0;
	
    nmeaTIME beiJingTime;    //����ʱ�� 

    /* �����������������Ϣ�ĺ��� */
    nmea_property()->trace_func = &trace;
    nmea_property()->error_func = &error;
    nmea_property()->info_func = &gps_info;

		if(new_parse )                //���µĽ�����Ϣ   
		{    
			/* �Խ�����ʱ�����ת����ת���ɱ���ʱ�� */
			GMTconvert(&info.utc,&beiJingTime,8,1);
					
			//info.lat lon�еĸ�ʽΪ[degree][min].[sec/60]��ʹ�����º���ת����[degree].[degree]��ʽ
			deg_lat = nmea_ndeg2degree(info.lat);
			deg_lon = nmea_ndeg2degree(info.lon);

//			m_lat=(long)(deg_lat*(1e6));
//			m_lon=(long)(deg_lon*1000000);
//			m_alt=(int)(info.elv);
//			m_speed=(int)(info.speed*10);
//			m_bearing=(int)(info.direction);
			
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

