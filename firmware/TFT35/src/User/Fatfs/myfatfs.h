#ifndef _MYFATFS_H_
#define _MYFATFS_H_

#include "stdint.h"

#define FOLDER_NUM   255
#define FILE_NUM     255
#define MAX_PATH_LEN 1024

typedef struct 
{
	char title[MAX_PATH_LEN];			    //·���������1024�ֽ�
	char *folder[FOLDER_NUM];				//���255���ļ���
	char *file[FILE_NUM];					//���255���ļ�
	uint16_t F_num;
	uint16_t f_num;
	uint16_t cur_page;					    //��ǰ��ʾ�ĵڼ�ҳ
}MYFILE;	

extern MYFILE  infoFile;

char mountSDCard(void);
char scanPrintFiles(void);
char EnterDir(char *nextdir);
void ExitDir(void);
char IsRootDir(void);
void clearInfoFile(void);
void resetInfoFile(void);
char Get_NewestGcode(const char* path);

#endif 
