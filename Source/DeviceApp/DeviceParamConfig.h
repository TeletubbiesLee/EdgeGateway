/**
 * @file DeviceParamConfig.h
 * @brief 设备参数的配置相关程序
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.05.28
 *
 * @author Lei.L
 * @version ver 1.0
 */


#ifndef _DEVICE_PARAM_CONFIG_H_
#define _DEVICE_PARAM_CONFIG_H_

#include "../ParserConfig/Interface_S2J.h"
#include "../IEC60870/iec60870_configure.h"
#include "../DataStruct.h"

void FreePointArray(void *pointArray[], int num);
void ModbusParamConfig(EdgeGatewayConfig *configInfo, int *processNum, char *username[], UartInfo *uart[], int deviceNum[], int *deviceId[], int sersorType);
void MqttParamConfig(EdgeGatewayConfig *configInfo, int *processNum, char *username[]);
void IEC101ParamConfig(EdgeGatewayConfig *configInfo, Configure101 *info101, char *filename);
void IEC104ParamConfig(EdgeGatewayConfig *configInfo, Configure104 *info104, char *filename);

#endif
