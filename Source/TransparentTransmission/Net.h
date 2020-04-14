/**
 * @file Net.h
 * @brief Linux系统中网络设备相关操作函数文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.02.17
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _NET_H_
#define _NET_H_


#define BACKLOG		10		//最大客户端连接数

int TCP_NetConnect(char *ipAddress, int serverPort);
int TCP_NetListen(int serverPort);
int TCP_NetAccept(int sockfd);
int TCP_CloseConnect(int *sockfd);
int UDP_NetConnect(int serverPort);
void SetNetNonBlock(int sockfd);
void SetRemoteAddress(char *ipAddress, int port, struct sockaddr_in *remoteAddr);

#endif
