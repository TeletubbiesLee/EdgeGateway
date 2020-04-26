/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      client.h
  * @brief:		  客户端
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2019-03-19
  * @update:    [2019-03-19][Chen][newly increased]
  */
#ifndef _CLIENT_H_
#define _CLIENT_H_
/* INCLUDE --------------------------------------------------------------------------*/
#include <stdint.h>
#include "tcp_interface.h"
/* DEFINE ---------------------------------------------------------------------------*/
#define CLIENT_2404_RX_BUFSIZE          (256*14)
#define CLIENT_2404_TX_BUFSIZE          (512)

#define CLIENT_TCP_DISCONNECT           0x20
#define CLIENT_TCP_LINKUP               0x40
#define CLIENT_SENDING                  0x80

/* STRUCT ---------------------------------------------------------------------------*/
/* client网络设置 */
struct client_cfg
{
	uint8_t client_no;
	unsigned short port;
	char ip[32];
  uint8_t monitor_pdrv;
};
typedef struct client_cfg  *client_cfg_t;

struct client_api
{
	int (*monitor_callback)(int fd, uint8_t pdrv, char *buf, int len);
	uint8_t (*queue_write_block)(void *buf, uint8_t *pdata, uint16_t len);
	uint8_t (*queue_read)(uint8_t *ret, void *buf);
};

/* PUBLIC FUNCTION ------------------------------------------------------------------*/
uint16_t ethernet_client_get(uint8_t client_no, uint8_t *buf, int len);
uint16_t ethernet_client_put(uint8_t client_no, uint8_t *buf, int len);
uint8_t getClientLinkState(uint8_t client_no);
void setClientCloseLink(uint8_t client_no);
void client_task(struct tagTcpSocketCfg cfg, uint8_t pdrv);

#endif /* _CLIENT_H_ */
/* FILE END  ------------------------------------------------------------------------*/
