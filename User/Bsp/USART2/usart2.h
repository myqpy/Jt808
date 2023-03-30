#ifndef __USART2_H
#define __USART2_H	 
#include "./sys/sys.h"   

#define USART2_MAX_RECV_LEN		1024*5				//�����ջ����ֽ���
#define USART2_MAX_SEND_LEN		1024*5				//����ͻ����ֽ���
#define USART2_RX_EN 			1					//0,������;1,����.
extern u8 Scan_Wtime;
extern u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
extern u16 USART2_RX_STA;   						//��������״̬

void USART2_Init(u32 bound);				//����2��ʼ�� 
void UART_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar);
void UART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u16 len);
void u2_printf(char* fmt, ...);


#endif













