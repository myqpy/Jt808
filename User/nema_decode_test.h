//#include "nmea/nmea.h"
#include "./sys/sys.h"
//nmeaPARSER parser;      
//nmeaINFO info;  

int nmea_decode_test(double *v_latitude, double *v_longitude, float *v_altitude, 
										float  *v_speed, float *v_bearing, unsigned char *v_timestamp,
										uint8_t new_parse);
uint8_t gpsData_Receive(uint8_t *new_parse);		
										