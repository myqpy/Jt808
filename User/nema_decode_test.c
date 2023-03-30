#include "./usart/usart.h"
#include "./gps/gps_config.h"
#include <string.h>


nmeaPARSER parser;      
nmeaINFO info;  

char bufTime[13]={0};

uint8_t gpsData_Receive(uint8_t *new_parse)
{
	if(GPS_HalfTransferEnd)     
	{
	
		nmea_parse(&parser, (const char*)&gps_rbuff[0], HALF_GPS_RBUFF_SIZE, &info);
		
		GPS_HalfTransferEnd = 0;  
		*new_parse = 1;         
	}
	else if(GPS_TransferEnd)   
	{
		
		nmea_parse(&parser, (const char*)&gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &info);
	 
		GPS_TransferEnd = 0;
		*new_parse = 1;
	}
	return *new_parse;
}



int nmea_decode_test(double *v_latitude, double *v_longitude, float *v_altitude, 
										float  *v_speed, float *v_bearing, unsigned char *v_timestamp,
											uint8_t new_parse)
{
		double deg_lat;//ת����[degree].[degree]��ʽ��γ��
		double deg_lon;//ת����[degree].[degree]��ʽ�ľ���

	
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

