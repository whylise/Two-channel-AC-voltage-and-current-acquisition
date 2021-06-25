#include "recevice.h"

extern u8 PushRate;

CURR_RTCM FT_rtcm;//下板数据结构体
CURR_RTCM NEW_DATA_FT;//发送的数据，发送过程中不能更新数据值
STATUS_RTCM FT_status;//下板状态结构体
STATUS_RTCM NEW_status_FT;//发送的状态，发送过程中不能更新数据值
u8 recevice_buff[MAX_RECEIVE];//串口1缓存数组
u16 recevice_cnt=0;//串口1接收字节的个数
u8 recevice_flag=0;//串口1状态


//*********************
//描述:下板的数据复制发送的数据结构体中去
void UPDATA_FT(void)
{
		NEW_DATA_FT.R_RTCM_slave=FT_rtcm.R_RTCM_slave;
		NEW_DATA_FT.R_RTCM_Acurrent=FT_rtcm.R_RTCM_Acurrent;
		NEW_DATA_FT.R_RTCM_Bcurrent=FT_rtcm.R_RTCM_Bcurrent;
		NEW_DATA_FT.R_RTCM_Ccurrent=FT_rtcm.R_RTCM_Ccurrent;
		NEW_DATA_FT.R_RTCM_Lcurrent=FT_rtcm.R_RTCM_Lcurrent;
		NEW_DATA_FT.R_RTCM_Atemp=FT_rtcm.R_RTCM_Atemp;
		NEW_DATA_FT.R_RTCM_Btemp=FT_rtcm.R_RTCM_Btemp;
		NEW_DATA_FT.R_RTCM_Ctemp=FT_rtcm.R_RTCM_Ctemp;
		NEW_DATA_FT.R_RTCM_Etemp=FT_rtcm.R_RTCM_Etemp;	
		NEW_DATA_FT.R_RTCM_Avoltage=FT_rtcm.R_RTCM_Avoltage;
		NEW_DATA_FT.R_RTCM_Bvoltage=FT_rtcm.R_RTCM_Bvoltage;
		NEW_DATA_FT.R_RTCM_Cvoltage=FT_rtcm.R_RTCM_Cvoltage;	
		NEW_DATA_FT.R_RTCM_Bit=FT_rtcm.R_RTCM_Bit;
		NEW_DATA_FT.R_RTCM_Bit2=FT_rtcm.R_RTCM_Bit2;
		NEW_DATA_FT.R_RTCM_AcAvalue=FT_rtcm.R_RTCM_AcAvalue;
		NEW_DATA_FT.R_RTCM_BcAvalue=FT_rtcm.R_RTCM_BcAvalue;
		NEW_DATA_FT.R_RTCM_CcAvalue=FT_rtcm.R_RTCM_CcAvalue;
		NEW_DATA_FT.R_RTCM_LcAvalue=FT_rtcm.R_RTCM_LcAvalue;
		NEW_DATA_FT.R_RTCM_AtAvalue=FT_rtcm.R_RTCM_AtAvalue;
		NEW_DATA_FT.R_RTCM_BtAvalue=FT_rtcm.R_RTCM_BtAvalue;
		NEW_DATA_FT.R_RTCM_CtAvalue=FT_rtcm.R_RTCM_CtAvalue;
		NEW_DATA_FT.R_RTCM_EtAvalue=FT_rtcm.R_RTCM_EtAvalue;
		NEW_DATA_FT.R_RTCM_HardwareVersion_Number=FT_rtcm.R_RTCM_HardwareVersion_Number;
		NEW_DATA_FT.R_RTCM_SoftwareVersion_Number=FT_rtcm.R_RTCM_SoftwareVersion_Number;
		NEW_DATA_FT.R_RTCM_AcActionTime=FT_rtcm.R_RTCM_AcActionTime;
		NEW_DATA_FT.R_RTCM_BcActionTime=FT_rtcm.R_RTCM_BcActionTime;
		NEW_DATA_FT.R_RTCM_CcActionTime=FT_rtcm.R_RTCM_CcActionTime;
		NEW_DATA_FT.R_RTCM_LcActionTime=FT_rtcm.R_RTCM_LcActionTime;
		NEW_DATA_FT.R_RTCM_AtActionTime=FT_rtcm.R_RTCM_AtActionTime;
		NEW_DATA_FT.R_RTCM_BtActionTime=FT_rtcm.R_RTCM_BtActionTime;	
		NEW_DATA_FT.R_RTCM_CtActionTime=FT_rtcm.R_RTCM_CtActionTime;	
		NEW_DATA_FT.R_RTCM_EtActionTime=FT_rtcm.R_RTCM_EtActionTime;	
		
		NEW_status_FT.RTCM_Status=FT_status.RTCM_Status;
		NEW_status_FT.RTCM_S=FT_status.RTCM_S;
		NEW_status_FT.RTCM_CSA=FT_status.RTCM_CSA;
		NEW_status_FT.RTCM_CSB=FT_status.RTCM_CSB;
		NEW_status_FT.RTCM_CSC=FT_status.RTCM_CSC;
		NEW_status_FT.RTCM_LS=FT_status.RTCM_LS;
		NEW_status_FT.RTCM_TS1=FT_status.RTCM_TS1;
		NEW_status_FT.RTCM_TS2=FT_status.RTCM_TS2;
		NEW_status_FT.RTCM_TS3=FT_status.RTCM_TS3;
		NEW_status_FT.RTCM_TS4=FT_status.RTCM_TS4;	
		NEW_status_FT.RTCM_VSA=FT_status.RTCM_VSA;
		NEW_status_FT.RTCM_VSB=FT_status.RTCM_VSB;
		NEW_status_FT.RTCM_VSC=FT_status.RTCM_VSC;		
}

//*********************
//描述:清除串口1缓存
void clear_USART2buff(void)
{
		u16 i=0;
		for(i=0;i<MAX_RECEIVE;i++)recevice_buff[i]=0;
		recevice_cnt=0;
		recevice_flag=0;
}

//*********************
//描述:发送获取基本数据命令到探测器,法泰专用
//参数:slave num从机地址,add 访问寄存器地址,Quantity访问的数量
void Send_Read_Command(u8 slave_num,u8 start_add,u8 Quantity)
{
		u8 buffer[32]={0};
		u16 send_crc16_value=0;
		buffer[0]=slave_num;//地址
		buffer[1]=0x03;//功能码
		buffer[2]=0x00;
		buffer[3]=start_add;//开始地址
		buffer[4]=0x00;
		buffer[5]=Quantity;//寄存器个数
		send_crc16_value=get_crc16_value(buffer,6);//计算CRC
		buffer[6]=send_crc16_value>>8;//高位
		buffer[7]=send_crc16_value;//低位
		clear_USART2buff();
		UART_string_amount(buffer,8,2);//发送
}

//*********************
//描述:读取基本数据，并判断探测器响应是否成功,法泰专用
//参数:slave num从机地址
//返回:1:成功；0:失败
u8 R_BasicData_command(u8 slave_num)
{
		u16 time_over=0,read_crc16_value=0;
		Send_Read_Command(slave_num,R_Leak_CURRENT,0x0D);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag是放在串口接受中断中，有一个数据来就会置位  
				else time_over++;
		}
		if(recevice_flag!=0)//接收到数据
		{
				while(recevice_flag!=0)//等待数据传输完毕
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1个字符的时间*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x03&&recevice_buff[2]==0x1A)//判断数据头节是否正确
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//计算前recevice_cnt-2的数组的CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//判断CRC高低位是否都一样
						{
								FT_rtcm.R_RTCM_Lcurrent=recevice_buff[3]<<8|recevice_buff[4];//漏电流							    
								FT_rtcm.R_RTCM_Acurrent=recevice_buff[5]<<8|recevice_buff[6];//A电流
								FT_rtcm.R_RTCM_Bcurrent=recevice_buff[7]<<8|recevice_buff[8];//B电流
								FT_rtcm.R_RTCM_Ccurrent=recevice_buff[9]<<8|recevice_buff[10];//C电流
								FT_rtcm.R_RTCM_Atemp=recevice_buff[11]<<8|recevice_buff[12];//A温度
								FT_rtcm.R_RTCM_Btemp=recevice_buff[13]<<8|recevice_buff[14];//B温度
								FT_rtcm.R_RTCM_Ctemp=recevice_buff[15]<<8|recevice_buff[16];//C温度
								FT_rtcm.R_RTCM_Etemp=recevice_buff[17]<<8|recevice_buff[18];//环境温度
								FT_rtcm.R_RTCM_Avoltage=recevice_buff[19]<<8|recevice_buff[20];//A相电压
								FT_rtcm.R_RTCM_Bvoltage=recevice_buff[21]<<8|recevice_buff[22];//B相电压
								FT_rtcm.R_RTCM_Cvoltage=recevice_buff[23]<<8|recevice_buff[24];//C相电压
								FT_rtcm.R_RTCM_Bit=recevice_buff[25]<<8|recevice_buff[26];//标志位
								FT_rtcm.R_RTCM_Bit2=recevice_buff[27]<<8|recevice_buff[28];//标志位
								get_status_of_rtcm();//解析标志位
								if(abs(FT_rtcm.R_RTCM_Lcurrent)==32768)FT_status.RTCM_LS=Statue_NONE;//判断有无安装
								if(abs(FT_rtcm.R_RTCM_Acurrent)==32768)FT_status.RTCM_CSA=Statue_NONE;//判断有无安装
								if(abs(FT_rtcm.R_RTCM_Bcurrent)==32768)FT_status.RTCM_CSB=Statue_NONE;//判断有无安装
								if(abs(FT_rtcm.R_RTCM_Ccurrent)==32768)FT_status.RTCM_CSC=Statue_NONE;//判断有无安装
								if(abs(FT_rtcm.R_RTCM_Atemp)==32768)FT_status.RTCM_TS1=Statue_NONE;//判断有无安装
								if(abs(FT_rtcm.R_RTCM_Btemp)==32768)FT_status.RTCM_TS2=Statue_NONE;//判断有无安装
								if(abs(FT_rtcm.R_RTCM_Ctemp)==32768)FT_status.RTCM_TS3=Statue_NONE;//判断有无安装
								if(abs(FT_rtcm.R_RTCM_Etemp)==32768)FT_status.RTCM_TS4=Statue_NONE;//判断有无安装
								if(abs(FT_rtcm.R_RTCM_Avoltage)==32768)FT_status.RTCM_VSA=Statue_NONE;//判断有无安装
								if(abs(FT_rtcm.R_RTCM_Bvoltage)==32768)FT_status.RTCM_VSB=Statue_NONE;//判断有无安装
								if(abs(FT_rtcm.R_RTCM_Cvoltage)==32768)FT_status.RTCM_VSC=Statue_NONE;//判断有无安装
								
								if(FT_rtcm.R_RTCM_Lcurrent==32768)   FT_rtcm.R_RTCM_Lcurrent=0;
								if(FT_rtcm.R_RTCM_Acurrent==32768)   FT_rtcm.R_RTCM_Acurrent=0;
								if(FT_rtcm.R_RTCM_Bcurrent==32768)   FT_rtcm.R_RTCM_Bcurrent=0;
								if(FT_rtcm.R_RTCM_Ccurrent==32768)   FT_rtcm.R_RTCM_Ccurrent=0;
								if(FT_rtcm.R_RTCM_Avoltage==32768)   FT_rtcm.R_RTCM_Avoltage=0;
								if(FT_rtcm.R_RTCM_Bvoltage==32768)   FT_rtcm.R_RTCM_Bvoltage=0;
								if(FT_rtcm.R_RTCM_Cvoltage==32768)   FT_rtcm.R_RTCM_Cvoltage=0;
								
								FT_rtcm.R_RTCM_APower=FT_rtcm.R_RTCM_Acurrent*FT_rtcm.R_RTCM_Avoltage;
								FT_rtcm.R_RTCM_BPower=FT_rtcm.R_RTCM_Bcurrent*FT_rtcm.R_RTCM_Bvoltage;
								FT_rtcm.R_RTCM_CPower=FT_rtcm.R_RTCM_Ccurrent*FT_rtcm.R_RTCM_Cvoltage;
								
								
								clear_USART2buff();
								return 1;//crc校验正确
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
	}

//*********************
//描述:读取门限数据，并判断探测器响应是否成功,法泰专用
//参数:slave num从机地址
//返回:1:成功；0:失败
u8 R_AlarmingValue_command(u8 slave_num)
{
		u16 time_over=0,read_crc16_value=0;
		Send_Read_Command(slave_num,R_Leak_CURRENT_Alarming_Value,0x08);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag是放在串口接受中断中，有一个数据来就会置位  
				else time_over++;
		}
		if(recevice_flag!=0)//接收到数据
		{
				while(recevice_flag!=0)//等待数据传输完毕
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1个字符的时间*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x03&&recevice_buff[2]==0x10)//判断数据头节是否正确
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//计算前recevice_cnt-2的数组的CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//判断CRC高低位是否都一样
						{
								FT_rtcm.R_RTCM_LcAvalue=recevice_buff[3]<<8|recevice_buff[4];//漏电流门限值
								FT_rtcm.R_RTCM_AcAvalue=recevice_buff[5]<<8|recevice_buff[6];//A电流门限值
								FT_rtcm.R_RTCM_BcAvalue=recevice_buff[7]<<8|recevice_buff[8];//B电流门限值
								FT_rtcm.R_RTCM_CcAvalue=recevice_buff[9]<<8|recevice_buff[10];//C电流门限值
								FT_rtcm.R_RTCM_AtAvalue=recevice_buff[11]<<8|recevice_buff[12];//A温度门限值
								FT_rtcm.R_RTCM_BtAvalue=recevice_buff[13]<<8|recevice_buff[14];//B温度门限值
								FT_rtcm.R_RTCM_CtAvalue=recevice_buff[15]<<8|recevice_buff[16];//C温度门限值
								FT_rtcm.R_RTCM_EtAvalue=recevice_buff[17]<<8|recevice_buff[18];//环境温度门限值		
								clear_USART2buff();
								return 1;//crc校验正确
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//描述:读取硬件版本数据，并判断探测器响应是否成功,法泰专用
//参数:slave num从机地址
//返回:1:成功；0:失败
u8 R_HardwareVersionNumber_command(u8 slave_num)
{
		u16 time_over=0,read_crc16_value=0;
		Send_Read_Command(slave_num,R_Hardware_Version_Number,0x01);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag是放在串口接受中断中，有一个数据来就会置位  
				else time_over++;
		}
		if(recevice_flag!=0)//接收到数据
		{
				while(recevice_flag!=0)//等待数据传输完毕
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1个字符的时间*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x03&&recevice_buff[2]==0x02)//判断数据头节是否正确
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//计算前recevice_cnt-2的数组的CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//判断CRC高低位是否都一样
						{
								FT_rtcm.R_RTCM_HardwareVersion_Number=recevice_buff[3]<<8|recevice_buff[4];//下板硬件版本号	
								clear_USART2buff();
								return 1;//crc校验正确
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//描述:读取软件版本数据，并判断探测器响应是否成功,法泰专用
//参数:slave num从机地址
//返回:1:成功；0:失败
u8 R_SoftwareVersionNumber_command(u8 slave_num)
{
		u16 time_over=0,read_crc16_value=0;
		Send_Read_Command(slave_num,R_Software_Version_Number,0x01);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag是放在串口接受中断中，有一个数据来就会置位  
				else time_over++;
		}
		if(recevice_flag!=0)//接收到数据
		{
				while(recevice_flag!=0)//等待数据传输完毕
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1个字符的时间*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x03&&recevice_buff[2]==0x02)//判断数据头节是否正确
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//计算前recevice_cnt-2的数组的CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//判断CRC高低位是否都一样
						{
								FT_rtcm.R_RTCM_SoftwareVersion_Number=recevice_buff[3]<<8|recevice_buff[4];//下板软件版本号	
								clear_USART2buff();
								return 1;//crc校验正确
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//描述:读取动作时间数据，并判断探测器响应是否成功,法泰专用
//参数:slave num从机地址
//返回:1:成功；0:失败
u8 R_ActionTime_command(u8 slave_num)
{
		u16 time_over=0,read_crc16_value=0;
		Send_Read_Command(slave_num,R_Leak_CURRENT_Action_Time,0x08);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag是放在串口接受中断中，有一个数据来就会置位  
				else time_over++;
		}
		if(recevice_flag!=0)//接收到数据
		{
				while(recevice_flag!=0)//等待数据传输完毕
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1个字符的时间*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x03&&recevice_buff[2]==0x10)//判断数据头节是否正确
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//计算前recevice_cnt-2的数组的CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//判断CRC高低位是否都一样
						{
								FT_rtcm.R_RTCM_LcActionTime=recevice_buff[3]<<8|recevice_buff[4];//漏电流动作时间
								FT_rtcm.R_RTCM_AcActionTime=recevice_buff[5]<<8|recevice_buff[6];//A电流动作时间
								FT_rtcm.R_RTCM_BcActionTime=recevice_buff[7]<<8|recevice_buff[8];//B电流动作时间
								FT_rtcm.R_RTCM_CcActionTime=recevice_buff[9]<<8|recevice_buff[10];//C电流动作时间
								FT_rtcm.R_RTCM_AtActionTime=recevice_buff[11]<<8|recevice_buff[12];//A温度动作时间
								FT_rtcm.R_RTCM_BtActionTime=recevice_buff[13]<<8|recevice_buff[14];//B温度动作时间
								FT_rtcm.R_RTCM_CtActionTime=recevice_buff[15]<<8|recevice_buff[16];//C温度动作时间
								FT_rtcm.R_RTCM_EtActionTime=recevice_buff[17]<<8|recevice_buff[18];//环境温度动作时间
								clear_USART2buff();
								return 1;//crc校验正确
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//描述:向探测器写门限值，并判断探测器响应是否成功,法泰专用
//参数:slave num从机地址,A,B,C相电流，漏电流门限，A,B,C相,环境温度门限
//返回:1:成功；0:失败
u8 send_Write_Alarming_Value_command(u8 slave_num,u16 Lc_Value,u16 Ac_Value,u16 Bc_Value,u16 Cc_Value,s16 At_Value,s16 Bt_Value,s16 Ct_Value,s16 Et_Value)
{
		u8 buffer[32];
		u16 send_crc16_value=0,read_crc16_value=0,time_over=0;
		buffer[0]=slave_num;//地址
		buffer[1]=0x10;//功能码
		buffer[2]=0x00;
		buffer[3]=W_Leak_CURRENT_Alarming_Value;//开始地址
		buffer[4]=0x00;
		buffer[5]=0x08;//寄存器个数
		buffer[6]=0x10;//字节个数
		buffer[7]=Lc_Value>>8;buffer[8]=Lc_Value;//漏电流门限值
		buffer[9]=Ac_Value>>8;buffer[10]=Ac_Value;//A电流门限值
		buffer[11]=Bc_Value>>8;buffer[12]=Bc_Value;//B电流门限值
		buffer[13]=Cc_Value>>8;buffer[14]=Cc_Value;//C电流门限值
		buffer[15]=At_Value>>8;buffer[16]=At_Value;//A温度门限值
		buffer[17]=Bt_Value>>8;buffer[18]=Bt_Value;//B温度门限值
		buffer[19]=Ct_Value>>8;buffer[20]=Ct_Value;//C温度门限值
		buffer[21]=Et_Value>>8;buffer[22]=Et_Value;//环境温度门限值
		send_crc16_value=get_crc16_value(buffer,23);//计算CRC
		buffer[23]=send_crc16_value>>8;buffer[24]=send_crc16_value;
		clear_USART2buff();
		UART_string_amount(buffer,25,2);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag是放在串口接受中断中，有一个数据来就会置位  
				else time_over++;
		}
		if(recevice_flag!=0)//接收到数据
		{
				while(recevice_flag!=0)//等待数据传输完毕
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1个字符的时间*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x10&&recevice_buff[2]==0x00&&recevice_buff[3]==W_Leak_CURRENT_Alarming_Value)//判断数据头节是否正确
				{
					read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//计算前recevice_cnt-2的数组的CRC
					if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//判断CRC高低位是否都一样
					{
							clear_USART2buff();return 1;//crc校验正确
					}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//描述:向探测器写动作时间，并判断探测器响应是否成功,法泰专用
//参数:slave num从机地址,A,B,C相电流，漏电流动作时间，A,B,C相,环境温度动作时间
//返回:1:成功；0:失败
u8 send_Write_Action_Time_command(u8 slave_num,u16 Lc_Value,u16 Ac_Value,u16 Bc_Value,u16 Cc_Value,s16 At_Value,s16 Bt_Value,s16 Ct_Value,s16 Et_Value)
{
		u8 buffer[32];
		u16 send_crc16_value=0,read_crc16_value=0,time_over=0;
		buffer[0]=slave_num;//地址
		buffer[1]=0x10;//功能码
		buffer[2]=0x00;
		buffer[3]=W_Leak_CURRENT_Action_Time;//开始地址
		buffer[4]=0x00;
		buffer[5]=0x08;//寄存器个数
		buffer[6]=0x10;//字节个数
		buffer[7]=Lc_Value>>8;buffer[8]=Lc_Value;//漏电流动作时间
		buffer[9]=Ac_Value>>8;buffer[10]=Ac_Value;//A电流动作时间
		buffer[11]=Bc_Value>>8;buffer[12]=Bc_Value;//B电流动作时间
		buffer[13]=Cc_Value>>8;buffer[14]=Cc_Value;//C电流动作时间
		buffer[15]=At_Value>>8;buffer[16]=At_Value;//A温度动作时间
		buffer[17]=Bt_Value>>8;buffer[18]=Bt_Value;//B温度动作时间
		buffer[19]=Ct_Value>>8;buffer[20]=Ct_Value;//C温度动作时间
		buffer[21]=Et_Value>>8;buffer[22]=Et_Value;//环境温度动作时间
		send_crc16_value=get_crc16_value(buffer,23);//计算CRC
		buffer[23]=send_crc16_value>>8;buffer[24]=send_crc16_value;
		clear_USART2buff();
		UART_string_amount(buffer,25,2);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag是放在串口接受中断中，有一个数据来就会置位  
				else time_over++;
		}
		if(recevice_flag!=0)//接收到数据
		{
			while(recevice_flag!=0)//等待数据传输完毕
			{
					recevice_flag=0;
					delay_ms(CalCurrentBaud1Char()*2);//1个字符的时间*2 
			}
			if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x10&&recevice_buff[2]==0x00&&recevice_buff[3]==W_Leak_CURRENT_Action_Time)//判断数据头节是否正确
			{
					read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//计算前recevice_cnt-2的数组的CRC
					if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//判断CRC高低位是否都一样
					{
							clear_USART2buff();return 1;//crc校验正确
					}
			}
			else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//描述:向探测器写复位，并判断探测器响应是否成功,法泰专用
//参数:slave num从机地址
//返回:1:成功；0:失败
u8 send_Wirte_RESET_command(u8 slave_num)
{
		u8 buffer[16];
		u16 send_crc16_value=0,read_crc16_value=0,time_over=0;
		buffer[0]=slave_num;//地址
		buffer[1]=0x10;//功能码
		buffer[2]=0x00;
		buffer[3]=W_RESET;//开始地址
		buffer[4]=0x00;
		buffer[5]=0x01;//寄存器个数
		buffer[6]=0x02;//字节个数
		buffer[7]=0x00;buffer[8]=0x01;
		send_crc16_value=get_crc16_value(buffer,9);
		buffer[9]=send_crc16_value>>8;buffer[10]=send_crc16_value;
		clear_USART2buff();
		UART_string_amount(buffer,11,2);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag是放在串口接受中断中，有一个数据来就会置位  
				else time_over++;
		}
		if(recevice_flag!=0)//接收到数据
		{
				while(recevice_flag!=0)//等待数据传输完毕
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1个字符的时间*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x10&&recevice_buff[2]==0x00&&recevice_buff[3]==W_RESET)//判断数据头节是否正确
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//计算前recevice_cnt-2的数组的CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//判断CRC高低位是否都一样
						{
								clear_USART2buff();return 1;//crc校验正确
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//描述:向探测器写自检，并判断探测器响应是否成功,法泰专用
//参数:slave num从机地址
//返回:1:成功；0:失败
u8 send_Write_SelfCheck_command(u8 slave_num)
{
		u8 buffer[16];
		u16 send_crc16_value=0,read_crc16_value=0,time_over=0;
		buffer[0]=slave_num;//地址
		buffer[1]=0x10;//功能码
		buffer[2]=0x00;
		buffer[3]=W_Self_Check;//开始地址
		buffer[4]=0x00;
		buffer[5]=0x01;//寄存器个数
		buffer[6]=0x02;//字节个数
		buffer[7]=0x00;buffer[8]=0x01;
		send_crc16_value=get_crc16_value(buffer,9);
		buffer[9]=send_crc16_value>>8;buffer[10]=send_crc16_value;
		clear_USART2buff();
		UART_string_amount(buffer,11,2);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag是放在串口接受中断中，有一个数据来就会置位  
				else time_over++;
		}
		if(recevice_flag!=0)//接收到数据
		{
				while(recevice_flag!=0)//等待数据传输完毕
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1个字符的时间*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x10&&recevice_buff[2]==0x00&&recevice_buff[3]==W_Self_Check)//判断数据头节是否正确
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//计算前recevice_cnt-2的数组的CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//判断CRC高低位是否都一样
						{
								clear_USART2buff();return 1;//crc校验正确
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//描述:向探测器写联防联动配置，并判断探测器响应是否成功,法泰专用
//参数:slave num从机地址,mode开通还是关闭
//返回:1:成功；0:失败
u8 send_Write_FireLinkage_command(u8 slave_num,u8 mode)
{
		u8 buffer[16];
		u16 send_crc16_value=0,read_crc16_value=0,time_over=0;
		buffer[0]=slave_num;//地址
		buffer[1]=0x10;//功能码
		buffer[2]=0x00;
		buffer[3]=W_Fire_Linkage;//开始地址
		buffer[4]=0x00;
		buffer[5]=0x01;//寄存器个数
		buffer[6]=0x02;//字节个数
		buffer[7]=0x00;
		if(mode==1)buffer[8]=0x01;
		else buffer[8]=0x00;
		send_crc16_value=get_crc16_value(buffer,9);
		buffer[9]=send_crc16_value>>8;buffer[10]=send_crc16_value;
		clear_USART2buff();
		UART_string_amount(buffer,11,2);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag是放在串口接受中断中，有一个数据来就会置位  
				else time_over++;
		}
		if(recevice_flag!=0)//接收到数据
		{
				while(recevice_flag!=0)//等待数据传输完毕
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1个字符的时间*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x10&&recevice_buff[2]==0x00&&recevice_buff[3]==W_Fire_Linkage)//判断数据头节是否正确
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//计算前recevice_cnt-2的数组的CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//判断CRC高低位是否都一样
						{
								clear_USART2buff();return 1;//crc校验正确
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//描述:向探测器写继电器工作模式，并判断探测器响应是否成功,法泰专用
//参数:slave num从机地址,mode开通还是关闭
//返回:1:成功；0:失败
u8 send_Write_RelayMode_command(u8 slave_num,u8 mode)
{
		u8 buffer[16];
		u16 send_crc16_value=0,read_crc16_value=0,time_over=0;
		buffer[0]=slave_num;//地址
		buffer[1]=0x10;//功能码
		buffer[2]=0x00;
		buffer[3]=W_Relay_Mode;//开始地址
		buffer[4]=0x00;
		buffer[5]=0x01;//寄存器个数
		buffer[6]=0x02;//字节个数
		buffer[7]=0x00;
		if(mode==1)buffer[8]=0x01;
		else buffer[8]=0x00;
		send_crc16_value=get_crc16_value(buffer,9);
		buffer[9]=send_crc16_value>>8;buffer[10]=send_crc16_value;
		clear_USART2buff();
		UART_string_amount(buffer,11,2);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag是放在串口接受中断中，有一个数据来就会置位  
				else time_over++;
		}
		if(recevice_flag!=0)//接收到数据
		{
				while(recevice_flag!=0)//等待数据传输完毕
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1个字符的时间*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x10&&recevice_buff[2]==0x00&&recevice_buff[3]==W_Relay_Mode)//判断数据头节是否正确
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//计算前recevice_cnt-2的数组的CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//判断CRC高低位是否都一样
						{
							clear_USART2buff();return 1;//crc校验正确
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//描述:向探测器写GPRS通信故障，并判断探测器响应是否成功,法泰专用
//参数:slave num从机地址,mode开通还是关闭
//返回:1:成功；0:失败
u8 send_Write_GPRSCommunication_command(u8 slave_num,u8 mode)
{
		u8 buffer[16];
		u16 send_crc16_value=0,read_crc16_value=0,time_over=0;
		buffer[0]=slave_num;//地址
		buffer[1]=0x10;//功能码
		buffer[2]=0x00;
		buffer[3]=W_GPRS_Communication_Fault;//开始地址
		buffer[4]=0x00;
		buffer[5]=0x01;//寄存器个数
		buffer[6]=0x02;//字节个数
		buffer[7]=0x00;
		if(mode==1)buffer[8]=0x01;
		else buffer[8]=0x00;
		send_crc16_value=get_crc16_value(buffer,9);
		buffer[9]=send_crc16_value>>8;buffer[10]=send_crc16_value;
		clear_USART2buff();
		UART_string_amount(buffer,11,2);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag是放在串口接受中断中，有一个数据来就会置位  
				else time_over++;
		}
		if(recevice_flag!=0)//接收到数据
		{
				while(recevice_flag!=0)//等待数据传输完毕
				{
					recevice_flag=0;
					delay_ms(CalCurrentBaud1Char()*2);//1个字符的时间*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x10&&recevice_buff[2]==0x00&&recevice_buff[3]==W_GPRS_Communication_Fault)//判断数据头节是否正确
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//计算前recevice_cnt-2的数组的CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//判断CRC高低位是否都一样
						{
								clear_USART2buff();return 1;//crc校验正确
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//***********************
//描述:动态上传专用，Pushrate为浮动大小，当与上次的值比较，变化大于该值时，返回1
//返回:大于范围返回1，全都没超范围，返回0
u8 ABS_PushMode(void)
{
		if(abs(FT_rtcm.last_R_RTCM_Acurrent-FT_rtcm.R_RTCM_Acurrent)>PushRate/100.0*400)//A相电流判断
			return 1;
		else if(abs(FT_rtcm.last_R_RTCM_Bcurrent-FT_rtcm.R_RTCM_Bcurrent)>PushRate/100.0*400)//B相电流判断
			return 1;
		else if(abs(FT_rtcm.last_R_RTCM_Ccurrent-FT_rtcm.R_RTCM_Ccurrent)>PushRate/100.0*400)//C相电流判断
			return 1;
		else if(abs(FT_rtcm.last_R_RTCM_Lcurrent-FT_rtcm.R_RTCM_Lcurrent)>PushRate/100.0*100)//漏电流判断
			return 1;
		else if(abs(FT_rtcm.last_R_RTCM_Atemp-FT_rtcm.last_R_RTCM_Atemp)>PushRate/100.0*150)//A相温度
			return 1;
		else if(abs(FT_rtcm.last_R_RTCM_Btemp-FT_rtcm.last_R_RTCM_Btemp)>PushRate/100.0*150)//B相温度
			return 1;
		else if(abs(FT_rtcm.last_R_RTCM_Ctemp-FT_rtcm.last_R_RTCM_Ctemp)>PushRate/100.0*150)//C相温度
			return 1;
		else if(abs(FT_rtcm.last_R_RTCM_Etemp-FT_rtcm.last_R_RTCM_Etemp)>PushRate/100.0*150)//环境温度
			return 1;
		return 0;
}

//***********************
//描述:判断状态位，报警，短路，断路，正常
//返回:当前的状态变化有哪些类型，0x01 0x02 0x04 0x08
u8 Judge_Status(void)
{
		u8 flag_judge_return=0;
		static u8 first_judge=0;
		if(first_judge==0)//第一次判断
		{
				if((FT_status.RTCM_CSA==Status_Alarming)||(FT_status.RTCM_CSB==Status_Alarming)||(FT_status.RTCM_CSC==Status_Alarming)||(FT_status.RTCM_LS==Status_Alarming))
						flag_judge_return=0x01;//报警
				if((FT_status.RTCM_TS1==Status_Alarming)||(FT_status.RTCM_TS2==Status_Alarming)||(FT_status.RTCM_TS3==Status_Alarming)||(FT_status.RTCM_TS4==Status_Alarming))
						flag_judge_return=0x01;//报警
				if((FT_status.RTCM_VSA==Status_Overvoltage)||(FT_status.RTCM_VSB==Status_Overvoltage)||(FT_status.RTCM_VSC==Status_Overvoltage))
						flag_judge_return=0x01;//报警
				if((FT_status.RTCM_VSA==Status_Undervoltage)||(FT_status.RTCM_VSB==Status_Undervoltage)||(FT_status.RTCM_VSC==Status_Undervoltage))
						flag_judge_return=0x01;//报警
				if(FT_status.RTCM_LS==Status_Broken)
						flag_judge_return=flag_judge_return|0x02;//断线
				if((FT_status.RTCM_TS1==Status_Broken)||(FT_status.RTCM_TS2==Status_Broken)||(FT_status.RTCM_TS3==Status_Broken)||(FT_status.RTCM_TS4==Status_Broken))
						flag_judge_return=flag_judge_return|0x02;//断线
				if(FT_status.RTCM_LS==Status_Short)
						flag_judge_return=flag_judge_return|0x04;//短路
				if((FT_status.RTCM_TS1==Status_Short)||(FT_status.RTCM_TS2==Status_Short)||(FT_status.RTCM_TS3==Status_Short)||(FT_status.RTCM_TS4==Status_Short))
						flag_judge_return=flag_judge_return|0x04;//短路
				first_judge=1;
		}
		else 
		{
				if((FT_status.RTCM_CSA!=FT_status.last_RTCM_CSA&&FT_status.RTCM_CSA==Status_Alarming)
					||(FT_status.RTCM_CSB!=FT_status.last_RTCM_CSB&&FT_status.RTCM_CSB==Status_Alarming)
					||(FT_status.RTCM_CSC!=FT_status.last_RTCM_CSC&&FT_status.RTCM_CSC==Status_Alarming)
					||(FT_status.RTCM_LS!=FT_status.last_RTCM_LS&&FT_status.RTCM_LS==Status_Alarming))
						flag_judge_return=0x01;//报警
				if((FT_status.RTCM_TS1!=FT_status.last_RTCM_TS1&&FT_status.RTCM_TS1==Status_Alarming)
					||(FT_status.RTCM_TS2!=FT_status.last_RTCM_TS2&&FT_status.RTCM_TS2==Status_Alarming)
					||(FT_status.RTCM_TS3!=FT_status.last_RTCM_TS3&&FT_status.RTCM_TS3==Status_Alarming)
					||(FT_status.RTCM_TS4!=FT_status.last_RTCM_TS4&&FT_status.RTCM_TS4==Status_Alarming))
						flag_judge_return=0x01;//报警
				if((FT_status.RTCM_VSA!=FT_status.last_RTCM_VSA&&FT_status.RTCM_VSA==Status_Alarming)
					||(FT_status.RTCM_VSB!=FT_status.last_RTCM_VSB&&FT_status.RTCM_VSB==Status_Alarming)
					||(FT_status.RTCM_VSC!=FT_status.last_RTCM_VSC&&FT_status.RTCM_VSC==Status_Alarming))
						flag_judge_return=0x01;//报警
				if(FT_status.RTCM_LS!=FT_status.last_RTCM_LS&&FT_status.RTCM_LS==Status_Broken)
						flag_judge_return=flag_judge_return|0x02;//断线
				if(FT_status.RTCM_LS!=FT_status.last_RTCM_LS&&FT_status.RTCM_LS==Status_Short)
						flag_judge_return=flag_judge_return|0x04;//短路
				if((FT_status.RTCM_TS1!=FT_status.last_RTCM_TS1&&FT_status.RTCM_TS1==Status_Broken)
					||(FT_status.RTCM_TS2!=FT_status.last_RTCM_TS2&&FT_status.RTCM_TS2==Status_Broken)
					||(FT_status.RTCM_TS3!=FT_status.last_RTCM_TS3&&FT_status.RTCM_TS3==Status_Broken)
					||(FT_status.RTCM_TS4!=FT_status.last_RTCM_TS4&&FT_status.RTCM_TS4==Status_Broken))
						flag_judge_return=flag_judge_return|0x02;//断线
				if((FT_status.RTCM_TS1!=FT_status.last_RTCM_TS1&&FT_status.RTCM_TS1==Status_Short)
					||(FT_status.RTCM_TS2!=FT_status.last_RTCM_TS2&&FT_status.RTCM_TS2==Status_Short)
					||(FT_status.RTCM_TS3!=FT_status.last_RTCM_TS3&&FT_status.RTCM_TS3==Status_Short)
					||(FT_status.RTCM_TS4!=FT_status.last_RTCM_TS4&&FT_status.RTCM_TS4==Status_Short))
						flag_judge_return=flag_judge_return|0x04;//短路	
				if((FT_status.RTCM_VSA!=FT_status.last_RTCM_VSA&&FT_status.RTCM_VSA==Status_Overvoltage)
					||(FT_status.RTCM_VSB!=FT_status.last_RTCM_VSB&&FT_status.RTCM_VSB==Status_Overvoltage)
					||(FT_status.RTCM_VSC!=FT_status.last_RTCM_VSC&&FT_status.RTCM_VSC==Status_Overvoltage))
						flag_judge_return=flag_judge_return|0x02;//过压
				if((FT_status.RTCM_VSA!=FT_status.last_RTCM_VSA&&FT_status.RTCM_VSA==Status_Undervoltage)
					||(FT_status.RTCM_VSB!=FT_status.last_RTCM_VSB&&FT_status.RTCM_VSB==Status_Undervoltage)
					||(FT_status.RTCM_VSC!=FT_status.last_RTCM_VSC&&FT_status.RTCM_VSC==Status_Undervoltage))
						flag_judge_return=flag_judge_return|0x04;//欠压	
				if(FT_status.RTCM_S==Status_Normal&&FT_status.RTCM_S!=FT_status.last_RTCM_S)
						flag_judge_return=flag_judge_return|0x08;//从不正常状态变为正常状态
				else if(FT_status.RTCM_CSA==Status_Normal&&FT_status.last_RTCM_CSA!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//从不正常状态变为正常状态
				else if(FT_status.RTCM_CSB==Status_Normal&&FT_status.last_RTCM_CSB!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//从不正常状态变为正常状态
				else if(FT_status.RTCM_CSC==Status_Normal&&FT_status.last_RTCM_CSC!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//从不正常状态变为正常状态
				else if(FT_status.RTCM_LS==Status_Normal&&FT_status.last_RTCM_LS!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//从不正常状态变为正常状态
				else if(FT_status.RTCM_TS1==Status_Normal&&FT_status.last_RTCM_TS1!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//从不正常状态变为正常状态
				else if(FT_status.RTCM_TS2==Status_Normal&&FT_status.last_RTCM_TS2!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//从不正常状态变为正常状态
				else if(FT_status.RTCM_TS3==Status_Normal&&FT_status.last_RTCM_TS3!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//从不正常状态变为正常状态
				else if(FT_status.RTCM_TS4==Status_Normal&&FT_status.last_RTCM_TS4!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//从不正常状态变为正常状态
				else if(FT_status.RTCM_VSA==Status_Normal&&FT_status.last_RTCM_VSA!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//从不正常状态变为正常状态
				else if(FT_status.RTCM_VSB==Status_Normal&&FT_status.last_RTCM_VSB!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//从不正常状态变为正常状态
				else if(FT_status.RTCM_VSC==Status_Normal&&FT_status.last_RTCM_VSC!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//从不正常状态变为正常状态
		}
		return flag_judge_return;
}

//***********************
//描述:获取下板的当前状态
void get_status_of_rtcm(void)
{
		if((FT_rtcm.R_RTCM_Bit&0xc000)==0xc000)//1100 0000 0000 0000
				FT_status.RTCM_LS=Status_Alarming;//漏电流超限
		else if((FT_rtcm.R_RTCM_Bit&0x4000)==0x4000)//0100 0000 0000 0000
				FT_status.RTCM_LS=Status_Broken;//漏电流断线
		else if((FT_rtcm.R_RTCM_Bit&0x8000)==0x8000)//1000 0000 0000 0000
				FT_status.RTCM_LS=Status_Short;//漏电流短线
		else 
				FT_status.RTCM_LS=Status_Normal;//漏电流正常
		
		if((FT_rtcm.R_RTCM_Bit&0x3000)==0x3000)//0011 0000 0000 0000
				FT_status.RTCM_CSA=Status_Alarming;//A相电流超限
//		else if((FT_rtcm.R_RTCM_Bit&0x1000)==0x1000)//0001 0000 0000 0000
//				FT_status.RTCM_CSA=Status_Broken;//A相电流断线
//		else if((FT_rtcm.R_RTCM_Bit&0x2000)==0x2000)//0010 0000 0000 0000
//				FT_status.RTCM_CSA=Status_Short;//A相电流短线
		else 
				FT_status.RTCM_CSA=Status_Normal;//A相电流正常
		
		if((FT_rtcm.R_RTCM_Bit&0x0c00)==0x0c00)//0000 1100 0000 0000
				FT_status.RTCM_CSB=Status_Alarming;//B相电流超限
//		else if((FT_rtcm.R_RTCM_Bit&0x1000)==0x1000)//0001 0000 0000 0000
//				FT_status.RTCM_CSB=Status_Broken;//B相电流断线
//		else if((FT_rtcm.R_RTCM_Bit&0x2000)==0x2000)//0010 0000 0000 0000
//				FT_status.RTCM_CSB=Status_Short;//B相电流短线
		else 
				FT_status.RTCM_CSB=Status_Normal;//B相电流正常
		
		if((FT_rtcm.R_RTCM_Bit&0x0300)==0x0300)//0000 0011 0000 0000
				FT_status.RTCM_CSC=Status_Alarming;//C相电流超限
//		else if((FT_rtcm.R_RTCM_Bit&0x0400)==0x0400)//0000 0100 0000 0000
//				FT_status.RTCM_CSC=Status_Broken;//C相电流断线
//		else if((FT_rtcm.R_RTCM_Bit&0x0800)==0x0800)//0000 1000 0000 0000
//				FT_status.RTCM_CSC=Status_Short;//C相电流短线
		else 
				FT_status.RTCM_CSC=Status_Normal;//C相电流正常
		
		if((FT_rtcm.R_RTCM_Bit&0x00c0)==0x00c0)//0000 0000 1100 0000
				FT_status.RTCM_TS1=Status_Alarming;//A相温度超限
		else if((FT_rtcm.R_RTCM_Bit&0x0040)==0x0040)//0000 0000 0100 0000
				FT_status.RTCM_TS1=Status_Broken;//A相温度断线
		else if((FT_rtcm.R_RTCM_Bit&0x0080)==0x0080)//0000 0000 1000 0000
				FT_status.RTCM_TS1=Status_Short;//A相温度短线
		else 
				FT_status.RTCM_TS1=Status_Normal;//A相温度正常	
		
		if((FT_rtcm.R_RTCM_Bit&0x0030)==0x0030)//0000 0000 0011 0000
				FT_status.RTCM_TS2=Status_Alarming;//B相温度超限
		else if((FT_rtcm.R_RTCM_Bit&0x0010)==0x0010)//0000 0000 0001 0000
				FT_status.RTCM_TS2=Status_Broken;//B相温度断线
		else if((FT_rtcm.R_RTCM_Bit&0x0020)==0x0020)//0000 0000 0010 0000
				FT_status.RTCM_TS2=Status_Short;//B相温度短线
		else 
				FT_status.RTCM_TS2=Status_Normal;//B相温度正常	
		
		if((FT_rtcm.R_RTCM_Bit&0x000c)==0x000c)//0000 0000 0000 1100
				FT_status.RTCM_TS3=Status_Alarming;//C相温度超限
		else if((FT_rtcm.R_RTCM_Bit&0x0004)==0x0004)//0000 0000 0000 0100
				FT_status.RTCM_TS3=Status_Broken;//C相温度断线
		else if((FT_rtcm.R_RTCM_Bit&0x0008)==0x0008)//0000 0000 0000 1000
				FT_status.RTCM_TS3=Status_Short;//C相温度短线
		else 
				FT_status.RTCM_TS3=Status_Normal;//C相温度正常	
		
		if((FT_rtcm.R_RTCM_Bit&0x0003)==0x0003)//0000 0000 0000 0011
				FT_status.RTCM_TS4=Status_Alarming;//环境温度超限
		else if((FT_rtcm.R_RTCM_Bit&0x0001)==0x0001)//0000 0000 0000 0001
				FT_status.RTCM_TS4=Status_Broken;//环境温度断线
		else if((FT_rtcm.R_RTCM_Bit&0x0002)==0x0002)//0000 0000 0000 0010
				FT_status.RTCM_TS4=Status_Short;//环境温度短线
		else 
				FT_status.RTCM_TS4=Status_Normal;//环境温度正常

		if((FT_rtcm.R_RTCM_Bit2&0x0030)==0x0030)//0000 0000 0011 0000
				FT_status.RTCM_VSA=Status_Overvoltage;//A相电压过压
		else if((FT_rtcm.R_RTCM_Bit2&0x0020)==0x0020)//0000 0000 0010 0000
				FT_status.RTCM_VSA=Status_Undervoltage;//A相电压欠压
		else 
				FT_status.RTCM_VSA=Status_Normal;//A相电压正常	
		
		if((FT_rtcm.R_RTCM_Bit2&0x000c)==0x000c)//0000 0000 0000 1100
				FT_status.RTCM_VSB=Status_Overvoltage;//B相电压过压
		else if((FT_rtcm.R_RTCM_Bit2&0x0008)==0x0008)//0000 0000 0000 1000
				FT_status.RTCM_VSB=Status_Undervoltage;//B相电压欠压
		else 
				FT_status.RTCM_VSB=Status_Normal;//B相电压正常	
		
		if((FT_rtcm.R_RTCM_Bit2&0x0003)==0x0003)//0000 0000 0000 0011
				FT_status.RTCM_VSC=Status_Overvoltage;//C相电压过压
		else if((FT_rtcm.R_RTCM_Bit2&0x0002)==0x0002)//0000 0000 0000 0010
				FT_status.RTCM_VSC=Status_Undervoltage;//C相电压欠压
		else 
				FT_status.RTCM_VSC=Status_Normal;//C相电压正常	
		
		if( FT_status.RTCM_CSA==Status_Alarming||FT_status.RTCM_CSB==Status_Alarming||FT_status.RTCM_CSC==Status_Alarming||FT_status.RTCM_LS==Status_Alarming
			||FT_status.RTCM_TS1==Status_Alarming||FT_status.RTCM_TS2==Status_Alarming||FT_status.RTCM_TS3==Status_Alarming||FT_status.RTCM_TS4==Status_Alarming
			||FT_status.RTCM_VSA==Status_Overvoltage||FT_status.RTCM_VSB==Status_Overvoltage||FT_status.RTCM_VSC==Status_Overvoltage
			||FT_status.RTCM_VSA==Status_Undervoltage||FT_status.RTCM_VSB==Status_Undervoltage||FT_status.RTCM_VSC==Status_Undervoltage)
				FT_status.RTCM_S=Status_Alarming;
		else if(FT_status.RTCM_CSA==Status_Broken||FT_status.RTCM_CSB==Status_Broken||FT_status.RTCM_CSC==Status_Broken||FT_status.RTCM_LS==Status_Broken
			||FT_status.RTCM_TS1==Status_Broken||FT_status.RTCM_TS2==Status_Broken||FT_status.RTCM_TS3==Status_Broken||FT_status.RTCM_TS4==Status_Broken)
				FT_status.RTCM_S=Status_Fault;
		else if(FT_status.RTCM_CSA==Status_Short||FT_status.RTCM_CSB==Status_Short||FT_status.RTCM_CSC==Status_Short||FT_status.RTCM_LS==Status_Short
			||FT_status.RTCM_TS1==Status_Short||FT_status.RTCM_TS2==Status_Short||FT_status.RTCM_TS3==Status_Short||FT_status.RTCM_TS4==Status_Short)
				FT_status.RTCM_S=Status_Fault;
		else
				FT_status.RTCM_S=Status_Normal;
		FT_status.RTCM_Status=FT_status.RTCM_S;
}

