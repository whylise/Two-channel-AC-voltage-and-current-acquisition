#ifndef __CRC16_H
#define __CRC16_H 	

#include "all.h" 
#include "crc16.h"

u16 get_crc16_value(u8 *ptr,u16 len);
u8 CalCurrentBaud1Char(void);
u16 strsearch(const u8 *p1,const u8 *p2);
u16 string_cint(u8 *p,u8 len);
void ClearArray(u8 *Pdata,u16 Len);
void CoypArraryAtoArraryB(const u8 *Pa, u8 *Pb,u16 num);
u16 CalculateLen(const u8 * Pdata);
u8 HexStringToHex(u8 *pdata,u8 *ptag,u16 len);
s32 HexStringCint(u8 *p,u8 len);
s32 HexStringCint(u8 *p,u8 len);
u16 CalculateLenFromEnd(const u8 *Pdata,u16 MaxLen);
u8 *U16IntToStr(u16 data);
u8* hextoasciihex(u8 h);
u8 *num_change(u8 *ph);
u8 HexConvernum(u8 n);
void CoypArraryAtoArraryB_LR(const u8 *Pa, u8 *Pb,u16 num,u16 LR);
u32 stingcnt_to_uint32(u8 *ptr);
int gsmDecode7bit(const unsigned char* pSrc, char* pDst, int nSrcLength);
u8 UnicodeAppendCommonBuf(const u8 *src);
u8 *S16IntToStr(s16 data);
s32 stingcnt_to_sint32(u8 *ptr);

#endif

