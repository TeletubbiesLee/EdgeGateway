/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      cs104SlaveDisk.h
  * @brief:		104子站接口 头文件
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2018-11-07
  * @update:    [2018-11-07][Chen][newly increased]
  */
  
#ifndef	_CS104_SLAVE_DISK_H_
#define _CS104_SLAVE_DISK_H_


/* INCLUDE FILES -------------------------------------------------------------*/
#include "stdint.h"
#include "../port/iec_interface.h"
#include "../common/common.h"
//#include "F:\000-eclipse\iec60870\src\point\dataSwitch_point_table.h"

/* DEFINE --------------------------------------------------------------------*/

/* Enum ----------------------------------------------------------------------*/

/* PUBLIC VARIABLES ----------------------------------------------------------*/
struct CS104_SLAVE_DISK
{
  uint16_t SoeOut;		/*变化SOE出口*/
  struct Telesignal_COS TelesignalCosRep[COS_MAX_NUM]; /*变化遥信存储数组*/
  struct Telesignal_COS *TelesignalCosIn;		  /*变化遥信入口*/
  struct Telesignal_COS *TelesignalCosOut;		/*变化遥信出口*/
  uint8_t telesignalBackup[YX_MAX_NUM];	      /*备份YX 定时比较变化*/

  struct Telemetry_NVA TelemetryNVARep[NVA_MAX_NUM]; /*变化NVA存储数组*/
  struct Telemetry_NVA *TelemetryNVAIn;		  /*变化NVA入口*/
  struct Telemetry_NVA *TelemetryNVAOut;		/*变化NVA出口*/
  float telemetryBackup[YC_MAX_NUM];        /*备份YC 定时比较变化*/

};
/* PUBLIC FUNCTION -----------------------------------------------------------*/
void CS104_SLAVE_HandleCtrlProcess(uint8_t pdrv, uint8_t *pbuf);
void CS104_SLAVE_CallAllDataProcess(uint8_t pdrv, uint8_t *pbuf);
uint8_t getDzPreTimeOut104(uint8_t pdrv);
uint8_t CS104_Slave_DZ_RevokeOperation(uint8_t pdrv);
void CS104_SLAVE_SYNProcess(uint8_t pdrv, uint8_t *pbuf);
uint64_t CS104_SLAVE_getAlarmOfMs(uint8_t pdrv);
void CS104_SLAVE_ResetProcess(uint8_t pdrv, uint8_t *pbuf);
void CS104_Slave_exit(uint8_t pdrv);
void CS104_SLAVE_FixedParaProcess(uint8_t pdrv, uint8_t *pbuf);
void CS104_SLAVE_FileHandleProcess(uint8_t pdrv, uint8_t *pbuf);
void CS104_SLAVE_SoftwareUpdate(uint8_t pdrv, uint8_t *pbuf);
uint8_t CS104_SLAVE_StopLink(uint8_t pdrv);
uint16_t CS104_Slave_WriteData(uint8_t pdrv, uint8_t *pbuf, uint16_t count);
uint16_t CS104_SLAVE_ReadYxData(uint8_t pdrv, uint16_t addr, uint16_t num, uint8_t *pbuf);
uint8_t CS104_SLAVE_H_COS(uint8_t pdrv);
void CS104_SLAVE_R_COS(uint8_t pdrv, uint8_t *pbuf);
uint8_t CS104_SLAVE_H_SOE(uint8_t pdrv);
uint8_t CS104_SLAVE_H_NVA(uint8_t pdrv);
void CS104_SLAVE_R_NVA(uint8_t pdrv, uint8_t *pbuf);
uint8_t CS104_SLAVE_H_FEvent(uint8_t pdrv);
void CS104_SLAVE_R_FEvent(uint8_t pdrv, uint8_t *pbuf);
void CS104_SLAVE_IdleProcess(uint8_t pdrv, uint8_t *pbuf);
uint8_t CS104_Slave_CheckLink(uint8_t pdrv);
uint16_t CS104_Slave_ReadData(uint8_t pdrv, uint8_t *pbuf, uint16_t count);
uint8_t CS104_SLAVE_H_Encrypt(uint8_t pdrv);
uint16_t CS104_SLAVE_ReadYcData(uint8_t pdrv, uint16_t addr, uint16_t num, uint8_t *pbuf);
void CS104_SLAVE_R_SOE(uint8_t pdrv, uint8_t *pbuf);
//uint8_t CS104_Slave_C_REPLY(uint8_t pdrv,tagControlCfg_t reInfo);			TXL注释
uint8_t CS104_Slave_pause(void);
uint8_t CS104_Slave_resume(uint8_t num);
int CS104_Slave_create(uint8_t pdrv, int server_no, IecParam sParam);
void *CS104_Slave_Startup(void *arg);
#endif /* END _CS104_SLAVE_DISK_H_*/
    

/* END OF FILE ---------------------------------------------------------------*/
