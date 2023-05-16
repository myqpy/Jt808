#include "./gpio/gpio.h"   
#include "./delay/delay.h"

void GPIO_Config(GPIO_InitTypeDef *GPIO_InitStructure)
{		

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOF, ENABLE);	 
	
	//PA-1 init
	GPIO_InitStructure->GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 
//	| GPIO_Pin_8
	;				 
	GPIO_InitStructure->GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_50MHz;		 
	GPIO_Init(GPIOA, GPIO_InitStructure);
			 



	//PC-1 / PC-5  init
	GPIO_InitStructure->GPIO_Pin = GPIO_Pin_1|GPIO_Pin_5;
	GPIO_InitStructure->GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_50MHz;		 
	GPIO_Init(GPIOC, GPIO_InitStructure);	
	
	//PF-7 init
	GPIO_InitStructure->GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure->GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_50MHz;		 
	GPIO_Init(GPIOF, GPIO_InitStructure);
	
	//PF-8 init
	GPIO_InitStructure->GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure->GPIO_Mode = GPIO_Mode_AIN; 		 
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_50MHz;		 
	GPIO_Init(GPIOF, GPIO_InitStructure);
	
	//PD-0 init
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD ,ENABLE);
	GPIO_InitStructure->GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure->GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOD, GPIO_InitStructure);
	
	//PD-2 init
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 
//	GPIO_Init(GPIOD, &GPIO_InitStructure);	  				 
//	GPIO_SetBits(GPIOD,GPIO_Pin_2); 						 
}

/*********************************************END OF FILE**********************/
