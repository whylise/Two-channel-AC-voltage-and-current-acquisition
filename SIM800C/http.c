#include "http.h"

extern u8 GSMUART_buff[MAX_GSMUART];

//**************************************************************
//描述:http初始化函数
//参数:bearer init
//返回:	0:成功   others:失败 
u8 http_init(void)
{
		UART_string_newline("AT+SAPBR=3,1,\"Contype\",\"GPRS\"",1);//配置承载场景1
		delay_ms(500); 
		UART_string_newline("AT+SAPBR=3,1,\"APN\",\"CMNET\"",1);//配置承载场景1
		delay_ms(500);
		UART_string_newline("AT+SAPBR=1,1",1);//激活一个GPRS上下文
		delay_ms(500); 
		clear_USART1buff();
		UART_string_newline("AT+SAPBR=2,1",1);//查询GPRS上下文
		delay_ms(500);  
		if(!strsearch(GSMUART_buff,"0.0.0.0"))return 0;//判断是否激活成功
		return 1;
}

//************************
//描述: 写入apn  bearer 
//返回:	0:成功   
u8 write_bearer_apn(void)
{
		if(WriteApnName22("CMNET"))//写入APN名
				WriteApnName22("CMNET");//第一次写入失败,重写
		return 0;
}
