#include "diskio.h"	
#include "sd.h"	



#define SD_CARD	 0  //SD��,���Ϊ0


DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{ 
	return RES_OK;
}  


DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	u8 res=0;	    
	switch(pdrv)
	{
		case SD_CARD://SD��
			res=SD_Init();//SD����ʼ�� 
  			break;
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; //��ʼ���ɹ� 
} 

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	u8 res=0; 
    if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������		 	 
	switch(pdrv)
	{
		case SD_CARD://SD��
			res=SD_Read_Data(buff,sector,count);	 
			while(res)//������
			{
				SD_Init();	//���³�ʼ��SD��
				res=SD_Read_Data(buff,sector,count);	
			}
			break;
		default:
			res=1; 
	}
   //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res==0x00)return RES_OK;	 
    else return RES_ERROR;	   
}

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	u8 res=0;  
    if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������		 	 
	switch(pdrv)
	{
		case SD_CARD://SD��
			res=SD_Write_Data((u8*)buff,sector,count);
			while(res)//д����
			{
				SD_Init();	//���³�ʼ��SD��
				res=SD_Write_Data((u8*)buff,sector,count);	
				//printf("sd wr error:%d\r\n",res);
			}
			break;
		default:
			res=1; 
	}
    //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;	
}
#endif

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	u8 result= RES_OK;
	switch (pdrv)
	{
		case SD_CARD : 
				switch(cmd)
				{
					case CTRL_SYNC:
					SD_SPI_CS_Set(0);
					if(SD_Wait_Ready())result = RES_ERROR;/*�ȴ���׼����*/
					SD_SPI_CS_Set(1);
					break;
					case GET_SECTOR_SIZE:
					*(WORD*)buff = 512;/*sd �������ֽ���*/
					break;
					case GET_BLOCK_SIZE:
					*(WORD*)buff = 8;/*���С*/
					break;
					case GET_SECTOR_COUNT:
					*(DWORD*)buff = SD_Get_Sector_Count();/*��ȡ SD ������������*/
					break;
					default: result = RES_PARERR;
					break;
				}
				break;
		
		default:		break;
	}
	if(!result)return RES_OK;
	return RES_PARERR;
}
#endif
 

#include "stdlib.h"
void *ff_memalloc(UINT msize)
{
	return malloc(msize);
}
void ff_memfree(void *mblock)
{
	free(mblock);
}
DWORD get_fattime (void)
{
	u32 date;
	date=(	((2019-1980)<<25)|(3<<21)|(15<<16)|(16<<11)|(40<<5)|(0)  );
	return date;
}

