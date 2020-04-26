/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      iec60870_common.h
  * @brief:		iec60870通用头文件
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2018-12-14
  * @update:    [2018-12-14][Chen][newly increased]
  */
#ifndef	_COMMON_H_
#define _COMMON_H_
/* INCLUDE FILES -------------------------------------------------------------*/
#include <stdint.h>

/* DEFINE --------------------------------------------------------------------*/
#define NULL ((void *)0)

#ifndef  FALSE
#define  FALSE     0
#define  TRUE      !FALSE
#endif

#define SDRAM_ADDR_UPDATE 0xc0000000	//升级文件保存起始地址

#define IEC60870_YX_STARTADDR 0x0001		//遥信
#define IEC60870_YC_STARTADDR 0x4001		//遥测
#define IEC60870_YK_STARTADDR 0x6001		//遥控

#define _Debug   1

#define MAKEWORD(low, high)           ((uint16_t)((uint8_t)(low) | (((uint16_t)((uint8_t)(high))) << 8)))
#define LOBYTE(w)                     ((uint8_t)(w))
#define HIBYTE(w)                     ((uint8_t)((uint16_t)(w) >> 8))
#define LOWORD(l)                     ((uint16_t)(l))
#define HIWORD(l)                     ((uint16_t)((uint32_t)(l) >> 16))

#define SIZE64						  64
#define SIZE256						  256
#define COS_MAX_NUM                   64
#define SOE_MAX_NUM                   256
#define SOE_NEW_MAX_NUM               64
#define CO_MAX_NUM                    64
#define FEVENT_MAX_NUM                16
#define NVA_MAX_NUM                   64
#define ULOG_MAX_NUM                  32
#define YX_MAX_NUM                    1024
#define YC_MAX_NUM                    512
/* LOCK ----------------------------------------------------------------------*/
/* 通讯互锁定时 */
#define COMMUNICATLOCKSTATUS      0xFF //无设备使用
#define COMMUNICATLOCKRESET       0xFE //复位标志

#define COMMUNICATLOCKREADDIR     0x01 //读目录标志
#define COMMUNICATLOCKREADFILE    0x02 //读文件标志
#define COMMUNICATLOCKWRITEFILE   0x04 //写文件标志
enum
{
    COM_YK=0,
    COM_PARA,
    COM_FILE,
    COM_MAX
};

/* STRUCT --------------------------------------------------------------------*/
#pragma pack(push,1)

typedef enum
{
    SLAVE101_ID0 = 0,
    SLAVE101_ID1,
	SLAVE101_ID2,
    SLAVE101_ID3,
    SLAVE104_ID0,
    SLAVE104_ID1,
    DEV_MAX_NUM
} DEV_ID;	/* 设备枚举 */

/* CP56Time2a_t时标 */
struct CP56Time2a_t
{
    uint8_t msecondL;
    uint8_t msecondH;
    uint8_t minute;
    uint8_t hour;
    uint8_t dayofWeek;	// WEEK(D7-D5)day(D4-D0)
    uint8_t month;
    uint8_t year;
};
/* COS数据结构体 */
struct COS_Str
{
    uint16_t addr;
    uint8_t  value;
};
/* iec COS数据结构体 */
struct Telesignal_COS
{
    uint16_t addr;
    uint8_t  value;
    uint8_t uptype;/*上送类型*/
};
/* SOE数据结构体 */
struct SOE_Str
{
    uint16_t addr;
    uint8_t  value;
    struct CP56Time2a_t time;
};
/* iec SOE数据结构体 */
struct Telesignal_SOE
{
    uint16_t addr;
    uint8_t  value;
    struct CP56Time2a_t time;
    uint8_t  uptype;/*上送类型*/
};
/* 变化遥测数据结构体 */
struct NVA_Str
{
    uint16_t addr;
    float value;
};
/* iec NVA数据结构体 */
struct Telemetry_NVA
{
    uint16_t addr;
    float value;
    uint8_t  uptype;/*上送类型*/
    float MultiplRate;
};
/*定值参数结构体*/
struct tagValueDZ
{
	uint16_t addr;
	uint8_t  tag;
	uint8_t  size;
	uint8_t  value[64];
};
/*遥控参数结构体*/
struct tagCtlRomteCfg
{
    uint16_t cot;  //传送原因(原始数据)
    uint16_t addr; //地址(原始数据)
    uint8_t  oper; //操作(原始数据)
    uint8_t value; //动作 分，合(双点)
};
typedef struct tagCtlRomteCfg *tagCtlRomteCfg_t;

#pragma pack(pop)
/* PUBLIC FUNCTION PROTOTYPES ------------------------------------------------*/

#endif	/*_COMMON_H_*/
/* END OF FILE ---------------------------------------------------------------*/
