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
 * 串口的电气接口类型枚举
 */
typedef enum TagSerialInterface
{
	RS232_TYPE = 1,
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


/*
 * Modbus传感器类型
 */
typedef enum TagSersorType
{
	NOISE_SERSOR = 1,
	AIR_QUALITY_SERSOR,
	SOJO_RELAY,
}SersorType;


#endif

