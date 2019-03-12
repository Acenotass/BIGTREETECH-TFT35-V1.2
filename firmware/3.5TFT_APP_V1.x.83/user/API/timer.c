#include "includes.h"
#include "timer.h"


void TIM4_Config(u16 psc,u16 arr)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
 
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  
	
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );
	TIM_ITConfig(TIM4,TIM_IT_Update ,ENABLE);
	TIM_Cmd(TIM4, ENABLE);  
}
u32 os_time=0;
void TIM4_IRQHandler(void)   //TIM4�ж�
{
	static bool touch=false;
	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		os_time++;
//��ȡ����ֵ
		if(!TPEN())
		{
			if(touch)
			{
				press=true;
			}
			else
			{
				touch=true;
			}
		}
		else
		{
			press=false;
			touch=false;
		}
//��ʱ����M105��ѯ�¶�	
		if((os_time%infoHeatUpdate.get_time==0) && !infoHeatUpdate.update && !infoHeatUpdate.wait_M105)
		{
			infoHeatUpdate.update=true;
		}
//��ӡ������ͳ�ƴ�ӡ��ʱ		
		if(os_time%100==0)
		{
			if(infoPrinting.ok&&!infoPrinting.pause)
			{
				infoPrinting.time++;
				outage_save |= 0x02;
			}
		}
//���������Ϣ
		if(err_flag>1)
		{
			err_flag--;
//			if(err_flag==100)
//			{
//				err_flag=1;
//			}
		}
//��ʱ���		
		if(os_time==(u32)(~0))
		{
			os_time=0;
		}
		
		
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
	}
}


u32 OS_GetTime(void)
{
	return os_time;
}






