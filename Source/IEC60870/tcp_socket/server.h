/**
  * @copyright  Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      server.h
  * @brief:     the device driver of server
  * @version:   V1.0.0
  * @author:    Sunxr
  * @date:      2018-09-04
  * @update:    [2018-09-04][Sunxr][newly increased]
  * @update:    [2019-03-03][Chen][modify code format]
  */
#ifndef _SERVER_H_
#define _SERVER_H_
/* INCLUDE --------------------------------------------------------------------------*/
#include <stdint.h>
#include "tcp_interface.h"
/* DEFINE ---------------------------------------------------------------------------*/
#define ETHERNET_DEBUG

#ifdef  ETHERNET_DEBUG
#define ETHERNET_PRINTF(...)           (printf("%s-%s-%d: ", __FILE__, __FUNCTION__, __LINE__), printf)
#else
#define ETHERNET_PRINTF(...)
#endif  /* ETHERNET_DEBUG */

#define EN_MONITOR                      // 允许监听

#define SERVER_2404_RX_BUFSIZE          (256*14)
#define SERVER_2404_TX_BUFSIZE          (512)

#define ETHERNET_TCP_SERVER_DISCONNECT  0x20
#define ETHERNET_TCP_SERVER_LINKUP      0x40
#define ETHERNET_SENDING                0x80

/* STRUCT ---------------------------------------------------------------------------*/
/* server网络设置 */
struct ethernet_cfg
{
	uint8_t server_no;
	unsigned short port;
	char ip[32];
  uint8_t monitor_pdrv;
};

struct external_api
{
	int (*monitor_callback)(int fd, uint8_t pdrv, char *buf, int len);
	uint8_t (*queue_write_block)(void *buf, uint8_t *pdata, uint16_t len);
	uint8_t (*queue_read)(uint8_t *ret, void *buf);
};

/* PUBLIC FUNCTION ------------------------------------------------------------------*/
void server_task(struct tagTcpSocketCfg cfg, uint8_t pdrv);
uint8_t getServerLinkState(uint8_t server_no);
void setServerCloseLink(uint8_t server_no);
uint16_t ethernet_server_put(uint8_t server_no, uint8_t *buf, int len);
uint16_t ethernet_server_get(uint8_t server_no, uint8_t *buf, int len);
#endif /* _SERVER_H_ */

/* FILE END  ------------------------------------------------------------------------*/
