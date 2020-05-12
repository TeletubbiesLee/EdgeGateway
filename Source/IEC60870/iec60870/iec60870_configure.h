/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      iec60870_configure.h
  * @brief:		通用对外配置头文件
  * @version:   V1.0.0
  * @author:    txl
  * @date:      2020-05-11
  * @update:    [2020-05-11][txl][newly increased]
  */

#ifndef	_IEC60870_CONDIFURE_H_
#define _IEC60870_CONDIFURE_H_

#include "stdint.h"

//从机最多个数
#define SLAVE_MAX_NUM  16

//101配置结构体
typedef struct TagConfigure101{
	uint8_t  num;								//从机个数

	uint8_t  portNo; 							/*串口号 0—2*/
	uint8_t  baudRate;							/*波特率 0: 2400 4800 9600 38400 115200*/
	uint8_t  parity;	    					/*奇偶校验 0无 1奇 2偶*/

	uint8_t  balanMode;							/*平衡or非平衡 1平衡*/
	uint16_t sourceAddr; 						/*链路地址*/
	uint8_t  linkAddrSize;						/*链路地址长度 1 2*/
	uint16_t ASDUCotSize;						/*传送原因长度*/
	uint16_t ASDUAddr;							/*ASDU地址*/
	uint8_t  ASDUAddrSize;						/*ASDU地址长度 1 2*/
	uint8_t  InfoAddrSize;						/*信息体地址长度 2 3*/

	uint16_t sModuleId[SLAVE_MAX_NUM];    		/* 模块ID */
	uint8_t  sMstate;							/* 通讯状态 */
	uint16_t sMsourceAddr[SLAVE_MAX_NUM]; 		/* 链路地址 */
	uint8_t  sMportNo; 							/* 串口号 */
}Configure101;

//104配置结构体
typedef struct TagConfigure104{
	uint8_t  num;								//从机个数

	char     *ip;           					/*IP地址(点分十进制)*/
	uint8_t  balanMode;							/*平衡or非平衡 1平衡*/
	uint16_t sourceAddr; 						/*链路地址*/
	uint8_t  linkAddrSize;						/*链路地址长度 1 2*/
	uint16_t ASDUCotSize;						/*传送原因长度*/
	uint16_t ASDUAddr;							/*ASDU地址*/
	uint8_t  ASDUAddrSize;						/*ASDU地址长度 1 2*/
	uint8_t  InfoAddrSize;						/*信息体地址长度 2 3*/

	uint16_t sModuleId[SLAVE_MAX_NUM];    		/* 模块ID */
	uint8_t  sMstate;							/* 通讯状态 */
	uint16_t sMsourceAddr[SLAVE_MAX_NUM]; 		/* 链路地址 */
	uint8_t  sMnetEn; 							/* 网口使用 */
	char     *sMip[SLAVE_MAX_NUM];				//从机IP
}Configure104;


#endif
