#ifndef _DELAY_H_
#define _DELAY_H_
#include "includes.h"

void Delay_init(u8 clk); 		//��ʼ��
void Delay_ms(u16 ms);				//��ʱ���뼶
void Delay_us(u32 us);				//��ʱ΢�뼶
void Delay(int time);				//�����ʱ

#endif
