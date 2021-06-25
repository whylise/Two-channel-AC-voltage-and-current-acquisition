#ifndef _HJ212_H_
#define _HJ212_H_
#include "all.h"

//帧格式
#define HJ212_FRAME_BAOTOU				"##"
#define HJ212_FRAME_QN					"QN="
#define HJ212_FRAME_ST					"ST="
#define HJ212_FRAME_CN					"CN="
#define HJ212_FRAME_PW					"PW="
#define HJ212_FRAME_MN					"MN="
#define HJ212_FRAME_FLAG				"Flag="
#define HJ212_FRAME_PNUM				"PNUM="
#define HJ212_FRAME_PNO					"PNO="
#define HJ212_FRAME_CP_START			"CP=&&"
#define HJ212_FRAME_CP_END				"&&"
#define HJ212_FRAME_BAOWEI				"\r\n"
#define HJ212_BYTES_FRAME_BAOTOU		2
#define HJ212_BYTES_FRAME_LEN			4
#define HJ212_BYTES_FRAME_CRC			4
#define HJ212_BYTES_FRAME_BAOWEI		2
#define HJ212_BYTES_FRAME_QN			20
#define HJ212_BYTES_FRAME_ST			5
#define HJ212_BYTES_FRAME_CN			7
#define HJ212_BYTES_FRAME_PW			9
#define HJ212_BYTES_FRAME_MN			27
#define HJ212_BYTES_QN					17
#define HJ212_BYTES_ST					2
#define HJ212_BYTES_CN					4
#define HJ212_BYTES_PW					6
#define HJ212_BYTES_MN					24

//拆分包及应答标志
#define HJ212_FLAG_PTCL_VER				0x04
#define HJ212_FLAG_PACKET_DIV_EN		0x02
#define HJ212_FLAG_ACK_EN				0x01

//相关配置信息
#define HJ212_ST_SYS_COMM				"91"
#define HJ212_BYTES_TIME_CALI_REQ		150
#define HJ212_BYTES_DATA_RTD			230
#define HJ212_BYTES_DATA_MIN			460
#define HJ212_REPORT_REPLY_EN			RT_FALSE

//字段名
#define HJ212_FIELD_NAME_SYSTIME		"SystemTime="
#define HJ212_FIELD_NAME_QNRTN			"QnRtn="
#define HJ212_FIELD_NAME_EXERTN			"ExeRtn="
#define HJ212_FIELD_NAME_RTDINTERVAL	"RtdInterval="
#define HJ212_FIELD_NAME_MININTERVAL	"MinInterval="
#define HJ212_FIELD_NAME_RESTARTTIME	"RestartTime="
#define HJ212_FIELD_NAME_DATA_RTD		"-Rtd="
#define HJ212_FIELD_NAME_DATA_MIN		"-Min="
#define HJ212_FIELD_NAME_DATA_MAX		"-Max="
#define HJ212_FIELD_NAME_DATA_AVG		"-Avg="
#define HJ212_FIELD_NAME_DATA_COU		"-Cou="
#define HJ212_FIELD_NAME_DATA_FLAG		"-Flag="
#define HJ212_FIELD_NAME_BEGIN_TIME		"BeginTime="
#define HJ212_FIELD_NAME_END_TIME		"EndTime="
#define HJ212_FIELD_NAME_DATA_TIME		"DataTime="
#define HJ212_FIELD_NAME_NEW_PW			"NewPW="
#define HJ212_FIELD_NAME_OVER_TIME		"OverTime="
#define HJ212_FIELD_NAME_RECOUNT		"ReCount="
#define HJ212_FIELD_NAME_RUN_STA		"SB%d-RS=%c;"
#define HJ212_FIELD_NAME_RUN_TIME		"SB%d-RT=%.1f;"
//#define Ia                              "P10001-Ia="
//#define Ib                              "P10001-Ib="
//#define Ic                              "P10001-Ic="
//#define Pa                              "P10001-Pa="
//#define Pb                              "P10001-Pb="
//#define Pc                              "P10001-Pc="
//#define Pt                              "P10001-Pt="
//#define Ept                             "P10001-Ept="
//#define Ua                              "P10001-Ua="
//#define Ub                              "P10001-Ub="
//#define Uc                              "P10001-Uc="

//exertn
#define HJ212_EXERTN_OK					1
#define HJ212_EXERTN_ERR_NO_REASON		2
#define HJ212_EXERTN_ERR_CMD			3
#define HJ212_EXERTN_ERR_COMM			4
#define HJ212_EXERTN_ERR_BUSY			5
#define HJ212_EXERTN_ERR_SYSTEM			6
#define HJ212_EXERTN_ERR_NO_DATA		100

//qnrtn
#define HJ212_QNRTN_EXCUTE				1
#define HJ212_QNRTN_REJECTED			2
#define HJ212_QNRTN_ERR_PW				3
#define HJ212_QNRTN_ERR_MN				4
#define HJ212_QNRTN_ERR_ST				5
#define HJ212_QNRTN_ERR_FLAG			6
#define HJ212_QNRTN_ERR_QN				7
#define HJ212_QNRTN_ERR_CN				8
#define HJ212_QNRTN_ERR_CRC				9
#define HJ212_QNRTN_ERR_UNKNOWN			100

//数据标记
#define HJ212_DATA_FLAG_RUN				'N'
#define HJ212_DATA_FLAG_STOP			'F'
#define HJ212_DATA_FLAG_MAINTAIN		'M'
#define HJ212_DATA_FLAG_MANUAL			'S'
#define HJ212_DATA_FLAG_ERR				'D'
#define HJ212_DATA_FLAG_CALIBRATE		'C'
#define HJ212_DATA_FLAG_LIMIT			'T'
#define HJ212_DATA_FLAG_COMM			'B'

//命令编码
#define HJ212_CN_SET_OT_RECOUNT			1000
#define HJ212_CN_GET_SYS_TIME			1011
#define HJ212_CN_SET_SYS_TIME			1012
#define HJ212_CN_REQ_SYS_TIME			1013
#define HJ212_CN_GET_RTD_INTERVAL		1061
#define HJ212_CN_SET_RTD_INTERVAL		1062
#define HJ212_CN_GET_MIN_INTERVAL		1063
#define HJ212_CN_SET_MIN_INTERVAL		1064
#define HJ212_CN_SET_NEW_PW				1072
#define HJ212_CN_RTD_DATA				2011
#define HJ212_CN_STOP_RTD_DATA			2012
#define HJ212_CN_RS_DATA				2021
#define HJ212_CN_STOP_RS_DATA			2022
#define HJ212_CN_DAY_DATA				2031
#define HJ212_CN_DAY_RS_DATA			2041
#define HJ212_CN_MIN_DATA				2051
#define HJ212_CN_HOUR_DATA				2061
#define HJ212_CN_RESTART_TIME			2081
#define HJ212_CN_GET_QNRTN				9011
#define HJ212_CN_GET_EXERTN				9012
#define HJ212_CN_GET_INFORM_RTN			9013
#define HJ212_CN_GET_DATA_RTN			9014

unsigned int CRC16_Checkout(char *puchMsg,unsigned int usDataLe);
u8 _hj212_packdatatoUP(u8 *Pack,u8 *pdata,u16 ST,u16 CN,char *MN);





#endif


