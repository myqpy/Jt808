#include "./ec20/ec20.h"
#include "./usart/usart.h"		
#include "./delay/delay.h"	 
#include "./key/key.h"	 	 	 	 	 
#include "string.h"    
#include "./usart2/usart2.h" 

u8 Scan_Wtime = 0;//保存扫描需要的时间
u8 BT_Scan_mode=0;//蓝牙扫描设备模式标志

//usmart支持部分 
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART2_RX_STA;
//     1,清零USART2_RX_STA;
void sim_at_response(u8 mode)
{
    if(USART2_RX_STA&0X8000)		//接收到一次数据了
    {
        USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//添加结束符
        printf("%s",USART2_RX_BUF);	//发送到串口
        if(mode)USART2_RX_STA=0;
    }
}
//////////////////////////////////////////////////////////////////////////////////
//ATK-ec20 各项测试(拨号测试、短信测试、GPRS测试、蓝牙测试)共用代码

//ec20发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//其他,期待应答结果的位置(str的位置)
u8* ec20_check_cmd(u8 *str)
{
    char *strx=0;
    if(USART2_RX_STA&0X8000)		//接收到一次数据了
    {
        USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//添加结束符
        strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
    }
    return (u8*)strx;
}
//向ec20发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 ec20_send_cmd(u8 *cmd,u8 *Re1,u8 *Re2,u8 *Re3,u16 waittime)
{
    u8 res=0;
    USART2_RX_STA=0;
    if((u32)cmd<=0XFF)
    {
        while(DMA1_Channel7->CNDTR!=0);	//等待通道7传输完成
        USART2->DR=(u32)cmd;
    }else
    {
        u2_printf("%s\r\n",cmd);//发送命令
        printf("%s\r\n",cmd);//发送命令
    }

    if(waittime==1100)//11s后读回串口数据(蓝牙扫描模式)
    {
        Scan_Wtime = 11;  //需要定时的时间
        TIM4_SetARR(9999);//产生1S定时中断
    }


    if((Re1&&waittime)||(Re3&&waittime)||(Re2&&waittime))		//需要等待应答
    {
        while(--waittime)	//等待倒计时
        {
            if(BT_Scan_mode)//蓝牙扫描模式
            {
                res=KEY_Scan(0);//返回上一级
//                if(res==WKUP_PRES)return 2;
            }
            delay_ms(10);
            if(USART2_RX_STA&0X8000)//接收到期待的应答结果
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

//接收ec20返回数据（蓝牙测试模式下使用）
//request:期待接收命令字符串
//waittimg:等待时间(单位：10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 ec20_wait_request(u8 *request ,u16 waittime)
{
    u8 res = 1;
//    u8 key;
    if(request && waittime)
    {
        while(--waittime)
        {
//            key=KEY_Scan(0);
//            if(key==WKUP_PRES) return 2;//返回上一级
//            delay_ms(10);
            if(USART2_RX_STA &0x8000)//接收到期待的应答结果
            {
                if(ec20_check_cmd(request)) break;//得到有效数据
                USART2_RX_STA=0;
            }
        }
        if(waittime==0)res=0;
    }
    return res;
}

//将1个字符转换为16进制数字
//chr:字符,0~9/A~F/a~F
//返回值:chr对应的16进制数值
u8 ec20_chr2hex(u8 chr)
{
    if(chr>='0'&&chr<='9')return chr-'0';
    if(chr>='A'&&chr<='F')return (chr-'A'+10);
    if(chr>='a'&&chr<='f')return (chr-'a'+10);
    return 0;
}
//将1个16进制数字转换为字符
//hex:16进制数字,0~15;
//返回值:字符
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
	u8 pohnenumlen=0;	//号码长度,最大15个数 
	u8 *p,*p1,*p2;
	u8 oldmode=0;
	u8 cmode=0;	//模式
				//0:等待拨号
				//1:拨号中
	            //2:通话中
				//3:接收到来电 

        if(ec20_send_cmd("AT+CLIP=1","OK",200))return 1;	//设置来电显示
        if(ec20_send_cmd("AT+COLP=1","OK",200))return 2;	//设置被叫号码显示
 	p1=mymalloc(20);								//申请20直接用于存放号码
	if(p1==NULL)return 2;	

	while(1)
	{
		delay_ms(10);
		if(USART2_RX_STA&0X8000)		//接收到数据
		{
			sim_at_response(0);
			if(cmode==1||cmode==2)
			{
                                if(cmode==1)if(ec20_check_cmd("+COLP:"))cmode=2;	//拨号成功
                                if(ec20_check_cmd("NO CARRIER"))cmode=0;	//拨号失败
                                if(ec20_check_cmd("NO ANSWER"))cmode=0;	//拨号失败
                                if(ec20_check_cmd("ERROR"))cmode=0;		//拨号失败
			}
                        if(ec20_check_cmd("+CLIP:"))//接收到来电
			{
				cmode=3;
                                p=ec20_check_cmd("+CLIP:");
				p+=8;
				p2=(u8*)strstr((const char *)p,"\"");
				p2[0]=0;//添加结束符 
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
                    delay_ms(20);//延时55
				    u2_printf("AT+CTTS=2,\"%c\"\r\n",kbd_tbl[key-1][0]); //TTS语音									
				}else if(cmode==2)//通话中
				{ 
					u2_printf("AT+CLDTMF=2,\"%c\"\r\n",kbd_tbl[key-1][0]);
					delay_ms(100);
					u2_printf("AT+VTS=%c\r\n",kbd_tbl[key-1][0]); 
					LCD_ShowChar(40+56,90,kbd_tbl[key-1][0],16,0);
				}
			}else
			{
				if(key==13)if(pohnenumlen&&cmode==0)pohnenumlen--;//删除
				if(key==14)//执行拨号
				{
					if(cmode==0)//拨号模式
					{
						callbuf[pohnenumlen]=0;			//最后加入结束符 
						u2_printf("ATD%s;\r\n",callbuf);//拨号
						delay_ms(10);		        	//等待10ms  
						cmode=1;						//拨号中模式
					}else 
					{
                                                ec20_send_cmd("ATH","OK",100);//挂机
                                                ec20_send_cmd("ATH","OK",100);//挂机
						cmode=0;
					}
				}
				if(key==15)
				{
					if(cmode==3)//接收到来电
					{
                                                ec20_send_cmd("ATA","OK",200);//发送应答指令
						Show_Str(40+56,70,200,16,callbuf,16,0);
						cmode=2;
					}else
					{ 
                                                ec20_send_cmd("ATH",0,0);//不管有没有在通话,都结束通话
						break;//退出循环
					}
				}
			} 
			if(cmode==0)//只有在等待拨号模式有效
			{
				callbuf[pohnenumlen]=0; 
				LCD_Fill(40+56,70,239,70+16,WHITE);
				Show_Str(40+56,70,200,16,callbuf,16,0);  	
			}				
		}
		if(oldmode!=cmode)//模式变化了
		{
			switch(cmode)
			{
				case 0: 
					kbd_fn_tbl[0]="拨号";
					kbd_fn_tbl[1]="返回"; 
					POINT_COLOR=RED;
					Show_Str(40,70,200,16,"请拨号:",16,0);  
					LCD_Fill(40+56,70,239,70+16,WHITE);
					if(pohnenumlen)
					{
						POINT_COLOR=BLUE;
						Show_Str(40+56,70,200,16,callbuf,16,0);
					}
					break;
				case 1:
					POINT_COLOR=RED;
					Show_Str(40,70,200,16,"拨号中:",16,0); 
					pohnenumlen=0;
				case 2:
					POINT_COLOR=RED;
					if(cmode==2)Show_Str(40,70,200,16,"通话中:",16,0); 
					kbd_fn_tbl[0]="挂断";
					kbd_fn_tbl[1]="返回"; 	
					break;
				case 3:
					POINT_COLOR=RED;
					Show_Str(40,70,200,16,"有来电:",16,0); 
					POINT_COLOR=BLUE;
					Show_Str(40+56,70,200,16,p1,16,0); 
					kbd_fn_tbl[0]="挂断";
					kbd_fn_tbl[1]="接听"; 
					break;				
			}
			if(cmode==2)Show_Str(40,90,200,16,"DTMF音:",16,0);	//通话中,可以通过键盘输入DTMF音
			else LCD_Fill(40,90,120,90+16,WHITE);
                        ec20_load_keyboard(0,180,(u8**)kbd_tbl1);		//显示键盘
			oldmode=cmode; 
		}
		if((lenx%50)==0)
    {
		 LED0=!LED0;
		 u2_printf("AT\r\n");//必须加上这句，不然接收不到来电显示	
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
    if(ec20_send_cmd("AT","OK","NULL","NULL",1000))err|=1<<0;//?ì2aê?·?ó|′eAT??á?
    USART2_RX_STA=0;
    if(ec20_send_cmd("ATE0","OK","NULL","NULL",2000))err|=1<<1;//2?????
    USART2_RX_STA=0;
    if(ec20_send_cmd("AT+CPIN?","OK","NULL","NULL",2000))err|=1<<3;	//2é?ˉSIM?¨ê?·??ú??
    USART2_RX_STA=0;
    data = 0;
    //2é?ˉGSMí???×￠2á×′ì?￡?è·è??òí?3é1|
    while (ec20_send_cmd("AT+CREG?\r\n","\r\n+CREG: 0,1","NULL","NULL",2000)!= 1 && data < 10)
    {
        USART2_RX_STA=0;
        delay_ms(100);
        data++;
    }
    USART2_RX_STA=0;
    if (data == 10)
    {
        return ERROR;                                                                             //?òí?2?3é1|?￡?é????
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

