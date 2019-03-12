#include "lcd.h"
#include "includes.h"



u16 LCD_RD_DATA(void)
{										    	   
	vu16 ram;			
	ram=LCD->LCD_RAM;	
	return ram;	 		 
}					   


void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{	
	LCD->LCD_REG = LCD_Reg;
	LCD->LCD_RAM = LCD_RegValue;    		 
}	   


u16 LCD_ReadReg(u8 LCD_Reg)
{										   
	LCD_WR_REG(LCD_Reg);
	Delay_us(5);
	return LCD_RD_DATA();
}   


void LCD_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
   
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD |RCC_APB2Periph_GPIOE ,ENABLE);
 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
	/* ����FSMC���Ӧ��������,FSMC-D0~D15: PD 14 15 0 1 8 9 10,PE 7 8 9 10 11 12 13 14 15*/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_0  | GPIO_Pin_1
                                | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10;
	GPIO_Init(GPIOD, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10
	 						    | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14
	   						    | GPIO_Pin_15 | GPIO_Pin_2;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
    /* ����FSMC���Ӧ�Ŀ�����
	* PD4-FSMC_NOE   :LCD-RD
	* PD5-FSMC_NWE   :LCD-WR
	* PD7-FSMC_NE1	 :LCD-CS
	* PD11-FSMC_A23  :LCD-RS   LCD-RS���͵����ݴ������������ʾ����
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7  | GPIO_Pin_11;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void LCD_FSMC_Config(void)
{	
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming,writeTiming; 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

    readWriteTiming.FSMC_AddressSetupTime = 0x01;	 //��ַ����ʱ�䣨ADDSET��Ϊ2��HCLK 1/36M=27ns
    readWriteTiming.FSMC_AddressHoldTime = 0x00;
    readWriteTiming.FSMC_DataSetupTime = 0x0f;
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA 

    writeTiming.FSMC_AddressSetupTime = 0x00;	 //��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK  
    writeTiming.FSMC_AddressHoldTime = 0x00;
    writeTiming.FSMC_DataSetupTime = 0x03;		 ////���ݱ���ʱ��Ϊ4��HCLK	
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA 

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;  //ѡ����Ӵ洢����ĵ�ַ
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;//���������ߺ͵�ַ���Ƿ���
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;//������Ӵ洢��������
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//����FSMC�ӿڵ����ݿ��

    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;//���÷���ģʽ
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;//���õȴ��źŵļ���
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;//�����Ƿ�ʹ�÷Ƕ��뷽ʽ
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;//���õȴ��ź�ʲôʱ�����
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;//�����Ƿ�ʹ�õȴ��ź�
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;//�����Ƿ�����ͻ��д����

    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;//����д����ʹ��
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable ;//�����Ƿ�ʹ����չģʽ

    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;//��ʱ��
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //дʱ��

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
    /* ʹ�� FSMC Bank1_SRAM Bank */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}



#if HARDWARE_VERSION == V1_2
//ILI9488
void LCD_init_RGB(void) 
{
	LCD_WR_REG(0xC0); 
	LCD_WR_DATA(0x0c); 
	LCD_WR_DATA(0x02);	 
	LCD_WR_REG(0xC1); 
	LCD_WR_DATA(0x44); 
	LCD_WR_REG(0xC5); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x16); 
	LCD_WR_DATA(0x80);
	LCD_WR_REG(0x36); 
	LCD_WR_DATA(0x28); 
	LCD_WR_REG(0x3A); //Interface Mode Control
	LCD_WR_DATA(0x55);
	LCD_WR_REG(0XB0);  //Interface Mode Control  
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xB1);   //Frame rate 70HZ  
	LCD_WR_DATA(0xB0); 
	LCD_WR_REG(0xB4); 
	LCD_WR_DATA(0x02); 
	LCD_WR_REG(0xB6); //RGB/MCU Interface Control
	LCD_WR_DATA(0x02); 
	LCD_WR_DATA(0x02); 
	LCD_WR_REG(0xE9); 
	LCD_WR_DATA(0x00);	 
	LCD_WR_REG(0XF7);    
	LCD_WR_DATA(0xA9); 
	LCD_WR_DATA(0x51); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x82);
	LCD_WR_REG(0x11); 
	Delay_ms(120); 
	LCD_WR_REG(0x29); 
} 
#elif HARDWARE_VERSION == V1_1   
//RM68042
void LCD_init_RGB(void)
{
    LCD_WR_REG(0X11);
    Delay_ms(20);
    LCD_WR_REG(0XD0);//VCI1  VCL  VGH  VGL DDVDH VREG1OUT power amplitude setting
    LCD_WR_DATA(0X07); 
    LCD_WR_DATA(0X42); 
    LCD_WR_DATA(0X1C); 
    LCD_WR_REG(0XD1);//VCOMH VCOM_AC amplitude setting
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X19);
    LCD_WR_DATA(0X16); 
    LCD_WR_REG(0XD2);//Operational Amplifier Circuit Constant Current Adjust , charge pump frequency setting
    LCD_WR_DATA(0X01);
    LCD_WR_DATA(0X11);
    LCD_WR_REG(0XE4);
    LCD_WR_DATA(0X00A0);
    LCD_WR_REG(0XF3);
    LCD_WR_DATA(0X0000);
    LCD_WR_DATA(0X002A);
    LCD_WR_REG(0XC0);//REV SM GS 
    LCD_WR_DATA(0X10);
    LCD_WR_DATA(0X3B);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X02);
    LCD_WR_DATA(0X11);
    LCD_WR_REG(0XC5);// Frame rate setting = 72HZ  when setting 0x03
    LCD_WR_DATA(0X03);
    LCD_WR_REG(0XC8);//Gamma setting
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X35);
    LCD_WR_DATA(0X23);
    LCD_WR_DATA(0X07);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X04);
    LCD_WR_DATA(0X45);
    LCD_WR_DATA(0X53);
    LCD_WR_DATA(0X77);
    LCD_WR_DATA(0X70);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X04);
    LCD_WR_REG(0X20);//Exit invert mode
    LCD_WR_REG(0X36);
    LCD_WR_DATA(0X28);
    LCD_WR_REG(0X3A);
    LCD_WR_DATA(0X55);//16λģʽ 
    Delay_ms(120);
    LCD_WR_REG(0X29); 	 
}

#elif HARDWARE_VERSION == V1_0
    
#endif


void LCD_SetWindow(u16 sx, u16 sy, u16 ex, u16 ey)
{
	
	LCD_WR_REG(0x2A); 
	LCD_WR_DATA(sx>>8);LCD_WR_DATA(sx&0xFF);
	LCD_WR_DATA(ex>>8);LCD_WR_DATA(ex&0xFF);
	LCD_WR_REG(0x2B); 
	LCD_WR_DATA(sy>>8);LCD_WR_DATA(sy&0xFF);
	LCD_WR_DATA(ey>>8);LCD_WR_DATA(ey&0xFF);
	LCD_WR_REG(0x2C);
}

void LCD_DrawPoint(u16 x,u16 y,u16 c)
{	 
	LCD_SetWindow(x,y,x,y);
	LCD_WR_16BITS_DATA(c);
}

void LCD_Clear(u16 color)
{
	u16 i,j;   
	
	LCD_SetWindow(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);	  
	for(i=LCD_WIDTH;i>0;i--)
	{
		for(j=LCD_HEIGHT;j>0;j--)
		{
			LCD_WR_16BITS_DATA(color);
		}
	}
}  



void LCD_Init(void)
{
	LCD_GPIO_Config();
	
	LCD_FSMC_Config();
	
	LCD_init_RGB();
	LCD_Clear(BLACK);
}

