#ifndef __LED_H
#define __LED_H	 

#include "all.h"

#define GSM_POWER_IO PBout(12)
#define LED_POWER_IO PBout(4)

#define Check_IO GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)

void GSM_Power_Init(void);
void GSM_POWER_ON(void);
void GSM_POWER_OFF(void);
void CHECK_IO_Init(void);
void Soft_SystemReset(void);
void LED_IO_Init(void);
		 				    
#endif
