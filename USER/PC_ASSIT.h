#ifndef __PC_ASSIT_H
#define __PC_ASSIT_H 	

#include "all.h" 
#include "PC_ASSIT.h"

//�������ȼ�
#define PC_ASSIT_TASK_PRIO		6
//�����ջ��С	
#define PC_ASSIT_STK_SIZE 		FreeRTOS_ASSIT_STK_SIZE 
//������
void PC_ASSIT_task(void *pvParameters);

#define SAVE_CDKEY 0x11

#endif
