#include "./ec20/ec20.h"
#include "./usart/usart.h"		
#include "./delay/delay.h"	 
#include "./key/key.h"	 	 	 	 	 
#include "string.h"    
#include "./usart2/usart2.h" 

u8 Scan_Wtime = 0;//����ɨ����Ҫ��ʱ��
u8 BT_Scan_mode=0;//����ɨ���豸ģʽ��־

//usmart֧�ֲ��� 
//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART2_RX_STA;
//     1,����USART2_RX_STA;
void sim_at_response(u8 mode)
{
    if(USART2_RX_STA&0X8000)		//���յ�һ��������
    {
        USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//��ӽ�����
        printf("%s",USART2_RX_BUF);	//���͵�����
        if(mode)USART2_RX_STA=0;
    }
}
//////////////////////////////////////////////////////////////////////////////////
//ATK-ec20 �������(���Ų��ԡ����Ų��ԡ�GPRS���ԡ���������)���ô���

//ec20���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//����,�ڴ�Ӧ������λ��(str��λ��)
u8* ec20_check_cmd(u8 *str)
{
    char *strx=0;
    if(USART2_RX_STA&0X8000)		//���յ�һ��������
    {
        USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//��ӽ�����
        strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
    }
    return (u8*)strx;
}
//��ec20��������
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 ec20_send_cmd(u8 *cmd,u8 *Re1,u8 *Re2,u8 *Re3,u16 waittime)
{
    u8 res=0;
    USART2_RX_STA=0;
    if((u32)cmd<=0XFF)
    {
        while(DMA1_Channel7->CNDTR!=0);	//�ȴ�ͨ��7�������
        USART2->DR=(u32)cmd;
    }else
    {
        u2_printf("%s\r\n",cmd);//��������
        printf("%s\r\n",cmd);//��������
    }

    if(waittime==1100)//11s����ش�������(����ɨ��ģʽ)
    {
        Scan_Wtime = 11;  //��Ҫ��ʱ��ʱ��
        TIM4_SetARR(9999);//����1S��ʱ�ж�
    }


    if((Re1&&waittime)||(Re3&&waittime)||(Re2&&waittime))		//��Ҫ�ȴ�Ӧ��
    {
        while(--waittime)	//�ȴ�����ʱ
        {
            if(BT_Scan_mode)//����ɨ��ģʽ
            {
                res=KEY_Scan(0);//������һ��
//                if(res==WKUP_PRES)return 2;
            }
            delay_ms(10);
            if(USART2_RX_STA&0X8000)//���յ��ڴ���Ӧ����
            {
                printf((const char*)USART2_RX_BUF,"\r\n");
                if (ec20_check_cmd(Re1))
                {
                    return 1;
                }
                if (ec20_check_cmd(Re2))
                {
                    return 2;
                }
                if (ec20_check_cmd(Re3))
                {
                    return 3;
                }
                USART2_RX_STA=0;
            }
        }
    }
    return res;
} 

//����ec20�������ݣ���������ģʽ��ʹ�ã�
//request:�ڴ����������ַ���
//waittimg:�ȴ�ʱ��(��λ��10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 ec20_wait_request(u8 *request ,u16 waittime)
{
    u8 res = 1;
//    u8 key;
    if(request && waittime)
    {
        while(--waittime)
        {
//            key=KEY_Scan(0);
//            if(key==WKUP_PRES) return 2;//������һ��
//            delay_ms(10);
            if(USART2_RX_STA &0x8000)//���յ��ڴ���Ӧ����
            {
                if(ec20_check_cmd(request)) break;//�õ���Ч����
                USART2_RX_STA=0;
            }
        }
        if(waittime==0)res=0;
    }
    return res;
}

//��1���ַ�ת��Ϊ16��������
//chr:�ַ�,0~9/A~F/a~F
//����ֵ:chr��Ӧ��16������ֵ
u8 ec20_chr2hex(u8 chr)
{
    if(chr>='0'&&chr<='9')return chr-'0';
    if(chr>='A'&&chr<='F')return (chr-'A'+10);
    if(chr>='a'&&chr<='f')return (chr-'a'+10);
    return 0;
}
//��1��16��������ת��Ϊ�ַ�
//hex:16��������,0~15;
//����ֵ:�ַ�
u8 ec20_hex2chr(u8 hex)
{
    if(hex<=9)return hex+'0';
    if(hex>=10&&hex<=15)return (hex-10+'A');
    return '0';
}


/*

u8 ec20_call_test(void)
{
	u8 key;
	u16 lenx;
	u8 callbuf[20]; 
	u8 pohnenumlen=0;	//���볤��,���15���� 
	u8 *p,*p1,*p2;
	u8 oldmode=0;
	u8 cmode=0;	//ģʽ
				//0:�ȴ�����
				//1:������
	            //2:ͨ����
				//3:���յ����� 

        if(ec20_send_cmd("AT+CLIP=1","OK",200))return 1;	//����������ʾ
        if(ec20_send_cmd("AT+COLP=1","OK",200))return 2;	//���ñ��к�����ʾ
 	p1=mymalloc(20);								//����20ֱ�����ڴ�ź���
	if(p1==NULL)return 2;	

	while(1)
	{
		delay_ms(10);
		if(USART2_RX_STA&0X8000)		//���յ�����
		{
			sim_at_response(0);
			if(cmode==1||cmode==2)
			{
                                if(cmode==1)if(ec20_check_cmd("+COLP:"))cmode=2;	//���ųɹ�
                                if(ec20_check_cmd("NO CARRIER"))cmode=0;	//����ʧ��
                                if(ec20_check_cmd("NO ANSWER"))cmode=0;	//����ʧ��
                                if(ec20_check_cmd("ERROR"))cmode=0;		//����ʧ��
			}
                        if(ec20_check_cmd("+CLIP:"))//���յ�����
			{
				cmode=3;
                                p=ec20_check_cmd("+CLIP:");
				p+=8;
				p2=(u8*)strstr((const char *)p,"\"");
				p2[0]=0;//��ӽ����� 
				strcpy((char*)p1,(char*)p);
			}
			USART2_RX_STA=0;
		}
                key=ec20_get_keynum(0,180);
		if(key)
		{ 
			if(key<13)
			{
				if(cmode==0&&pohnenumlen<15)
				{ 
					callbuf[pohnenumlen++]=kbd_tbl[key-1][0];
					u2_printf("AT+CLDTMF=2,\"%c\"\r\n",kbd_tbl[key-1][0]);
                    delay_ms(20);//��ʱ55
				    u2_printf("AT+CTTS=2,\"%c\"\r\n",kbd_tbl[key-1][0]); //TTS����									
				}else if(cmode==2)//ͨ����
				{ 
					u2_printf("AT+CLDTMF=2,\"%c\"\r\n",kbd_tbl[key-1][0]);
					delay_ms(100);
					u2_printf("AT+VTS=%c\r\n",kbd_tbl[key-1][0]); 
					LCD_ShowChar(40+56,90,kbd_tbl[key-1][0],16,0);
				}
			}else
			{
				if(key==13)if(pohnenumlen&&cmode==0)pohnenumlen--;//ɾ��
				if(key==14)//ִ�в���
				{
					if(cmode==0)//����ģʽ
					{
						callbuf[pohnenumlen]=0;			//����������� 
						u2_printf("ATD%s;\r\n",callbuf);//����
						delay_ms(10);		        	//�ȴ�10ms  
						cmode=1;						//������ģʽ
					}else 
					{
                                                ec20_send_cmd("ATH","OK",100);//�һ�
                                                ec20_send_cmd("ATH","OK",100);//�һ�
						cmode=0;
					}
				}
				if(key==15)
				{
					if(cmode==3)//���յ�����
					{
                                                ec20_send_cmd("ATA","OK",200);//����Ӧ��ָ��
						Show_Str(40+56,70,200,16,callbuf,16,0);
						cmode=2;
					}else
					{ 
                                                ec20_send_cmd("ATH",0,0);//������û����ͨ��,������ͨ��
						break;//�˳�ѭ��
					}
				}
			} 
			if(cmode==0)//ֻ���ڵȴ�����ģʽ��Ч
			{
				callbuf[pohnenumlen]=0; 
				LCD_Fill(40+56,70,239,70+16,WHITE);
				Show_Str(40+56,70,200,16,callbuf,16,0);  	
			}				
		}
		if(oldmode!=cmode)//ģʽ�仯��
		{
			switch(cmode)
			{
				case 0: 
					kbd_fn_tbl[0]="����";
					kbd_fn_tbl[1]="����"; 
					POINT_COLOR=RED;
					Show_Str(40,70,200,16,"�벦��:",16,0);  
					LCD_Fill(40+56,70,239,70+16,WHITE);
					if(pohnenumlen)
					{
						POINT_COLOR=BLUE;
						Show_Str(40+56,70,200,16,callbuf,16,0);
					}
					break;
				case 1:
					POINT_COLOR=RED;
					Show_Str(40,70,200,16,"������:",16,0); 
					pohnenumlen=0;
				case 2:
					POINT_COLOR=RED;
					if(cmode==2)Show_Str(40,70,200,16,"ͨ����:",16,0); 
					kbd_fn_tbl[0]="�Ҷ�";
					kbd_fn_tbl[1]="����"; 	
					break;
				case 3:
					POINT_COLOR=RED;
					Show_Str(40,70,200,16,"������:",16,0); 
					POINT_COLOR=BLUE;
					Show_Str(40+56,70,200,16,p1,16,0); 
					kbd_fn_tbl[0]="�Ҷ�";
					kbd_fn_tbl[1]="����"; 
					break;				
			}
			if(cmode==2)Show_Str(40,90,200,16,"DTMF��:",16,0);	//ͨ����,����ͨ����������DTMF��
			else LCD_Fill(40,90,120,90+16,WHITE);
                        ec20_load_keyboard(0,180,(u8**)kbd_tbl1);		//��ʾ����
			oldmode=cmode; 
		}
		if((lenx%50)==0)
    {
		 LED0=!LED0;
		 u2_printf("AT\r\n");//���������䣬��Ȼ���ղ���������ʾ	
		}; 	    				 
		lenx++;	 
	} 
	myfree(p1);
	return 0;
}
}*/


ErrorStatus ec20_init(unsigned char *IPSERVER, int PORTSERVER)
{
    u8 data=0,ret=0;
    u8 err=0;
    char atstr[BUFLEN];
    USART2_RX_STA=0;
    if(ec20_send_cmd("AT","OK","NULL","NULL",1000))err|=1<<0;//?��2a��?��?��|��eAT??��?
    USART2_RX_STA=0;
    if(ec20_send_cmd("ATE0","OK","NULL","NULL",2000))err|=1<<1;//2?????
    USART2_RX_STA=0;
    if(ec20_send_cmd("AT+CPIN?","OK","NULL","NULL",2000))err|=1<<3;	//2��?��SIM?����?��??��??
    USART2_RX_STA=0;
    data = 0;
    //2��?��GSM��???����2�����䨬?��?������??����?3��1|
    while (ec20_send_cmd("AT+CREG?\r\n","\r\n+CREG: 0,1","NULL","NULL",2000)!= 1 && data < 10)
    {
        USART2_RX_STA=0;
        delay_ms(100);
        data++;
    }
    USART2_RX_STA=0;
    if (data == 10)
    {
        return ERROR;                                                                             //?����?2?3��1|?��?��????
    }
    ec20_send_cmd("AT+CGATT?\r\n","+CGATT: 1","OK","NULL",2000);
    USART2_RX_STA=0;
    delay_ms(200);
    ec20_send_cmd("AT+QIACT?\r\n","OK","NULL","NULL",2000);

    USART2_RX_STA=0;
    delay_ms(200);
    ec20_send_cmd("AT+QICLOSE=0\r\n","OK","NULL","NULL",2000);
    USART2_RX_STA=0;
    delay_ms(200);
	ec20_send_cmd("AT+CMGF=1\r\n","OK","NULL","NULL",2000);
    USART2_RX_STA=0;
    delay_ms(200);
	ec20_send_cmd("AT+CSCS=\"GSM\"\r\n","OK","NULL","NULL",2000);
    USART2_RX_STA=0;
    delay_ms(200);
	ec20_send_cmd("AT+CNMI=1,2,0,1,0 \r\n","OK","NULL","NULL",2000);
    USART2_RX_STA=0;
    delay_ms(200);
	ec20_send_cmd("AT+QURCCFG=\"urcport\",\"uart1\" \r\n","OK","NULL","NULL",2000);
	USART2_RX_STA=0;
    delay_ms(200);
	
    memset(atstr,0,BUFLEN);
    sprintf(atstr,"AT+QIOPEN=1,0,\"TCP\",\"%s\",%d,0,0\r\n",IPSERVER,PORTSERVER);
    data=ec20_send_cmd((u8*)atstr,"CONNECT","OK","NULL",2000);
    USART2_RX_STA=0;
    delay_ms(200);
    USART2_RX_STA=0;
    if (data == 1 || data == 2 || data == 3 || ret==1)
    {
        printf("data=%d\r\n",data);
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
} 

