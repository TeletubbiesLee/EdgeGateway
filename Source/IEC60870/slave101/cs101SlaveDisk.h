/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      cs101SlaveDisk.h
  * @brief:		101子站接口 头文件
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2018-11-07
  * @update:    [2018-11-07][Chen][newly increased]
  */
#ifndef	_CS101_SLAVE_DISK_H_
#define _CS101_SLAVE_DISK_H_

/* INCLUDE FILES -------------------------------------------------------------*/
#include <stdint.h>
#include "../port/iec_interface.h"
#include "../common/common.h"
//#include "dataSwitch_point_table.h"						TXL注释
/* PRIVATE VARIABLES ---------------------------------------------------------*/

/* PUBLIC VARIABLE -----------------------------------------------------------*/
struct CS101_SLAVE_DISK
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

/* PRIVATE FUNCTION PROTOTYPES -----------------------------------------------*/

/* PUBLIC FUNCTION PROTOTYPES ------------------------------------------------*/
uint16_t CS101_Slave_Read_drv(uint8_t port, uint8_t *pbuf, uint16_t count);
uint16_t CS101_Slave_Write_drv(uint8_t port, uint8_t *pbuf, uint16_t count);
uint16_t cs101SlaveReadData(uint8_t pdrv, uint8_t *pbuf, uint16_t count);
uint16_t CS101_Slave_WriteData(uint8_t pdrv, uint8_t *pbuf, uint16_t count);
void CS101_Slave_C_IC(uint8_t pdrv, uint8_t *pbuf);
void CS101_Slave_C_SC(uint8_t pdrv, uint8_t *pbuf);
void CS101_Slave_C_CS(uint8_t pdrv, uint8_t *pbuf);
uint64_t CS101_Slave_getAlarmOfMs(uint8_t pdrv);
void CS101_Slave_C_RP(uint8_t pdrv, uint8_t *pbuf);
void CS101_Slave_exit(uint8_t pdrv);
uint8_t getDzPreTimeOut101(uint8_t pdrv);
uint8_t CS101_Slave_DZ_RevokeOperation(uint8_t pdrv);
void CS101_Slave_C_SR(uint8_t pdrv, uint8_t *pbuf);
uint8_t CS101_Slave_C_YX(uint8_t pdrv);
uint8_t CS101_Slave_H_COS(uint8_t pdrv);
void CS101_Slave_R_COS(uint8_t pdrv, uint8_t *pbuf);
uint8_t CS101_Slave_H_SOE(uint8_t pdrv);
void CS101_Slave_R_SOE(uint8_t pdrv, uint8_t *pbuf);
uint8_t CS101_Slave_H_NVA(uint8_t pdrv);
void CS101_Slave_R_NVA(uint8_t pdrv, uint8_t *pbuf);
uint8_t CS101_Slave_H_FEvent(uint8_t pdrv);
void CS101_Slave_R_FEvent(uint8_t pdrv, uint8_t *pbuf);
uint16_t CS101_Slave_R_YXDATA(uint8_t pdrv,uint16_t addr,uint16_t num, uint8_t *pbuf);
uint16_t CS101_Slave_R_YCDATA(uint8_t pdrv,uint16_t addr,uint16_t num, uint8_t *pbuf);
void CS101_Slave_F_SR(uint8_t pdrv, uint8_t *pbuf);
void CS101_Slave_F_FR(uint8_t pdrv, uint8_t *pbuf);
void CS101_Slave_R_IDLE(uint8_t pdrv, uint8_t *pbuf);
uint8_t CS101_Slave_H_Encrypt(uint8_t pdrv);
//uint8_t CS101_Slave_C_REPLY(uint8_t pdrv,tagControlCfg_t reInfo);					TXL注释
uint8_t CS101_Slave_pause(void);
uint8_t CS101_Slave_resume(uint8_t num);
int CS101_Slave_create(uint8_t pdrv, int portFd, IecParam sParam);
void *CS101_Slave_Startup(void *arg);

#endif	/*_CS101_SLAVE_DISK_H_*/
/* END OF FILE ----------------------------------------------------------------*/
