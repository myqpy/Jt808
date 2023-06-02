#include "./key/key.h"
#include "./delay/delay.h"


//按键初始化函数 
//PA0.15和PC5 设置成输入
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);//使能GPIOE时钟
	/*使能PE2，3，4，5 按键 */
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
	GPIO_Init(GPIOE, &GPIO_InitStructure);

} 

u8 KEY_Scan(u8 mode)
{
    static u8 key_up=0;//按键按松开标志
    if(mode) key_up=1;  //支持连按
	
    if(key_up&&(KEY0==0||KEY1==0||KEY2==0||KEY3==0))
    {
        delay_ms(100);//去抖动
        key_up=0;
		
        if(KEY0==0)return KEY_confirmed_PRES;
		
		else if ((KEY1==0)&&(KEY2==0)) return KEY_up_down_PRES;
			
		else if(KEY2==0)return KEY_UpArrow_PRES;
		
        else if(KEY1==0)return KEY_DownArrow_PRES;
        
		else if(KEY3==0)return KEY_menu_PRES;
		
		
    } 
	else if(KEY0==1&&KEY1==1&&KEY2==1&&KEY3==1) key_up=1;
	
    return 0;// 无按键按下
}

