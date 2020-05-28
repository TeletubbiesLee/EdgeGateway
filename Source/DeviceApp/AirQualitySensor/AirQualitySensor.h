/**
 * @file AirQualitySensor.h
 * @brief 六合一空气质量传感器通信程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.24
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _AIR_QUALITY_SENSOR_H_
#define _AIR_QUALITY_SENSOR_H_

#include "../../DataStruct.h"


#define AIR_QUALITY_REGISTERS_ADDRESS		0x00		//保持寄存器地址
#define AIR_QUALITY_REGISTERS_NUMBER		7			//保持寄存器个数
#define AIR_QUALITY_MODBUS_INTERVAL			60			//噪声传感器Modbus通信时间间隔

int CreatAirQualityProcess(void);
int AirQualitySensor(UartInfo *uartInfo, int deviceId[], int deviceNum, char *filename);

#endif
