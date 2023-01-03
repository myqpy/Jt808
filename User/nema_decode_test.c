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
//#include "ff.h"
#include "nmea/nmea.h"
#include <stdio.h>
#include <stdlib.h>

char bufTime[12]={0};
int nmea_decode_test(void)
{
		double deg_lat;//转换成[degree].[degree]格式的纬度
		double deg_lon;//转换成[degree].[degree]格式的经度
	  long m_lat=0;
		long m_lon;
		int m_alt;
		int m_speed;
		int m_bearing;
	
    nmeaINFO info;          //GPS解码后得到的信息
    nmeaPARSER parser;      //解码时使用的数据结构  
    uint8_t new_parse=0;    //是否有新的解码数据标志
  
    nmeaTIME beiJingTime;    //北京时间 

    /* 设置用于输出调试信息的函数 */
    nmea_property()->trace_func = &trace;
    nmea_property()->error_func = &error;
    nmea_property()->info_func = &gps_info;

    /* 初始化GPS数据结构 */
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);
	  //char bufTime[64];
	

    while(1)
    {
      if(GPS_HalfTransferEnd)     /* 接收到GPS_RBUFF_SIZE一半的数据 */
      {
        /* 进行nmea格式解码 */
        nmea_parse(&parser, (const char*)&gps_rbuff[0], HALF_GPS_RBUFF_SIZE, &info);
        
        GPS_HalfTransferEnd = 0;   //清空标志位
        new_parse = 1;             //设置解码消息标志 
      }
      else if(GPS_TransferEnd)    /* 接收到另一半数据 */
      {

        nmea_parse(&parser, (const char*)&gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &info);
       
        GPS_TransferEnd = 0;
        new_parse =1;
      }
      
      if(new_parse )                //有新的解码消息   
      {    
        /* 对解码后的时间进行转换，转换成北京时间 */
        GMTconvert(&info.utc,&beiJingTime,8,1);
        
        /* 输出解码得到的信息 */
				printf("\r\n时间%02d-%02d-%02d  %02d:%02d:%02d\r\n", ((beiJingTime.year+1900)%2000), beiJingTime.mon,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
				//printf("\r\n时间%d-%02d-%02d,%d:%d:%d\r\n", beiJingTime.year+1900, beiJingTime.mon,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
				
				//info.lat lon中的格式为[degree][min].[sec/60]，使用以下函数转换成[degree].[degree]格式
				deg_lat = nmea_ndeg2degree(info.lat);
				deg_lon = nmea_ndeg2degree(info.lon);
				
				printf("\r\n纬度：%f,经度%f\r\n",deg_lat,deg_lon);
        printf("\r\n海拔高度：%f 米 ", info.elv);
        printf("\r\n速度：%f km/h ", info.speed);
        printf("\r\n航向：%f 度", info.direction);
				
				printf("\r\n正在使用的GPS卫星：%d,可见GPS卫星：%d",info.satinfo.inuse,info.satinfo.inview);

				printf("\r\n正在使用的北斗卫星：%d,可见北斗卫星：%d",info.BDsatinfo.inuse,info.BDsatinfo.inview);
				printf("\r\nPDOP：%f,HDOP：%f，VDOP：%f\n\n",info.PDOP,info.HDOP,info.VDOP);
				
				m_lat=(long)(deg_lat*(1e6));
				m_lon=(long)(deg_lon*1000000);
				m_alt=(int)(info.elv);
				m_speed=(int)(info.speed*10);
				m_bearing=(int)(info.direction);
				
				if(m_lat!=0)
				{
					printf("纬度：%ld\n",m_lat);
					printf("经度: %ld\n",m_lon);
					printf("海拔: %d 米 \n", m_alt);
					printf("速度：%d km/h \n", m_speed);
					printf("航向：%d 度\n", m_bearing);
					printf("时间: %02d-%02d-%02d  %02d:%02d:%02d\n", ((beiJingTime.year+1900)%2000), beiJingTime.mon,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
					
					//char *bufTime=(char *)malloc(64);
					sprintf(bufTime,"%02d%02d%02d%02d%02d%02d",((beiJingTime.year+1900)%2000), beiJingTime.mon,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
					printf("bufTime: %s\n",bufTime);
					//free(bufTime);
				}

        new_parse = 0;
				memset(bufTime,0,12);
				break;
      }
	
	}

    /* 释放GPS数据结构 */
     nmea_parser_destroy(&parser);

    
		return 0;
}







/**************************************************end of file****************************************/

