#ifndef _USART1_H_
#define _USART1_H_
#include "stm32f10x.h"

#define BUFSIZE 8
#define MAX_CMD_SIZE 96

#define MAX_ACK_SIZE 200
#define USART1_DMA_CHANNEL	DMA1_Channel5


void USART1_Config(u32 bound);
void USART_Putc(USART_TypeDef* USARTx, char data);
void USART1_Puts(char *s );
void Send_CMD(void);



typedef struct 
{
	char 	queue[BUFSIZE][MAX_CMD_SIZE];
	u8	 	index_r ; // Ring buffer read position
	u8		index_w ; // Ring buffer write position
	u8		count ; 	// Count of commands in the queue
	
}USART1_CMD;

typedef struct
{	
	char wait;															//���͵������Ƿ���Ӧ��		0����Ӧ��  1��δӦ�𡢵ȴ�Ӧ�����ܼ���������һ������
	char rx_ok;
	char link_flag;							//1:�����Ӵ�ӡ��   0��δ���Ӵ�ӡ��
}USART1_FLAG;

extern USART1_CMD infoCMD ;
extern USART1_FLAG infoUSART1_CMD ;

void loopProcess (void);

#endif 




















