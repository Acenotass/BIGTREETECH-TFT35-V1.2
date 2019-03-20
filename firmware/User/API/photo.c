#include "photo.h"
#include "includes.h"


#if   HARDWARE_VERSION == V1_2
    #define IS_16BIT 1
#elif HARDWARE_VERSION == V1_1 ||  HARDWARE_VERSION == V1_0
    #define IS_16BIT 0
#endif

//SPI2 --> LCD_RAM dma����
//16λ,SPI1_RX���䵽LCD_RAM.
void LCD_DMA_Config(void)
{  
	RCC->AHBENR|=1<<0;				//����DMA1ʱ��
	Delay_ms(5);					//�ȴ�DMAʱ���ȶ�
	DMA1_Channel2->CPAR=(u32)&SPI1->DR;			//�����ַΪ:SPI1->DR
	DMA1_Channel2->CMAR=(u32)&LCD->LCD_RAM;//Ŀ���ַΪLCD_RAM
 	DMA1_Channel2->CNDTR=0;			//DMA1,����������,��ʱ����Ϊ0
	DMA1_Channel2->CCR=0X00000000;	//��λ
	DMA1_Channel2->CCR|=0<<4;		//�������
	DMA1_Channel2->CCR|=0<<5;		//��ͨģʽ
	DMA1_Channel2->CCR|=0<<6;		//�����ַ������ģʽ
	DMA1_Channel2->CCR|=0<<7;		//�洢��������ģʽ
	DMA1_Channel2->CCR|=IS_16BIT<<8;		//�������ݿ��Ϊ16λ
	DMA1_Channel2->CCR|=IS_16BIT<<10;		//�洢�����ݿ��16λ
	DMA1_Channel2->CCR|=1<<12;		//�е����ȼ�
	DMA1_Channel2->CCR|=0<<14;		//�Ǵ洢�����洢��ģʽ
} 
//����һ��SPI��LCD��DMA�Ĵ��� 
#define LCD_DMA_MAX_TRANS	65535		//DMAһ����ഫ�� 65535 �ֽ�	

// ���� SPI->DR �� FSMC �� DMA ���� 
// ���������� ���ܳ��� LCD_DMA_MAX_TRANS 65535
void lcd_frame_segment_display(u16 size, u32 addr)
{     
    DMA1_Channel2->CNDTR = size;				    //���ô��䳤��

	W25Qxx_SPI_CS_Set(0);                        //ʹ������ 
	W25Qxx_SPI_Read_Write_Byte(CMD_FAST_READ_DATA);	//���Ϳ��ٶ�ȡ����   
	W25Qxx_SPI_Read_Write_Byte((u8)((addr)>>16));	//����24bit��ַ    
	W25Qxx_SPI_Read_Write_Byte((u8)((addr)>>8));   
	W25Qxx_SPI_Read_Write_Byte((u8)addr);  
	W25Qxx_SPI_Read_Write_Byte(0XFF);	           //8 dummy clock  

//���� SPI �� 16bit DMA ����ģʽ    
    SPI1->CR1 &= ~(1<<6); 	
    SPI1->CR2 |= 1<<0;		    //ʹ��DMA���� 
    SPI1->CR1 |= IS_16BIT<<11;	//16bit���ݸ�ʽ	
    SPI1->CR1 |= 1<<10; 		//������ģʽ  
    
    DMA1_Channel2->CCR |= 1<<0;				//ͨ������
    SPI1->CR1 |= 1<<6;                      //�����˴δ���
    
    while((DMA1->ISR&(1<<5)) == 0);			//�ȴ��������  
    DMA1_Channel2->CCR &= (u32)(~(1<<0));	//�ر�ͨ��
    DMA1->IFCR |= (u32)(1<<5);				//����ϴεĴ�����ɱ�� 
	W25Qxx_SPI_CS_Set(1) ;  	 	 		//��ֹW25QXX
	
//���� SPI ����ͨģʽ    
    RCC->APB2RSTR |= 1<<12;     //��λSPI1
    RCC->APB2RSTR &= ~(1<<12);
    SPI1->CR1 = 0x34C;          //����CR1  
    SPI1->I2SCFGR &= ~(1<<11);  //ѡ��SPIģʽ 
}

void lcd_frame_display(u16 sx,u16 sy,u16 w,u16 h, u32 addr)
{
    u32 cur=0;
    u32 segmentSize;
    u32 totalSize = w*h*(2-IS_16BIT);
    
	LCD_SetWindow(sx,sy,sx+w-1,sy+h-1);
	LCD->LCD_REG=0x2C;    
 
    for(cur = 0; cur < totalSize; cur += LCD_DMA_MAX_TRANS)
    {
        segmentSize = cur+LCD_DMA_MAX_TRANS<=totalSize ? LCD_DMA_MAX_TRANS : totalSize-cur;
        lcd_frame_segment_display(segmentSize,addr+cur*(IS_16BIT + 1));
    }

}

void LOGO_ReadDisplay(void)
{
	lcd_frame_display(0, 0, LCD_WIDTH, LCD_HEIGHT, LOGO_ADDR);
}

#define ICON_WIDTH  95
#define ICON_HEIGHT 95

void ICON_ReadDisplay(u16 sx,u16 sy, u8 icon)
{
	lcd_frame_display(sx, sy, ICON_WIDTH, ICON_HEIGHT, ICON_ADDR(icon));
}

void ICON_PressedDisplay(u16 sx,u16 sy, u8 icon)
{
	u16 mode=0x0FF0;
	u16 x,y;
	u16 color = 0;
    u32 address = ICON_ADDR(icon);
	
	LCD_SetWindow(sx, sy, sx+ICON_WIDTH-1, sy+ICON_HEIGHT-1);
	LCD->LCD_REG=0x2C;
    
    W25Qxx_SPI_CS_Set(0);
    W25Qxx_SPI_Read_Write_Byte(CMD_READ_DATA);
    W25Qxx_SPI_Read_Write_Byte((address & 0xFF0000) >> 16);
    W25Qxx_SPI_Read_Write_Byte((address& 0xFF00) >> 8);
    W25Qxx_SPI_Read_Write_Byte(address & 0xFF);
	
	for(y=sy; y<sy+ICON_WIDTH; y++)
	{	
		for(x=sx; x<sx+ICON_HEIGHT; x++)
		{
			color  = (W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE)<<8);
			color |= W25Qxx_SPI_Read_Write_Byte(W25QXX_DUMMY_BYTE);
			LCD_WR_16BITS_DATA(color&mode);
		}
	}		
    W25Qxx_SPI_CS_Set(1);		
}
