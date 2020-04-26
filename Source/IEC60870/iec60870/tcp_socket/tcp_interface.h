/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      tcp_interface.h
  * @brief:		tcp通用对外接口
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2019-06-28
  * @update:    [2019-06-28][Chen][new]
  */
#ifndef	_TCP_INTERFACE_H_
#define _TCP_INTERFACE_H_
/* INCLUDE FILES -------------------------------------------------------------*/
#include <stdint.h>
/* DEFINE ---------------------------------------------------------------------------*/
#define SERVER_NUM      2   /*最大server数*/
#define CLIENT_NUM      16   /*最大client数*/
#define IEC_PORT        2404
/* STRUCT --------------------------------------------------------------------*/
typedef enum
{
    SERVER = 1,
    CLIENT,
    TCP_VOLUMES
}TCPTYPE;

/* tcp网络设置 */
struct tagTcpSocketCfg
{
    uint8_t stype;          /* 类型 1服务器 2客户端 */
	char    ip[32];            /* IP地址 */
    unsigned short port;    /* 端口 */
    uint8_t tcp_no;         /* 网口号 */
};
typedef struct tagTcpSocketCfg *tagTcpSocketCfg_t;
/* PUBLIC FUNCTION PROTOTYPES ------------------------------------------------*/
int tcp_start(uint8_t tcp_type, char *ip);

#endif	/*_TCP_INTERFACE_H_*/
/* END OF FILE ---------------------------------------------------------------*/
