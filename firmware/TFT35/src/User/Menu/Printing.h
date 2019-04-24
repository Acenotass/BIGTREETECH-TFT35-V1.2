#ifndef _PRINTING_H_
#define _PRINTING_H_
#include "stm32f10x.h"
#include "stdbool.h"
#include "ff.h"

typedef struct 
{	
  FIL     file;

  u32     time;           //��ӡʱ��  ��λ����
  u32     size;           //����ӡ�ļ������ֽ���
  u32     cur;            //�Ѷ�ȡ�����ֽ���
  u8      progress;
  bool    printing;      //��ӡ��־  ���С�׼���С���ӡ�С���ӡ���
  bool    pause;          //1����ͣ  0��δ��ͣ
}PRINTING;



void exitPrinting(void);
void endPrinting(void);
void completePrinting(void);
void haltPrinting(void);

bool setPrintPause(bool is_pause);

bool isPrinting(void);	
bool isPause(void);
void setPrintingTime(u32 RTtime);

void setPrintSize(u32 size);
void setPrintCur(u32 cur);

u8   getPrintProgress(void);
u32  getPrintTime(void);

void startGcodeExecute(void);
void endGcodeExecute(void);

void getGcodeFromFile(void);

void menuBeforePrinting(void);
void menuPrinting(void);
void menuStopPrinting(void);
#endif
