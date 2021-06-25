#include "led.h"   

extern CURR_RTCM FT_rtcm;//�°����ݽṹ��

//***********************
//����:STM32ϵͳ��λ
void Soft_SystemReset(void)
{
		FLASH_Unlock();
					
		MyEE_WriteWord(EM_Flash_Before_point_H,(u16)((u32)FT_rtcm.R_RTCM_EM>>16));
		MyEE_WriteWord(EM_Flash_Before_point_L,(u16)((u32)FT_rtcm.R_RTCM_EM&0x0000FFFF));
		MyEE_WriteWord(EM_Flash_After_point,   (u16)((FT_rtcm.R_RTCM_EM-(u32)FT_rtcm.R_RTCM_EM)*10000));
						
		FLASH_Lock();
		
		__set_FAULTMASK(1);//�ر������ж�
		NVIC_SystemReset();//��λ
}
    
//***********************
//����:GSM��Դ����IO��ʼ��
void GSM_Power_Init(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PB�˿�ʱ��
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(GPIOB, &GPIO_InitStructure);
	 GSM_POWER_IO=1;
}

//***********************
//����:ָʾ��IO��ʼ��
void LED_IO_Init(void)
{
		GPIO_InitTypeDef  GPIO_InitStructure;	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);//ʹ�ܶ˿�ʱ��
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);  
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		LED_POWER_IO=1;
}

//***********************
//����:����IO����ʼ������
void CHECK_IO_Init(void)
{ 
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTAʱ��
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//���ó���������
		GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
}

//***********************
//����:GSM��Դ���� �򿪹���
void GSM_POWER_ON(void)
{
		GSM_POWER_IO=0;
	    delay_ms(5000);
}

//GSM��Դ���� �ر�
void GSM_POWER_OFF(void)
{
		GSM_POWER_IO=1;
	  delay_ms(5000);
}
 
