#ifndef __ASSIT_H
#define __ASSIT_H 	

#include "all.h" 
#include "ASSIT.h"

//�������ȼ�
#define ASSIT_TASK_PRIO		2
//�����ջ��С	
#define ASSIT_STK_SIZE 		FreeRTOS_ASSIT_STK_SIZE 
//������
void ASSIT_task(void *pvParameters);

#define ASSIT_SystemReset 0x01

#endif
