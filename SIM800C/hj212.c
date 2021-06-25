#include "all.h" 
#include "stdlib.h"
extern CURR_RTCM NEW_DATA_FT;//发送的数据，发送过程中不能更新数据值
extern u8 IMEIbuff[18];//IMEI
extern u8 TIMEbuff[25];
//用于存放从flash读取的数据
extern u8  FlashS;
extern u8 FlashIMEI[25];
extern u8 FlashIP[20];
extern u8 FlashPORT[15];
extern u16 FlashST;
extern u8 FlashPW[15];
extern u8 FlashMN[30];
extern u8 FlashDC[15];
extern u8 FlashPLOID[15];
extern u8 FlashWIRING;
extern u8 FlashCT[15];
extern u8 FlashPT[15];
extern u8 FlashisSAVE[15];
extern u8 FlashLNG[15];
extern u8 FlashLAT[15];
extern u8 FlashUID[15];
extern u8 FlashUGUID[50];
extern u8 FlashCMD[15];

extern CURR_RTCM FT_rtcm;//下板数据结构体

char *MN="202001010001202001010004";

/****************************************************************************************
函 数: CRC16_Checkout
描 述: CRC16 循环冗余校验算法。
参 数 一: *puchMsg：需要校验的字符串指针
参 数 二: usDataLen：要校验的字符串长度
返 回 值: 返回 CRC16 校验码
****************************************************************************************/
unsigned int CRC16_Checkout( char *puchMsg, unsigned int usDataLen)
{
	unsigned int i,j,crc_reg,check;
	crc_reg = 0xFFFF;
	for(i=0;i<usDataLen;i++)
	{
		crc_reg = (crc_reg>>8) ^ puchMsg[i];
		for(j=0;j<8;j++)
		{
			check = crc_reg & 0x0001;
			crc_reg >>= 1;
			if(check==0x0001)
			{
				crc_reg ^= 0xA001;
			}
		}
	}
	return crc_reg;
}
u16 PdataLen=0;
u8 _hj212_packdatatoUP(u8 *Pack,u8 *pdata,u16 ST,u16 CN,char *MN)
{
	char LEnbuff[10]={0};
	char CRCbuff[10]={0};
	char QNbuff[20]={0};
	char databuff[10]={0};
	sprintf(QNbuff,"%s%d",TIMEbuff,rand()%100);
	strcat((char *)Pack,HJ212_FRAME_BAOTOU);
	//数据部分
	strcat((char *)pdata,HJ212_FRAME_QN);
	strcat((char *)pdata,QNbuff);
	strcat((char *)pdata,";");
	strcat((char *)pdata,HJ212_FRAME_ST);	
	strcat((char *)pdata,(char*)U16IntToStr(ST));
	strcat((char *)pdata,";");
	strcat((char *)pdata,HJ212_FRAME_CN);
	strcat((char *)pdata,(char*)U16IntToStr(CN));
	strcat((char *)pdata,";");	
    strcat((char *)pdata,HJ212_FRAME_PW);
	strcat((char *)pdata,(char*)FlashPW);
	strcat((char *)pdata,";");
	strcat((char *)pdata,HJ212_FRAME_MN);
	strcat((char *)pdata,MN);
	strcat((char *)pdata,";");
	strcat((char *)pdata,HJ212_FRAME_FLAG);
	strcat((char *)pdata,"5");
	strcat((char *)pdata,";");
    strcat((char *)pdata,HJ212_FRAME_CP_START);
	strcat((char *)pdata,HJ212_FIELD_NAME_DATA_TIME);
	strcat((char *)pdata,(char *)TIMEbuff);
	strcat((char *)pdata,";");
	strcat((char *)pdata,(char*)FlashDC);
	strcat((char *)pdata,"-Ia=");
	sprintf(databuff,"%d",FT_rtcm.R_RTCM_Acurrent);
	strcat((char *)pdata,databuff);
	strcat((char *)pdata,",");	
	strcat((char *)pdata,(char*)FlashDC);
	strcat((char *)pdata,"-Ib=");
	sprintf(databuff,"%d",FT_rtcm.R_RTCM_Bcurrent);
	strcat((char *)pdata,databuff);
	strcat((char *)pdata,",");	
	strcat((char *)pdata,(char*)FlashDC); 
	strcat((char *)pdata,"-Ic=");
	sprintf(databuff,"%d",FT_rtcm.R_RTCM_Ccurrent);
	strcat((char *)pdata,databuff);
	strcat((char *)pdata,",");	
	strcat((char *)pdata,(char*)FlashDC);
	strcat((char *)pdata,"-Pa=");  //注意功率范围，数值要够大，现测试
	sprintf(databuff,"%d",FT_rtcm.R_RTCM_APower);
	strcat((char *)pdata,databuff);
	strcat((char *)pdata,",");
	strcat((char *)pdata,(char*)FlashDC);
    strcat((char *)pdata,"-Pb=");  //注意功率范围，数值要够大，现测试
	sprintf(databuff,"%d",FT_rtcm.R_RTCM_BPower);
	strcat((char *)pdata,databuff);
	strcat((char *)pdata,",");
	strcat((char *)pdata,(char*)FlashDC);
    strcat((char *)pdata,"-Pc=");  //注意功率范围，数值要够大，现测试
	sprintf(databuff,"%d",FT_rtcm.R_RTCM_CPower);
	strcat((char *)pdata,databuff);;
	strcat((char *)pdata,",");	
	strcat((char *)pdata,(char*)FlashDC);
	strcat((char *)pdata,"-Pt=");  //注意功率范围，数值要够大，现测试
	sprintf(databuff,"%d",(FT_rtcm.R_RTCM_APower+FT_rtcm.R_RTCM_BPower+FT_rtcm.R_RTCM_CPower));
	strcat((char *)pdata,databuff);
	//strcat((char *)pdata,(char*)U16IntToStr(NEW_DATA_FT.R_RTCM_Acurrent*NEW_DATA_FT.R_RTCM_Avoltage+NEW_DATA_FT.R_RTCM_Bcurrent*NEW_DATA_FT.R_RTCM_Bvoltage+NEW_DATA_FT.R_RTCM_Ccurrent*NEW_DATA_FT.R_RTCM_Cvoltage));
	strcat((char *)pdata,",");
	strcat((char *)pdata,(char*)FlashDC);
	strcat((char *)pdata,"-Ept=");  //注意功率范围，数值要够大，现测试
	sprintf(databuff,"%.4lf",FT_rtcm.R_RTCM_EM);
	strcat((char *)pdata,databuff);
	//strcat((char *)pdata,(char*)U16IntToStr(NEW_DATA_FT.R_RTCM_Acurrent*NEW_DATA_FT.R_RTCM_Avoltage+NEW_DATA_FT.R_RTCM_Bcurrent*NEW_DATA_FT.R_RTCM_Bvoltage+NEW_DATA_FT.R_RTCM_Ccurrent*NEW_DATA_FT.R_RTCM_Cvoltage));
	strcat((char *)pdata,",");
	strcat((char *)pdata,(char*)FlashDC);
	strcat((char *)pdata,"-Ua=");  //注意功率范围，数值要够大，现测试
	sprintf(databuff,"%d",FT_rtcm.R_RTCM_Avoltage);
	strcat((char *)pdata,databuff);
	strcat((char *)pdata,",");
	strcat((char *)pdata,(char*)FlashDC);
	strcat((char *)pdata,"-Ub=");  //注意功率范围，数值要够大，现测试
	sprintf(databuff,"%d",FT_rtcm.R_RTCM_Bvoltage);
	strcat((char *)pdata,databuff);
	strcat((char *)pdata,",");
	strcat((char *)pdata,(char*)FlashDC);	
	strcat((char *)pdata,"-Uc=");  //注意功率范围，数值要够大，现测试
	sprintf(databuff,"%d",FT_rtcm.R_RTCM_Cvoltage);
	strcat((char *)pdata,databuff);;
	PdataLen=strlen((char *)pdata);
	sprintf(LEnbuff,"%04d",PdataLen+1);
	strcat((char *)Pack,LEnbuff);
	strcat((char *)Pack,(char *)pdata);
	strcat((char *)Pack,HJ212_FRAME_CP_END);
	strcat((char *)pdata,HJ212_FRAME_CP_END);
	sprintf(CRCbuff,"%04X",CRC16_Checkout((char *)pdata,PdataLen+2));
	strcat((char *)Pack,CRCbuff);
	strcat((char *)Pack,HJ212_FRAME_BAOWEI);
	return 0;
}






