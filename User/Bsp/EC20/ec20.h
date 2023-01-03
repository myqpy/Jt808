#ifndef __ec20_H__
#define __ec20_H__
#include "./sys/sys.h"


#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8	//高低字节交换宏定义

extern u8 Scan_Wtime;

void sim_at_response(u8 mode);	
u8* ec20_check_cmd(u8 *str);
u8 ec20_send_cmd(u8 *cmd,u8 *Re1,u8 *Re2,u8 *Re3,u16 waittime);
u8 ec20_wait_request(u8 *request ,u16 waittime);
u8 ec20_chr2hex(u8 chr);
u8 ec20_hex2chr(u8 hex);

#endif





