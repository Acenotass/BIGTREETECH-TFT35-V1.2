#include "PowerFailed.h"
#include "includes.h"

BREAK_POINT  infoBreakPoint = {{0.0f,0.0f,0.0f,0.0f},3000,100,100,{0,0},0,0,false,false};

static bool powerFailedSave = false;
static bool create_ok = false;

void powerFailedEnable(bool enable)
{
  powerFailedSave = enable;
}

FIL fpPowerFailed;
bool powerFailedCreate(char *path) 
{ 
  UINT br;

  if(f_open(&fpPowerFailed, BREAK_POINT_FILE, FA_OPEN_ALWAYS | FA_WRITE)!=FR_OK)
  {
    create_ok = false;
    return false;
  }
  f_write(&fpPowerFailed, path, MAX_PATH_LEN, &br);
  f_write(&fpPowerFailed, &infoBreakPoint, sizeof(BREAK_POINT), &br);
  f_sync(&fpPowerFailed);

  create_ok = true;
  return true;	
}


void powerFailedCache(u32 offset) 
{
  UINT         br;
  static u32  nowTime = 0;

  if( OS_GetTime() < nowTime + 100)     return;
  if( create_ok == false )              return;
  if( powerFailedSave == false)         return;
  if( isPause() == true)                return;

  powerFailedSave = false;

  nowTime = OS_GetTime();

  infoBreakPoint.offset = offset;
  for(AXIS i=X_AXIS; i<TOTAL_AXIS; i++)
  {
    infoBreakPoint.axis[i] = coordinateGetAxis(i);
  }
  infoBreakPoint.feedrate = coordinateGetFeedRate();
  infoBreakPoint.speed = speedGetPercent(0);
  infoBreakPoint.flow = speedGetPercent(1);
  for(TOOL i=NOZZLE0; i<TOOL_NUM; i++)
  {
    infoBreakPoint.target[i] = heatGetTargetTemp(i);
  }
  infoBreakPoint.fan = fanGetSpeed();
  infoBreakPoint.relative = coorGetRelative();
  infoBreakPoint.relative_e = eGetRelative();

  f_lseek(&fpPowerFailed, MAX_PATH_LEN);
  f_write(&fpPowerFailed, &infoBreakPoint, sizeof(BREAK_POINT), &br);
  f_sync(&fpPowerFailed);
}

void powerFailedClose(void) 
{
  if(create_ok==false)   return;

  f_close(&fpPowerFailed);		
}

void  powerFailedDelete(void) 
{	
  if(create_ok==false)   return;

  f_close(&fpPowerFailed);	
  f_unlink(BREAK_POINT_FILE);	
}



static bool powerOffExist(void)
{
  FIL  fp;
  UINT br;
  if(f_open(&fp, BREAK_POINT_FILE, FA_OPEN_EXISTING|FA_READ) != FR_OK)    return false;
  if(f_read(&fp, infoFile.title,   MAX_PATH_LEN, &br) != FR_OK)           return false;
  if(f_close(&fp) != FR_OK)                                               return false;

  return true;
}


bool powerFailedlSeek(FIL* fp)
{
  if(f_lseek(fp,infoBreakPoint.offset) != FR_OK)  return false;
  
  return true;
}


bool powerOffGetData(void)
{
  FIL 	fp;
  UINT	br;

  if(f_open(&fp, BREAK_POINT_FILE, FA_OPEN_EXISTING|FA_READ)    != FR_OK)        return false;
  if(f_lseek(&fp, MAX_PATH_LEN)                                 != FR_OK)        return false;	
  if(f_read(&fp, &infoBreakPoint,  sizeof(infoBreakPoint), &br) != FR_OK)        return false;

  mustStoreCmd("M190 S%d\n", infoBreakPoint.target[BED]);
  if(infoBreakPoint.target[NOZZLE0] != 0)
    mustStoreCmd("M109 S%d\n", infoBreakPoint.target[NOZZLE0]);
  mustStoreCmd("G28\n");

  mustStoreCmd("M106 S%d\n", infoBreakPoint.fan);
  if(infoBreakPoint.feedrate != 0)
  {        
    mustStoreCmd("G1 Z%d\n", limitValue(0,infoBreakPoint.axis[Z_AXIS]+10,400));
    mustStoreCmd("M83\n");
    mustStoreCmd("G1 E30 F300\n");
    mustStoreCmd("G1 E-10 F4800\n");
    mustStoreCmd("G1 X%.5f Y%.5f Z%.5f F3000\n", 
      infoBreakPoint.axis[X_AXIS],
      infoBreakPoint.axis[Y_AXIS],
      infoBreakPoint.axis[Z_AXIS]);
    mustStoreCmd("G1 E10 F4800\n");
    mustStoreCmd("G92 E%.5f\n",infoBreakPoint.axis[E_AXIS]);
    mustStoreCmd("G1 F%d\n",infoBreakPoint.feedrate);        

    if(infoBreakPoint.relative_e == false)
    {
      mustStoreCmd("M82\n");
    }
    if(infoBreakPoint.relative == true)
    {
      mustStoreCmd("G91\n");
    }
  }

  f_close(&fp);
  return true;
}


#define POPUP_CONFIRM_RECT {90,  210, 210, 260}
#define POPUP_CANCEL_RECT  {270, 210, 390, 260}

static BUTTON bottomBtn[] = {
  //ȷ����ť                            ����״̬                ����״̬
  {POPUP_CONFIRM_RECT, NULL, 5, 1, GREEN, BLACK, WHITE,   GREEN, WHITE, BLACK},
  {POPUP_CANCEL_RECT,  NULL, 5, 1, GREEN, BLACK, WHITE,   GREEN, WHITE, BLACK},
};

static const GUI_RECT powerFailedRect[] ={POPUP_CONFIRM_RECT, POPUP_CANCEL_RECT};

void menuPowerOff(void)
{
  u16 key_num = IDLE_TOUCH;
  memset(&infoBreakPoint, 0, sizeof(infoBreakPoint));
  GUI_Clear(BK_COLOR);
  GUI_DispString((LCD_WIDTH - my_strlen(textSelect(LABEL_LOADING))*BYTE_WIDTH)/2, 130, textSelect(LABEL_LOADING),1);

  if(mountSDCard()==true && powerOffExist())
  {
    TSC_ReDrawIcon = windowReDrawButton;
    bottomBtn[0].context = textSelect(LABEL_CONFIRM);
    bottomBtn[1].context = textSelect(LABEL_CANNEL);
    windowSetButton(bottomBtn, 2);
    GUI_DrawWindow(&window, textSelect(LABEL_POWER_FAILED), (u8* )infoFile.title);        
    GUI_DrawButton(&bottomBtn[0], 0);
    GUI_DrawButton(&bottomBtn[1], 0); 
    while(infoMenu.menu[infoMenu.cur]==menuPowerOff)
    {
      key_num = KEY_GetValue(2,powerFailedRect);
      switch(key_num)
      {
        case KEY_POPUP_CONFIRM:		
          powerOffGetData();
          infoMenu.menu[1]=menuPrint;
          infoMenu.menu[2]=menuBeforePrinting;
          infoMenu.cur=2;
          break;
        
        case KEY_POPUP_CANCEL:	
          f_unlink(BREAK_POINT_FILE);
          ExitDir();
          infoMenu.cur--;
          break;				
      }
      loopProcess();
    }
  }
  else
  {
    infoMenu.cur--;
  }
}


