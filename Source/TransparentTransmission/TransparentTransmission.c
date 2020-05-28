/**
 * @file TransparentTransmission.c
 * @brief 透传功能程序对外接口程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.14
 *
 * @author Lei.L
 * @version ver 1.0
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "TransparentTransmission.h"
#include "../ProcessCommunication/ProcessSignal.h"
#include "../Config.h"
#include "../DeviceApp/DeviceParamConfig.h"


/**
 * @brief 创建透传进程
 * @param void
 * @return 成功：0 失败：其他
 */
int CreatNet2UartProcess(void)
{
	if(g_EdgeGatewayConfig == NULL)
	{
		printf_debug("g_EdgeGatewayConfig is NULL!\n");
		return POINT_NULL;
	}

	/* 透传功能需要的配置信息 */
	bool isError = true;
	pid_t pid = 0;
	int trsptTrsmsProcessNum = 0;			//透传功能进程数
	int trsptTrsmsType[PROTOCOL_MAX_PROCESS] = {0};
	UartInfo *trsptTrsmsUart[PROTOCOL_MAX_PROCESS] = {0};
	NetworkInfo *trsptTrsmsEth[PROTOCOL_MAX_PROCESS] = {0};
	for(int i = 0; i < PROTOCOL_MAX_PROCESS; i++)
	{
		if((trsptTrsmsUart[i] = malloc(sizeof(UartInfo))) == NULL)
		{
			printf_debug("trsptTrsmsUart[%d] malloc error\n", i);
			isError = false;
			break;
		}
		if((trsptTrsmsEth[i] = malloc(sizeof(NetworkInfo))) == NULL)
		{
			printf_debug("trsptTrsmsEth[%d] malloc error\n", i);
			isError = false;
			break;
		}
	}

	if(false == isError)
	{
		FreePointArray((void**)trsptTrsmsUart, PROTOCOL_MAX_PROCESS);
		FreePointArray((void**)trsptTrsmsEth, PROTOCOL_MAX_PROCESS);
		return MALLOC_FAIL;
	}

	TrsptTrsmsParamConfig(g_EdgeGatewayConfig, &trsptTrsmsProcessNum, trsptTrsmsType, trsptTrsmsUart, trsptTrsmsEth);
	/* 创建透传功能进程 */
	for(int i = 0; i < trsptTrsmsProcessNum; i++)
	{
		if((pid = fork()) == 0)
		{
			SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

			printf("TransparentTransmission (pid:%d) creat\n", getpid());
			TransparentTransmission(trsptTrsmsType[i], trsptTrsmsUart[i], trsptTrsmsEth[i]);		//透传功能
			printf("TransparentTransmission (pid:%d) exit\n", getpid());

			return NO_ERROR;
		}
	}

	FreePointArray((void**)trsptTrsmsUart, PROTOCOL_MAX_PROCESS);
	FreePointArray((void**)trsptTrsmsEth, PROTOCOL_MAX_PROCESS);
	return NO_ERROR;
}


/**
 * @brief 透传功能参数配置
 * @param configInfo 从配置文件获取到的配置信息
 * @param processNum 进程数
 * @param type 协议类型数组
 * @param uart 串口信息结构体数组
 * @param eth 网口信息结构体数组
 * @return void
 */
void TrsptTrsmsParamConfig(EdgeGatewayConfig *configInfo, int *processNum, int type[], UartInfo *uart[], NetworkInfo *eth[])
{
	UartToNetConfig *uart2Net = NULL;

	*processNum = configInfo->uartToNetNumber;

	for(int i = 0; i < *processNum; i++)
	{
		switch(i)
		{
			case 0:
				uart2Net = &configInfo->uartToNet1;
				break;
			case 1:
				uart2Net = &configInfo->uartToNet2;
				break;
			case 2:
				uart2Net = &configInfo->uartToNet3;
				break;
			case 3:
				uart2Net = &configInfo->uartToNet4;
				break;
			case 4:
				uart2Net = &configInfo->uartToNet5;
				break;
		}
		type[i] = uart2Net->ProtocolType;
		strcpy(uart[i]->uartName, uart2Net->uartName);
		uart[i]->bandrate = uart2Net->bandrate;
		uart[i]->uartType = uart2Net->uartType;
		strcpy(eth[i]->localAddress, uart2Net->localIP);
		eth[i]->localPort = uart2Net->localPort;
		strcpy(eth[i]->remoteAddress, uart2Net->remoteIP);
		eth[i]->remotePort = uart2Net->remotePort;
	}
}


/**
 * @breif 透传功能
 * @param type 透传功能的类型：UDP_TO_UART TCP_CLIENT_TO_UART TCP_SERVER_TO_UART
 * @param uartInfo 串口信息结构体指针
 * @param networkInfo 网口信息结构体指针
 * @return 成功0或失败
 */
int TransparentTransmission(int type, UartInfo *uartInfo, NetworkInfo *networkInfo)
{
	if(uartInfo == NULL || networkInfo == NULL)
	{
		printf_debug("error: arguments is NULL!\n");
		return POINT_NULL;
	}

	if(type == UDP_TO_UART)
	{
		UDP2Uart(uartInfo, networkInfo);
	}
	else if(type == TCP_SERVER_TO_UART)
	{
		TCP_Client2Uart(uartInfo, networkInfo);		//设备外接TCP服务器端和串口，嵌入式设备对应内部程序为TCP客户端转串口
	}
	else if(type == TCP_CLIENT_TO_UART)
	{
		TCP_Server2Uart(uartInfo, networkInfo);		//设备外接TCP客户端和串口，嵌入式设备对应内部程序为TCP服务器端转串口
	}
	else
	{
		printf_debug("error: arguments \'%d\' is wrong to program!\n", type);
		return ERROR_ARGUMENTS;
	}
	return NO_ERROR;
}
