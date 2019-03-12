#include "spi.h"
#include "includes.h"
 
//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ������SD Card/W25Q64/NRF24L01						  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI2�ĳ�ʼ��


SPI_InitTypeDef  SPI_InitStructure;
void SPI2_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	
   /* Enable the SPI periph */
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
   
   /* Enable SCK, MOSI, MISO and NSS GPIO clocks */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15); 

	 
   /* SPI configuration -------------------------------------------------------*/

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI2, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPI2, ENABLE); //ʹ��SPI����
	
//	SPI2_ReadWriteByte(0xff);
}   
//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ   
//SPI_BaudRatePrescaler_8   8��Ƶ   
//SPI_BaudRatePrescaler_16  16��Ƶ  
//SPI_BaudRatePrescaler_256 256��Ƶ 
  
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler;
	SPI_Init(SPI2,&SPI_InitStructure);
	SPI_Cmd(SPI2,ENABLE); 

} 

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				

	while (((SPI2->SR)&SPI_I2S_FLAG_TXE)== RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI2->DR = (u16)TxData; //ͨ������SPIx����һ������
	retry=0;

	while (((SPI2->SR)&SPI_I2S_FLAG_RXNE)== RESET)//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return (u8)(SPI2->DR); //����ͨ��SPIx������յ�����					    
}


//void DMA1_Init(void)
//{
//	DMA_InitTypeDef DMA_InitStructure;
//	
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
//	

////	
//	//SPI1 DMA
//	DMA_DeInit(DMA1_Channel3);
//	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&SPI1->DR;
//	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralDST;
//	DMA_InitStructure.DMA_BufferSize=512;
//	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
//	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
//	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;
//	DMA_InitStructure.DMA_Priority=DMA_Priority_High;
//	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;
//	DMA_Init(DMA1_Channel3,&DMA_InitStructure);
//	
//	DMA_DeInit(DMA1_Channel2);
//	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&SPI1->DR;
//	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;
//	DMA_InitStructure.DMA_BufferSize=512;
//	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
//	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
//	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;
//	DMA_InitStructure.DMA_Priority=DMA_Priority_High;
//	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;
//	DMA_Init(DMA1_Channel2,&DMA_InitStructure);
//	
//	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Rx,ENABLE);
//	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);
//}


//void DMA1_Star_SPI1_RX(u8 *buffer,u16 len)
//{
//    u8 temp=0xff;
//    	
//		DMA1_Channel2->CNDTR=len; 
//		DMA1_Channel2->CMAR=(uint32_t)buffer; 
//		

//		DMA1_Channel3->CNDTR=len; 
//		DMA1_Channel3->CMAR=(uint32_t)&temp; 
//		DMA1_Channel3->CCR&=~DMA_MemoryInc_Enable; 
//		
//		DMA_Cmd(DMA1_Channel2,ENABLE); 
//		DMA_Cmd(DMA1_Channel3,ENABLE); 
//		while(!DMA_GetFlagStatus(DMA1_FLAG_TC2)); 
//		DMA_ClearFlag(DMA1_FLAG_TC2); 
//		DMA_ClearFlag(DMA1_FLAG_TC3);
//		DMA_Cmd(DMA1_Channel2,DISABLE);
//		DMA_Cmd(DMA1_Channel3,DISABLE); 
//		
//		DMA1_Channel3->CCR|=DMA_MemoryInc_Enable;

//}

////��sd��д��һ�����ݰ������� 512�ֽ� ����DMA����
////buf:���ݻ�����
//void DMA1_Star_SPI1_TX(u8 *buffer,u16 len)
//{
//		DMA1_Channel3->CNDTR=len; //����Ҫ��������ݳ���
//		DMA1_Channel3->CMAR=(uint32_t)buffer; //����RAM��������ַ
//		
//		DMA_Cmd(DMA1_Channel3,ENABLE); //����DMA���� RAM->SPI
//		while(!DMA_GetFlagStatus(DMA1_FLAG_TC3)); //�ȴ�DMAͨ��5�������
//		DMA_ClearFlag(DMA1_FLAG_TC3); //���ͨ��5�������״̬���
//		DMA_Cmd(DMA1_Channel3,DISABLE); //ʹDMAͨ��5ֹͣ����
//}


























