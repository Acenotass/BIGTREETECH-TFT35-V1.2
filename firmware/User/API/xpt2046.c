#include "xpt2046.h"
#include "includes.h"


//XPT2046 SPI���
_SW_SPI xpt2046;

void XPT2046_SPI_Config(void)
{		
    SW_SPI_Config(&xpt2046, _SPI_MODE3,
    GPIOC,  GPIO_Pin_0,     //CS
    GPIOC,  GPIO_Pin_1,     //SCK
    GPIOC,  GPIO_Pin_2,     //MISO
    GPIOC,  GPIO_Pin_3      //MOSI
    );		
}

u8 XPT2046_ReadWriteByte(u8 TxData)
{		
	return SW_SPI_Read_Write(&xpt2046, TxData);			    
}

void XPT2046_CS_Set(u8 level)
{
    SW_SPI_CS_Set(&xpt2046, level);
}

//XPT2046 SPI�ͱ��жϳ�ʼ��
void XPT2046_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	//PC4-TPEN
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
    
    XPT2046_SPI_Config();
}

u8 XPT2046_Read_Pen(void)
{
    return ((GPIOC->IDR & GPIO_Pin_4)!=Bit_RESET);
}

//��ȡ XPT2046 ת���õ�ADֵ
u16 XPT2046_Read_AD(u8 CMD)
{
	u16 ADNum;
	XPT2046_CS_Set(0);
  
	XPT2046_ReadWriteByte(CMD);
	ADNum=XPT2046_ReadWriteByte(0xff);
	ADNum= ((ADNum)<<8) | XPT2046_ReadWriteByte(0xff);
	ADNum >>= 4;         //XPT2046����ֻ��12bits,��������λ
	
	XPT2046_CS_Set(1);
	return ADNum;
}

#define READ_TIMES 5 	//��ȡ����
#define LOST_VAL 1	  	//����ֵ
u16 XPT2046_Average_AD(u8 CMD)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0; i<READ_TIMES; i++) buf[i] = XPT2046_Read_AD(CMD);		 		    
	for(i=0; i<READ_TIMES-1; i++)//����
	{
		for(j=i+1; j<READ_TIMES; j++)
		{
			if(buf[i] > buf[j]) //��������
			{
				temp = buf[i];
				buf[i] = buf[j];
				buf[j] = temp;
			}
		}
	}
    
	for(i=LOST_VAL; i < READ_TIMES-LOST_VAL; i++) sum += buf[i];
	temp = sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 


#define ERR_RANGE 50 //��Χ 
u16 XPT2046_Repeated_Compare_AD(u8 CMD) 
{
	u16 ad1, ad2;
    ad1 = XPT2046_Average_AD(CMD);
    ad2 = XPT2046_Average_AD(CMD);
    
    if((ad2<=ad1&&ad1<ad2+ERR_RANGE) || (ad1<=ad2&&ad2<ad1+ERR_RANGE)) //ǰ���������С�� ERR_RANGE
    {
        return (ad1+ad2)/2;
    }
    else return 0;	  
} 

