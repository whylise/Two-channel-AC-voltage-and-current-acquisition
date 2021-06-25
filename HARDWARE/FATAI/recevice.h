#ifndef __RECEVICE_H
#define __RECEVICE_H 	

#include "all.h" 
#include "recevice.h"

#define DELAY_RECEIVE 100//�������ݺ����ʱ
#define MAX_RECEIVE 128 //��󴮿ڽ������鳤��

#define R_Leak_CURRENT 0x10//©����
#define R_A_PHASE_CURRENT 0x11//A�����
#define R_B_PHASE_CURRENT 0x12//B�����
#define R_C_PHASE_CURRENT 0x13//C�����

#define R_A_Temp 0x14//A���¶�
#define R_B_Temp 0x15//B���¶�
#define R_C_Temp 0x16//C���¶�
#define R_Environment_Temp 0x17//�����¶�

#define R_A_Voltage 0x18//A���ѹ
#define R_B_Voltage 0x19//B���ѹ
#define R_C_Voltage 0x1A//C���ѹ

#define R_Alarm_Status_Flag 0x1B//����/״̬��־λ
#define R_Alarm_Status_Flag2 0x1C//����/״̬��־λ2

#define R_Leak_CURRENT_Alarming_Value 0x1D//©������������ֵ
#define R_A_PHASE_CURRENT_Alarming_Value 0x1E//A�������������ֵ
#define R_B_PHASE_CURRENT_Alarming_Value 0x1F//B�������������ֵ
#define R_C_PHASE_CURRENT_Alarming_Value 0x20//C�������������ֵ

#define R_A_Temp_Alarming_Value 0x21//A���¶ȱ�������ֵ
#define R_B_Temp_Alarming_Value 0x22//B���¶ȱ�������ֵ
#define R_C_Temp_Alarming_Value 0x23//C���¶ȱ�������ֵ
#define R_Environment_Temp_Alarming_Value 0x24//�����¶ȱ�������ֵ

#define R_Hardware_Version_Number 0x25//Ӳ���汾��
#define R_Software_Version_Number 0x26//����汾��

#define R_Leak_CURRENT_Action_Time 0x27//©��������ʱ��
#define R_A_PHASE_CURRENT_Action_Time 0x28//A���������ʱ��
#define R_B_PHASE_CURRENT_Action_Time 0x29//B���������ʱ��
#define R_C_PHASE_CURRENT_Action_Time 0x2A//C���������ʱ��
#define R_A_Temp_CURRENT_Action_Time 0x2B//A���¶ȶ���ʱ��
#define R_B_Temp_CURRENT_Action_Time 0x2C//B���¶ȶ���ʱ��
#define R_C_Temp_CURRENT_Action_Time 0x2D//C���¶ȶ���ʱ��
#define R_Environment_Temp_CURRENT_Action_Time 0x2E//�����¶ȶ���ʱ��

#define W_Leak_CURRENT_Alarming_Value 0x80//©������������ֵ
#define W_A_PHASE_CURRENT_Alarming_Value 0x81//A�������������ֵ
#define W_B_PHASE_CURRENT_Alarming_Value 0x82//B�������������ֵ
#define W_C_PHASE_CURRENT_Alarming_Value 0x83//C�������������ֵ

#define W_A_Temp_Alarming_Value 0x84//A���¶ȱ�������ֵ
#define W_B_Temp_Alarming_Value 0x85//B���¶ȱ�������ֵ
#define W_C_Temp_Alarming_Value 0x86//C���¶ȱ�������ֵ
#define W_Environment_Temp_Alarming_Value 0x87//�����¶ȱ�������ֵ

#define W_Leak_CURRENT_Action_Time 0x88//©��������ʱ��
#define W_A_PHASE_CURRENT_Action_Time 0x89//A���������ʱ��
#define W_B_PHASE_CURRENT_Action_Time 0x8A//B���������ʱ��
#define W_C_PHASE_CURRENT_Action_Time 0x8B//C���������ʱ��
#define W_A_Temp_CURRENT_Action_Time 0x8C//A���¶ȶ���ʱ��
#define W_B_Temp_CURRENT_Action_Time 0x8D//B���¶ȶ���ʱ��
#define W_C_Temp_CURRENT_Action_Time 0x8E//C���¶ȶ���ʱ��
#define W_Environment_Temp_CURRENT_Action_Time 0x8F//�����¶ȶ���ʱ��

#define W_RESET 0x90//��λ
#define W_Self_Check 0x91//�Լ�
#define W_Fire_Linkage 0x92//������������
#define W_Relay_Mode 0X93//�̵�������ģʽ
#define W_GPRS_Communication_Fault 0x94//GPRSͨ�Ź���

//RTCM �����ݽṹ��
typedef struct _CURR_RTCM
{
		double R_RTCM_EM;
		u32 R_RTCM_APower;
		u32 R_RTCM_BPower;
		u32 R_RTCM_CPower;
		u8  R_RTCM_slave;//�ӻ���ַ
		u16 R_RTCM_Acurrent;//A�����
		u16 R_RTCM_Bcurrent;//B�����
		u16 R_RTCM_Ccurrent;//C�����
		u16 R_RTCM_Lcurrent;//©����
		s16 R_RTCM_Atemp;//A���¶�
		s16 R_RTCM_Btemp;//B���¶�
		s16 R_RTCM_Ctemp;//C���¶�
		s16 R_RTCM_Etemp;//�����¶�
		u16 R_RTCM_Avoltage;//A���ѹ
		u16 R_RTCM_Bvoltage;//A���ѹ
		u16 R_RTCM_Cvoltage;//A���ѹ
		u16 R_RTCM_Bit;//״̬��־λ
		u16 R_RTCM_Bit2;//״̬��־λ2
		u16 R_RTCM_AcAvalue;//A���������ֵ
		u16 R_RTCM_BcAvalue;//B���������ֵ
		u16 R_RTCM_CcAvalue;//C���������ֵ
		u16 R_RTCM_LcAvalue;//©��������ֵ
		s16 R_RTCM_AtAvalue;//A���¶�����ֵ
		s16 R_RTCM_BtAvalue;//B���¶�����ֵ
		s16 R_RTCM_CtAvalue;//C���¶�����ֵ
		s16 R_RTCM_EtAvalue;//�����¶�����ֵ
		u16 R_RTCM_HardwareVersion_Number;//Ӳ���汾��
		u16 R_RTCM_SoftwareVersion_Number;//����汾��
		u16 R_RTCM_AcActionTime;//A���������ʱ��
		u16 R_RTCM_BcActionTime;//B���������ʱ��
		u16 R_RTCM_CcActionTime;//C���������ʱ��
		u16 R_RTCM_LcActionTime;//©��������ʱ��
		u16 R_RTCM_AtActionTime;//A���¶ȶ���ʱ��
		u16 R_RTCM_BtActionTime;//B���¶ȶ���ʱ��
		u16 R_RTCM_CtActionTime;//C���¶ȶ���ʱ��
		u16 R_RTCM_EtActionTime;//�����¶ȶ���ʱ��
		
		u16 last_R_RTCM_Acurrent;//��һ��A�����
		u16 last_R_RTCM_Bcurrent;//��һ��B�����
		u16 last_R_RTCM_Ccurrent;//��һ��C�����
		u16 last_R_RTCM_Lcurrent;//��һ��©����
		s16 last_R_RTCM_Atemp;//��һ��A���¶�
		s16 last_R_RTCM_Btemp;//��һ��B���¶�
		s16 last_R_RTCM_Ctemp;//��һ��C���¶�
		s16 last_R_RTCM_Etemp;//��һ�λ����¶�
		u16 last_R_RTCM_Avoltage;//��һ��A���ѹ
		u16 last_R_RTCM_Bvoltage;//��һ��B���ѹ
		u16 last_R_RTCM_Cvoltage;//��һ��C���ѹ
}CURR_RTCM;

enum RTCM_All_Status
{
		Status_Normal=0,//���� 			0
		Status_Warning,//Ԥ�� 				1
		Status_Alarming,//���� 			2
		Status_Broken,//���� 				3
		Status_Abnormal,//�쳣				4
		Status_Fault,//���� 					5
		Status_Short,//��· 					6
		Status_Undervoltage=2,//��ѹ 	2
		Status_Overvoltage=2,//Ƿѹ		2
		Statue_NONE=-1//�ް�װ 			-1
};

//RTCM ��״̬�ṹ��
typedef struct _STATUS_RTCM
{
		s8 RTCM_Status;//ģ��״̬
		s8 RTCM_S;//̽����״̬
		s8 RTCM_CSA;//A�����״̬
		s8 RTCM_CSB;//B�����״̬
		s8 RTCM_CSC;//C�����״̬
		s8 RTCM_LS;//©����״̬
		s8 RTCM_TS1;//A���¶�״̬
		s8 RTCM_TS2;//B���¶�״̬
		s8 RTCM_TS3;//C���¶�״̬
		s8 RTCM_TS4;//�����¶�״̬
		s8 RTCM_VSA;//A���ѹ״̬
		s8 RTCM_VSB;//B���ѹ״̬
		s8 RTCM_VSC;//C���ѹ״̬
		
		s8 last_RTCM_Status;//��һ��ģ��״̬
		s8 last_RTCM_S;//��һ��̽����״̬
		s8 last_RTCM_CSA;//��һ��A�����״̬
		s8 last_RTCM_CSB;//��һ��B�����״̬
		s8 last_RTCM_CSC;//��һ��C�����״̬
		s8 last_RTCM_LS;//��һ��©����״̬
		s8 last_RTCM_TS1;//��һ��A���¶�״̬
		s8 last_RTCM_TS2;//��һ��B���¶�״̬
		s8 last_RTCM_TS3;//��һ��C���¶�״̬
		s8 last_RTCM_TS4;//��һ�λ����¶�״̬
		s8 last_RTCM_VSA;//��һ��A���ѹ״̬
		s8 last_RTCM_VSB;//��һ��B���ѹ״̬
		s8 last_RTCM_VSC;//��һ��C���ѹ״̬
}STATUS_RTCM;

void clear_USART2buff(void);
void Send_Read_Command(u8 slave_num,u8 start_add,u8 Quantity);
u8 R_BasicData_command(u8 slave_num);
u8 R_AlarmingValue_command(u8 slave_num);
u8 R_HardwareVersionNumber_command(u8 slave_num);
u8 R_SoftwareVersionNumber_command(u8 slave_num);
u8 R_ActionTime_command(u8 slave_num);
u8 send_Write_Alarming_Value_command(u8 slave_num,u16 Ac_Value,u16 Bc_Value,u16 Cc_Value,u16 Lc_Value,s16 At_Value,s16 Bt_Value,s16 Ct_Value,s16 Et_Value);
u8 send_Write_Action_Time_command(u8 slave_num,u16 Ac_Value,u16 Bc_Value,u16 Cc_Value,u16 Lc_Value,s16 At_Value,s16 Bt_Value,s16 Ct_Value,s16 Et_Value);
u8 send_Wirte_RESET_command(u8 slave_num);
u8 send_Write_SelfCheck_command(u8 slave_num);
u8 send_Write_FireLinkage_command(u8 slave_num,u8 mode);
u8 send_Write_RelayMode_command(u8 slave_num,u8 mode);
u8 send_Write_GPRSCommunication_command(u8 slave_num,u8 mode);
u8 ABS_PushMode(void);
u8 Judge_Status(void);
void get_status_of_rtcm(void);
void UPDATA_FT(void);

#endif

