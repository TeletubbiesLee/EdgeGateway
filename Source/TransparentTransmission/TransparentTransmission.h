/**
 * @file TransparentTransmission.h
 * @brief 透传功能程序对外接口程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.14
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _TRANSPARENT_TRANSMISSION_H_
#define _TRANSPARENT_TRANSMISSION_H_

#include "Task.h"
#include "../ParserConfig/Interface_S2J.h"

/*
 * 透传功能类型枚举
 */
typedef enum TagUartToNetworkType
{
	NO_UART_NET_TYPE = 0,
	UDP_TO_UART,
	TCP_SERVER_TO_UART,
	TCP_CLIENT_TO_UART
}UartToNetworkType;


int CreatNet2UartProcess(void);
void TrsptTrsmsParamConfig(EdgeGatewayConfig *configInfo, int *processNum, int type[], UartInfo *uart[], NetworkInfo *eth[]);
int TransparentTransmission(int type, UartInfo *uartInfo, NetworkInfo *networkInfo);

#endif
