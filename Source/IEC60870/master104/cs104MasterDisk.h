/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      cs104MasterDisk.h
  * @brief:		  104主站接口 头文件
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2019-03-18
  * @update:    [2019-03-18][Chen][newly increased]
  */
#ifndef	_CS104_MASTER_DISK_H_
#define _CS104_MASTER_DISK_H_
/* INCLUDE FILES -------------------------------------------------------------*/
#include "stdint.h"
#include "../port/iec_interface.h"
#include "../point/dataSwitch_point_table.h"
/* DEFINE --------------------------------------------------------------------*/

/* Enum ----------------------------------------------------------------------*/

/* PUBLIC VARIABLES ----------------------------------------------------------*/

/* PUBLIC FUNCTION -----------------------------------------------------------*/
uint16_t CS104_Master_ReadData(uint8_t pdrv, uint8_t *pbuf, uint16_t count);
uint16_t CS104_Master_WriteData(uint8_t pdrv, uint8_t *pbuf, uint16_t count);
uint8_t CS104_Master_CheckLink(uint8_t pdrv);
uint8_t CS104_Master_StopLink(uint8_t pdrv);
uint8_t CS104_Master_ReadRemoteCtlProcess(uint8_t pdrv, uint8_t *pbuf);
uint8_t CS104_Master_getRemoteCtlInfo(uint8_t pdrv, uint8_t *pbuf);
uint8_t CS104_Master_SYNProcess(uint8_t pdrv, ds_privateTime_t time);
uint64_t CS104_Master_getAlarmOfMs(uint8_t pdrv);
void CS104_Master_ResetProcess(uint8_t pdrv, uint8_t *pbuf);
uint16_t CS104_Master_ReadAllDataProcess(uint8_t pdrv, uint8_t *pbuf);
uint16_t CS104_Master_ReadYxProcess(uint8_t pdrv, uint8_t *pbuf);
uint16_t CS104_Master_ReadYcProcess(uint8_t pdrv, uint8_t *pbuf);
uint16_t CS104_Master_ReadSoeProcess(uint8_t pdrv, uint8_t *pbuf);
void CS104_Master_SoftwareUpdate(uint8_t pdrv, uint8_t *pbuf);
void CS104_Master_FileHandleProcess(uint8_t pdrv, uint8_t *pbuf);
void CS104_Master_IdleProcess(uint8_t pdrv, uint8_t *pbuf);
uint8_t CS104_Master_writeCommunicationSoe(uint8_t pdrv, uint8_t Connect);
int CS104_Master_C_REPLY(tagControlCfg_t reInfo);
int CS104_Master_create(uint8_t pdrv, int server_no, IecParam sParam);
uint8_t CS104_Master_AppConfigInit(uint8_t pdrv, int client_no, sMasterParam_t sParam);
void *CS104_Master_Startup(void *arg);
int CS104_Master_Pthread(uint8_t num);
#endif /* END _CS104_MASTER_DISK_H_*/
/* END OF FILE ---------------------------------------------------------------*/
