#include "mainpage.h"
#include "includes.h"

//1��title(����), ITEM_PER_PAGE��item(ͼ��+��ǩ) 
const MENUITEMS mainPageItems = {
//   title
     LABEL_READY,
//   icon                       label
   {{ICON_HEAT,                 LABEL_HEAT},
    {ICON_MOVE,                 LABEL_MOVE},
    {ICON_HOME,                 LABEL_HOME},
    {ICON_PRINT,                LABEL_PRINT},
    {ICON_EXTRUDE,              LABEL_EXTRUDE},
    {ICON_FAN,                  LABEL_FAN},
    {ICON_SETTINGS,             LABEL_SETTINGS},
    {ICON_LEVELING,             LABEL_LEVELING},}
};

//template
//#define ITEM_DEGREE_NUM 3
//const ITEM itemDegree[ITEM_DEGREE_NUM] = {
////   icon                       label
//    {ICON_1_DEGREE,             LABEL_1_DEGREE},
//    {ICON_5_DEGREE,             LABEL_5_DEGREE},
//    {ICON_10_DEGREE,            LABEL_10_DEGREE},
//};
//const  u8 item_degree[ITEM_DEGREE_NUM] = {1, 5, 10};
//static u8 item_degree_i = 1;



void menuMain(void)
{
	KEY_VALUES key_num=KEY_IDLE;
	
    menuDrawPage(&mainPageItems);
  
	while(infoMenu.menu[infoMenu.cur] == menuMain)
	{
		key_num = menuKeyGetValue();
		switch(key_num)
		{
			case KEY_ICON_0: infoMenu.menu[++infoMenu.cur] = menuHeat;      break;
			case KEY_ICON_1: infoMenu.menu[++infoMenu.cur] = menuMove;      break;
			case KEY_ICON_2: infoMenu.menu[++infoMenu.cur] = menuHome;      break;
			case KEY_ICON_3: 
				resetInfoFile();
				infoMenu.menu[++infoMenu.cur] = menuPrint;
				infoMenu.menu[++infoMenu.cur] = menuPowerOff;          		break;
			case KEY_ICON_4: infoMenu.menu[++infoMenu.cur] = menuExtrude;   break;
			case KEY_ICON_5: infoMenu.menu[++infoMenu.cur] = menuFan;       break;
			case KEY_ICON_6: infoMenu.menu[++infoMenu.cur] = menuSettings;  break;
			case KEY_ICON_7: mustStoreCmd("G29\n");                         break;
            default:break;
		}		
		loopProcess();
	}
	
}

