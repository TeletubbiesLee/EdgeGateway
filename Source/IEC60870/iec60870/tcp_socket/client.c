/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      client.c
  * @brief:		  客户端
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2019-03-19
  * @update:    [2019-03-19][Chen][newly increased]
  */
#define LOG_TAG "client"
/* INCLUDE FILES -------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "client.h"
#include "../queue/queue.h"
//#include "elog.h"
#include "../mysem/mysem.h"
/* PRIVATE VARIABLES ---------------------------------------------------------*/
static struct client_cfg sClientCfg[CLIENT_NUM];
static uint8_t s_ethernet_client_flag[CLIENT_NUM];
static uint16_t s_ethernet_client_len[CLIENT_NUM];
static uint8_t *s_ethernet_client_rx_buf[CLIENT_NUM];
static uint8_t *s_ethernet_client_tx_buf[CLIENT_NUM];
static ssize_t recv_size[CLIENT_NUM];
static uint8_t socket_rxbuffer[CLIENT_NUM][1024];
static int client_sock[CLIENT_NUM] = {-1};
static uint8_t running[CLIENT_NUM] = {0};
#ifdef USING_MY_QUEUE
static DataQueue s_ethernet_client_rxcb[CLIENT_NUM];
#endif /* USING_MY_QUEUE */

static struct client_api ex_api =
{
#ifdef USING_MONITOR
  .monitor_callback = monitor_getc,
#else
  .monitor_callback = NULL,
#endif /* USING_MONITOR */

#ifdef USING_MY_QUEUE
  .queue_read = QueueRead,
  .queue_write_block = QueueWriteBlock,
#else
  .queue_read = NULL,
  .queue_write_block = NULL,
#endif /* USING_MY_QUEUE */
};

/* PRIVATE FUNCTION -----------------------------------------------------------------*/
/**
  * @brief : 客户端线程启动.
  * @param : [void]
  * @return: void
  * @updata: [2019-03-19][Chen][newly increased]
  */
void *thread_client(void *arg)
{
  int max_sock;
  int ret, err;
  struct timeval tv;
  fd_set fdsr;
  struct sockaddr_in servaddr;
  static uint16_t i = 0;
  struct client_cfg cfg;

  strcpy(cfg.ip,((struct client_cfg*)arg)->ip);
  cfg.port = ((struct client_cfg*)arg)->port;
  cfg.client_no = ((struct client_cfg*)arg)->client_no;
  cfg.monitor_pdrv = ((struct client_cfg*)arg)->monitor_pdrv;
//  log_i("%d  %d  %s", cfg.client_no, cfg.port, cfg.ip);			TXL注释

  s_ethernet_client_rx_buf[cfg.client_no] = (uint8_t *)malloc(CLIENT_2404_RX_BUFSIZE);
  s_ethernet_client_tx_buf[cfg.client_no] = (uint8_t *)malloc(CLIENT_2404_TX_BUFSIZE);
  #ifdef USING_MY_QUEUE
  if (QueueCreate(&s_ethernet_client_rxcb[cfg.client_no], s_ethernet_client_rx_buf[cfg.client_no], CLIENT_2404_RX_BUFSIZE, NULL, NULL) == QUEUE_ERR)
  {
	  return NULL;
  }

  #endif /* USING_MY_QUEUE */
  memset(s_ethernet_client_rx_buf[cfg.client_no], 0, CLIENT_2404_RX_BUFSIZE);
  memset(s_ethernet_client_tx_buf[cfg.client_no], 0, CLIENT_2404_TX_BUFSIZE);
  s_ethernet_client_flag[cfg.client_no] = 0;
  s_ethernet_client_flag[cfg.client_no] |= CLIENT_TCP_LINKUP;
  s_ethernet_client_len[cfg.client_no] = 0;
  while (1)
  {
    client_sock[cfg.client_no] = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(cfg.port);
    err = inet_pton(AF_INET, cfg.ip, &servaddr.sin_addr);
    if(err != 1)
    {
//      log_e("inet_pton err:%d",err);									TXL注释
    }

       /* set NONBLOCK */
    //    int flags;
	// 	flags = fcntl(client_sock[cfg.client_no], F_GETFL, 0);
	// 	fcntl(client_sock[cfg.client_no], F_SETFL, flags &~ O_NONBLOCK);
//    log_i("connect client_sock[%d]:%d",cfg.client_no,client_sock[cfg.client_no]);		TXL注释
    if (connect(client_sock[cfg.client_no], (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
//      log_w("(%d)err:%d",cfg.client_no, errno);						TXL注释
      close(client_sock[cfg.client_no]);
      client_sock[cfg.client_no] = -1;
      perror("connect");
      continue;
    }
//    log_w("==========================(%d)connent ok============================",cfg.client_no);	TXL注释
    running[cfg.client_no] = 1;
    while (running[cfg.client_no])
    {
      tv.tv_sec = 0;
      tv.tv_usec = 1000;
      FD_ZERO(&fdsr);
      FD_SET(client_sock[cfg.client_no], &fdsr);

      max_sock = client_sock[cfg.client_no] + 1;
      ret = select(max_sock + 1, &fdsr, NULL, NULL, &tv);

      if ((ret < 0) && (errno != EINTR))
      {
//        log_i("(%d)errno=%d,%s", cfg.client_no,errno, strerror(errno));						TXL注释
        close(client_sock[cfg.client_no]);
        FD_CLR(client_sock[cfg.client_no], &fdsr);
//        log_i("---------------------------------------------------------------recv fail");	TXL注释
        perror("--------------------------------------------------------------recv fail");
        client_sock[cfg.client_no] = -1;
        running[cfg.client_no] = 0;
        break;
      }
      else if (ret == 0)
      {
        if ((s_ethernet_client_flag[cfg.client_no] & CLIENT_SENDING) == CLIENT_SENDING)
        {
//          sem_P(CLIENT_WRITE_SEM);															TXL注释
          err = write(client_sock[cfg.client_no], s_ethernet_client_tx_buf[cfg.client_no], s_ethernet_client_len[cfg.client_no]);
//          log_w("(%d)send_size:%d,err:%d errno:%d",cfg.client_no, s_ethernet_client_len[cfg.client_no], err, errno);
          if (err > 0)
          {
            s_ethernet_client_len[cfg.client_no] -= err;
          }
//          sem_V(CLIENT_WRITE_SEM);						TXL注释

          if ((err == -1) && (errno != EINTR))
          {
            perror("--------------------------------------------------------------write");
            close(client_sock[cfg.client_no]);
            client_sock[cfg.client_no] = -1;
            running[cfg.client_no] = 0;
            break;
          }
          if (s_ethernet_client_len[cfg.client_no] == 0)
          {
            s_ethernet_client_flag[cfg.client_no] &= ~CLIENT_SENDING;
          }
        }
      }
      else
      {
        recv_size[cfg.client_no] = read(client_sock[cfg.client_no], &socket_rxbuffer[cfg.client_no], 256);
        
//        log_i("(%d)recv_size:%d errno:%d",cfg.client_no, recv_size[cfg.client_no], errno);
        if ((recv_size[cfg.client_no] <= 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
          continue;
        if (((recv_size[cfg.client_no] <= 0) || (recv_size[cfg.client_no] > 256)) && ((errno != EINTR)))
        {
          perror("recv");
          close(client_sock[cfg.client_no]);
          client_sock[cfg.client_no] = -1;
          running[cfg.client_no] = 0;
//          log_i("-------------------------------------------------------------------------Server Closed!");
          break;
        }
        else
        {
//          log_i("(%d)recv_size:%d:",cfg.client_no, recv_size[cfg.client_no]);
            
          if (ex_api.queue_write_block != NULL)
          {
        	  ex_api.queue_write_block(&s_ethernet_client_rxcb[cfg.client_no], &socket_rxbuffer[cfg.client_no], recv_size[cfg.client_no]);
          }
        }
      }
      if ((s_ethernet_client_flag[cfg.client_no] & CLIENT_TCP_DISCONNECT) == CLIENT_TCP_DISCONNECT)
      {
        s_ethernet_client_flag[cfg.client_no] &= ~CLIENT_TCP_DISCONNECT;
      }
    }
    close(client_sock[cfg.client_no]);
    perror("close");
    // return 0;
  }
}

/* PUBLIC FUNCTION PROTOTYPES ------------------------------------------------*/
/**
  * @brief :从队列中读数据
  * @param : [client_no]-client NO.
  * @param : [buf]-recvive data pointer
  * @param : [len]-recvive data lenth
  * @return: the lenth of read
  * @update: [2019-03-19][Chen][newly increased]
  */
uint16_t ethernet_client_get(uint8_t client_no, uint8_t *buf, int len)
{
  uint16_t tmp;

  if ((buf == NULL) || (len == 0))
  {
//    log_e("(buf == NULL) || (len == 0)");						TXL注释
    return (0);
  }

  for (tmp = 0; tmp < len; tmp++, buf++)
  {
    #ifdef USING_MY_QUEUE
    if (ex_api.queue_read != NULL)
    {
      if (ex_api.queue_read(buf, (void *)&s_ethernet_client_rxcb[client_no]) == QUEUE_EMPTY)
      {
        break;
      }
    }
    #endif /* USING_MY_QUEUE */
  }

  return (tmp);
}

/**
  * @brief : 要发送的数据
  * @param : [client_no]-client number
  * @param : [buf]-send data pointer
  * @param : [len]-send data lenth
  * @return: the lenth of write
  * @update: [2019-03-19][Chen][newly increased]
  */
uint16_t ethernet_client_put(uint8_t client_no, uint8_t *buf, int len)
{
  if ((buf == NULL) || (len == 0))
  {
//    log_i("(buf == NULL) || (len == 0)");
    return (0);
  }

  if (len < CLIENT_2404_TX_BUFSIZE)
  {
//    sem_P(CLIENT_WRITE_SEM);												TXL注释
    memcpy(s_ethernet_client_tx_buf[client_no] + s_ethernet_client_len[client_no], buf, len);
    s_ethernet_client_flag[client_no] |= CLIENT_SENDING;
    s_ethernet_client_len[client_no] += len;
//    sem_V(CLIENT_WRITE_SEM);												TXL注释
    return len;
  }
  return (0);
}

/**
  * @brief : 返回client端连接状态
  * @param :
  * @param :
  * @param :
  * @return: 状态
  * @update: [2019-03-19][Chen][newly increased]
  */
uint8_t getClientLinkState(uint8_t client_no)
{
  if (running[client_no])
  {
//    return true;
	  return 1;
  }
  return 0;
//  return false;															TXL注释
}

/**
  * @brief : 关闭client端连接
  * @param :
  * @param :
  * @param :
  * @return: 状态
  * @update: [2019-03-19][Chen][newly increased]
  */
void setClientCloseLink(uint8_t client_no)
{
  close(client_sock[client_no]);
  client_sock[client_no] = -1;
  running[client_no] = 0;
}

/**
  * @brief : 客户端线程接口
  * @param : []
  * @param : []
  * @return: []
  * @update: [2019-03-19][Chen][newly increased]
  */
void client_task(struct tagTcpSocketCfg cfg, uint8_t pdrv)
{
  pthread_t tid;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  strcpy(sClientCfg[pdrv].ip,cfg.ip);
  sClientCfg[pdrv].port = cfg.port;
  sClientCfg[pdrv].client_no = cfg.tcp_no;
  sClientCfg[pdrv].monitor_pdrv = cfg.tcp_no;
//    log_w("sClientCfg[%d].ip:%s",pdrv,sClientCfg[pdrv].ip);								TXL注释
//    log_w("sClientCfg[%d].port:%d",pdrv,sClientCfg[pdrv].port);
//    log_w("sClientCfg[%d].client_no:%d",pdrv,sClientCfg[pdrv].client_no);
//    log_w("sClientCfg[%d].monitor_pdrv:%d",pdrv,sClientCfg[pdrv].monitor_pdrv);
//    log_w("sClientCfg:%p",&sClientCfg[pdrv]);
  pthread_create(&tid, &attr, thread_client, &sClientCfg[pdrv]);

  pthread_attr_destroy(&attr);
}

/* END OF FILE ----------------------------------------------------------------*/
