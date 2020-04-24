/**
 * @file ModbusInit.h
 * @brief modbus初始化相关程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.24
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _MODBUS_INIT_H_
#define _MODBUS_INIT_H_

#include "libmodbus/modbus.h"
#include "../DataStruct.h"

#define UART_PARITY         			'N'			//串口数据奇偶校验位
#define UART_DATA_BIT       			8			//串口数据位
#define UART_STOP_BIT       			1			//串口停止位


int ModbusInit(modbus_t **ctx, UartInfo *uartInfo);

#endif
