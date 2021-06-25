#ifndef _HTTP_API_H
#define _HTTP_API_H 	

#include "all.h"
#include "http_api.h"

u8 POST_HTTP_CDKEY_MOTHOD(void);
u8 POST_HTTP_SEFT_MOTHOD(void);
u8 POST_HTTP_BasicData_MOTHOD(void);
u8 GET_HTTP_Flags(void);
void Handle_mark_bit(void);
void POST_HTTP_Frist_MOTHOD(void);
void HTTP_INIT_MOTHOD(void);
u8 POST_HTTP_INSTALL_MOTHOD(void);
u8 POST_HTTP_Ver_MOTHOD(void);
u8 POST_HTTP_Alarming_Value_MOTHOD(void);
u8 GET_HTTP_Config_MOTHOD(void);
void anlysis_config(void);
u8 POST_HTTP_Clear_MOTHOD(u8 *FlagBIT);
void clear_FlagBIT(void);
u8 POST_HTTP_Num_MOTHOD(void);
void SAVE_URL_FLASH(u8 *DATABUF);
void SAVE_SCURL_FLASH(u8 *DATABUF);
u8 POST_HTTP_AlarmData_MOTHOD(void);
void CDHostUrlMode(void);
void clear_Packbuff(void);
void clear_Pdatabuff(void);

#endif
