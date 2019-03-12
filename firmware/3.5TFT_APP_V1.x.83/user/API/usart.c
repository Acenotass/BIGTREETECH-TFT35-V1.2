#include "usart.h"
#include "stdio.h"
#include "includes.h"
#include <string.h>


char DMA_Rec_Buf[MAX_ACK_SIZE];

static char code_index=0;
static char code_seen(char code)
{
	for(code_index=0;DMA_Rec_Buf[code_index]!=0;code_index++)
	{
		if(DMA_Rec_Buf[code_index]==code)
		return true;
	}
	return false;
}
static char str_seen(char *str)
{
	char *addr=strstr(DMA_Rec_Buf,str);
	if(addr!=NULL)
	{
		code_index=(addr-DMA_Rec_Buf+strlen(str)-1);
		return true;
	}
	return false;
}
static float code_value()
{
	return (strtod(&DMA_Rec_Buf[code_index+2],NULL));
}


void USART1_Config(u32 bound)
{
	
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef		DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE);		//����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);
	

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate=bound;
	USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART1,&USART_InitStructure);


	//�����ж�ͨ���Լ����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_ClearITPendingBit(USART1,USART_IT_IDLE);
	//���������ж� RXNEΪ�����ж�
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
	
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);		//ʹ�ܴ��ڽ���DMA
	USART_Cmd(USART1, ENABLE);											//ʹ�ܴ���
	
	DMA_DeInit(USART1_DMA_CHANNEL);															//ȱʡ��ʼ��DMA
	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)(&USART1->DR);		//���������ַΪ���ڽ������ݼĴ���
	DMA_InitStructure.DMA_MemoryBaseAddr=(u32)DMA_Rec_Buf;					//�ڴ��ַ��Ϊ���յ����ݴ�ŵĵ�ַ
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;							//����������ΪԴ��ַ
	DMA_InitStructure.DMA_BufferSize=MAX_ACK_SIZE;									//���ó���
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;	//�����ַ������
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;						//�ڴ��ַ����
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;		//���ݳ�1���ֽ�
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;			//����ģʽ
	DMA_InitStructure.DMA_Priority=DMA_Priority_Medium;	//�ж����ȼ�
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;		//ʧ���ڴ浽�ڴ�
	
	DMA_Init(USART1_DMA_CHANNEL,&DMA_InitStructure);
	DMA_Cmd(USART1_DMA_CHANNEL,ENABLE);							//ʹ��DMA
	
	
}


/*
	��������My_DMAEnable(DMA_Channel_TypeDef* DMAy_Channelx)
	�������ܣ���������DMA�ռ�
	��������
	����ֵ����
*/
void My_DMAEnable(DMA_Channel_TypeDef* DMAy_Channelx)
{
	memset(DMA_Rec_Buf,0,MAX_ACK_SIZE);
	DMA_Cmd(DMAy_Channelx,DISABLE);
	DMA_SetCurrDataCounter(DMAy_Channelx,MAX_ACK_SIZE);
	DMA_Cmd(DMAy_Channelx,ENABLE);
}

void USART1_IRQHandler(void)
{
	u8 DMA_bufrx=0;
	if(USART_GetITStatus(USART1,USART_IT_IDLE)!=RESET)
	{
		USART_ClearITPendingBit(USART1,USART_IT_IDLE);
		USART_ReceiveData(USART1);											//DMA+���ڿ����ж�  �жϷ������������һ�䣬��Ȼ���������֮�������
		
		DMA_bufrx=MAX_ACK_SIZE-DMA_GetCurrDataCounter(USART1_DMA_CHANNEL);
		
	
		if(DMA_Rec_Buf[DMA_bufrx-1]=='\n')
		{
			infoUSART1_CMD.rx_ok=1;
		}
		else if(DMA_bufrx>MAX_ACK_SIZE-5)
		{
			My_DMAEnable(USART1_DMA_CHANNEL);
			infoUSART1_CMD.wait=0;
		}
	}
}


/*
	��������USART_Putc(USART_TypeDef* USARTx,u8 data)
	�������ܣ����ڷ����ַ�
	������USARTX�����ĸ����ڷ�   data��Ҫ���͵��ַ�
	����ֵ����
*/
void USART_Putc(USART_TypeDef* USARTx,char data)
{
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);
	USART_SendData(USARTx,(u16)data);
}
/*
	��������USART1_Puts(u8 *s)
	�������ܣ��Ӵ���1�����ַ���
	������ *s��Ҫ���͵��ַ���
	����ֵ����
*/
void USART1_Puts(char *s )
{
	while (*s)
	{
		while((USART1->SR & USART_FLAG_TC) == (uint16_t)RESET);
		USART1->DR = ((u16)*s++ & (uint16_t)0x01FF);
	}
}
/*
	��������int fputc(int ch,FILE *f)
	�������ܣ�printf������ǰ�ú���
	������Ҫ��ӡ����Ϣ
	����ֵ����ӡ����Ϣ
*/
int fputc(int ch,FILE *f)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
	USART_SendData(USART1,ch);
	return ch;
}





static u8 cmd_index=0;

static bool cmd_seen(char code)
{
//	strchr_pointer=strchr(infoCMD.queue[infoCMD.index_r],code);
	
	for(cmd_index=0;infoCMD.queue[infoCMD.index_r][cmd_index]!=0;cmd_index++)
	{
		if(infoCMD.queue[infoCMD.index_r][cmd_index]==code)
		return true;
	}
	return false;
}
static u16 cmd_value(void)
{
	return (strtol(&infoCMD.queue[infoCMD.index_r][cmd_index+1],NULL,10));
}
static float cmd_float(void)
{
	return (strtod(&infoCMD.queue[infoCMD.index_r][cmd_index+1],NULL));
}
	
void Process_CMD(void)
{
	if(infoUSART1_CMD.rx_ok==1)
	{
		infoUSART1_CMD.rx_ok=0;
		if(DMA_Rec_Buf[0]=='o'&&DMA_Rec_Buf[1]=='k'&&DMA_Rec_Buf[2]=='\n')
		{
			infoUSART1_CMD.wait=0;
		}
		else
		{			
			if(code_seen('T'))
			{
				infoHeat.cur_nozzle=code_value()+0.5;
				if(code_seen('B'))
				{
					infoHeat.cur_bed=code_value()+0.5;
				}
			}
			else if(str_seen("Z Offset :"))
			{
				infoPrinting.baby_step=code_value();
			}
			if(strstr(DMA_Rec_Buf,"ok"))
			{
				infoUSART1_CMD.wait=0;
			}
		}
		if(infoUSART1_CMD.link_flag==0)
		{
				err_flag=2;
				infoUSART1_CMD.link_flag=1;
		}
		My_DMAEnable(USART1_DMA_CHANNEL);
	}
}

void	loopInfo(void)
{
	if(infoHeatUpdate.update)				//����������������л����ڡ�M105�����򲻽����ظ��洢 
	{
		if(storeCmd("M105\n")==true)
		{
			infoHeatUpdate.update=false;
			infoHeatUpdate.wait_M105=true;    //������������С�M105������
		}
	}
	if(err_flag==1)
	{
		GUI_FillRect(160,10,479,33,BLACK);
		
//		GUI_DispString(10,10,textSelect(cur_view),0);
		if(cur_view==MENU_PRINTING)
		{
			GUI_DispString(10+(my_strlen(textSelect(PRINTING)))*12,10,(void *)infoPrint.dispfile,0);				
		}
		else if(cur_view==MENU_OUTAGE)
		{
			GUI_DispString(10,10,(u8 *)infoOutage.path,0);
		}
		err_flag=0;
	}
}

bool isLimit(int cur, int tag, u8 range)
{
	if(cur>tag+range)
		return false;
	if(cur<tag-range)
		return false;
	return true;
}


void Send_CMD(void)
{
	u16 cmd=0;
	static bool relative=false;
	
	if(infoCMD.count>0&&infoUSART1_CMD.wait==0)
	{
		switch(infoCMD.queue[infoCMD.index_r][0])
		{
			case 'M':
				cmd_seen('M');
				cmd=cmd_value(); 
				switch(cmd)
				{
					case 109: 																								//M109
						infoCMD.queue[infoCMD.index_r][3]='4';
						infoPrinting.wait=1;					
						infoHeatUpdate.get_time=100;
					case 104:																									//M104
						if(cmd_seen('S'))
						{
							infoHeat.tag_nozzle=cmd_value(); 
							infoOutage.nozzle=infoHeat.tag_nozzle;
						}
						break;
						
					case 190: 																								//M190
						infoCMD.queue[infoCMD.index_r][2]='4';
						infoPrinting.wait=2;					
						infoHeatUpdate.get_time=100;
					case 140:																									//M140
						if(cmd_seen('S'))
						{
							infoHeat.tag_bed=cmd_value(); 
							infoOutage.bed=infoHeat.tag_bed;
						}
						break;
						
					case 105:																									//M105					
						infoHeatUpdate.wait_M105=false;
						break;
						
					case 106:																									//M106
						if(cmd_seen('S'))
						{
							infoFan.fan_speed=cmd_value();										
							infoOutage.fan=infoFan.fan_speed;
						}
						break;
						
					case 107:																									//M107
						infoFan.fan_speed=0; 
						infoOutage.fan=0;
						break;
				}
			break;
			case 'G':
				cmd_seen('G');
				cmd=cmd_value(); 
				switch(cmd)
				{
					case 0:;
					case 1:
						if(relative==false)
						{
 							if(cmd_seen('X'))			infoOutage.x=cmd_float(); 
							if(cmd_seen('Y'))			infoOutage.y=cmd_float(); 
							if(cmd_seen('Z'))			infoOutage.z=cmd_float(); 
							if(cmd_seen('E'))			infoOutage.e=cmd_float(); 
							if(cmd_seen('F'))			infoOutage.f=cmd_float(); 
						}
					break;
					case 90:
						relative=false;
					break;
					case 91:
						relative=true;
					break;
				}			
			break;				
		}

		infoUSART1_CMD.wait=1;
		
		USART1_Puts(infoCMD.queue[infoCMD.index_r]);
		--infoCMD.count;
		infoCMD.index_r = (infoCMD.index_r + 1) % BUFSIZE;
		
		outage_save|=0x01;
	}
	else if(infoUSART1_CMD.link_flag==0)
	{
		infoUSART1_CMD.wait=0;
	}

	switch(infoPrinting.wait)
	{
		case 1:	
			if(isLimit(infoHeat.cur_nozzle,infoHeat.tag_nozzle,2)==true)
			{
				infoPrinting.wait=0;		
				infoHeatUpdate.get_time=300;
			}
			break;
		case 2:
			if(infoHeat.cur_bed>=infoHeat.tag_bed-2)
			{
				infoPrinting.wait=0;		
				infoHeatUpdate.get_time=300;			
			}
			break;
	}
}

void loopProcess (void)
{

	Process_CMD();
	
	loopInfo();
	
	Send_CMD();
}
