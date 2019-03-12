#include "W25Qxx.h"
#include "includes.h"

#define W25QXX_TINY		1


void W25QXX_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	//MOSI -- PA7 SCK -- PA5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//CS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//MISO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	W25QXX_CS_HIGH();

	
	//SPIЭ��ĳ�ʼ��
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1,&SPI_InitStructure);

	SPI_Cmd(SPI1,ENABLE);
	W25QXX_CS_LOW();

}



u8 SPI1_Send_Receive_Byte(u8 data)
{
	while((SPI1->SR & SPI_I2S_FLAG_TXE) == (uint16_t)RESET);
	 SPI1->DR = data;
	while((SPI1->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);
	return (SPI1->DR);
}


#if	W25QXX_TINY==0
/****************************************************
��������W25QXX_WriteEnable
�βΣ�
����ֵ��
�������ܣ�дʹ��
****************************************************/
void W25QXX_WriteEnable(void)
{
  /*!< Select the FLASH: Chip Select low */
  W25QXX_CS_LOW();

  /*!< Send "Write Enable" instruction */
  SPI1_Send_Receive_Byte(CMD_WRITE_ENABLE);

  /*!< Deselect the FLASH: Chip Select high */
  W25QXX_CS_HIGH();
}
/****************************************************
��������W25QXX_WaitForWriteEnd
�βΣ�  
����ֵ��
�������ܣ��ж�FLASH�Ƿ�BUSY
****************************************************/
void W25QXX_WaitForWriteEnd(void)
{
  uint8_t flashstatus = 0;

  /*!< Select the FLASH: Chip Select low */
  W25QXX_CS_LOW();

  /*!< Send "Read Status Register" instruction */
  SPI1_Send_Receive_Byte(CMD_READ_REGISTER1);

  /*!< Loop as long as the memory is busy with a write cycle */
  do
  {
    /*!< Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    flashstatus = SPI1_Send_Receive_Byte(FLASH_DUMMY_BYTE);

  }
  while ((flashstatus & 0x01) == SET); /* Write in progress */

  /*!< Deselect the FLASH: Chip Select high */
  W25QXX_CS_HIGH();
}
/*
��FLASH��д���ݣ�дһ�ε����ݴ�СΪС��256��
1.�ж�FLASH�Ƿ���æ״̬ 1 æ  0 ��æ
2.дʹ��
3.ƬѡCS����
4.����д����
5.����24λ��ַ
6.��SPI��дС��4K���ȵ�����
7.CS����
8.�ж�FLASH״̬�Ƿ�æ
*/
/****************************************************
��������W25QXX_WritePage
�βΣ�  pBuffer -- Ҫ��ŵ���
        WriteAddr -- �����ĵ�ַ
        NumByteToWrite -- �����ĸ���
����ֵ��
�������ܣ�FLASH��ҳд����
****************************************************/
void W25QXX_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /*!< Enable the write access to the FLASH */
  W25QXX_WriteEnable();

  /*!< Select the FLASH: Chip Select low */
  W25QXX_CS_LOW();
  /*!< Send "Write to Memory " instruction */
  SPI1_Send_Receive_Byte(CMD_PAGE_PROGRAM);
  /*!< Send WriteAddr high nibble address byte to write to */
  SPI1_Send_Receive_Byte((WriteAddr & 0xFF0000) >> 16);
  /*!< Send WriteAddr medium nibble address byte to write to */
  SPI1_Send_Receive_Byte((WriteAddr & 0xFF00) >> 8);
  /*!< Send WriteAddr low nibble address byte to write to */
  SPI1_Send_Receive_Byte(WriteAddr & 0xFF);

  /*!< while there is data to be written on the FLASH */
  while (NumByteToWrite--)
  {
    /*!< Send the current byte */
    SPI1_Send_Receive_Byte(*pBuffer);
    /*!< Point on the next byte to be written */
    pBuffer++;
  }
//	DMA1_Star_SPI1_TX(pBuffer,NumByteToWrite);
	
	
  /*!< Deselect the FLASH: Chip Select high */
  W25QXX_CS_HIGH();

  /*!< Wait the end of Flash writing */
  W25QXX_WaitForWriteEnd();
}

//��FLASH�з���buffer������
/****************************************************
��������W25QXX_WriteBuffer
�βΣ�  pBuffer -- Ҫ��ŵ���
        WriteAddr -- �����ĵ�ַ
        NumByteToWrite -- �����ĸ���
����ֵ��
�������ܣ���FLASHд����
****************************************************/
void W25QXX_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % FLASH_SPI_PAGESIZE;//�����һҳ���ĸ���ַ��ʼд����
  count = FLASH_SPI_PAGESIZE - Addr;//�����ҳ������д��������
  NumOfPage =  NumByteToWrite / FLASH_SPI_PAGESIZE;//����Ҫ��������ݳ��ȿ���д����ҳ
  NumOfSingle = NumByteToWrite % FLASH_SPI_PAGESIZE;//���������ҳ�����⣬������д�����ݳ���

  if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      W25QXX_WritePage(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      while(NumOfPage--)
      {
        W25QXX_WritePage(pBuffer, WriteAddr, FLASH_SPI_PAGESIZE);
        WriteAddr +=  FLASH_SPI_PAGESIZE;
        pBuffer += FLASH_SPI_PAGESIZE;
      }
      W25QXX_WritePage(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sFLASH_PAGESIZE */
      {
        temp = NumOfSingle - count;

        W25QXX_WritePage(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        W25QXX_WritePage(pBuffer, WriteAddr, temp);
      }
      else
      {
        W25QXX_WritePage(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / FLASH_SPI_PAGESIZE;
      NumOfSingle = NumByteToWrite % FLASH_SPI_PAGESIZE;

      W25QXX_WritePage(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        W25QXX_WritePage(pBuffer, WriteAddr, FLASH_SPI_PAGESIZE);
        WriteAddr +=  FLASH_SPI_PAGESIZE;
        pBuffer += FLASH_SPI_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        W25QXX_WritePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

//��ID
/****************************************************
��������W25QXX_ReadID
�βΣ�
����ֵ�����ض�����ID��
�������ܣ���ȡFLASH��ID��
****************************************************/
uint32_t W25QXX_ReadID(void)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /*!< Select the FLASH: Chip Select low */
  W25QXX_CS_LOW();

  /*!< Send "RDID " instruction */
  SPI1_Send_Receive_Byte(CMD_READ_ID);

  /*!< Read a byte from the FLASH */
  Temp0 = SPI1_Send_Receive_Byte(FLASH_DUMMY_BYTE);

  /*!< Read a byte from the FLASH */
  Temp1 = SPI1_Send_Receive_Byte(FLASH_DUMMY_BYTE);

  /*!< Read a byte from the FLASH */
  Temp2 = SPI1_Send_Receive_Byte(FLASH_DUMMY_BYTE);

  /*!< Deselect the FLASH: Chip Select high */
  W25QXX_CS_HIGH();

  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}

/****************************************************
��������W25QXX_EraseSector
�βΣ�  SectorAddr������ַ
����ֵ��
�������ܣ���������
****************************************************/
void W25QXX_EraseSector(uint32_t SectorAddr)
{
  /*!< Send write enable instruction */
  W25QXX_WriteEnable();

  /*!< Sector Erase */
  /*!< Select the FLASH: Chip Select low */
  W25QXX_CS_LOW();
  /*!< Send Sector Erase instruction */
  SPI1_Send_Receive_Byte(CMD_SECTOR_ERASE);
  /*!< Send SectorAddr high nibble address byte */
  SPI1_Send_Receive_Byte((SectorAddr & 0xFF0000) >> 16);
  /*!< Send SectorAddr medium nibble address byte */
  SPI1_Send_Receive_Byte((SectorAddr & 0xFF00) >> 8);
  /*!< Send SectorAddr low nibble address byte */
  SPI1_Send_Receive_Byte(SectorAddr & 0xFF);
  /*!< Deselect the FLASH: Chip Select high */
  W25QXX_CS_HIGH();

  /*!< Wait the end of Flash writing */
  W25QXX_WaitForWriteEnd();
}

void W25QXX_EraseBlock(uint32_t BlockAddr)
{
  /*!< Send write enable instruction */
  W25QXX_WriteEnable();

  /*!< Sector Erase */
  /*!< Select the FLASH: Chip Select low */
  W25QXX_CS_LOW();
  /*!< Send Sector Erase instruction */
  SPI1_Send_Receive_Byte(CMD_BLOCK_ERASE);
  /*!< Send SectorAddr high nibble address byte */
  SPI1_Send_Receive_Byte((BlockAddr & 0xFF0000) >> 16);
  /*!< Send SectorAddr medium nibble address byte */
  SPI1_Send_Receive_Byte((BlockAddr & 0xFF00) >> 8);
  /*!< Send SectorAddr low nibble address byte */
  SPI1_Send_Receive_Byte(BlockAddr & 0xFF);
  /*!< Deselect the FLASH: Chip Select high */
  W25QXX_CS_HIGH();

  /*!< Wait the end of Flash writing */
  W25QXX_WaitForWriteEnd();
}

void W25QXX_EraseBulk(void)
{
  /*!< Send write enable instruction */
  W25QXX_WriteEnable();

  /*!< Bulk Erase */
  /*!< Select the FLASH: Chip Select low */
  W25QXX_CS_LOW();
  /*!< Send Bulk Erase instruction  */
  SPI1_Send_Receive_Byte(CMD_FLASH__BE);
  /*!< Deselect the FLASH: Chip Select high */
  W25QXX_CS_HIGH();

  /*!< Wait the end of Flash writing */
  W25QXX_WaitForWriteEnd();
}

#endif

/****************************************************
��������W25QXX_ReadBuffer
�βΣ�  pBuffer -- ��ȡ�����ݱ��������
        ReadAddr -- ��ȡ��ַ
        NumByteToRead -- ��ȡ�����ĸ���
����ֵ��
�������ܣ���FLASH�ж�����
****************************************************/
void W25QXX_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  /*!< Select the FLASH: Chip Select low */
  W25QXX_CS_LOW();

  /*!< Send "Read from Memory " instruction */
  SPI1_Send_Receive_Byte(CMD_READ_DATA);

  /*!< Send ReadAddr high nibble address byte to read from */
  SPI1_Send_Receive_Byte((ReadAddr & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte to read from */
  SPI1_Send_Receive_Byte((ReadAddr& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte to read from */
  SPI1_Send_Receive_Byte(ReadAddr & 0xFF);

  while (NumByteToRead--) /*!< while there is data to be read */
  {
    /*!< Read a byte from the FLASH */
    *pBuffer = SPI1_Send_Receive_Byte(FLASH_DUMMY_BYTE);
    /*!< Point to the next location where the byte read will be saved */
    pBuffer++;
  }
//	DMA1_Star_SPI1_RX(pBuffer,NumByteToRead);

  /*!< Deselect the FLASH: Chip Select high */
  W25QXX_CS_HIGH();
}
