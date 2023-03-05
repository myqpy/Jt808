#include "./gpio/gpio.h"   
#include "./delay/delay.h"

void LED_GPIO_Config(GPIO_InitTypeDef *GPIO_InitStructure)
{		

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD, ENABLE);	 //??PA,PD????

	//PA-8 init
	GPIO_InitStructure->GPIO_Pin = GPIO_Pin_8;				 
	GPIO_InitStructure->GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_50MHz;		 
	GPIO_Init(GPIOA, GPIO_InitStructure);					 

	//PC-1 init
	GPIO_InitStructure->GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure->GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_50MHz;		 
	GPIO_Init(GPIOC, GPIO_InitStructure);	
	GPIO_SetBits(GPIOC, GPIO_Pin_1);	

	//PD-2 init
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 
//	GPIO_Init(GPIOD, &GPIO_InitStructure);	  				 
//	GPIO_SetBits(GPIOD,GPIO_Pin_2); 						 
}

void PC1_Config(GPIO_InitTypeDef *GPIO_InitStructure)
{		

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);			 
	//PC-1 init
	GPIO_InitStructure->GPIO_Pin = GPIO_Pin_1|GPIO_Pin_5;
	GPIO_InitStructure->GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_50MHz;		 
	GPIO_Init(GPIOC, GPIO_InitStructure);	
	GPIO_SetBits(GPIOC, GPIO_Pin_1);
	delay_ms(1000);
	
	GPIO_SetBits(GPIOC, GPIO_Pin_5);	
	delay_ms(500);
	GPIO_ResetBits(GPIOC, GPIO_Pin_5);		
}	

/*********************************************END OF FILE**********************/
