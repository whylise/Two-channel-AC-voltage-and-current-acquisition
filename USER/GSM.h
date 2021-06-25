#ifndef __GSM_H
#define __GSM_H 	

#include "all.h" 
#include "GSM.h"

//�������ȼ�
#define GSM_TASK_PRIO		3
//�����ջ��С	
#define GSM_STK_SIZE 		FreeRTOS_GSM_STK_SIZE  
//������
void GSM_task(void *pvParameters);

//�������ȼ�
#define P_GSM_TASK_PRIO		8
//�����ջ��С	
#define P_GSM_STK_SIZE 		FreeRTOS_GSM_STK_SIZE  
//������
void P_GSM_task(void *pvParameters);

#define INIT_GSM 0x01//��ʼ��GSM
#define CONNECT_IP    0x02 //����TCPIP
#define GettimetoRTC  0x03 //��ȡʱ��д��SIM800 RTC
#define CorrectTime   0x04
#define UpdataTime    0x05
#define GetSingal     0x06
#define SEND_SEFT_SMS 0x81//���Ͷ��Ų�ѯ���
#define SEND_NUM_SMS 0x82//���Ͷ��Ų�ѯ����
#define SEND_ZJ_SMS_TO_ALL 0x83//�����к��뷢���Լ���ɶ���
#define SEND_FW_SMS_TO_ALL 0x84//�����к��뷢�͸�λ��ɶ���
#define SEND_CXBJ_SMS_TO_ALL 0x85//�����к��뷢�ͳ��ޱ�������
#define SEND_GZ_SMS_TO_ALL 0x86//�����к��뷢�͹��϶���
#define POST_HTTP_SEFT 0x31//�������POST�����Ϣ
#define POST_HTTP_BasicData 0x32//�������POST��������
#define POST_HTTP_AlarmData 0x33//�������POST��������
#define POST_HTTP_Frist 0x34//�ϴ���һ�ε���Ϣ
#define POST_HTTP_FlagBIT 0x35//�ϴ���־λ
#define POST_HTTP_Num 0x36//�ϴ�����
#define GET_HTTP_FLAGS  0x41//�������GET��־λ
#define GET_HTTP_Config 0x42//�������GET������Ϣ

#define POST_HTTP_CDKEY 0xA1//�ϴ����к�

//#define AlreadyWrite    (0x0000)
//#define IMEI_ADDRESS    (0x0003)
//#define S_ADDRESS       (0x0020)
//#define IP_ADDRESS      (0x0025)
//#define PORT_ADDRESS    (0x0050)
//#define ST_ADDRESS      (0x0060)
//#define PW_ADDRESS      (0x0070)
//#define MN_ADDRESS      (0x0080)
//#define DC_ADDRESS      (0x0110)
//#define PLOID_ADDRESS   (0x0120)
//#define WIRING_ADDRESS  (0x0130)
//#define CT_ADDRESS      (0x0135)
//#define PT_ADDRESS      (0x0145)
//#define isSAVE_ADDRESS  (0x0155)
//#define LNG_ADDRESS     (0x0175)
//#define LAT_ADDRESS     (0x0190)
//#define UID_ADDRESS     (0x0205)
//#define UGUID_ADDRESS   (0x0210)
//#define CMD_ADDRESS     (0x0260)

#define LossCnt 3

u8 AnalysisRrceiveDataNOAT(void);
u8 WritetoFlash(void);
u8 ReadFlashconfig(void);

#endif
