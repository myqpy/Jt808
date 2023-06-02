
#include "./sys/sys.h"    
//#include "terminal_parameter.h"




extern unsigned char vehicle_driver_info[][24];
extern unsigned char overTimeDriveRecord[][24];
extern unsigned char car_plate_color[][24];
extern unsigned char Chinese_car_plate[][24];
extern unsigned char pulseRatio[][24];
extern unsigned char car_plate_province[][24];
extern unsigned char car_IO_status[][24];
extern unsigned char changeLoadingStatus[][24];

void LCD_Clear(void);
int showMainMenu(void);
void displayEmpty(unsigned char page,unsigned char CaddrH,unsigned char CaddrL);
void displayChinese_16x16(unsigned char page,unsigned char CaddrH,unsigned char CaddrL,unsigned char arr[][24],unsigned char startWord, unsigned char endWord);

void displayWeekday(unsigned char page,unsigned char CaddrH,unsigned char CaddrL,int weekday);


void displayIcon(unsigned char page,unsigned char CaddrH,unsigned char CaddrL, unsigned char arr[2][16]);


int WeekYearday(int years, int months, int days);


















