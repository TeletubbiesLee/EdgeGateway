/**
 * @file SaveConfig.h
 * @brief 保存配置信息的对外接口程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.05.21
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _SAVE_CONFIG_H_
#define _SAVE_CONFIG_H_


#include "Interface_S2J.h"

typedef struct TagUart2NetInfo
{
	int uartToNetNumber;
	UartToNetConfig uartToNet[5];
}Uart2NetInfo;


typedef struct TagModbusInfo
{
	int sersorNumber;
	ModbusConfig sersor[5];
}ModbusInfo;

void GetConfig(void);
void SaveUart2NetConfig(Uart2NetInfo *uart2NetInfo);
void SaveNoiseConfig(ModbusInfo *noiseInfo);
void SaveAirConfig(ModbusInfo *airInfo);
void SaveTempConfig(ModbusInfo *tempInfo);
void SaveIec101Config(IEC101Config *iec101);
void SaveIec104Config(IEC104Config *iec104);
void SaveMqttConfig(MqttConfig *mqttAccess);

#endif


