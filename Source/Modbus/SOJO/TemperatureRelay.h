/**
 * @file TemperatureRelay.h
 * @brief 双杰测温接收中继程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.26
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _TEMPERATURE_RELAY_H_
#define _TEMPERATURE_RELAY_H_

#include "../../DataStruct.h"

#define TEMP_RELAY_REGISTERS_ADDRESS		0x0008		//保持寄存器地址
#define TEMP_RELAY_REGISTERS_NUMBER			256			//保持寄存器个数
#define TEMP_RELAY_SERVER_ID				1			//从机地址
#define TEMP_RELAY_MODBUS_INTERVAL			10			//噪声传感器Modbus通信时间间隔

int TemperatureRelay(UartInfo *uartInfo);

#endif

