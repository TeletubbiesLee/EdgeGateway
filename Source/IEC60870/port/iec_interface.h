/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      iec_interface.h
  * @brief:		通用对外接口头文件
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2018-10-24
  * @update:    [2018-10-24][Chen][newly increased]
  */
#ifndef	_IEC_INTERFACE_H_
#define _IEC_INTERFACE_H_
/* INCLUDE FILES -------------------------------------------------------------*/
#include <stdint.h>
#include "../common/common.h"
#include "../point/dataSwitch_format_common.h"
#include "../point/dataSwitch_point_table.h"
/* STRUCT --------------------------------------------------------------------*/
typedef enum
{
	SLAVE101 = 1,
	SLAVE104,
	MASTER101,
	MASTER104,
    MASTERMODBUS,
	TYPE_VOLUMES
}STYPE;	/*协议类型*/

/* PUBLIC VARIABLE -----------------------------------------------------------*/
extern uint8_t g_CommunicatFlag[COM_MAX]; // 通讯互锁标志
// extern uint8_t g_CommunicatFileOper;
/* iec配置信息 结构体 */
struct sIecParam_t
{
	uint8_t  stype; 		/*协议类型 0代表s101 1代表s104*/

	uint8_t  portNo; 		/*串口号 0—2*/
	uint8_t  baudRate;		/*波特率 0: 2400 4800 9600 38400 115200*/
	uint8_t  parity;	    /*奇偶校验 0无 1奇 2偶*/

	char     *ip;           /*IP地址(点分十进制)*/
	uint8_t  LocalNetPort;	/*本机网口 1 2*/
	uint8_t	 ModuleSN;	    /*模块ID号*/
	uint8_t  encrypt;		/*是否加密 0不加密 1加密*/
	uint8_t  balanMode;		/*平衡or非平衡 1平衡*/
	uint16_t sourceAddr; 	/*链路地址*/
	uint8_t  linkAddrSize;	/*链路地址长度 1 2*/
	uint16_t ASDUCotSize;	/*传送原因长度*/
	uint16_t ASDUAddr;		/*ASDU地址*/
	uint8_t  ASDUAddrSize;	/*ASDU地址长度 1 2*/
	uint8_t  InfoAddrSize;	/*信息体地址长度 2 3*/

	void *allInfoDisk; //全局配置结构
};
typedef struct sIecParam_t *IecParam;

struct sMasterParam
{
    uint16_t module_id;                         /* 模块ID */
    struct tagSwitchQuantityIn communication_state;  /* 通讯状态 */
    uint16_t YX_len;                            /* 遥信长度 */
    tagSwitchQuantityIn_t p_YX_data;            /* 遥信数据 */
    uint16_t YC_len;                            /* 遥测长度 */
    tagAnalogQuantityIn_t p_YC_data;            /* 遥测数据 */
    uint16_t YK_len;                            /* 遥控长度 */
    tagControlOutputIn_t p_YK_data;             /* 遥控数据 */
	uint16_t sourceAddr; 	                    /* 链路地址 */
	uint8_t  portNo; 		                    /* 串口号 */
    uint8_t  netEn; 		                    /* 网口使用 */
//	char     ip[32];           					/*IP地址(点分十进制)*/
    char     *ip;
    uint8_t  yk_type;		                    /* 遥控单双点 */
    int(*master_YK_fun)(tagControlCfg_t);		/* 遥控函数 */
    int(*master_CLOCK_SYNC_fun)(tagControlCfg_t);/* 对时通知函数 */
};
typedef struct sMasterParam *sMasterParam_t; 
/* PUBLIC FUNCTION PROTOTYPES ------------------------------------------------*/
int iec_init(IecParam sParam);
uint8_t MasterModleParam(sMasterParam_t sParam);
uint8_t thread_pause(uint8_t stype);
uint8_t thread_resume(uint8_t stype);
//uint8_t iec_receiveData(tagControlCfg_t reInfo);
int iec_start(uint8_t stype);

#endif	/*_IEC_INTERFACE_H_*/
/* END OF FILE ---------------------------------------------------------------*/
