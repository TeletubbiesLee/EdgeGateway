/**
 * @file Task.c
 * @brief 任务相关的程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.02.17
 *
 * @author Lei.L
 * @version ver 1.0
 */


#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <Pthread.h>
#include "Task.h"
#include "Net.h"
#include "Uart.h"
#include "../Config.h"



static void TCPClientReceivePthread(void *param);
static void TCPClientSendPthread(void *param);
static void TCPServerReceivePthread(void *param);
static void TCPServerSendPthread(void *param);
static void UDPReceivePthread(void *param);
static void UDPSendPthread(void *param);
static void UartReceivePthread(void *param);
static void UartSendPthread(void *param);
static void PrintfData(char *prefixName, char *data, int lenth);




/**
 * @breif TCP客户端转串口，外接TCP服务器端和串口
 * @param uartInfo 串口信息结构体指针
 * @param networkInfo 网口信息结构体指针
 * @return 成功0或失败-1
 */
int TCP_Client2Uart(UartInfo *uartInfo, NetworkInfo *networkInfo)
{
	int uartFd = 0, socketFd = 0;		//串口和网卡设备的文件描述符
	int uart2TcpPipe[2] = {0}, tcp2UartPipe[2] = {0};		//串口转TCP的管道识别号，TCP转串口的管道识别号
	int paramArray[4][3] = {{0}, {0}, {0}, {0}};			//用于给线程传递参数
	pthread_t tcpSendtPid, uartSendPid, tcpReceivePid, uartReceivePid;		//网口与串口转换的线程ID号
	int ret = -1;
	void *status = NULL;

	/* 建立管道pipe */
	if(pipe(uart2TcpPipe) < 0)
	{
		printf_debug("uart to tcp pipe error!\n");
		return FUNCTION_FAIL;
	}
	if(pipe(tcp2UartPipe) < 0)
	{
		printf_debug("tcp to uart pipe error!\n");
		return FUNCTION_FAIL;
	}

	/* 建立连接服务器端的socket，打开串口 */
	socketFd = TCP_NetConnect(networkInfo->remoteAddress, networkInfo->remotePort);
	if(FUNCTION_FAIL == socketFd)
	{
		printf_debug("TCP Client Connect error!\n");
		goto TCP_CLIENT_CLOSE;
	}
	uartFd = UartInit(uartInfo->uartName, uartInfo->bandrate, uartInfo->uartType);
	if(FUNCTION_FAIL == uartFd)
	{
		printf_debug("Uart Init error!\n");
		goto TCP_CLIENT_CLOSE;
	}

	/* 网口与串口透传的线程 */
	paramArray[0][0] = tcp2UartPipe[1];		//写管道
	paramArray[0][1] = (int)&socketFd;
	paramArray[0][2] = (int)networkInfo;
	ret = pthread_create(&tcpReceivePid, NULL, (void*)TCPClientReceivePthread, paramArray[0]);
	if(0 != ret)
	{
		printf_debug("pthread TCP Client Receive create error!\n");
		goto TCP_CLIENT_CLOSE;
	}

	paramArray[1][0] = uart2TcpPipe[0];		//读管道
	paramArray[1][1] = (int)&socketFd;
	ret = pthread_create(&tcpSendtPid, NULL, (void*)TCPClientSendPthread, paramArray[1]);
	if(0 != ret)
	{
		printf_debug("pthread TCP Client Send create error!\n");
		goto TCP_CLIENT_CLOSE;
	}

	paramArray[2][0] = uart2TcpPipe[1];		//写管道
	paramArray[2][1] = uartFd;
	ret = pthread_create(&uartReceivePid, NULL, (void*)UartReceivePthread, paramArray[2]);
	if(0 != ret)
	{
		printf_debug("pthread Uart Receive create error!\n");
		goto TCP_CLIENT_CLOSE;
	}

	paramArray[3][0] = tcp2UartPipe[0];		//读管道
	paramArray[3][1] = uartFd;
	ret = pthread_create(&uartSendPid, NULL, (void*)UartSendPthread, paramArray[3]);
	if(0 != ret)
	{
		printf_debug("pthread Uart Send create error!\n");
		goto TCP_CLIENT_CLOSE;
	}

TCP_CLIENT_CLOSE:
	pthread_join(tcpReceivePid, &status);
	pthread_join(tcpSendtPid, &status);
	pthread_join(uartReceivePid, &status);
	pthread_join(uartSendPid, &status);
	close(socketFd);
	close(uartFd);

	return NO_ERROR;
}


/**
 * @breif TCP服务端转串口，外接TCP客户端和串口
 * @param uartInfo 串口信息结构体指针
 * @param networkInfo 网口信息结构体指针
 * @return 成功0或失败-1
 */
int TCP_Server2Uart(UartInfo *uartInfo, NetworkInfo *networkInfo)
{
	int uartFd = 0, socketFd = 0, clientFd = 0;
	int uart2TcpPipe[2] = {0}, tcp2UartPipe[2] = {0};		//串口转TCP的管道识别号，TCP转串口的管道识别号
	int paramArray[4][3] = {{0}, {0}, {0}, {0}};			//用于给线程传递参数
	pthread_t tcpSendtPid = 0, uartSendPid = 0, tcpReceivePid = 0, uartReceivePid = 0;		//网口与串口转换的线程ID号
	int ret = -1;
	void *status = NULL;

	/* 建立管道pipe */
	if(pipe(uart2TcpPipe) < 0)
	{
		printf_debug("uart to tcp pipe error!\n");
		return FUNCTION_FAIL;
	}
	if(pipe(tcp2UartPipe) < 0)
	{
		printf_debug("tcp to uart pipe error!\n");
		return FUNCTION_FAIL;
	}

	/* 侦听客户端并且连接，建立socket，打开串口 */
	socketFd = TCP_NetListen(networkInfo->localPort);
	if(FUNCTION_FAIL == socketFd)
	{
		printf_debug("TCP Server Listen error!\n");
		goto TCP_SERVER_CLOSE;
	}
	uartFd = UartInit(uartInfo->uartName, uartInfo->bandrate, uartInfo->uartType);
	if(FUNCTION_FAIL == uartFd)
	{
		printf_debug("Uart Init error!\n");
		goto TCP_SERVER_CLOSE;
	}
	clientFd = TCP_NetAccept(socketFd);
	if(FUNCTION_FAIL == clientFd)
	{
		printf_debug("TCP Server Accept error!\n");
		goto TCP_SERVER_CLOSE;
	}

	/* 网口与串口透传的线程 */
	paramArray[0][0] = tcp2UartPipe[1];		//写管道
	paramArray[0][1] = socketFd;
	paramArray[0][2] = (int)&clientFd;
	ret = pthread_create(&tcpReceivePid, NULL, (void*)TCPServerReceivePthread, paramArray[0]);
	if(0 != ret)
	{
		printf_debug("pthread TCP Server Receive create error!\n");
		goto TCP_SERVER_CLOSE;
	}

	paramArray[1][0] = uart2TcpPipe[0];		//读管道
	paramArray[1][1] = (int)&clientFd;
	ret = pthread_create(&tcpSendtPid, NULL, (void*)TCPServerSendPthread, paramArray[1]);
	if(0 != ret)
	{
		printf_debug("pthread TCP Server Send create error!\n");
		goto TCP_SERVER_CLOSE;
	}

	paramArray[2][0] = uart2TcpPipe[1];		//写管道
	paramArray[2][1] = uartFd;
	ret = pthread_create(&uartReceivePid, NULL, (void*)UartReceivePthread, paramArray[2]);
	if(0 != ret)
	{
		printf_debug("pthread Uart Receive create error!\n");
		goto TCP_SERVER_CLOSE;
	}

	paramArray[3][0] = tcp2UartPipe[0];		//读管道
	paramArray[3][1] = uartFd;
	ret = pthread_create(&uartSendPid, NULL, (void*)UartSendPthread, paramArray[3]);
	if(0 != ret)
	{
		printf_debug("pthread Uart Send create error!\n");
		goto TCP_SERVER_CLOSE;
	}

TCP_SERVER_CLOSE:
	pthread_join(tcpReceivePid, &status);
	pthread_join(tcpSendtPid, &status);
	pthread_join(uartReceivePid, &status);
	pthread_join(uartSendPid, &status);
	close(clientFd);
	close(socketFd);
	close(uartFd);

	return NO_ERROR;
}


/**
 * @breif UDP转串口，外接UDP和串口
 * @param uartInfo 串口信息结构体指针
 * @param networkInfo 网口信息结构体指针
 * @return 成功0或失败-1
 */
int UDP2Uart(UartInfo *uartInfo, NetworkInfo *networkInfo)
{
	int uartFd = 0, socketFd = 0;
	int uart2UdpPipe[2] = {0}, udp2UartPipe[2] = {0};		//串口转UDP的管道识别号，UDP转串口的管道识别号
	int paramArray[4][3] = {{0}, {0}, {0}, {0}};			//用于给线程传递参数
	pthread_t udpSendtPid = 0, uartSendPid = 0, udpReceive = 0, uartReceive = 0;		//网口与串口转换的线程ID号
	struct sockaddr_in remoteAddr;		//远端IP和端口等信息结构体
	int ret = -1;
	void *status = NULL;

	/* 建立管道pipe */
	if(pipe(uart2UdpPipe) < 0)
	{
		printf_debug("uart to udp pipe error!\n");
		return FUNCTION_FAIL;
	}
	if(pipe(udp2UartPipe) < 0)
	{
		printf_debug("udp to uart pipe error!\n");
		return FUNCTION_FAIL;
	}

	/* 连接UDP并建立socket，打开串口，设置远端配置的IP和端口号 */
	socketFd = UDP_NetConnect(networkInfo->localPort);
	if(FUNCTION_FAIL == socketFd)
	{
		printf_debug("UDP Connect error!\n");
		goto UDP_CLOSE;
	}
	uartFd = UartInit(uartInfo->uartName, uartInfo->bandrate, uartInfo->uartType);
	if(FUNCTION_FAIL == uartFd)
	{
		printf_debug("Uart Init error!\n");
		goto UDP_CLOSE;
	}
	SetRemoteAddress(networkInfo->remoteAddress, networkInfo->remotePort, &remoteAddr);


	/* 网口与串口透传的线程 */
	paramArray[0][0] = udp2UartPipe[1];		//写管道
	paramArray[0][1] = socketFd;
	paramArray[0][2] = (int)&remoteAddr;
	ret = pthread_create(&udpReceive, NULL, (void*)UDPReceivePthread, paramArray[0]);
	if(0 != ret)
	{
		printf_debug("pthread UDP Receive create error!\n");
		goto UDP_CLOSE;
	}

	paramArray[1][0] = uart2UdpPipe[0];		//读管道
	paramArray[1][1] = socketFd;
	paramArray[1][2] = (int)&remoteAddr;
	ret = pthread_create(&udpSendtPid, NULL, (void*)UDPSendPthread, paramArray[1]);
	if(0 != ret)
	{
		printf_debug("pthread UDP Send create error!\n");
		goto UDP_CLOSE;
	}

	paramArray[2][0] = uart2UdpPipe[1];		//写管道
	paramArray[2][1] = uartFd;
	ret = pthread_create(&uartReceive, NULL, (void*)UartReceivePthread, paramArray[2]);
	if(0 != ret)
	{
		printf_debug("pthread Uart Receive create error!\n");
		goto UDP_CLOSE;
	}

	paramArray[3][0] = udp2UartPipe[0];		//读管道
	paramArray[3][1] = uartFd;
	ret = pthread_create(&uartSendPid, NULL, (void*)UartSendPthread, paramArray[3]);
	if(0 != ret)
	{
		printf_debug("pthread Uart Send create error!\n");
		goto UDP_CLOSE;
	}

UDP_CLOSE:
	pthread_join(udpReceive, &status);
	pthread_join(udpSendtPid, &status);
	pthread_join(uartReceive, &status);
	pthread_join(uartSendPid, &status);
	close(socketFd);
	close(uartFd);

	return NO_ERROR;
}


/**
 * @breif 串口接口转发到TCP客户端的线程程序
 * @param param 整型数组，第一个数存放读管道号，第二个数存放网络socket描述符指针
 * @return void
 */
static void TCPClientSendPthread(void *param)
{
    int *socketFd = NULL, pipeReadFd = 0;
    int dataBytes = 0;			//发送的字节数
    char dataBuffer[MAX_DATA_SIZE] = {0};		//发送缓存区

    pipeReadFd = ((int*)param)[0];
    socketFd = (int*)((int*)param)[1];

    while(1)
    {
    	dataBytes = read(pipeReadFd, dataBuffer, sizeof(dataBuffer));
        if(dataBytes > 0)
        {
        	printf_data_net2uart("TCPClientSend: ", dataBuffer, dataBytes);
            if(send(*socketFd, dataBuffer, dataBytes, 0) == -1)
            {
            	printf_debug("send error！\r\n");
                continue;
            }
        }
    }
}


/**
 * @breif TCP客户端接口转发到串口的线程程序
 * @param param 整型数组，第一个数存放写管道号，第二个数存放网络socket描述符指针，第三个数存放远端IP和端口号结构体指针
 * @return void
 */
static void TCPClientReceivePthread(void *param)
{
    int *socketFd = NULL, pipeWriteFd = 0;
    int dataBytes = 0;      //接收到的字节数
    char dataBuffer[MAX_DATA_SIZE] = {0};       //接收缓存区
    NetworkInfo *networkInfo = NULL;			//本地和远端IP、端口号

    pipeWriteFd = ((int*)param)[0];
    socketFd = (int*)((int*)param)[1];
    networkInfo = (NetworkInfo *)((int*)param)[2];

    while(1)
    {
    	dataBytes = recv(*socketFd, dataBuffer, MAX_DATA_SIZE, 0);
        if(dataBytes > 0)
        {
        	printf_data_net2uart("TCPClientReceive: ", dataBuffer, dataBytes);
            if(write(pipeWriteFd, dataBuffer, dataBytes) == -1)
            {
            	printf_debug("write error！\r\n");
                continue;
            }
        }
        else if(dataBytes == 0)		//远端断开连接
        {
        	TCP_CloseConnect(socketFd);		//关闭连接

			*socketFd = TCP_NetConnect(networkInfo->remoteAddress, networkInfo->remotePort);		//重新连接服务端
			if(*socketFd == -1)
				return;
        }
    } 
}


/**
 * @breif 串口接口转发到TCP服务器端的线程程序
 * @param param 整型数组，第一个数存放读管道号，第二个数存放网络socket描述符指针
 * @return void
 */
static void TCPServerSendPthread(void *param)
{
    int *clientFd = NULL, pipeReadFd = 0;
    int dataBytes = 0;				//发送的字节数
    char dataBuffer[MAX_DATA_SIZE] = {0};		//发送缓存区

    pipeReadFd = ((int*)param)[0];
    clientFd = (int*)((int*)param)[1];

    while(1)
    {
    	dataBytes = read(pipeReadFd, dataBuffer, sizeof(dataBuffer));
        if(dataBytes > 0)
        {
        	printf_data_net2uart("TCPServerSend: ", dataBuffer, dataBytes);
            if(send(*clientFd, dataBuffer, dataBytes, 0) == -1)
            {
            	printf_debug("send error！\r\n");
                continue;
            }
        }
    }
}


/**
 * @breif TCP服务端接口转发到串口的线程程序
 * @param param 整型数组，第一个数存放写管道号，第二个数存放网络socket描述符，第三个数存放网络client描述符的指针
 * @return void
 */
static void TCPServerReceivePthread(void *param)
{
    int socketFd = 0, pipeWriteFd = 0, *clientFd = NULL;
    int dataBytes = 0;      //接收到的字节数
    char dataBuffer[MAX_DATA_SIZE] = {0};       //接收缓存区

    pipeWriteFd = ((int*)param)[0];
    socketFd = ((int*)param)[1];
    clientFd = (int*)((int*)param)[2];

    while(1)
    {
    	dataBytes = recv(*clientFd, dataBuffer, MAX_DATA_SIZE, 0);
        if(dataBytes > 0)
        {
        	printf_data_net2uart("TCPServerReceive: ", dataBuffer, dataBytes);
            if(write(pipeWriteFd, dataBuffer, dataBytes) == -1)
            {
            	printf_debug("write error！\r\n");
                continue;
            }
        }
        else if(dataBytes == 0)			//远端断开连接
        {
        	TCP_CloseConnect(clientFd);		//关闭连接

			*clientFd = TCP_NetAccept(socketFd);		//重新连接服务端
			if(*clientFd == -1)
				return;
        }
    }
}


/**
 * @breif UDP发送的线程程序
 * @param param 整型数组，第一个数存放读管道号，第二个数存放网络socket描述符，第三个存放远端信息结构体的指针
 * @return void
 */
static void UDPSendPthread(void *param)
{
    int socketFd = 0, pipeReadFd = 0;
    char dataBuffer[MAX_DATA_SIZE] = {0};		//数据缓存区
	int dataBytes = 0;			//数据字节数
    struct sockaddr_in *remoteAddr = NULL;
    socklen_t sinSize = sizeof(struct sockaddr_in);

    pipeReadFd = ((int*)param)[0];
    socketFd = ((int*)param)[1];
    remoteAddr = (struct sockaddr_in *)((int*)param)[2];

    while(1)
	{
		/* 从管道中读取数据 */
    	dataBytes = read(pipeReadFd, dataBuffer, sizeof(dataBuffer));
    	if(dataBytes > 0)
    	{
    		printf_data_net2uart("UDPSend: ", dataBuffer, dataBytes);
			if(sendto(socketFd, dataBuffer, dataBytes, 0, (struct sockaddr *)remoteAddr, sinSize) == -1)
			{
				printf_debug("send error！\r\n");
                continue;
			}
    	}
	}
}


/**
 * @breif UDP接收的线程程序
 * @param param 整型数组，第一个数存放写管道号，第二个数存放网络socket描述符，第三个存放远端信息结构体的指针
 * @return void
 */
static void UDPReceivePthread(void *param)
{
    int socketFd = 0, pipeWriteFd = 0;
    int dataBytes = 0;		//数据字节数
    struct sockaddr_in *remoteAddr = NULL;
	char dataBuffer[MAX_DATA_SIZE] = {0};		//数据缓存区
    socklen_t sinSize = sizeof(struct sockaddr_in);

    pipeWriteFd = ((int*)param)[0];
    socketFd = ((int*)param)[1];
    remoteAddr = (struct sockaddr_in *)((int*)param)[2];

    while(1)
    {
    	dataBytes = recvfrom(socketFd, dataBuffer, sizeof(dataBuffer)-1, 0, (struct sockaddr *)remoteAddr, &sinSize);
        if(dataBytes > 0)
        {
        	printf_data_net2uart("UDPReceive: ", dataBuffer, dataBytes);
            /* 将接收到的数据写入管道 */
        	if(write(pipeWriteFd, dataBuffer, dataBytes) == -1)
        	{
        		printf_debug("write error！\r\n");
                continue;
        	}
        }
    }

}


/**
 * @breif 串口发送的线程程序
 * @param param 整型数组，第一个数存放读管道号，第二个数存放串口描述符
 * @return void
 */
static void UartSendPthread(void *param)
{
    int uartFd = 0, pipeReadFd = 0;
    int dataBytes = 0;		//数据字节数
	char dataBuffer[MAX_DATA_SIZE] = {0};		//数据缓存区

    pipeReadFd = ((int*)param)[0];
    uartFd = ((int*)param)[1];

    while(1)
    {
    	/* 从管道中读取数据 */
    	dataBytes = read(pipeReadFd, dataBuffer, sizeof(dataBuffer));
    	if(dataBytes > 0)
    	{
    		printf_data_net2uart("UartSend: ", dataBuffer, dataBytes);
			if(write(uartFd, dataBuffer, dataBytes) == -1)
			{
				printf_debug("write error！\r\n");
			}
    	}
    }
}


/**
 * @breif 串口接收的线程程序
 * @param param 整型数组，第一个数存放写管道号，第二个数存放串口描述符
 * @return void
 */
static void UartReceivePthread(void *param)
{
    int uartFd = 0, pipeWriteFd = 0;
    char dataBuffer[MAX_DATA_SIZE] = {0};		//数据缓存区
	int dataBytes = 0;			//数据字节数

    pipeWriteFd = ((int*)param)[0];
    uartFd = ((int*)param)[1];

    while(1)
	{
    	dataBytes = read(uartFd, dataBuffer, sizeof(dataBuffer));
		if(dataBytes > 0)
		{
			printf_data_net2uart("UartReceive: ", dataBuffer, dataBytes);
			/* 将接收到的数据写入管道 */
			if(write(pipeWriteFd, dataBuffer, dataBytes) == -1)
			{
				printf_debug("write error！\r\n");
                continue;
			}
		}
	}
}


/**
 * @breif 打印数据
 * @param prefixName 打印数据的前缀信息
 * @param data 数据
 * @param lenth 数据长度
 * @return void
 */
static void PrintfData(char *prefixName, char *data, int lenth)
{
	printf("%s", prefixName);
	for(int i = 0; i < lenth; i++)
	{
		printf("%02x ", data[i]);
	}
	printf("\n");
}



