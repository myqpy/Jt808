#ifndef __KEY_H
#define __KEY_H	 
#include "./sys/sys.h"
 

#define KEY0	GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)//读取按键0
#define KEY1	GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)//读取按键1
#define KEY2   	GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)//读取按键2 
#define KEY3   	GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5)//读取按键3

#define KEY_confirmed_PRES	1		//KEY0  
#define KEY_DownArrow_PRES	2		//KEY1 
#define KEY_UpArrow_PRES	3		//KEY2  
#define KEY_menu_PRES	4			//KEY3  
#define KEY_up_down_PRES 5			//key ↑ + ↓

void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8 mode);  	//按键扫描函数					    
#endif
