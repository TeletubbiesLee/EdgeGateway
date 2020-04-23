/**
 * @file Uart.h
 * @brief Linux系统中串口设备相关操作函数文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.02.17
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _UART_H_
#define _UART_H_

#define UART_DATA_BITS_NUM		8		//串口传输数据位
#define UART_STOP_BITS_NUM		1		//串口传输停止位
#define UART_PARITY_NUM			'N'		//串口传输校验位

int UartInit(char *device, int bandrate, int type);
int OpenDevice(char *dev);
int SetPort(int fd, int nSpeed, int nBits, char nEvent, int nStop);
void PrintUartUsage();

#endif
