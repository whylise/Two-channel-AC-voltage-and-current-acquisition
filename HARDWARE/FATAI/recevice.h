#ifndef __RECEVICE_H
#define __RECEVICE_H 	

#include "all.h" 
#include "recevice.h"

#define DELAY_RECEIVE 100//发送数据后的延时
#define MAX_RECEIVE 128 //最大串口接受数组长度

#define R_Leak_CURRENT 0x10//漏电流
#define R_A_PHASE_CURRENT 0x11//A相电流
#define R_B_PHASE_CURRENT 0x12//B相电流
#define R_C_PHASE_CURRENT 0x13//C相电流

#define R_A_Temp 0x14//A相温度
#define R_B_Temp 0x15//B相温度
#define R_C_Temp 0x16//C相温度
#define R_Environment_Temp 0x17//环境温度

#define R_A_Voltage 0x18//A相电压
#define R_B_Voltage 0x19//B相电压
#define R_C_Voltage 0x1A//C相电压

#define R_Alarm_Status_Flag 0x1B//报警/状态标志位
#define R_Alarm_Status_Flag2 0x1C//报警/状态标志位2

#define R_Leak_CURRENT_Alarming_Value 0x1D//漏电流报警门限值
#define R_A_PHASE_CURRENT_Alarming_Value 0x1E//A相电流报警门限值
#define R_B_PHASE_CURRENT_Alarming_Value 0x1F//B相电流报警门限值
#define R_C_PHASE_CURRENT_Alarming_Value 0x20//C相电流报警门限值

#define R_A_Temp_Alarming_Value 0x21//A相温度报警门限值
#define R_B_Temp_Alarming_Value 0x22//B相温度报警门限值
#define R_C_Temp_Alarming_Value 0x23//C相温度报警门限值
#define R_Environment_Temp_Alarming_Value 0x24//环境温度报警门限值

#define R_Hardware_Version_Number 0x25//硬件版本号
#define R_Software_Version_Number 0x26//软件版本号

#define R_Leak_CURRENT_Action_Time 0x27//漏电流动作时间
#define R_A_PHASE_CURRENT_Action_Time 0x28//A相电流动作时间
#define R_B_PHASE_CURRENT_Action_Time 0x29//B相电流动作时间
#define R_C_PHASE_CURRENT_Action_Time 0x2A//C相电流动作时间
#define R_A_Temp_CURRENT_Action_Time 0x2B//A相温度动作时间
#define R_B_Temp_CURRENT_Action_Time 0x2C//B相温度动作时间
#define R_C_Temp_CURRENT_Action_Time 0x2D//C相温度动作时间
#define R_Environment_Temp_CURRENT_Action_Time 0x2E//环境温度动作时间

#define W_Leak_CURRENT_Alarming_Value 0x80//漏电流报警门限值
#define W_A_PHASE_CURRENT_Alarming_Value 0x81//A相电流报警门限值
#define W_B_PHASE_CURRENT_Alarming_Value 0x82//B相电流报警门限值
#define W_C_PHASE_CURRENT_Alarming_Value 0x83//C相电流报警门限值

#define W_A_Temp_Alarming_Value 0x84//A相温度报警门限值
#define W_B_Temp_Alarming_Value 0x85//B相温度报警门限值
#define W_C_Temp_Alarming_Value 0x86//C相温度报警门限值
#define W_Environment_Temp_Alarming_Value 0x87//环境温度报警门限值

#define W_Leak_CURRENT_Action_Time 0x88//漏电流动作时间
#define W_A_PHASE_CURRENT_Action_Time 0x89//A相电流动作时间
#define W_B_PHASE_CURRENT_Action_Time 0x8A//B相电流动作时间
#define W_C_PHASE_CURRENT_Action_Time 0x8B//C相电流动作时间
#define W_A_Temp_CURRENT_Action_Time 0x8C//A相温度动作时间
#define W_B_Temp_CURRENT_Action_Time 0x8D//B相温度动作时间
#define W_C_Temp_CURRENT_Action_Time 0x8E//C相温度动作时间
#define W_Environment_Temp_CURRENT_Action_Time 0x8F//环境温度动作时间

#define W_RESET 0x90//复位
#define W_Self_Check 0x91//自检
#define W_Fire_Linkage 0x92//消防联动设置
#define W_Relay_Mode 0X93//继电器工作模式
#define W_GPRS_Communication_Fault 0x94//GPRS通信故障

//RTCM 的数据结构体
typedef struct _CURR_RTCM
{
		double R_RTCM_EM;
		u32 R_RTCM_APower;
		u32 R_RTCM_BPower;
		u32 R_RTCM_CPower;
		u8  R_RTCM_slave;//从机地址
		u16 R_RTCM_Acurrent;//A相电流
		u16 R_RTCM_Bcurrent;//B相电流
		u16 R_RTCM_Ccurrent;//C相电流
		u16 R_RTCM_Lcurrent;//漏电流
		s16 R_RTCM_Atemp;//A相温度
		s16 R_RTCM_Btemp;//B相温度
		s16 R_RTCM_Ctemp;//C相温度
		s16 R_RTCM_Etemp;//环境温度
		u16 R_RTCM_Avoltage;//A相电压
		u16 R_RTCM_Bvoltage;//A相电压
		u16 R_RTCM_Cvoltage;//A相电压
		u16 R_RTCM_Bit;//状态标志位
		u16 R_RTCM_Bit2;//状态标志位2
		u16 R_RTCM_AcAvalue;//A相电流门限值
		u16 R_RTCM_BcAvalue;//B相电流门限值
		u16 R_RTCM_CcAvalue;//C相电流门限值
		u16 R_RTCM_LcAvalue;//漏电流门限值
		s16 R_RTCM_AtAvalue;//A相温度门限值
		s16 R_RTCM_BtAvalue;//B相温度门限值
		s16 R_RTCM_CtAvalue;//C相温度门限值
		s16 R_RTCM_EtAvalue;//环境温度门限值
		u16 R_RTCM_HardwareVersion_Number;//硬件版本号
		u16 R_RTCM_SoftwareVersion_Number;//软件版本号
		u16 R_RTCM_AcActionTime;//A相电流动作时间
		u16 R_RTCM_BcActionTime;//B相电流动作时间
		u16 R_RTCM_CcActionTime;//C相电流动作时间
		u16 R_RTCM_LcActionTime;//漏电流动作时间
		u16 R_RTCM_AtActionTime;//A相温度动作时间
		u16 R_RTCM_BtActionTime;//B相温度动作时间
		u16 R_RTCM_CtActionTime;//C相温度动作时间
		u16 R_RTCM_EtActionTime;//环境温度动作时间
		
		u16 last_R_RTCM_Acurrent;//上一次A相电流
		u16 last_R_RTCM_Bcurrent;//上一次B相电流
		u16 last_R_RTCM_Ccurrent;//上一次C相电流
		u16 last_R_RTCM_Lcurrent;//上一次漏电流
		s16 last_R_RTCM_Atemp;//上一次A相温度
		s16 last_R_RTCM_Btemp;//上一次B相温度
		s16 last_R_RTCM_Ctemp;//上一次C相温度
		s16 last_R_RTCM_Etemp;//上一次环境温度
		u16 last_R_RTCM_Avoltage;//上一次A相电压
		u16 last_R_RTCM_Bvoltage;//上一次B相电压
		u16 last_R_RTCM_Cvoltage;//上一次C相电压
}CURR_RTCM;

enum RTCM_All_Status
{
		Status_Normal=0,//正常 			0
		Status_Warning,//预警 				1
		Status_Alarming,//报警 			2
		Status_Broken,//断线 				3
		Status_Abnormal,//异常				4
		Status_Fault,//故障 					5
		Status_Short,//短路 					6
		Status_Undervoltage=2,//过压 	2
		Status_Overvoltage=2,//欠压		2
		Statue_NONE=-1//无安装 			-1
};

//RTCM 的状态结构体
typedef struct _STATUS_RTCM
{
		s8 RTCM_Status;//模块状态
		s8 RTCM_S;//探测器状态
		s8 RTCM_CSA;//A相电流状态
		s8 RTCM_CSB;//B相电流状态
		s8 RTCM_CSC;//C相电流状态
		s8 RTCM_LS;//漏电流状态
		s8 RTCM_TS1;//A相温度状态
		s8 RTCM_TS2;//B相温度状态
		s8 RTCM_TS3;//C相温度状态
		s8 RTCM_TS4;//环境温度状态
		s8 RTCM_VSA;//A相电压状态
		s8 RTCM_VSB;//B相电压状态
		s8 RTCM_VSC;//C相电压状态
		
		s8 last_RTCM_Status;//上一次模块状态
		s8 last_RTCM_S;//上一次探测器状态
		s8 last_RTCM_CSA;//上一次A相电流状态
		s8 last_RTCM_CSB;//上一次B相电流状态
		s8 last_RTCM_CSC;//上一次C相电流状态
		s8 last_RTCM_LS;//上一次漏电流状态
		s8 last_RTCM_TS1;//上一次A相温度状态
		s8 last_RTCM_TS2;//上一次B相温度状态
		s8 last_RTCM_TS3;//上一次C相温度状态
		s8 last_RTCM_TS4;//上一次环境温度状态
		s8 last_RTCM_VSA;//上一次A相电压状态
		s8 last_RTCM_VSB;//上一次B相电压状态
		s8 last_RTCM_VSC;//上一次C相电压状态
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

