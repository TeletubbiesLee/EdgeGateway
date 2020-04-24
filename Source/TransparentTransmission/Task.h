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

#ifndef _PTHREAD_H_
#define _PTHREAD_H_

#include "../DataStruct.h"

#define MAX_DATA_SIZE		1600		//数据最大数量


#ifdef DATA_PRINTF_NET_UART
#define printf_data_net2uart(prefixName,data,lenth) do{PrintfData(prefixName,data,lenth);}while(0)
#else
#define printf_data_net2uart(prefixName,data,lenth) do{PrintfData(NULL,NULL,0);}while(0)
#endif

int TCP_Client2Uart(UartInfo *uartInfo, NetworkInfo *networkInfo);
int TCP_Server2Uart(UartInfo *uartInfo, NetworkInfo *networkInfo);
int UDP2Uart(UartInfo *uartInfo, NetworkInfo *networkInfo);


#endif
