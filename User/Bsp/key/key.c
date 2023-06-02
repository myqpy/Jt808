#include "./key/key.h"
#include "./delay/delay.h"


//������ʼ������ 
//PA0.15��PC5 ���ó�����
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);//ʹ��GPIOEʱ��
	/*ʹ��PE2��3��4��5 ���� */
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
	GPIO_Init(GPIOE, &GPIO_InitStructure);

} 

u8 KEY_Scan(u8 mode)
{
    static u8 key_up=0;//�������ɿ���־
    if(mode) key_up=1;  //֧������
	
    if(key_up&&(KEY0==0||KEY1==0||KEY2==0||KEY3==0))
    {
        delay_ms(100);//ȥ����
        key_up=0;
		
        if(KEY0==0)return KEY_confirmed_PRES;
		
		else if ((KEY1==0)&&(KEY2==0)) return KEY_up_down_PRES;
			
		else if(KEY2==0)return KEY_UpArrow_PRES;
		
        else if(KEY1==0)return KEY_DownArrow_PRES;
        
		else if(KEY3==0)return KEY_menu_PRES;
		
		
    } 
	else if(KEY0==1&&KEY1==1&&KEY2==1&&KEY3==1) key_up=1;
	
    return 0;// �ް�������
}

