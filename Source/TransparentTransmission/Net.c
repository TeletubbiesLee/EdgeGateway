/**
 * @file Net.c
 * @brief Linux系统中网络设备相关操作函数文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.02.17
 *
 * @author Lei.L
 * @version ver 1.0
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include "Net.h"
#include "../Config.h"


/**
 * @breif 打开网口设备并TCP连接
 * @param ipAddress IP地址，格式："192.168.1.1"
 * @param serverPort 服务端串口号
 * @return 设备文件描述符或-1
 */
int TCP_NetConnect(char *ipAddress, int serverPort)
{
    int socketFd = -1;
    struct hostent *host = NULL;
    struct sockaddr_in serverAddr;

    if((host = gethostbyname(ipAddress)) == NULL)
	{
    	printf_debug("gethostbyname %s error!\n", ipAddress);
		return FUNCTION_FAIL;
	}

    if((socketFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
    	printf_debug("socket create error!\n");
		return FUNCTION_FAIL;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);
	serverAddr.sin_addr = *((struct in_addr *)host->h_addr);
	bzero(&(serverAddr.sin_zero), 8);
	while(connect(socketFd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) == -1)
	{
		printf_debug("TCP_NetConnect:connect error!\n");
		sleep(1);
	}
	printf("connect success!\n");
	write(socketFd, "SOJO", strlen("SOJO"));

    return socketFd;
}


/**
 * @breif 网口设备侦听
 * @param serverPort 服务端串口号
 * @return 设备文件描述符或-1
 */
int TCP_NetListen(int serverPort)
{
    int socketFd = -1;
    struct sockaddr_in loaclAddr;     /* loacl */

    if ((socketFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
    	printf_debug("socket fail!\n");
        return FUNCTION_FAIL;
	}

	loaclAddr.sin_family = AF_INET;
	loaclAddr.sin_port = htons(serverPort);
	loaclAddr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(loaclAddr.sin_zero), 8);

	if(bind(socketFd, (struct sockaddr *)&loaclAddr, sizeof(struct sockaddr)) == -1)
	{
		printf_debug("bind error!\n");
		return FUNCTION_FAIL;
	}

	if(listen(socketFd, BACKLOG) == -1)
	{
		printf_debug("listen error!\n");
		return FUNCTION_FAIL;
	}

    return socketFd;
}


/**
 * @breif 网口设备连接
 * @param sockfd 服务端socket文件描述符
 * @return 设备文件描述符或-1
 */
int TCP_NetAccept(int socketFd)
{
    int clientFd = -1;
    struct sockaddr_in remoteAddr;
    socklen_t sinSize = 0;

    sinSize = sizeof(struct sockaddr_in);

	while((clientFd = accept(socketFd, (struct sockaddr *)&remoteAddr, &sinSize)) == -1)
	{
		printf_debug("accept error!\n");
		sleep(1);
	}

	printf("Receive From： %s\n", inet_ntoa(remoteAddr.sin_addr));
	write(clientFd, "SOJO", strlen("SOJO"));

    return clientFd;
}


/**
 * @breif 关闭TCP连接
 * @param sockfd 要关闭的socket的指针
 * @return void
 */
int TCP_CloseConnect(int *socketFd)
{
	while(shutdown(*socketFd, SHUT_RDWR));		//关闭连接
	*socketFd = -1;
	printf("connect close!\n");
	return NO_ERROR;
}


/**
 * @breif 打开网口设备并UDP连接
 * @param serverPort 服务端串口号
 * @return 设备文件描述符或-1
 */
int UDP_NetConnect(int serverPort)
{
    int socketFd = -1;
    struct sockaddr_in localAddr;         /* loacl */

    if((socketFd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
    	printf_debug("socket fail!\n");
        return FUNCTION_FAIL;
	}

	localAddr.sin_family = AF_INET;
	localAddr.sin_port = htons(serverPort);
	localAddr.sin_addr.s_addr = INADDR_ANY;

	bzero(&(localAddr.sin_zero), 8);

	if(bind(socketFd, (struct sockaddr *)&localAddr, sizeof(struct sockaddr)) == -1)
	{
		printf_debug("bind error!\n");
		return FUNCTION_FAIL;
	}

    return socketFd;
}


/**
 * @breif 设置网口非阻塞模式
 * @param sockfd 网口文件描述符
 * @return void
 */
void SetNetNonBlock(int socketFd)
{
    int flags;
    
    flags = fcntl(socketFd, F_GETFL, 0);				//获取原始sockfd属性
	fcntl(socketFd, F_SETFL, flags | O_NONBLOCK);		//添加非阻塞
}


/**
 * @breif 设置远端IP和端口信息
 * @param ipAddress 远端IP地址
 * @param port 远端端口号
 * @param remoteAddr 远端IP地址和端口号设置
 * @return void
 */
void SetRemoteAddress(char *ipAddress, int port, struct sockaddr_in *remoteAddr)
{
	remoteAddr->sin_family = AF_INET;
	remoteAddr->sin_port = htons(port);
	remoteAddr->sin_addr.s_addr = inet_addr(ipAddress);
}









