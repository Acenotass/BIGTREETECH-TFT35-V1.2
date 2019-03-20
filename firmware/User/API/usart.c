#include "usart.h"
#include "includes.h"



static const char errormagic[]  = "Error:";
static const char echomagic[]   = "echo:";

static char DMA_Rec_Buf[MAX_ACK_SIZE];

static u16 code_index=0;

static char str_seen(const char *str)
{
	u16 i;	
	for(code_index=0;DMA_Rec_Buf[code_index]!=0 && code_index < MAX_ACK_SIZE ;code_index++)
	{
		for(i=0; DMA_Rec_Buf[code_index+i]==str[i] && str[i]!=0 && DMA_Rec_Buf[code_index+i]!=0;i++)
        {}
        if(str[i]==0)
        {
            code_index+=i;      
            return true;
        }
	}
	return false;
}
static char str_cmp(const char *str1, const char *str2)
{
	u8 i;
	for(i=0;str1[i]!=0 && str2[i]!=0 && i<255;i++)
	{
		if(str1[i]!=str2[i])
			return false;
	}
	if(str1[i]!=0) return false;
	return true;
}


static float code_value()
{
	return (strtod(&DMA_Rec_Buf[code_index],NULL));
}




void USART1_Config(u32 bound)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC->AHBENR  |= 1<<0; //���� DMA1EN ʱ��
    RCC->APB2ENR |= 1<<2; //���� PORTA ʱ��
    RCC->APB2ENR |= 1<<14; //���� USART1 ʱ��

    GPIOA->CRH &= 0xFFFFF00F;
    GPIOA->CRH |= 0x000008B0;// PA9 �������   PA10��������

    //�����ж�ͨ���Լ����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);	

    USART1->BRR=(72*1000000)/(bound);// ����������	 
    USART1->SR  &= ~(1<<4);           //��������ж�
    USART1->CR1 |=0X201C;  	         //ʹ��USART1. ʹ�ܿ����ж� 1λֹͣ,��У��λ.
    USART1->CR3 |= 1<<6;	           //ʹ�ܴ��ڽ���DMA	

    USART1_RXDMA_CHANNEL->CPAR = (u32)(&USART1->DR);		//�����ַ
    USART1_RXDMA_CHANNEL->CMAR = (u32)DMA_Rec_Buf;			//Ŀ���ַ
    USART1_RXDMA_CHANNEL->CNDTR = MAX_ACK_SIZE;					//����������	
    USART1_RXDMA_CHANNEL->CCR=0X00000000;	//��λ
    USART1_RXDMA_CHANNEL->CCR |= 3<<12;   //11��ͨ�����ȼ����
    USART1_RXDMA_CHANNEL->CCR |= 1<<7;    //1��ִ�д洢����ַ��������
    USART1_RXDMA_CHANNEL->CCR |= 1<<0;    ////ʹ��DMA		
}


/*
	�������ܣ���������DMA�ռ�
*/
void USART1_DMAReEnable(void)
{
    memset(DMA_Rec_Buf,0,MAX_ACK_SIZE);
    USART1_RXDMA_CHANNEL->CCR   &= ~(1<<0);
    USART1_RXDMA_CHANNEL->CNDTR = MAX_ACK_SIZE;  
    USART1_RXDMA_CHANNEL->CCR   |= 1<<0;    //ʹ��DMA			
}

void USART1_IRQHandler(void)
{
	u16 DMA_bufrx=0;
	
	if((USART1->SR & (1<<4))!=0)
	{
		USART1->SR = ~(1<<4);
		USART1->DR;   //DMA+���ڿ����ж�  �жϷ������������һ�� �����ݼĴ�������Ȼ���������֮�������
		
		DMA_bufrx=MAX_ACK_SIZE-USART1_RXDMA_CHANNEL->CNDTR;
		
		if(DMA_Rec_Buf[DMA_bufrx-1]=='\n')
		{
			infoHost.rx_ok=true;
		}
		else if(DMA_bufrx>MAX_ACK_SIZE-5)
		{
			infoHost.rx_ok=true;
		}
	}
}

void USART1_Puts(char *s )
{
	while (*s)
	{
		while((USART1->SR & USART_FLAG_TC) == (uint16_t)RESET);
		USART1->DR = ((u16)*s++ & (uint16_t)0x01FF);
	}
}


void parseReceiveAck(void)
{
	if(infoHost.rx_ok !=true) return;   /* δ�յ�Ӧ������ */
	 
	if(str_cmp(DMA_Rec_Buf,"ok\r\n") || str_cmp(DMA_Rec_Buf,"ok\n"))
	{
		infoHost.wait=false;	
	}
	else
	{
		if(str_seen("ok"))
		{
			infoHost.wait=false;		
			if(infoHost.connected==false)
			{
				infoHost.connected=true;
			}
		}					
		if(str_seen("T:")) 
        {
			heatSetCurrentTemp(NOZZLE0,code_value()+0.5);
			if(str_seen("B:"))					
            {
				heatSetCurrentTemp(BED,code_value()+0.5);
			}
		}
		else if(str_seen("B:"))		
        {
			heatSetCurrentTemp(BED,code_value()+0.5);
		}
        else if(infoMenu.menu[infoMenu.cur] != menuPopup)
        {
            if(str_seen(errormagic))
            {
                popupSetContext((u8* )errormagic, (u8 *)DMA_Rec_Buf + code_index, textSelect(LABEL_CONFIRM), NULL);
                infoMenu.menu[++infoMenu.cur] = menuPopup;
            }
            else if(infoHost.connected && str_seen(echomagic))
            {
                popupSetContext((u8* )echomagic, (u8 *)DMA_Rec_Buf + code_index, textSelect(LABEL_CONFIRM), NULL);
                infoMenu.menu[++infoMenu.cur] = menuPopup;
            }    
        }
	}
    
	infoHost.rx_ok=false;
	USART1_DMAReEnable();
}









