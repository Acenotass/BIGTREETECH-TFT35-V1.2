#include "heat.h"
#include "includes.h"

//1��title(����), ITEM_PER_PAGE��item(ͼ��+��ǩ) 
MENUITEMS heatItems = {
//  title
LABEL_HEAT,
// icon                       label
 {{ICON_DEC,                  LABEL_DEC},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_INC,                  LABEL_INC},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_5_DEGREE,             LABEL_5_DEGREE},
  {ICON_STOP,                 LABEL_STOP},
  {ICON_BACK,                 LABEL_BACK},}
};


const ITEM itemTool[TOOL_NUM] = {
// icon                       label
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_BED,                  LABEL_BED},
};    

#define ITEM_DEGREE_NUM 3
const ITEM itemDegree[ITEM_DEGREE_NUM] = {
// icon                       label
  {ICON_1_DEGREE,             LABEL_1_DEGREE},
  {ICON_5_DEGREE,             LABEL_5_DEGREE},
  {ICON_10_DEGREE,            LABEL_10_DEGREE},
};
const  u8 item_degree[ITEM_DEGREE_NUM] = {1, 5, 10};
static u8 item_degree_i = 1;

u16 heat_max_temp[TOOL_NUM]=HEAT_MAX_TEMP;
static HEATER  heater;
static u32     update_time = 300;
static bool    update_waiting = false;
static bool    send_waiting[TOOL_NUM];

/*����Ŀ���¶�*/
void heatSetTargetTemp(TOOL tool,u16 temp)
{
  heater.target[tool]=temp;
}

/*��ȡĿ����¶�*/
u16 heatGetTargetTemp(TOOL tool)
{
  return heater.target[tool];
}

/* ���õ�ǰ���¶� */
void heatSetCurrentTemp(TOOL tool, s16 temp)
{
  heater.current[tool]=limitValue(-99, temp, 999);
}

/* ��ȡ��ǰ���¶� */
s16 heatGetCurrentTemp(TOOL tool)
{
  return heater.current[tool];
}

/* �Ƿ�ȴ����������� */
bool heatGetIsWaiting(TOOL tool)
{
  return heater.waiting[tool];
}

/* ��ѯ�Ƿ�����Ҫ�ȴ��ļ����� */
bool heatHasWaiting(void)
{
  TOOL i;
  for(i=NOZZLE0; i<TOOL_NUM; i++)
  {
    if(heater.waiting[i] == true)
    return true;
  }
  return false;
}

/* �����Ƿ�ȴ����������� */
void heatSetIsWaiting(TOOL tool,bool isWaiting)
{
  heater.waiting[tool] = isWaiting;
  if(isWaiting == true)
  {
    update_time = 100;
  }
  else if(heatHasWaiting() == false)
  {
    update_time = 300;		
  }
}

/* ���õ�ǰ����ͷ�����ȴ� */
void heatSetCurrentTool(TOOL tool)
{
  heater.tool=tool;
}
/* ��ȡ��ǰ����ͷ�����ȴ� */
TOOL heatGetCurrentTool(void)
{
  return heater.tool;
}

/* ���ò�ѯ�¶ȵ�ʱ���� */
void heatSetUpdateTime(u32 time)
{
  update_time=time;
}
/* ���õ�ǰ�Ƿ���Ҫ��ѯ�¶� */
void heatSetUpdateWaiting(bool isWaiting)
{
  update_waiting = isWaiting;
}

/* �����Ƿ��Ѿ����ͼ������� */
void heatSetSendWaiting(TOOL tool, bool isWaiting)
{
  send_waiting[tool] = isWaiting;
}

void showTemperature(void)
{
  const char* const label[TOOL_NUM] = {" E0","Bed"};
  GUI_DispString(220,90,(u8*)label[heater.tool],1);
  GUI_DispDec(232-12*3,120,heater.current[heater.tool],3,1,RIGHT);
  GUI_DispChar(232,120,'/',0);
  GUI_DispDec(232+12,120,heater.target[heater.tool],3,1,LEFT);
}

void currentReDraw(void)
{
  GUI_DispDec(232-12*3,120,heater.current[heater.tool],3,1,RIGHT);
}

void targetReDraw(void)
{
  GUI_DispDec(232+12,120,heater.target[heater.tool],3,1,LEFT);
}

void menuHeat(void)
{
  HEATER      nowHeater = heater;
  KEY_VALUES  key_num = KEY_IDLE;

  update_time=100;

  menuDrawPage(&heatItems);
  showTemperature();

  while(infoMenu.menu[infoMenu.cur] == menuHeat)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        if(heater.target[heater.tool]>0)
        {
          heater.target[heater.tool] = 
            limitValue( 0, 
                        heater.target[heater.tool] - item_degree[item_degree_i], 
                        heat_max_temp[heater.tool]);
        }
        break;
      
      case KEY_ICON_3:
        if(heater.target[heater.tool] < heat_max_temp[heater.tool])
        {
          heater.target[heater.tool] = 
            limitValue( 0, 
                        heater.target[heater.tool] + item_degree[item_degree_i], 
                        heat_max_temp[heater.tool]);
        }
        break;
        
      case KEY_ICON_4:                
        heater.tool = (TOOL)((heater.tool+1) % TOOL_NUM);
        heatItems.items[key_num] = itemTool[heater.tool];
        menuDrawItem(&heatItems.items[key_num], key_num);
        showTemperature();
        break;
      
      case KEY_ICON_5:
        item_degree_i = (item_degree_i+1) % ITEM_DEGREE_NUM;
        heatItems.items[key_num] = itemDegree[item_degree_i];
        menuDrawItem(&heatItems.items[key_num], key_num);
        break;
      
      case KEY_ICON_6:
        heater.target[heater.tool] = 0;
        break;
      
      case KEY_ICON_7:
        infoMenu.cur--;
        break;
      
      default :
        break;
    }

    if(nowHeater.current[heater.tool] != heater.current[heater.tool])
    {			
      nowHeater.current[heater.tool] = heater.current[heater.tool];
      currentReDraw();
    }

    char *heatCmd[TOOL_NUM]={"M104","M140"};
    if(nowHeater.target[heater.tool] != heater.target[heater.tool])
    {
      nowHeater.target[heater.tool] = heater.target[heater.tool];
      targetReDraw();
      if(send_waiting[heater.tool] != true)
      {
        send_waiting[heater.tool] = true;
        storeCmd("%s ",heatCmd[heater.tool]);
      }
    }

    loopProcess();
  }
  if(heatHasWaiting()==false)
  update_time=300;
}


void loopCheckHeater(void)
{
  u8 i;
  static u32  nowTime=0;

  do
  {  /* ��ʱ����M105��ѯ�¶�	*/
    if(update_waiting == true)                {nowTime=OS_GetTime();break;}
    if(OS_GetTime()<nowTime+update_time)       break;

    if(storeCmd("M105\n")==false)              break;

    nowTime=OS_GetTime();
    update_waiting=true;
  }while(0);

  /* ��ѯ��Ҫ�ȴ��¶������ļ��������Ƿ�ﵽ�趨�¶� */
  for(i=0;i<TOOL_NUM;i++)
  {
    if(heater.waiting[i] == false)                                        continue;
    if( i==BED && heater.current[BED]+2 <= heater.target[BED])            continue;
    if( i!=BED && inRange(heater.current[i],heater.target[i],2)!=true)    continue;

    heater.waiting[i]=false;
    if(heatHasWaiting()==true)                                             continue;

    if(infoMenu.menu[infoMenu.cur] == menuHeat)                            break;
    update_time=300;
  }
}





