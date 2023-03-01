/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  WJSHM
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   led应用函数接口
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
  
#include "bsp_led.h"   

 /**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
void LED_GPIO_Config(void)
{		

	
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD, ENABLE);	 //??PA,PD????

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //LED0-->PA.8 ????
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //????
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO????50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //?????????GPIOA.8
	GPIO_SetBits(GPIOA,GPIO_Pin_8);						 //PA.8 ???

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 //LED1-->PD.2 ????, ????
	GPIO_Init(GPIOD, &GPIO_InitStructure);	  				 //???? ,IO????50MHz
	GPIO_SetBits(GPIOD,GPIO_Pin_2); 						 //PD.2 ??? 
}	

/*********************************************END OF FILE**********************/
