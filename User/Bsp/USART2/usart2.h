#ifndef __USART2_H
#define __USART2_H	 
#include "./sys/sys.h"   

#define USART2_MAX_RECV_LEN		1024*5				//�����ջ����ֽ���
#define USART2_MAX_SEND_LEN		1024*5				//����ͻ����ֽ���
#define USART2_RX_EN 			1					//0,������;1,����.

extern u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
extern u16 USART2_RX_STA;   						//��������״̬

void USART2_Init(u32 bound);				//����2��ʼ�� 
void TIM4_Set(u8 sta);
void TIM4_SetARR(u16 period);
void TIM4_Init(u16 arr,u16 psc);
void UART_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar);
void UART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u16 len);
void u2_printf(char* fmt, ...);

void UartSend_Non_transliterated(USART_TypeDef * pUSARTx,u8* msg, uint16_t number);
int UartRecv_Non_transliterated(void);
void Usart_SendStr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen );
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);

#endif













