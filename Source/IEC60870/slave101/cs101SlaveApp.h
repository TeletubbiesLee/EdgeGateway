/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      cs101SlaveApp.h
  * @brief:		101app交互处理
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2018-11-09
  * @update:    [2018-11-09][Chen][newly increased]
  */
#ifndef	_CS101_SLAVE_APP_H_
#define _CS101_SLAVE_APP_H_
/* INCLUDE FILES -------------------------------------------------------------*/
#include "cs101SlaveConfig.h"
#include <stdint.h>

/* PRIVATE VARIABLES ---------------------------------------------------------*/

/* PUBLIC VARIABLE -----------------------------------------------------------*/
#pragma pack(push,1)
typedef struct
{
	uint8_t Port;            //端口
	uint8_t DevId;		 	 //设备号
	uint8_t ModuleSN;		 	 //模块ID号
	uint8_t Encrypt;	     // 加密
	uint8_t BalanMode;	     // 1平衡，0非平衡
	uint8_t IEC_DIR;         // 终端0x80,主站0x00

	uint16_t SourceAddr;	 // 源地址，即本机地址

	uint8_t LinkAddrSize;   //链路地址长度
	uint8_t ASDUCotSize;    //传送原因长度

	uint16_t ASDUAddr;       //ASDU地址
	uint8_t ASDUAddrSize;    //ASDU地址长度
	uint8_t FixFrmLength;	 //固定帧长度

	uint64_t ClockTimers;		//调用超出次数，程序执行一次，控制运行周期
	uint64_t TimeOutValue;		//超时重发间隔
	uint64_t AskStaOutValue;    //链路请求间隔
  	uint64_t TimeOutLink;		//链路超时
	uint64_t TimeOutReset;		//复位进程超时时间
	uint8_t  resetFlag;			//复位进程标志
	uint8_t  updateFlag;		//加密升级结束标志

	uint16_t YX_AllNum;       //遥信总数
	uint16_t YX_FirstAddr;    //遥信首地址
	uint16_t YX_FrameNum;	  //遥信每组数量
	uint16_t YC_AllNum;		  //遥测总数
	uint16_t YC_FirstAddr;    //遥信首地址
	uint16_t YC_FrameNum;	  //遥测每组数量
}sCS101_Slave_Pad;
#pragma pack(pop)

extern sCS101_Slave_Pad CS101_Slave_Pad[CS101_SLAVE_DISK_VOLUMES];
/* PRIVATE FUNCTION PROTOTYPES -----------------------------------------------*/

/* PUBLIC FUNCTION PROTOTYPES ------------------------------------------------*/
uint8_t CS101_Slave_Reply(uint8_t pdrv, uint8_t *pbuf);
uint8_t CS101_Slave_Appinit(void);
uint8_t CS101_Slave_Clock(uint8_t pdrv);



#endif /*_CS101_SLAVE_APP_H_*/
/* END OF FILE ----------------------------------------------------------------*/
