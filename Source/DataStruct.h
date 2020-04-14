/**
 * @file DataStruct.h
 * @brief 数据结构相关的头文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.14
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _DATA_STRUCT_H_
#define _DATA_STRUCT_H_


/*
 * 透传功能类型枚举
 */
typedef enum TagUartToNetworkType
{
	UDP_TO_UART = 0,
	TCP_SERVER_TO_UART,
	TCP_CLIENT_TO_UART
}UartToNetworkType;


/*
 * 串口的电气接口类型枚举
 */
typedef enum TagSerialInterface
{
	RS232_TYPE = 0,
	RS485_TYPE
}SerialInterface;


/*
 * 串口信息结构体
 */
typedef struct TagUartInfo
{
	char uartName[20];
	int bandrate;
	SerialInterface uartType;
}UartInfo;


/*
 * 网口的信息结构体
 */
typedef struct TagNetworkInfo
{
	char localAddress[20];
	int localPort;
	char remoteAddress[20];
	int remotePort;
}NetworkInfo;

#endif

