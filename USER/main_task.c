#include "main_task.h"

#define Delay_Time 100//与下板通信如果失败，延时此时间后再次尝试通信
#define CNT_RED 5//与下板通信失败的尝试次数

//任务句柄
TaskHandle_t MAIN_Task_Handler;

u8 MTSF=SUCCEED_BIT;//与探测器通信是否成功的状态 0:成功,1失败
u8 GPRS_FALUT_STATUS=0;//GPRS是否成功的状态 0:正常,1故障
u8 SEND_FT_GPRS_FALUT_STATUS=0;
u8 RelayMode=SUCCEED_BIT;//继电器模式打开状态 0:开通 1:关闭
extern u8 err_for_gprs_cnt;
extern QueueHandle_t FT_Message_Queue;//信息队列句柄
extern QueueHandle_t GSM_Message_Queue;	//信息队列句柄
extern QueueHandle_t ASSIT_Message_Queue;//信息队列句柄
extern CURR_RTCM FT_rtcm;
extern STATUS_RTCM FT_status;
extern u8 PushMode;
extern u8 NeedToSMS;
extern u8 updataing;
extern u8 DownFlash_EM_BIT;

u16 testmain=0;
//MAIN_task任务函数 
void MAIN_task(void *pvParameters)
{ 
		u16 Get_EM_Flash_Before_point_H=0,Get_EM_Flash_Before_point_L=0,Get_EM_Flash_After_point=0;
	
		Get_EM_Flash_Before_point_H=MyEE_ReadWord(EM_Flash_Before_point_H);
		Get_EM_Flash_Before_point_L=MyEE_ReadWord(EM_Flash_Before_point_L);
		Get_EM_Flash_After_point=MyEE_ReadWord(EM_Flash_After_point);
		
		FT_rtcm.R_RTCM_EM=(Get_EM_Flash_Before_point_H<<16)  +  Get_EM_Flash_Before_point_L  +  Get_EM_Flash_After_point*0.0001;
		
    while(1)
    {
			
				if(R_BasicData_command(0x01)==1)
						MTSF=SUCCEED_BIT;
				else
						MTSF=FAULT_BIT;
				if(MTSF==SUCCEED_BIT)FT_rtcm.R_RTCM_EM=FT_rtcm.R_RTCM_EM+(FT_rtcm.R_RTCM_APower+FT_rtcm.R_RTCM_BPower+FT_rtcm.R_RTCM_CPower)/3600.0/1000.0;
        
				if(DownFlash_EM_BIT==1)
				{
						DownFlash_EM_BIT=0;
						FLASH_Unlock();
					
						MyEE_WriteWord(EM_Flash_Before_point_H,(u16)((u32)FT_rtcm.R_RTCM_EM>>16));
						MyEE_WriteWord(EM_Flash_Before_point_L,(u16)((u32)FT_rtcm.R_RTCM_EM&0x0000FFFF));
						MyEE_WriteWord(EM_Flash_After_point,   (u16)((FT_rtcm.R_RTCM_EM-(u32)FT_rtcm.R_RTCM_EM)*10000));
						
						FLASH_Lock();
				}
				
				delay_ms(1000);
    }
}

