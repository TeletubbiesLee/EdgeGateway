/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      tcp_interface.c
  * @brief:		  tcp通用对外接口
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2019-06-28
  * @update:    [2019-06-28][Chen][new]
  */
#define LOG_TAG    "tcp_interface"
/* INCLUDE FILES -------------------------------------------------------------*/
#include "tcp_interface.h"
#include "server.h"
#include "client.h"
//#include "elog.h"
#include<stdio.h>
#include<string.h>
/* PRIVATE VARIABLES ---------------------------------------------------------*/
static uint8_t serverNum = 0;
static uint8_t clientNum = 0;
/* PUBLIC FUNCTION PROTOTYPES ------------------------------------------------*/
/**
  * @brief : tcp网络配置启动.
  * @param : tagTcpSocketCfg_t
  * @return: true or false
  * @updata: [2019-06-28][Chen][new]
  */
int tcp_start(uint8_t tcp_type, char *ip)
{
  struct tagTcpSocketCfg cfg;
  cfg.stype = tcp_type;
  strcpy(cfg.ip,ip);
  cfg.port = IEC_PORT;

  if(cfg.stype == SERVER)
  {
//    log_w("serverNum:%d",serverNum);
    cfg.tcp_no = serverNum;
    server_task(cfg,serverNum);
    serverNum++;
    return cfg.tcp_no;
  }

  if(cfg.stype == CLIENT)
  {
//    log_w("clientNum:%d",clientNum);
    cfg.tcp_no = clientNum;
    client_task(cfg,clientNum);
    clientNum++;
    return cfg.tcp_no;
  }
  
  return (-1);
}
/* END OF FILE ----------------------------------------------------------------*/
