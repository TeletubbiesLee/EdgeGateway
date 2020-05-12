/**
 * @file NoiseSensor.h
 * @brief 噪声传感器通信程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.20
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _NOISE_SERSOR_H_
#define _NOISE_SERSOR_H_

#include "../../DataStruct.h"

#define NOISE_REGISTERS_ADDRESS			0x00		//保持寄存器地址
#define NOISE_REGISTERS_NUMBER			1			//保持寄存器个数
#define NOISE_MODBUS_INTERVAL			60			//噪声传感器Modbus通信时间间隔


int NoiseSensor(UartInfo *uartInfo, int deviceId[], int deviceNum, char *filename);

#endif

