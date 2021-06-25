#include "recevice.h"

extern u8 PushRate;

CURR_RTCM FT_rtcm;//�°����ݽṹ��
CURR_RTCM NEW_DATA_FT;//���͵����ݣ����͹����в��ܸ�������ֵ
STATUS_RTCM FT_status;//�°�״̬�ṹ��
STATUS_RTCM NEW_status_FT;//���͵�״̬�����͹����в��ܸ�������ֵ
u8 recevice_buff[MAX_RECEIVE];//����1��������
u16 recevice_cnt=0;//����1�����ֽڵĸ���
u8 recevice_flag=0;//����1״̬


//*********************
//����:�°�����ݸ��Ʒ��͵����ݽṹ����ȥ
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
//����:�������1����
void clear_USART2buff(void)
{
		u16 i=0;
		for(i=0;i<MAX_RECEIVE;i++)recevice_buff[i]=0;
		recevice_cnt=0;
		recevice_flag=0;
}

//*********************
//����:���ͻ�ȡ�����������̽����,��̩ר��
//����:slave num�ӻ���ַ,add ���ʼĴ�����ַ,Quantity���ʵ�����
void Send_Read_Command(u8 slave_num,u8 start_add,u8 Quantity)
{
		u8 buffer[32]={0};
		u16 send_crc16_value=0;
		buffer[0]=slave_num;//��ַ
		buffer[1]=0x03;//������
		buffer[2]=0x00;
		buffer[3]=start_add;//��ʼ��ַ
		buffer[4]=0x00;
		buffer[5]=Quantity;//�Ĵ�������
		send_crc16_value=get_crc16_value(buffer,6);//����CRC
		buffer[6]=send_crc16_value>>8;//��λ
		buffer[7]=send_crc16_value;//��λ
		clear_USART2buff();
		UART_string_amount(buffer,8,2);//����
}

//*********************
//����:��ȡ�������ݣ����ж�̽������Ӧ�Ƿ�ɹ�,��̩ר��
//����:slave num�ӻ���ַ
//����:1:�ɹ���0:ʧ��
u8 R_BasicData_command(u8 slave_num)
{
		u16 time_over=0,read_crc16_value=0;
		Send_Read_Command(slave_num,R_Leak_CURRENT,0x0D);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag�Ƿ��ڴ��ڽ����ж��У���һ���������ͻ���λ  
				else time_over++;
		}
		if(recevice_flag!=0)//���յ�����
		{
				while(recevice_flag!=0)//�ȴ����ݴ������
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1���ַ���ʱ��*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x03&&recevice_buff[2]==0x1A)//�ж�����ͷ���Ƿ���ȷ
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//����ǰrecevice_cnt-2�������CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//�ж�CRC�ߵ�λ�Ƿ�һ��
						{
								FT_rtcm.R_RTCM_Lcurrent=recevice_buff[3]<<8|recevice_buff[4];//©����							    
								FT_rtcm.R_RTCM_Acurrent=recevice_buff[5]<<8|recevice_buff[6];//A����
								FT_rtcm.R_RTCM_Bcurrent=recevice_buff[7]<<8|recevice_buff[8];//B����
								FT_rtcm.R_RTCM_Ccurrent=recevice_buff[9]<<8|recevice_buff[10];//C����
								FT_rtcm.R_RTCM_Atemp=recevice_buff[11]<<8|recevice_buff[12];//A�¶�
								FT_rtcm.R_RTCM_Btemp=recevice_buff[13]<<8|recevice_buff[14];//B�¶�
								FT_rtcm.R_RTCM_Ctemp=recevice_buff[15]<<8|recevice_buff[16];//C�¶�
								FT_rtcm.R_RTCM_Etemp=recevice_buff[17]<<8|recevice_buff[18];//�����¶�
								FT_rtcm.R_RTCM_Avoltage=recevice_buff[19]<<8|recevice_buff[20];//A���ѹ
								FT_rtcm.R_RTCM_Bvoltage=recevice_buff[21]<<8|recevice_buff[22];//B���ѹ
								FT_rtcm.R_RTCM_Cvoltage=recevice_buff[23]<<8|recevice_buff[24];//C���ѹ
								FT_rtcm.R_RTCM_Bit=recevice_buff[25]<<8|recevice_buff[26];//��־λ
								FT_rtcm.R_RTCM_Bit2=recevice_buff[27]<<8|recevice_buff[28];//��־λ
								get_status_of_rtcm();//������־λ
								if(abs(FT_rtcm.R_RTCM_Lcurrent)==32768)FT_status.RTCM_LS=Statue_NONE;//�ж����ް�װ
								if(abs(FT_rtcm.R_RTCM_Acurrent)==32768)FT_status.RTCM_CSA=Statue_NONE;//�ж����ް�װ
								if(abs(FT_rtcm.R_RTCM_Bcurrent)==32768)FT_status.RTCM_CSB=Statue_NONE;//�ж����ް�װ
								if(abs(FT_rtcm.R_RTCM_Ccurrent)==32768)FT_status.RTCM_CSC=Statue_NONE;//�ж����ް�װ
								if(abs(FT_rtcm.R_RTCM_Atemp)==32768)FT_status.RTCM_TS1=Statue_NONE;//�ж����ް�װ
								if(abs(FT_rtcm.R_RTCM_Btemp)==32768)FT_status.RTCM_TS2=Statue_NONE;//�ж����ް�װ
								if(abs(FT_rtcm.R_RTCM_Ctemp)==32768)FT_status.RTCM_TS3=Statue_NONE;//�ж����ް�װ
								if(abs(FT_rtcm.R_RTCM_Etemp)==32768)FT_status.RTCM_TS4=Statue_NONE;//�ж����ް�װ
								if(abs(FT_rtcm.R_RTCM_Avoltage)==32768)FT_status.RTCM_VSA=Statue_NONE;//�ж����ް�װ
								if(abs(FT_rtcm.R_RTCM_Bvoltage)==32768)FT_status.RTCM_VSB=Statue_NONE;//�ж����ް�װ
								if(abs(FT_rtcm.R_RTCM_Cvoltage)==32768)FT_status.RTCM_VSC=Statue_NONE;//�ж����ް�װ
								
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
								return 1;//crcУ����ȷ
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
	}

//*********************
//����:��ȡ�������ݣ����ж�̽������Ӧ�Ƿ�ɹ�,��̩ר��
//����:slave num�ӻ���ַ
//����:1:�ɹ���0:ʧ��
u8 R_AlarmingValue_command(u8 slave_num)
{
		u16 time_over=0,read_crc16_value=0;
		Send_Read_Command(slave_num,R_Leak_CURRENT_Alarming_Value,0x08);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag�Ƿ��ڴ��ڽ����ж��У���һ���������ͻ���λ  
				else time_over++;
		}
		if(recevice_flag!=0)//���յ�����
		{
				while(recevice_flag!=0)//�ȴ����ݴ������
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1���ַ���ʱ��*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x03&&recevice_buff[2]==0x10)//�ж�����ͷ���Ƿ���ȷ
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//����ǰrecevice_cnt-2�������CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//�ж�CRC�ߵ�λ�Ƿ�һ��
						{
								FT_rtcm.R_RTCM_LcAvalue=recevice_buff[3]<<8|recevice_buff[4];//©��������ֵ
								FT_rtcm.R_RTCM_AcAvalue=recevice_buff[5]<<8|recevice_buff[6];//A��������ֵ
								FT_rtcm.R_RTCM_BcAvalue=recevice_buff[7]<<8|recevice_buff[8];//B��������ֵ
								FT_rtcm.R_RTCM_CcAvalue=recevice_buff[9]<<8|recevice_buff[10];//C��������ֵ
								FT_rtcm.R_RTCM_AtAvalue=recevice_buff[11]<<8|recevice_buff[12];//A�¶�����ֵ
								FT_rtcm.R_RTCM_BtAvalue=recevice_buff[13]<<8|recevice_buff[14];//B�¶�����ֵ
								FT_rtcm.R_RTCM_CtAvalue=recevice_buff[15]<<8|recevice_buff[16];//C�¶�����ֵ
								FT_rtcm.R_RTCM_EtAvalue=recevice_buff[17]<<8|recevice_buff[18];//�����¶�����ֵ		
								clear_USART2buff();
								return 1;//crcУ����ȷ
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//����:��ȡӲ���汾���ݣ����ж�̽������Ӧ�Ƿ�ɹ�,��̩ר��
//����:slave num�ӻ���ַ
//����:1:�ɹ���0:ʧ��
u8 R_HardwareVersionNumber_command(u8 slave_num)
{
		u16 time_over=0,read_crc16_value=0;
		Send_Read_Command(slave_num,R_Hardware_Version_Number,0x01);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag�Ƿ��ڴ��ڽ����ж��У���һ���������ͻ���λ  
				else time_over++;
		}
		if(recevice_flag!=0)//���յ�����
		{
				while(recevice_flag!=0)//�ȴ����ݴ������
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1���ַ���ʱ��*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x03&&recevice_buff[2]==0x02)//�ж�����ͷ���Ƿ���ȷ
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//����ǰrecevice_cnt-2�������CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//�ж�CRC�ߵ�λ�Ƿ�һ��
						{
								FT_rtcm.R_RTCM_HardwareVersion_Number=recevice_buff[3]<<8|recevice_buff[4];//�°�Ӳ���汾��	
								clear_USART2buff();
								return 1;//crcУ����ȷ
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//����:��ȡ����汾���ݣ����ж�̽������Ӧ�Ƿ�ɹ�,��̩ר��
//����:slave num�ӻ���ַ
//����:1:�ɹ���0:ʧ��
u8 R_SoftwareVersionNumber_command(u8 slave_num)
{
		u16 time_over=0,read_crc16_value=0;
		Send_Read_Command(slave_num,R_Software_Version_Number,0x01);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag�Ƿ��ڴ��ڽ����ж��У���һ���������ͻ���λ  
				else time_over++;
		}
		if(recevice_flag!=0)//���յ�����
		{
				while(recevice_flag!=0)//�ȴ����ݴ������
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1���ַ���ʱ��*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x03&&recevice_buff[2]==0x02)//�ж�����ͷ���Ƿ���ȷ
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//����ǰrecevice_cnt-2�������CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//�ж�CRC�ߵ�λ�Ƿ�һ��
						{
								FT_rtcm.R_RTCM_SoftwareVersion_Number=recevice_buff[3]<<8|recevice_buff[4];//�°�����汾��	
								clear_USART2buff();
								return 1;//crcУ����ȷ
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//����:��ȡ����ʱ�����ݣ����ж�̽������Ӧ�Ƿ�ɹ�,��̩ר��
//����:slave num�ӻ���ַ
//����:1:�ɹ���0:ʧ��
u8 R_ActionTime_command(u8 slave_num)
{
		u16 time_over=0,read_crc16_value=0;
		Send_Read_Command(slave_num,R_Leak_CURRENT_Action_Time,0x08);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag�Ƿ��ڴ��ڽ����ж��У���һ���������ͻ���λ  
				else time_over++;
		}
		if(recevice_flag!=0)//���յ�����
		{
				while(recevice_flag!=0)//�ȴ����ݴ������
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1���ַ���ʱ��*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x03&&recevice_buff[2]==0x10)//�ж�����ͷ���Ƿ���ȷ
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//����ǰrecevice_cnt-2�������CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//�ж�CRC�ߵ�λ�Ƿ�һ��
						{
								FT_rtcm.R_RTCM_LcActionTime=recevice_buff[3]<<8|recevice_buff[4];//©��������ʱ��
								FT_rtcm.R_RTCM_AcActionTime=recevice_buff[5]<<8|recevice_buff[6];//A��������ʱ��
								FT_rtcm.R_RTCM_BcActionTime=recevice_buff[7]<<8|recevice_buff[8];//B��������ʱ��
								FT_rtcm.R_RTCM_CcActionTime=recevice_buff[9]<<8|recevice_buff[10];//C��������ʱ��
								FT_rtcm.R_RTCM_AtActionTime=recevice_buff[11]<<8|recevice_buff[12];//A�¶ȶ���ʱ��
								FT_rtcm.R_RTCM_BtActionTime=recevice_buff[13]<<8|recevice_buff[14];//B�¶ȶ���ʱ��
								FT_rtcm.R_RTCM_CtActionTime=recevice_buff[15]<<8|recevice_buff[16];//C�¶ȶ���ʱ��
								FT_rtcm.R_RTCM_EtActionTime=recevice_buff[17]<<8|recevice_buff[18];//�����¶ȶ���ʱ��
								clear_USART2buff();
								return 1;//crcУ����ȷ
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//����:��̽����д����ֵ�����ж�̽������Ӧ�Ƿ�ɹ�,��̩ר��
//����:slave num�ӻ���ַ,A,B,C�������©�������ޣ�A,B,C��,�����¶�����
//����:1:�ɹ���0:ʧ��
u8 send_Write_Alarming_Value_command(u8 slave_num,u16 Lc_Value,u16 Ac_Value,u16 Bc_Value,u16 Cc_Value,s16 At_Value,s16 Bt_Value,s16 Ct_Value,s16 Et_Value)
{
		u8 buffer[32];
		u16 send_crc16_value=0,read_crc16_value=0,time_over=0;
		buffer[0]=slave_num;//��ַ
		buffer[1]=0x10;//������
		buffer[2]=0x00;
		buffer[3]=W_Leak_CURRENT_Alarming_Value;//��ʼ��ַ
		buffer[4]=0x00;
		buffer[5]=0x08;//�Ĵ�������
		buffer[6]=0x10;//�ֽڸ���
		buffer[7]=Lc_Value>>8;buffer[8]=Lc_Value;//©��������ֵ
		buffer[9]=Ac_Value>>8;buffer[10]=Ac_Value;//A��������ֵ
		buffer[11]=Bc_Value>>8;buffer[12]=Bc_Value;//B��������ֵ
		buffer[13]=Cc_Value>>8;buffer[14]=Cc_Value;//C��������ֵ
		buffer[15]=At_Value>>8;buffer[16]=At_Value;//A�¶�����ֵ
		buffer[17]=Bt_Value>>8;buffer[18]=Bt_Value;//B�¶�����ֵ
		buffer[19]=Ct_Value>>8;buffer[20]=Ct_Value;//C�¶�����ֵ
		buffer[21]=Et_Value>>8;buffer[22]=Et_Value;//�����¶�����ֵ
		send_crc16_value=get_crc16_value(buffer,23);//����CRC
		buffer[23]=send_crc16_value>>8;buffer[24]=send_crc16_value;
		clear_USART2buff();
		UART_string_amount(buffer,25,2);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag�Ƿ��ڴ��ڽ����ж��У���һ���������ͻ���λ  
				else time_over++;
		}
		if(recevice_flag!=0)//���յ�����
		{
				while(recevice_flag!=0)//�ȴ����ݴ������
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1���ַ���ʱ��*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x10&&recevice_buff[2]==0x00&&recevice_buff[3]==W_Leak_CURRENT_Alarming_Value)//�ж�����ͷ���Ƿ���ȷ
				{
					read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//����ǰrecevice_cnt-2�������CRC
					if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//�ж�CRC�ߵ�λ�Ƿ�һ��
					{
							clear_USART2buff();return 1;//crcУ����ȷ
					}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//����:��̽����д����ʱ�䣬���ж�̽������Ӧ�Ƿ�ɹ�,��̩ר��
//����:slave num�ӻ���ַ,A,B,C�������©��������ʱ�䣬A,B,C��,�����¶ȶ���ʱ��
//����:1:�ɹ���0:ʧ��
u8 send_Write_Action_Time_command(u8 slave_num,u16 Lc_Value,u16 Ac_Value,u16 Bc_Value,u16 Cc_Value,s16 At_Value,s16 Bt_Value,s16 Ct_Value,s16 Et_Value)
{
		u8 buffer[32];
		u16 send_crc16_value=0,read_crc16_value=0,time_over=0;
		buffer[0]=slave_num;//��ַ
		buffer[1]=0x10;//������
		buffer[2]=0x00;
		buffer[3]=W_Leak_CURRENT_Action_Time;//��ʼ��ַ
		buffer[4]=0x00;
		buffer[5]=0x08;//�Ĵ�������
		buffer[6]=0x10;//�ֽڸ���
		buffer[7]=Lc_Value>>8;buffer[8]=Lc_Value;//©��������ʱ��
		buffer[9]=Ac_Value>>8;buffer[10]=Ac_Value;//A��������ʱ��
		buffer[11]=Bc_Value>>8;buffer[12]=Bc_Value;//B��������ʱ��
		buffer[13]=Cc_Value>>8;buffer[14]=Cc_Value;//C��������ʱ��
		buffer[15]=At_Value>>8;buffer[16]=At_Value;//A�¶ȶ���ʱ��
		buffer[17]=Bt_Value>>8;buffer[18]=Bt_Value;//B�¶ȶ���ʱ��
		buffer[19]=Ct_Value>>8;buffer[20]=Ct_Value;//C�¶ȶ���ʱ��
		buffer[21]=Et_Value>>8;buffer[22]=Et_Value;//�����¶ȶ���ʱ��
		send_crc16_value=get_crc16_value(buffer,23);//����CRC
		buffer[23]=send_crc16_value>>8;buffer[24]=send_crc16_value;
		clear_USART2buff();
		UART_string_amount(buffer,25,2);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag�Ƿ��ڴ��ڽ����ж��У���һ���������ͻ���λ  
				else time_over++;
		}
		if(recevice_flag!=0)//���յ�����
		{
			while(recevice_flag!=0)//�ȴ����ݴ������
			{
					recevice_flag=0;
					delay_ms(CalCurrentBaud1Char()*2);//1���ַ���ʱ��*2 
			}
			if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x10&&recevice_buff[2]==0x00&&recevice_buff[3]==W_Leak_CURRENT_Action_Time)//�ж�����ͷ���Ƿ���ȷ
			{
					read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//����ǰrecevice_cnt-2�������CRC
					if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//�ж�CRC�ߵ�λ�Ƿ�һ��
					{
							clear_USART2buff();return 1;//crcУ����ȷ
					}
			}
			else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//����:��̽����д��λ�����ж�̽������Ӧ�Ƿ�ɹ�,��̩ר��
//����:slave num�ӻ���ַ
//����:1:�ɹ���0:ʧ��
u8 send_Wirte_RESET_command(u8 slave_num)
{
		u8 buffer[16];
		u16 send_crc16_value=0,read_crc16_value=0,time_over=0;
		buffer[0]=slave_num;//��ַ
		buffer[1]=0x10;//������
		buffer[2]=0x00;
		buffer[3]=W_RESET;//��ʼ��ַ
		buffer[4]=0x00;
		buffer[5]=0x01;//�Ĵ�������
		buffer[6]=0x02;//�ֽڸ���
		buffer[7]=0x00;buffer[8]=0x01;
		send_crc16_value=get_crc16_value(buffer,9);
		buffer[9]=send_crc16_value>>8;buffer[10]=send_crc16_value;
		clear_USART2buff();
		UART_string_amount(buffer,11,2);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag�Ƿ��ڴ��ڽ����ж��У���һ���������ͻ���λ  
				else time_over++;
		}
		if(recevice_flag!=0)//���յ�����
		{
				while(recevice_flag!=0)//�ȴ����ݴ������
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1���ַ���ʱ��*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x10&&recevice_buff[2]==0x00&&recevice_buff[3]==W_RESET)//�ж�����ͷ���Ƿ���ȷ
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//����ǰrecevice_cnt-2�������CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//�ж�CRC�ߵ�λ�Ƿ�һ��
						{
								clear_USART2buff();return 1;//crcУ����ȷ
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//����:��̽����д�Լ죬���ж�̽������Ӧ�Ƿ�ɹ�,��̩ר��
//����:slave num�ӻ���ַ
//����:1:�ɹ���0:ʧ��
u8 send_Write_SelfCheck_command(u8 slave_num)
{
		u8 buffer[16];
		u16 send_crc16_value=0,read_crc16_value=0,time_over=0;
		buffer[0]=slave_num;//��ַ
		buffer[1]=0x10;//������
		buffer[2]=0x00;
		buffer[3]=W_Self_Check;//��ʼ��ַ
		buffer[4]=0x00;
		buffer[5]=0x01;//�Ĵ�������
		buffer[6]=0x02;//�ֽڸ���
		buffer[7]=0x00;buffer[8]=0x01;
		send_crc16_value=get_crc16_value(buffer,9);
		buffer[9]=send_crc16_value>>8;buffer[10]=send_crc16_value;
		clear_USART2buff();
		UART_string_amount(buffer,11,2);
		delay_ms(DELAY_RECEIVE);
		while(1)
		{
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag�Ƿ��ڴ��ڽ����ж��У���һ���������ͻ���λ  
				else time_over++;
		}
		if(recevice_flag!=0)//���յ�����
		{
				while(recevice_flag!=0)//�ȴ����ݴ������
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1���ַ���ʱ��*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x10&&recevice_buff[2]==0x00&&recevice_buff[3]==W_Self_Check)//�ж�����ͷ���Ƿ���ȷ
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//����ǰrecevice_cnt-2�������CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//�ж�CRC�ߵ�λ�Ƿ�һ��
						{
								clear_USART2buff();return 1;//crcУ����ȷ
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//����:��̽����д�����������ã����ж�̽������Ӧ�Ƿ�ɹ�,��̩ר��
//����:slave num�ӻ���ַ,mode��ͨ���ǹر�
//����:1:�ɹ���0:ʧ��
u8 send_Write_FireLinkage_command(u8 slave_num,u8 mode)
{
		u8 buffer[16];
		u16 send_crc16_value=0,read_crc16_value=0,time_over=0;
		buffer[0]=slave_num;//��ַ
		buffer[1]=0x10;//������
		buffer[2]=0x00;
		buffer[3]=W_Fire_Linkage;//��ʼ��ַ
		buffer[4]=0x00;
		buffer[5]=0x01;//�Ĵ�������
		buffer[6]=0x02;//�ֽڸ���
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
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag�Ƿ��ڴ��ڽ����ж��У���һ���������ͻ���λ  
				else time_over++;
		}
		if(recevice_flag!=0)//���յ�����
		{
				while(recevice_flag!=0)//�ȴ����ݴ������
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1���ַ���ʱ��*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x10&&recevice_buff[2]==0x00&&recevice_buff[3]==W_Fire_Linkage)//�ж�����ͷ���Ƿ���ȷ
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//����ǰrecevice_cnt-2�������CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//�ж�CRC�ߵ�λ�Ƿ�һ��
						{
								clear_USART2buff();return 1;//crcУ����ȷ
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//����:��̽����д�̵�������ģʽ�����ж�̽������Ӧ�Ƿ�ɹ�,��̩ר��
//����:slave num�ӻ���ַ,mode��ͨ���ǹر�
//����:1:�ɹ���0:ʧ��
u8 send_Write_RelayMode_command(u8 slave_num,u8 mode)
{
		u8 buffer[16];
		u16 send_crc16_value=0,read_crc16_value=0,time_over=0;
		buffer[0]=slave_num;//��ַ
		buffer[1]=0x10;//������
		buffer[2]=0x00;
		buffer[3]=W_Relay_Mode;//��ʼ��ַ
		buffer[4]=0x00;
		buffer[5]=0x01;//�Ĵ�������
		buffer[6]=0x02;//�ֽڸ���
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
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag�Ƿ��ڴ��ڽ����ж��У���һ���������ͻ���λ  
				else time_over++;
		}
		if(recevice_flag!=0)//���յ�����
		{
				while(recevice_flag!=0)//�ȴ����ݴ������
				{
						recevice_flag=0;
						delay_ms(CalCurrentBaud1Char()*2);//1���ַ���ʱ��*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x10&&recevice_buff[2]==0x00&&recevice_buff[3]==W_Relay_Mode)//�ж�����ͷ���Ƿ���ȷ
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//����ǰrecevice_cnt-2�������CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//�ж�CRC�ߵ�λ�Ƿ�һ��
						{
							clear_USART2buff();return 1;//crcУ����ȷ
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//*********************
//����:��̽����дGPRSͨ�Ź��ϣ����ж�̽������Ӧ�Ƿ�ɹ�,��̩ר��
//����:slave num�ӻ���ַ,mode��ͨ���ǹر�
//����:1:�ɹ���0:ʧ��
u8 send_Write_GPRSCommunication_command(u8 slave_num,u8 mode)
{
		u8 buffer[16];
		u16 send_crc16_value=0,read_crc16_value=0,time_over=0;
		buffer[0]=slave_num;//��ַ
		buffer[1]=0x10;//������
		buffer[2]=0x00;
		buffer[3]=W_GPRS_Communication_Fault;//��ʼ��ַ
		buffer[4]=0x00;
		buffer[5]=0x01;//�Ĵ�������
		buffer[6]=0x02;//�ֽڸ���
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
				if((recevice_flag!=0)||(time_over==60000))break;//recevice_flag�Ƿ��ڴ��ڽ����ж��У���һ���������ͻ���λ  
				else time_over++;
		}
		if(recevice_flag!=0)//���յ�����
		{
				while(recevice_flag!=0)//�ȴ����ݴ������
				{
					recevice_flag=0;
					delay_ms(CalCurrentBaud1Char()*2);//1���ַ���ʱ��*2 
				}
				if(recevice_buff[0]==slave_num&&recevice_buff[1]==0x10&&recevice_buff[2]==0x00&&recevice_buff[3]==W_GPRS_Communication_Fault)//�ж�����ͷ���Ƿ���ȷ
				{
						read_crc16_value=get_crc16_value(recevice_buff,recevice_cnt-2);//����ǰrecevice_cnt-2�������CRC
						if(recevice_buff[recevice_cnt-2]==read_crc16_value>>8&&recevice_buff[recevice_cnt-1]==(u8)read_crc16_value)//�ж�CRC�ߵ�λ�Ƿ�һ��
						{
								clear_USART2buff();return 1;//crcУ����ȷ
						}
				}
				else{clear_USART2buff();return 0;}
		}
		clear_USART2buff();
		return 0;
}

//***********************
//����:��̬�ϴ�ר�ã�PushrateΪ������С�������ϴε�ֵ�Ƚϣ��仯���ڸ�ֵʱ������1
//����:���ڷ�Χ����1��ȫ��û����Χ������0
u8 ABS_PushMode(void)
{
		if(abs(FT_rtcm.last_R_RTCM_Acurrent-FT_rtcm.R_RTCM_Acurrent)>PushRate/100.0*400)//A������ж�
			return 1;
		else if(abs(FT_rtcm.last_R_RTCM_Bcurrent-FT_rtcm.R_RTCM_Bcurrent)>PushRate/100.0*400)//B������ж�
			return 1;
		else if(abs(FT_rtcm.last_R_RTCM_Ccurrent-FT_rtcm.R_RTCM_Ccurrent)>PushRate/100.0*400)//C������ж�
			return 1;
		else if(abs(FT_rtcm.last_R_RTCM_Lcurrent-FT_rtcm.R_RTCM_Lcurrent)>PushRate/100.0*100)//©�����ж�
			return 1;
		else if(abs(FT_rtcm.last_R_RTCM_Atemp-FT_rtcm.last_R_RTCM_Atemp)>PushRate/100.0*150)//A���¶�
			return 1;
		else if(abs(FT_rtcm.last_R_RTCM_Btemp-FT_rtcm.last_R_RTCM_Btemp)>PushRate/100.0*150)//B���¶�
			return 1;
		else if(abs(FT_rtcm.last_R_RTCM_Ctemp-FT_rtcm.last_R_RTCM_Ctemp)>PushRate/100.0*150)//C���¶�
			return 1;
		else if(abs(FT_rtcm.last_R_RTCM_Etemp-FT_rtcm.last_R_RTCM_Etemp)>PushRate/100.0*150)//�����¶�
			return 1;
		return 0;
}

//***********************
//����:�ж�״̬λ����������·����·������
//����:��ǰ��״̬�仯����Щ���ͣ�0x01 0x02 0x04 0x08
u8 Judge_Status(void)
{
		u8 flag_judge_return=0;
		static u8 first_judge=0;
		if(first_judge==0)//��һ���ж�
		{
				if((FT_status.RTCM_CSA==Status_Alarming)||(FT_status.RTCM_CSB==Status_Alarming)||(FT_status.RTCM_CSC==Status_Alarming)||(FT_status.RTCM_LS==Status_Alarming))
						flag_judge_return=0x01;//����
				if((FT_status.RTCM_TS1==Status_Alarming)||(FT_status.RTCM_TS2==Status_Alarming)||(FT_status.RTCM_TS3==Status_Alarming)||(FT_status.RTCM_TS4==Status_Alarming))
						flag_judge_return=0x01;//����
				if((FT_status.RTCM_VSA==Status_Overvoltage)||(FT_status.RTCM_VSB==Status_Overvoltage)||(FT_status.RTCM_VSC==Status_Overvoltage))
						flag_judge_return=0x01;//����
				if((FT_status.RTCM_VSA==Status_Undervoltage)||(FT_status.RTCM_VSB==Status_Undervoltage)||(FT_status.RTCM_VSC==Status_Undervoltage))
						flag_judge_return=0x01;//����
				if(FT_status.RTCM_LS==Status_Broken)
						flag_judge_return=flag_judge_return|0x02;//����
				if((FT_status.RTCM_TS1==Status_Broken)||(FT_status.RTCM_TS2==Status_Broken)||(FT_status.RTCM_TS3==Status_Broken)||(FT_status.RTCM_TS4==Status_Broken))
						flag_judge_return=flag_judge_return|0x02;//����
				if(FT_status.RTCM_LS==Status_Short)
						flag_judge_return=flag_judge_return|0x04;//��·
				if((FT_status.RTCM_TS1==Status_Short)||(FT_status.RTCM_TS2==Status_Short)||(FT_status.RTCM_TS3==Status_Short)||(FT_status.RTCM_TS4==Status_Short))
						flag_judge_return=flag_judge_return|0x04;//��·
				first_judge=1;
		}
		else 
		{
				if((FT_status.RTCM_CSA!=FT_status.last_RTCM_CSA&&FT_status.RTCM_CSA==Status_Alarming)
					||(FT_status.RTCM_CSB!=FT_status.last_RTCM_CSB&&FT_status.RTCM_CSB==Status_Alarming)
					||(FT_status.RTCM_CSC!=FT_status.last_RTCM_CSC&&FT_status.RTCM_CSC==Status_Alarming)
					||(FT_status.RTCM_LS!=FT_status.last_RTCM_LS&&FT_status.RTCM_LS==Status_Alarming))
						flag_judge_return=0x01;//����
				if((FT_status.RTCM_TS1!=FT_status.last_RTCM_TS1&&FT_status.RTCM_TS1==Status_Alarming)
					||(FT_status.RTCM_TS2!=FT_status.last_RTCM_TS2&&FT_status.RTCM_TS2==Status_Alarming)
					||(FT_status.RTCM_TS3!=FT_status.last_RTCM_TS3&&FT_status.RTCM_TS3==Status_Alarming)
					||(FT_status.RTCM_TS4!=FT_status.last_RTCM_TS4&&FT_status.RTCM_TS4==Status_Alarming))
						flag_judge_return=0x01;//����
				if((FT_status.RTCM_VSA!=FT_status.last_RTCM_VSA&&FT_status.RTCM_VSA==Status_Alarming)
					||(FT_status.RTCM_VSB!=FT_status.last_RTCM_VSB&&FT_status.RTCM_VSB==Status_Alarming)
					||(FT_status.RTCM_VSC!=FT_status.last_RTCM_VSC&&FT_status.RTCM_VSC==Status_Alarming))
						flag_judge_return=0x01;//����
				if(FT_status.RTCM_LS!=FT_status.last_RTCM_LS&&FT_status.RTCM_LS==Status_Broken)
						flag_judge_return=flag_judge_return|0x02;//����
				if(FT_status.RTCM_LS!=FT_status.last_RTCM_LS&&FT_status.RTCM_LS==Status_Short)
						flag_judge_return=flag_judge_return|0x04;//��·
				if((FT_status.RTCM_TS1!=FT_status.last_RTCM_TS1&&FT_status.RTCM_TS1==Status_Broken)
					||(FT_status.RTCM_TS2!=FT_status.last_RTCM_TS2&&FT_status.RTCM_TS2==Status_Broken)
					||(FT_status.RTCM_TS3!=FT_status.last_RTCM_TS3&&FT_status.RTCM_TS3==Status_Broken)
					||(FT_status.RTCM_TS4!=FT_status.last_RTCM_TS4&&FT_status.RTCM_TS4==Status_Broken))
						flag_judge_return=flag_judge_return|0x02;//����
				if((FT_status.RTCM_TS1!=FT_status.last_RTCM_TS1&&FT_status.RTCM_TS1==Status_Short)
					||(FT_status.RTCM_TS2!=FT_status.last_RTCM_TS2&&FT_status.RTCM_TS2==Status_Short)
					||(FT_status.RTCM_TS3!=FT_status.last_RTCM_TS3&&FT_status.RTCM_TS3==Status_Short)
					||(FT_status.RTCM_TS4!=FT_status.last_RTCM_TS4&&FT_status.RTCM_TS4==Status_Short))
						flag_judge_return=flag_judge_return|0x04;//��·	
				if((FT_status.RTCM_VSA!=FT_status.last_RTCM_VSA&&FT_status.RTCM_VSA==Status_Overvoltage)
					||(FT_status.RTCM_VSB!=FT_status.last_RTCM_VSB&&FT_status.RTCM_VSB==Status_Overvoltage)
					||(FT_status.RTCM_VSC!=FT_status.last_RTCM_VSC&&FT_status.RTCM_VSC==Status_Overvoltage))
						flag_judge_return=flag_judge_return|0x02;//��ѹ
				if((FT_status.RTCM_VSA!=FT_status.last_RTCM_VSA&&FT_status.RTCM_VSA==Status_Undervoltage)
					||(FT_status.RTCM_VSB!=FT_status.last_RTCM_VSB&&FT_status.RTCM_VSB==Status_Undervoltage)
					||(FT_status.RTCM_VSC!=FT_status.last_RTCM_VSC&&FT_status.RTCM_VSC==Status_Undervoltage))
						flag_judge_return=flag_judge_return|0x04;//Ƿѹ	
				if(FT_status.RTCM_S==Status_Normal&&FT_status.RTCM_S!=FT_status.last_RTCM_S)
						flag_judge_return=flag_judge_return|0x08;//�Ӳ�����״̬��Ϊ����״̬
				else if(FT_status.RTCM_CSA==Status_Normal&&FT_status.last_RTCM_CSA!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//�Ӳ�����״̬��Ϊ����״̬
				else if(FT_status.RTCM_CSB==Status_Normal&&FT_status.last_RTCM_CSB!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//�Ӳ�����״̬��Ϊ����״̬
				else if(FT_status.RTCM_CSC==Status_Normal&&FT_status.last_RTCM_CSC!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//�Ӳ�����״̬��Ϊ����״̬
				else if(FT_status.RTCM_LS==Status_Normal&&FT_status.last_RTCM_LS!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//�Ӳ�����״̬��Ϊ����״̬
				else if(FT_status.RTCM_TS1==Status_Normal&&FT_status.last_RTCM_TS1!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//�Ӳ�����״̬��Ϊ����״̬
				else if(FT_status.RTCM_TS2==Status_Normal&&FT_status.last_RTCM_TS2!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//�Ӳ�����״̬��Ϊ����״̬
				else if(FT_status.RTCM_TS3==Status_Normal&&FT_status.last_RTCM_TS3!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//�Ӳ�����״̬��Ϊ����״̬
				else if(FT_status.RTCM_TS4==Status_Normal&&FT_status.last_RTCM_TS4!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//�Ӳ�����״̬��Ϊ����״̬
				else if(FT_status.RTCM_VSA==Status_Normal&&FT_status.last_RTCM_VSA!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//�Ӳ�����״̬��Ϊ����״̬
				else if(FT_status.RTCM_VSB==Status_Normal&&FT_status.last_RTCM_VSB!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//�Ӳ�����״̬��Ϊ����״̬
				else if(FT_status.RTCM_VSC==Status_Normal&&FT_status.last_RTCM_VSC!=Status_Normal)
						flag_judge_return=flag_judge_return|0x08;//�Ӳ�����״̬��Ϊ����״̬
		}
		return flag_judge_return;
}

//***********************
//����:��ȡ�°�ĵ�ǰ״̬
void get_status_of_rtcm(void)
{
		if((FT_rtcm.R_RTCM_Bit&0xc000)==0xc000)//1100 0000 0000 0000
				FT_status.RTCM_LS=Status_Alarming;//©��������
		else if((FT_rtcm.R_RTCM_Bit&0x4000)==0x4000)//0100 0000 0000 0000
				FT_status.RTCM_LS=Status_Broken;//©��������
		else if((FT_rtcm.R_RTCM_Bit&0x8000)==0x8000)//1000 0000 0000 0000
				FT_status.RTCM_LS=Status_Short;//©��������
		else 
				FT_status.RTCM_LS=Status_Normal;//©��������
		
		if((FT_rtcm.R_RTCM_Bit&0x3000)==0x3000)//0011 0000 0000 0000
				FT_status.RTCM_CSA=Status_Alarming;//A���������
//		else if((FT_rtcm.R_RTCM_Bit&0x1000)==0x1000)//0001 0000 0000 0000
//				FT_status.RTCM_CSA=Status_Broken;//A���������
//		else if((FT_rtcm.R_RTCM_Bit&0x2000)==0x2000)//0010 0000 0000 0000
//				FT_status.RTCM_CSA=Status_Short;//A���������
		else 
				FT_status.RTCM_CSA=Status_Normal;//A���������
		
		if((FT_rtcm.R_RTCM_Bit&0x0c00)==0x0c00)//0000 1100 0000 0000
				FT_status.RTCM_CSB=Status_Alarming;//B���������
//		else if((FT_rtcm.R_RTCM_Bit&0x1000)==0x1000)//0001 0000 0000 0000
//				FT_status.RTCM_CSB=Status_Broken;//B���������
//		else if((FT_rtcm.R_RTCM_Bit&0x2000)==0x2000)//0010 0000 0000 0000
//				FT_status.RTCM_CSB=Status_Short;//B���������
		else 
				FT_status.RTCM_CSB=Status_Normal;//B���������
		
		if((FT_rtcm.R_RTCM_Bit&0x0300)==0x0300)//0000 0011 0000 0000
				FT_status.RTCM_CSC=Status_Alarming;//C���������
//		else if((FT_rtcm.R_RTCM_Bit&0x0400)==0x0400)//0000 0100 0000 0000
//				FT_status.RTCM_CSC=Status_Broken;//C���������
//		else if((FT_rtcm.R_RTCM_Bit&0x0800)==0x0800)//0000 1000 0000 0000
//				FT_status.RTCM_CSC=Status_Short;//C���������
		else 
				FT_status.RTCM_CSC=Status_Normal;//C���������
		
		if((FT_rtcm.R_RTCM_Bit&0x00c0)==0x00c0)//0000 0000 1100 0000
				FT_status.RTCM_TS1=Status_Alarming;//A���¶ȳ���
		else if((FT_rtcm.R_RTCM_Bit&0x0040)==0x0040)//0000 0000 0100 0000
				FT_status.RTCM_TS1=Status_Broken;//A���¶ȶ���
		else if((FT_rtcm.R_RTCM_Bit&0x0080)==0x0080)//0000 0000 1000 0000
				FT_status.RTCM_TS1=Status_Short;//A���¶ȶ���
		else 
				FT_status.RTCM_TS1=Status_Normal;//A���¶�����	
		
		if((FT_rtcm.R_RTCM_Bit&0x0030)==0x0030)//0000 0000 0011 0000
				FT_status.RTCM_TS2=Status_Alarming;//B���¶ȳ���
		else if((FT_rtcm.R_RTCM_Bit&0x0010)==0x0010)//0000 0000 0001 0000
				FT_status.RTCM_TS2=Status_Broken;//B���¶ȶ���
		else if((FT_rtcm.R_RTCM_Bit&0x0020)==0x0020)//0000 0000 0010 0000
				FT_status.RTCM_TS2=Status_Short;//B���¶ȶ���
		else 
				FT_status.RTCM_TS2=Status_Normal;//B���¶�����	
		
		if((FT_rtcm.R_RTCM_Bit&0x000c)==0x000c)//0000 0000 0000 1100
				FT_status.RTCM_TS3=Status_Alarming;//C���¶ȳ���
		else if((FT_rtcm.R_RTCM_Bit&0x0004)==0x0004)//0000 0000 0000 0100
				FT_status.RTCM_TS3=Status_Broken;//C���¶ȶ���
		else if((FT_rtcm.R_RTCM_Bit&0x0008)==0x0008)//0000 0000 0000 1000
				FT_status.RTCM_TS3=Status_Short;//C���¶ȶ���
		else 
				FT_status.RTCM_TS3=Status_Normal;//C���¶�����	
		
		if((FT_rtcm.R_RTCM_Bit&0x0003)==0x0003)//0000 0000 0000 0011
				FT_status.RTCM_TS4=Status_Alarming;//�����¶ȳ���
		else if((FT_rtcm.R_RTCM_Bit&0x0001)==0x0001)//0000 0000 0000 0001
				FT_status.RTCM_TS4=Status_Broken;//�����¶ȶ���
		else if((FT_rtcm.R_RTCM_Bit&0x0002)==0x0002)//0000 0000 0000 0010
				FT_status.RTCM_TS4=Status_Short;//�����¶ȶ���
		else 
				FT_status.RTCM_TS4=Status_Normal;//�����¶�����

		if((FT_rtcm.R_RTCM_Bit2&0x0030)==0x0030)//0000 0000 0011 0000
				FT_status.RTCM_VSA=Status_Overvoltage;//A���ѹ��ѹ
		else if((FT_rtcm.R_RTCM_Bit2&0x0020)==0x0020)//0000 0000 0010 0000
				FT_status.RTCM_VSA=Status_Undervoltage;//A���ѹǷѹ
		else 
				FT_status.RTCM_VSA=Status_Normal;//A���ѹ����	
		
		if((FT_rtcm.R_RTCM_Bit2&0x000c)==0x000c)//0000 0000 0000 1100
				FT_status.RTCM_VSB=Status_Overvoltage;//B���ѹ��ѹ
		else if((FT_rtcm.R_RTCM_Bit2&0x0008)==0x0008)//0000 0000 0000 1000
				FT_status.RTCM_VSB=Status_Undervoltage;//B���ѹǷѹ
		else 
				FT_status.RTCM_VSB=Status_Normal;//B���ѹ����	
		
		if((FT_rtcm.R_RTCM_Bit2&0x0003)==0x0003)//0000 0000 0000 0011
				FT_status.RTCM_VSC=Status_Overvoltage;//C���ѹ��ѹ
		else if((FT_rtcm.R_RTCM_Bit2&0x0002)==0x0002)//0000 0000 0000 0010
				FT_status.RTCM_VSC=Status_Undervoltage;//C���ѹǷѹ
		else 
				FT_status.RTCM_VSC=Status_Normal;//C���ѹ����	
		
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

