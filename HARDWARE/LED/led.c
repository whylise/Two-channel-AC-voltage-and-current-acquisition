#include "led.h"   

extern CURR_RTCM FT_rtcm;//下板数据结构体

//***********************
//描述:STM32系统复位
void Soft_SystemReset(void)
{
		FLASH_Unlock();
					
		MyEE_WriteWord(EM_Flash_Before_point_H,(u16)((u32)FT_rtcm.R_RTCM_EM>>16));
		MyEE_WriteWord(EM_Flash_Before_point_L,(u16)((u32)FT_rtcm.R_RTCM_EM&0x0000FFFF));
		MyEE_WriteWord(EM_Flash_After_point,   (u16)((FT_rtcm.R_RTCM_EM-(u32)FT_rtcm.R_RTCM_EM)*10000));
						
		FLASH_Lock();
		
		__set_FAULTMASK(1);//关闭所有中端
		NVIC_SystemReset();//复位
}
    
//***********************
//描述:GSM电源控制IO初始化
void GSM_Power_Init(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能PB端口时钟
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(GPIOB, &GPIO_InitStructure);
	 GSM_POWER_IO=1;
}

//***********************
//描述:指示灯IO初始化
void LED_IO_Init(void)
{
		GPIO_InitTypeDef  GPIO_InitStructure;	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);//使能端口时钟
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);  
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		LED_POWER_IO=1;
}

//***********************
//描述:生产IO检测初始化函数
void CHECK_IO_Init(void)
{ 
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能PORTA时钟
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//设置成上拉输入
		GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
}

//***********************
//描述:GSM电源控制 打开供电
void GSM_POWER_ON(void)
{
		GSM_POWER_IO=0;
	    delay_ms(5000);
}

//GSM电源控制 关闭
void GSM_POWER_OFF(void)
{
		GSM_POWER_IO=1;
	  delay_ms(5000);
}
 
