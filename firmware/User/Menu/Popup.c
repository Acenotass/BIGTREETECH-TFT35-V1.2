#include "Popup.h"
#include "includes.h"


#define POPUP_CONFIRM_RECT {180, 210, 300, 260}

#define POPUP_RECT         {80, 50, 400, 270}

WINDOW window = {
     POPUP_RECT,              //����������
     10,                      //�Ľ�Բ���İ뾶
     3,                       //��ߵ��߿�
     0x5D7B,                  //��ߺͱ������ı���ɫ
    {BRED, 0x5D7B, 40},        //������ ����ɫ/����ɫ/�߶�
    {WHITE, BLACK,  110},     //�ı��� ����ɫ/����ɫ/�߶�
    {WHITE, GRAY,   70},      //�ײ� (����ɫ)/����ɫ/(�߶�)
};


static const BUTTON *windowButton =  NULL;
static u16 buttonNum = 0;

void windowSetButton(const BUTTON *btn, u16 btnNum)
{
    windowButton = btn;   
    buttonNum = btnNum;
}   

void windowReDrawButton(u8 positon, u8 pressed)
{
    
    if(positon >= buttonNum)            return;
    if(pressed >= 2)                    return;
    if(windowButton == NULL)            return;
    if(windowButton->context == NULL)   return;
    
    GUI_DrawButton(windowButton + positon, pressed);
}


static const u8 *windowTitle = NULL;
static const u8  windowContext[MAX_ACK_SIZE];
static const u8 *windowYes = NULL;
static const u8 *windowNo = NULL;
void popupSetContext(const u8 *title, const u8 *context, const u8 *yes, const u8 *no)
{
    windowTitle = title;
    strcpy((char *)windowContext, (char *)context);
    windowYes = yes;
    windowNo = no;
}

#define BUTTON_NUM 1

static  BUTTON bottomBtn = {
//ȷ����ť                            ����״̬              ����״̬
POPUP_CONFIRM_RECT, NULL, 5, 1, GREEN, BLACK, WHITE,   GREEN, WHITE, BLACK
};

static const GUI_RECT popupMenuRect = POPUP_CONFIRM_RECT;

void menuPopup(void)
{
	u16 key_num = IDLE_TOUCH;    
    
    TSC_ReDrawIcon = windowReDrawButton;
    bottomBtn.context = windowYes;
    windowSetButton(&bottomBtn, BUTTON_NUM);
    
    GUI_DrawWindow(&window, windowTitle, windowContext);
    GUI_DrawButton(&bottomBtn, 0);    
    
	while(infoMenu.menu[infoMenu.cur] == menuPopup)
	{
		key_num = KEY_GetValue(BUTTON_NUM, &popupMenuRect);
		switch(key_num)
		{            
			case KEY_POPUP_CONFIRM: 
                infoMenu.cur--; break;
			default:break;            
		}
        
		loopProcess();
	}
}






