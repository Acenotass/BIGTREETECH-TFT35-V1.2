#ifndef _TOUCH_H_
#define _TOUCH_H_
#include "stm32f10x.h"
/*
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
*/

#define TPEN() 	((GPIOC->IDR & GPIO_Pin_4)!=Bit_RESET)
#define TDIN() ((GPIOC->IDR & GPIO_Pin_2)!=Bit_RESET)

#define TCS(x)  do{if(x)(GPIOC->BSRR = GPIO_Pin_0);else (GPIOC->BRR = GPIO_Pin_0);}while(0)
#define TCLK(x) do{if(x)(GPIOC->BSRR = GPIO_Pin_1);else GPIO_ResetBits(GPIOC,GPIO_Pin_1);}while(0)
#define TOUT(x) do{if(x)(GPIOC->BSRR = GPIO_Pin_3);else GPIO_ResetBits(GPIOC,GPIO_Pin_3);}while(0)

#define CMD_RDX 0xD0
#define CMD_RDY 0x90
extern u32 A,B,C,D,E,F,K;

#define ADJUST_IDADDR  0x0803F000	///У׼��ʾ �� ֵ�洢λ��
#define ADJUST_COMPLETE   0x12345678	///оƬID�洢��ԭʼ����

/*******************************************************************************
������������ȡSTM32flashָ����ַ������
���������u32 faddr����ַ
�����������ַ��Ӧ��u32����
��    ע: ��
*************************************************************************/
u32 STMFLASH_ReadHalfWord(u32 faddr);

/*******************************************************************************
�����������洢����У׼ֵ��FLASH
���������u32 faddr����ַ
���������void
��    ע: ��
*************************************************************************/
void STMFLASH_Store_Cost(u32 faddr);

/*******************************************************************************
������������ȡ����� A B C D E F K��ֵ
���������u32 faddr����ַ
���������void
��    ע: ��
*************************************************************************/
void STMFLASH_Read_Calibration(void);

void Read_Lcd_xy(void);

void TP_GPIOConfig(void);
void TP_Write_Cmd(u8 CMD);
void TP_Read_Data(u16 *AD_Data);
u16 TP_Read_AD(u8 CMD);
void TP_Read_xy(u16 *x,u16 *y);
void Test_touch(void);
void Three_adjust(void);
void Adjust_test(u16 x,u16 y);
//void Display_LCD_xy(void);

void Get_Lcdvalue(u16 *lcd_x,u16 *lcd_y);

u8 Get_Place(u8 view_flag);
u8 Key_value(u16 x,u16 y);

void Back_display(u8 press,u8 view_flag,u8 key_num);

//void Key_PicAndNum_create(void);
//void Key_PicAndNum_alone(u8 i,u8 j,u16 linec,u16 fullc,u16 zic);
#endif

