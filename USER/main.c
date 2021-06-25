#include "all.h"

#define  Max_The_Number_Of_FTProjects 64//������󳤶�
#define  Max_The_Number_Of_GSMProjects 64//������󳤶�
#define  Max_The_Number_Of_ASSITProjects 64//������󳤶�
#define  Max_The_Number_Of_PCASSITProjects 64//������󳤶�

extern TaskHandle_t GSM_Task_Handler;
extern TaskHandle_t ASSIT_Task_Handler;
extern TaskHandle_t MAIN_Task_Handler;
extern TaskHandle_t REALTIME_Task_Handler;
extern TaskHandle_t P_REALTIME_Task_Handler;
extern TaskHandle_t P_GSM_Task_Handler;
extern TaskHandle_t PC_Task_Handler;
extern TaskHandle_t PC_ASSIT_Task_Handler;

QueueHandle_t GSM_Message_Queue;//��Ϣ���о��
QueueHandle_t FT_Message_Queue;//��Ϣ���о��
QueueHandle_t ASSIT_Message_Queue;//��Ϣ���о��
QueueHandle_t P_GSM_Message_Queue;//��Ϣ���о��
QueueHandle_t PCASSIT_Message_Queue;//��Ϣ���о��

//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128
//������
TaskHandle_t StartTask_Handler;
//������
u8 check_mode_IO=0;//���CHECKģʽ��Ӧ��IO״̬
void start_task(void *pvParameters);

//�������ȼ�
#define PRODUCT_TASK_PRIO		6
//�����ջ��С	
#define PRODUCT_STK_SIZE 		128
//������
TaskHandle_t PRODUCTTask_Handler;
//������
void PRODUCT_task(void *pvParameters);

int main(void)
{	 
		 SCB->VTOR = FLASH_BASE | 0x2800; /* APP1 RUN ADDRESS. */
		//SCB->VTOR = FLASH_BASE | 0xC800; /* APP2 RUN ADDRESS. */	
	
		
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 
		delay_init();//��ʱ������ʼ��	 
		delay_ms(500);//������ʱ0.5s
		GSM_Power_Init();//����GSMģ���ԴIO��ʼ��
		CHECK_IO_Init();//���ģʽIO��ʼ��
		LED_IO_Init();//ָʾ��LED��ʼ��
		uart1_init(115200);//����1��ʼ��
		InitAndCheckFlash();	
		IWDG_Init(4,2500);//���Ź���ʼ�������Ƶ��Ϊ64,����ֵΪ625,���ʱ��Ϊ1s,���ߵ���Ҫ�ر�		
		if(Check_IO==0)//���IO�Ƿ�Ϊ�͵�ƽ
		{
				delay_ms(10);//����
				if(Check_IO==0)//�ٴμ��IO�Ƿ�Ϊ�͵�ƽ
					check_mode_IO=1;//��������ģʽ
		}
		else
				check_mode_IO=0;//������������ģʽ
		if(check_mode_IO==1)
		{
				uart2_init(9600);//����2��ʼ�� LCD1602�Ĳ�������9600
				InitAndCheckFlash();//�״ο���Flash���
				//������������ 
				xTaskCreate((TaskFunction_t )PRODUCT_task,            //������
										(const char*    )"PRODUCT_task",          //��������
										(uint16_t       )PRODUCT_STK_SIZE,        //�����ջ��С
										(void*          )NULL,                    //���ݸ��������Ĳ���
										(UBaseType_t    )PRODUCT_TASK_PRIO,       //�������ȼ�
										(TaskHandle_t*  )&PRODUCTTask_Handler);   //������
		}
		else
		{
				uart2_init(115200);//����2��ʼ��
				//����������������
				xTaskCreate((TaskFunction_t )start_task,            //������
										(const char*    )"start_task",          //��������
										(uint16_t       )START_STK_SIZE,        //�����ջ��С
										(void*          )NULL,                  //���ݸ��������Ĳ���
										(UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
										(TaskHandle_t*  )&StartTask_Handler);   //������
		}									
		vTaskStartScheduler();//�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();//�����ٽ���
    //������Ϣ����
		FT_Message_Queue=xQueueCreate(Max_The_Number_Of_FTProjects,1);//������ϢFT_Message_Queue,��Ŀ��Max_The_Number_Of_FTProjects,ÿ����Ŀ����1
		GSM_Message_Queue=xQueueCreate(Max_The_Number_Of_GSMProjects,1);//������ϢGSM_Message_Queue,��Ŀ��Max_The_Number_Of_FTProjects,ÿ����Ŀ����1
		ASSIT_Message_Queue=xQueueCreate(Max_The_Number_Of_ASSITProjects,1);//������ϢASSIT_Message_Queue,��ĿMax_The_Number_Of_ASSITProjects,ÿ����Ŀ����1
	  //����REALTIME����
    xTaskCreate((TaskFunction_t )REALTIME_task,     (const char*    )"REALTIME_task",   	
                (uint16_t       )REALTIME_STK_SIZE, (void*          )NULL,				
                (UBaseType_t    )REALTIME_TASK_PRIO,(TaskHandle_t*  )&REALTIME_Task_Handler);
		//����MAIN����
    xTaskCreate((TaskFunction_t )MAIN_task,     (const char*    )"MAIN_task",   
                (uint16_t       )MAIN_STK_SIZE, (void*          )NULL,
                (UBaseType_t    )MAIN_TASK_PRIO,(TaskHandle_t*  )&MAIN_Task_Handler); 
	  //����GSM����
    xTaskCreate((TaskFunction_t )GSM_task,     (const char*    )"GSM_task",   	
                (uint16_t       )GSM_STK_SIZE, (void*          )NULL,				
                (UBaseType_t    )GSM_TASK_PRIO,(TaskHandle_t*  )&GSM_Task_Handler);   
   //����ASSIT����
    xTaskCreate((TaskFunction_t )ASSIT_task,     (const char*    )"ASSIT_task",   
                (uint16_t       )ASSIT_STK_SIZE, (void*          )NULL,
                (UBaseType_t    )ASSIT_TASK_PRIO,(TaskHandle_t*  )&ASSIT_Task_Handler);    								
    vTaskDelete(StartTask_Handler);//ɾ����ʼ����
    taskEXIT_CRITICAL();//�˳��ٽ���
}

//��ʼ����������
void PRODUCT_task(void *pvParameters)
{
		taskENTER_CRITICAL();//�����ٽ���
		P_GSM_Message_Queue=xQueueCreate(Max_The_Number_Of_GSMProjects,1);//������ϢFT_Message_Queue,��Ŀ��Max_The_Number_Of_FTProjects,ÿ����Ŀ����1
		PCASSIT_Message_Queue=xQueueCreate(Max_The_Number_Of_PCASSITProjects,1);//������ϢFT_Message_Queue,��Ŀ64,ÿ����Ŀ����1
		//����P_REALTIME����
		xTaskCreate((TaskFunction_t )P_REALTIME_task,     (const char*    )"P_REALTIME_task",   	
                (uint16_t       )P_REALTIME_STK_SIZE, (void*          )NULL,				
                (UBaseType_t    )P_REALTIME_TASK_PRIO,(TaskHandle_t*  )&P_REALTIME_Task_Handler);
		//����PC����
    xTaskCreate((TaskFunction_t )PC_task,     (const char*    )"PC_task",   
                (uint16_t       )PC_STK_SIZE, (void*          )NULL,
                (UBaseType_t    )PC_TASK_PRIO,(TaskHandle_t*  )&PC_Task_Handler);						
		//����P_GSM����
    xTaskCreate((TaskFunction_t )P_GSM_task,     (const char*    )"P_GSM_task",   	
                (uint16_t       )P_GSM_STK_SIZE, (void*          )NULL,				
                (UBaseType_t    )P_GSM_TASK_PRIO,(TaskHandle_t*  )&P_GSM_Task_Handler); 							
    vTaskDelete(PRODUCTTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();//�˳��ٽ���
}




