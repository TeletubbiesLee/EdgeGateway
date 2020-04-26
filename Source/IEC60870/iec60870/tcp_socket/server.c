/**
  * @copyright  Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      ethernet.c
  * @brief:     the device driver of ethernet
  * @version:   V1.0.0
  * @author:    Sunxr
  * @date:      2018-09-04
  * @update:    [2018-09-04][Sunxr][newly increased]
	* @update:    [2019-03-01][Chen][]
  */
#define LOG_TAG    "server"
/* INCLUDE --------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <net/if.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "server.h"
#include "../queue/queue.h"
//#include "elog.h"
#include "../mysem/mysem.h"
//#include "monitor.h"
/* PRIVATE VARIABLES ----------------------------------------------------------------*/
static struct ethernet_cfg sServerCfg[SERVER_NUM];
static uint8_t s_ethernet_server_flag[SERVER_NUM];
static uint16_t s_ethernet_server_len[SERVER_NUM];
static uint8_t *s_ethernet_server_rx_buf[SERVER_NUM];
static uint8_t *s_ethernet_server_tx_buf[SERVER_NUM];
static int client_sock[SERVER_NUM] = {-1};
static uint8_t running[SERVER_NUM];
#ifdef USING_MY_QUEUE
static DataQueue s_ethernet_server_rxcb[SERVER_NUM];
#endif /* USING_MY_QUEUE */

static struct external_api ex_api =
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
  * @brief : ethernet init
  * @param : [cfg]-ethernet config struct pointer
  * @return: [-1]-failed
  * @return: [server_sock]-server socket file descriptor
  * @update: [2018-09-19][Sunxr][newly increased]
  * @update: [2018-09-30][Sunxr][package]
  */
static int enternet_startup(struct ethernet_cfg *cfg)
{
    #define 	        CLIENT_MAX_NUM 		10
	int 				on = 1;
	int                 server_sock;
	struct sockaddr_in 	server_addr;
	fd_set 				fdsr;

//	log_i("start Ether!");
	server_sock = socket(AF_INET, SOCK_STREAM, 0);						//创建套接字

	if (server_sock < 0)
	{
		perror("socket");
		return -1;
	}

	FD_ZERO(&fdsr);
	FD_SET(server_sock, &fdsr);

	// 设置套接字选项避免地址使用错误
	if ((setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
    {
        perror("setsockopt failed");
		close(server_sock);
		return -1;
    }

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	//server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_addr.s_addr = inet_addr(cfg->ip);
	server_addr.sin_port = htons(cfg->port);
	memset(server_addr.sin_zero,0,sizeof(server_addr.sin_zero));

	if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)	//绑定套接字
	{
		perror("connect err:");
		close(server_sock);
		return -1;
	}

	if (listen(server_sock, CLIENT_MAX_NUM) == -1)										//监听最大个数为10个
	{
		perror("listen err:");
		close(server_sock);
		return -1;
	}

    if ((cfg->server_no >= SERVER_NUM) || (cfg->server_no < 0))
    {
    	return -1;
    }

	s_ethernet_server_rx_buf[cfg->server_no] = (uint8_t *)malloc(SERVER_2404_RX_BUFSIZE);	//申请读取数据内存
	s_ethernet_server_tx_buf[cfg->server_no] = (uint8_t *)malloc(SERVER_2404_TX_BUFSIZE);	//申请发送数据内存

  #ifdef USING_MY_QUEUE
	if (QueueCreate(&s_ethernet_server_rxcb[cfg->server_no], s_ethernet_server_rx_buf[cfg->server_no], SERVER_2404_RX_BUFSIZE, NULL, NULL) == QUEUE_ERR)
	{
		return -1;
	}
  #endif /* USING_MY_QUEUE */

    memset (s_ethernet_server_rx_buf[cfg->server_no], 0, SERVER_2404_RX_BUFSIZE);
    memset (s_ethernet_server_tx_buf[cfg->server_no], 0, SERVER_2404_TX_BUFSIZE);
	return server_sock;
}

/**
  * @brief : ethernet thread
  * @param : [arg]-entry parameter pointer
  * @return: [0]
  * @updata: [2018-09-19][Sunxr][newly increased]
  * @update: [2018-09-30][Sunxr][package]
  */
void *thread_ethernet(void *arg)
{
	int max_sock;
	int server_sock;
	static uint8_t socket_rxbuffer[1024];
	static ssize_t recv_size = 0;
	int ret, linknum=0, err = 0;
	unsigned int addr_size;
	struct sockaddr_in 	client_addr;
	struct timeval 		tv;
	fd_set 				fdsr;
	// uint32_t i = 0;
	int flags;

	struct ethernet_cfg cfg;

	strcpy(cfg.ip,((struct ethernet_cfg*)arg)->ip);
	cfg.port = ((struct ethernet_cfg*)arg)->port;
	cfg.server_no = ((struct ethernet_cfg*)arg)->server_no;
	cfg.monitor_pdrv = ((struct ethernet_cfg*)arg)->monitor_pdrv;

//	log_i("%d  %d  %s", cfg.server_no, cfg.port, cfg.ip);

	server_sock = enternet_startup(&cfg);		//创建套接字、绑定套接字
	while (1)
	{
		addr_size = sizeof(client_addr);
		client_sock[cfg.server_no] = -1;
//		log_i("ethernet task...!");

		client_sock[cfg.server_no] = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
    /* set NONBLOCK */
		flags = fcntl(client_sock[cfg.server_no], F_GETFL, 0);			//获得文件状态标记
		fcntl(client_sock[cfg.server_no], F_SETFL, flags | O_NONBLOCK);	//设置文件状态标记   非阻塞I/O

		if (client_sock[cfg.server_no] == -1)
		{
//			log_i("accept err!");
			close(server_sock);
			return 0;
		}
		perror("client_sock");
		running[cfg.server_no] = 1;
		s_ethernet_server_flag[cfg.server_no] = 0;
		s_ethernet_server_flag[cfg.server_no] |= ETHERNET_TCP_SERVER_LINKUP;
		s_ethernet_server_len[cfg.server_no]  = 0;

//		log_i("Link! %02d", ++linknum);

		while (running[cfg.server_no])
		{
			tv.tv_sec = 0;
			tv.tv_usec = 1000;

			FD_ZERO(&fdsr);
			FD_SET(client_sock[cfg.server_no], &fdsr);

			max_sock = server_sock > client_sock[cfg.server_no] ? server_sock : client_sock[cfg.server_no];

			ret = select(max_sock + 1, &fdsr, NULL, NULL, &tv);
			if ((ret < 0) &&(errno!=EINTR))
		 	{
//				log_i("errno=%d,%s", errno, strerror(errno));
				close(client_sock[cfg.server_no]);
				FD_CLR(client_sock[cfg.server_no], &fdsr);
//				log_i("recv fail");
				client_sock[cfg.server_no] = -1;
				running[cfg.server_no] = 0;
				break;
			}
			else if (ret == 0)
			{

				if ((s_ethernet_server_flag[cfg.server_no] & ETHERNET_SENDING) == ETHERNET_SENDING)
				{
					sem_P(SERVER_WRITE_SEM);
					err = write(client_sock[cfg.server_no], s_ethernet_server_tx_buf[cfg.server_no], s_ethernet_server_len[cfg.server_no]);
                    /* ?????? */
//                    send_monitor_data(INTERNET_PORT, cfg.server_no, s_ethernet_server_tx_buf[cfg.server_no], s_ethernet_server_len[cfg.server_no], SEND_FLAG);	//TXL注释

//					log_w("send_size:%d,err:%d errno:%d",s_ethernet_server_len[cfg.server_no],err,errno);	TXL注释
					if(err > 0)
					  {s_ethernet_server_len[cfg.server_no] -= err;}
					sem_V(SERVER_WRITE_SEM);

					if ((err == -1) && (errno!=EINTR))
					{
						perror("send");
						close(client_sock[cfg.server_no]);
						linknum--;
						client_sock[cfg.server_no] = -1;
						running[cfg.server_no] = 0;
//						log_i("server Closed!");
						break;
					}
					if(s_ethernet_server_len[cfg.server_no] == 0)
						{s_ethernet_server_flag[cfg.server_no] &= ~ETHERNET_SENDING;}
				}
			}
			else
			{
				recv_size = recv(client_sock[cfg.server_no], &socket_rxbuffer, 256, MSG_DONTWAIT);	//client_sock[cfg.server_no] 为客户端套接字
//				log_i("recv_size:%d errno:%d", recv_size,errno);
				perror("recv");

				if ((recv_size < 0) && ((errno == EAGAIN)||(errno == EWOULDBLOCK)))
					continue;
				if (((recv_size <= 0) || (recv_size > 256)) && ((errno!=EINTR)))
				{
					perror("recv");
					close(client_sock[cfg.server_no]);
					linknum--;
					client_sock[cfg.server_no] = -1;
					running[cfg.server_no] = 0;
//					log_i("Client Closed!");
					break;
				}
				else
				{
					if (ex_api.queue_write_block != NULL)
					{
						ex_api.queue_write_block(&s_ethernet_server_rxcb[cfg.server_no], socket_rxbuffer, recv_size);
//                        /* ?????? */
//                        send_monitor_data(INTERNET_PORT, cfg.server_no, socket_rxbuffer, recv_size, RECEIVE_FLAG);
					}
				}
			}

			if ((s_ethernet_server_flag[cfg.server_no] & ETHERNET_TCP_SERVER_DISCONNECT) == ETHERNET_TCP_SERVER_DISCONNECT)
			{
				s_ethernet_server_flag[cfg.server_no] &= ~ETHERNET_TCP_SERVER_DISCONNECT;
				//running[cfg.server_no] = 0;
			}
		}
	}

	close(server_sock);
	client_sock[cfg.server_no] = -1;
	s_ethernet_server_flag[cfg.server_no] &= ~ETHERNET_TCP_SERVER_LINKUP;

	return 0;
}

/* PUBLIC FUNCTION ------------------------------------------------------------------*/
/**
  * @brief : ethernet sent
  * @param : [server_no]-server NO.
  * @param : [buf]-recvive data pointer
  * @param : [len]-recvive data lenth
  * @return: the lenth of read
  * @update: [2018-09-29][Sunxr][newly increased]
  */
uint16_t ethernet_server_get(uint8_t server_no, uint8_t *buf, int len)
{
    uint16_t tmp;

    if ((buf == NULL) || (len == 0))
    {
//    	log_e("(buf == NULL) || (len == 0)");		TXL注释
        return (0);
    }

    for (tmp = 0; tmp < len; tmp++, buf++)
    {
      #ifdef USING_MY_QUEUE
    	if (ex_api.queue_read != NULL)
    	{
            if (ex_api.queue_read(buf, (void *)&s_ethernet_server_rxcb[server_no]) == QUEUE_EMPTY)
            {
                break;
            }
    	}
      #endif /* USING_MY_QUEUE */
    }

	return (tmp);
}

/**
  * @brief : ethernet sent
  * @param : [server_no]-server number
  * @param : [buf]-send data pointer
  * @param : [len]-send data lenth
  * @return: the lenth of write
  * @update: [2018-09-29][Sunxr][newly increased]
  */
uint16_t ethernet_server_put(uint8_t server_no, uint8_t *buf, int len)
{
	if ((buf == NULL) || (len == 0) )
	{
//		log_i("(buf == NULL) || (len == 0)");
		return (0);
	}
	
	if (len < SERVER_2404_TX_BUFSIZE)
	{
		sem_P(SERVER_WRITE_SEM);
		memcpy(s_ethernet_server_tx_buf[server_no]+s_ethernet_server_len[server_no], buf, len);
		s_ethernet_server_flag[server_no] |= ETHERNET_SENDING;
		s_ethernet_server_len[server_no] += len;
//		log_i("s_ethernet_server_len[%d]:%d",server_no,s_ethernet_server_len[server_no]);
		sem_V(SERVER_WRITE_SEM);
		return len;
	}

	return (0);
}

/**
  * @brief : 返回server端连接状态
  * @param :
  * @param :
  * @param :
  * @return: 状态
  * @update: [2018-12-21][Chen][newly increased]
  */
uint8_t getServerLinkState(uint8_t server_no)
{
	if(client_sock[server_no] == -1)
	{
//		return false;					//TXL注释
		return 0;
	}
	else
	{
//		return true;					//TXL注释
		return 1;
	}

}

/**
  * @brief : 关闭server端连接
  * @param :
  * @param :
  * @param :
  * @return: 状态
  * @update: [2018-12-21][Chen][newly increased]
  */
void setServerCloseLink(uint8_t server_no)
{
	close(client_sock[server_no]);
	client_sock[server_no] = -1;
	running[server_no] = 0;
}

/**
  * @brief : ethernet task
  * @param : [cfg[]]-ethernet config struct
  * @param : [num]-network card num
  * @return: [none]
  * @update: [2018-09-17][Sunxr][newly increased]
  */
void server_task(struct tagTcpSocketCfg cfg, uint8_t pdrv)
{
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	strcpy(sServerCfg[pdrv].ip,cfg.ip);
	sServerCfg[pdrv].port = cfg.port;
	sServerCfg[pdrv].server_no = cfg.tcp_no;
	sServerCfg[pdrv].monitor_pdrv = cfg.tcp_no;
//    log_w("sServerCfg[%d].ip:%s",pdrv,sServerCfg[pdrv].ip);					TXL注释
//    log_w("sServerCfg[%d].port:%d",pdrv,sServerCfg[pdrv].port);
//    log_w("sServerCfg[%d].server_no:%d",pdrv,sServerCfg[pdrv].server_no);
//    log_w("sServerCfg[%d].monitor_pdrv:%d",pdrv,sServerCfg[pdrv].monitor_pdrv);
//    log_w("sServerCfg:%p",&sServerCfg[pdrv]);
    pthread_create(&tid, &attr, thread_ethernet, &sServerCfg[pdrv]);

	pthread_attr_destroy(&attr);
}
/* FILE END  ------------------------------------------------------------------------*/

