#ifndef __SMS_H
#define __SMS_H 	

#include "all.h" 
#include "sms.h"

#define COMMONBUFMAX 1536//1536个空间

#define SMSUnitLen 268//67个中文,这个短信中最大允许发送的中文数 

u8 readSMS(void);
u8 DetelIndexSms(u8 *index);
u8 PhoneNumRecover(u8 *Num);
u8 GetSimBalanceForBuf(u8 *pdata);
void ClearCommonBuf(void);
u8 check_sms_unicode(u8 *pdata,u16 Maxlen);
u8 SendSmsByCommonbuf(u8 *TargetNum);
void SendGetBalanceSms(void);
void AsciiToUnicodeAppendCommonBuf(u8 *src);
u8  AScSwitchUni(unsigned char *p,unsigned char *p2);
u8 check_includs_num(u8 *cur_num);
u8 CheckSmsCmd(u8 *pdata,u8 *ReceNum);
void PhoneNumlist(u8 *DATABUF,u8 flashmode);
void SMSCMD(u8 admin_level,u8 *SEND_NUMBER_SMS);
u8 pdu_send_start(u8 *PhNum);
void SMS_To_ALL_Of_ZJ(void);
void SMS_To_ALL_Of_FW(void);
void SMS_To_ALL_Of_Alarming(void);
void SMS_Alarming(void);
void SMS_To_ZT(void);
void SMS_To_ALL_Of_BrokenAndShort(void);
void SMS_Broken_Short(void);
void SendGetNumSms(void);
u8 GetSimNumForBuf(u8 *pdata);

#endif
