/**
 * @file DeviceParamConfig.c
 * @brief 设备参数的配置相关程序
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.05.28
 *
 * @author Lei.L
 * @version ver 1.0
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "DeviceParamConfig.h"


/**
 * @brief 释放指针数组中的空间
 * @param void
 * @return v
 */
void FreePointArray(void *pointArray[], int num)
{
	for(int i = 0; i < num; i++)
	{
		if(pointArray[i] != NULL)
		{
			free(pointArray[i]);
			pointArray[i] = NULL;
		}
	}
}


/**
 * @brief 透传功能参数配置
 * @param configInfo 从配置文件获取到的配置信息
 * @param processNum 进程数
 * @param username 保存数据的数据表名
 * @param uart 串口信息结构体数组
 * @param deviceNum 设备数
 * @param deviceId 设备号
 * @param sersorType 传感器类型
 * @return void
 */
void ModbusParamConfig(EdgeGatewayConfig *configInfo, int *processNum, char *username[], UartInfo *uart[], int deviceNum[], int *deviceId[], int sersorType)
{
	ModbusConfig **modbusInfo = NULL;
	ModbusConfig *noiseInfo = NULL, *airQulityInfo = NULL, *sojoRelay = NULL;

	switch(sersorType)
	{
	case NOISE_SERSOR:
		modbusInfo = &noiseInfo;
		*processNum = configInfo->noiseNumber;
		break;
	case AIR_QUALITY_SERSOR:
		modbusInfo = &airQulityInfo;
		*processNum = configInfo->airNumber;
		break;
	case SOJO_RELAY:
		modbusInfo = &sojoRelay;
		*processNum = configInfo->tempNumber;
		break;
	}

	for(int i = 0; i < *processNum; i++)
	{
		switch(i)
		{
		case 0:
			noiseInfo = &configInfo->noiseSersor1;
			airQulityInfo = &configInfo->airSersor1;
			sojoRelay = &configInfo->tempSersor1;
			break;
		case 1:
			noiseInfo = &configInfo->noiseSersor2;
			airQulityInfo = &configInfo->airSersor2;
			sojoRelay = &configInfo->tempSersor2;
			break;
		case 2:
			noiseInfo = &configInfo->noiseSersor3;
			airQulityInfo = &configInfo->airSersor3;
			sojoRelay = &configInfo->tempSersor3;
			break;
		case 3:
			noiseInfo = &configInfo->noiseSersor4;
			airQulityInfo = &configInfo->airSersor4;
			sojoRelay = &configInfo->tempSersor4;
			break;
		case 4:
			noiseInfo = &configInfo->noiseSersor5;
			airQulityInfo = &configInfo->airSersor5;
			sojoRelay = &configInfo->tempSersor5;
			break;
		}

		strcpy(username[i], (*modbusInfo)->dataFilename);
		uart[i]->bandrate = (*modbusInfo)->bandrate;
		uart[i]->uartType = (*modbusInfo)->uartType;
		strcpy(uart[i]->uartName, (*modbusInfo)->uartName);
		deviceNum[i] = (*modbusInfo)->slaveNumber;
		for(int j = 0; j < deviceNum[i]; j++)
		{
			deviceId[i][j] = (*modbusInfo)->slaveID[j];
		}
	}
}


/**
 * @brief 透传功能参数配置
 * @param configInfo 从配置文件获取到的配置信息
 * @param processNum 进程数
 * @param username 保存数据的数据表名
 * @return void
 */
void MqttParamConfig(EdgeGatewayConfig *configInfo, int *processNum, char *username[])
{
	MqttConfig *mqttInfo = NULL;
	char *tempString = NULL;

	mqttInfo = &configInfo->mqttAccess;
	*processNum = mqttInfo->mqttNumber;
	for(int i = 0; i < *processNum; i++)
	{
		switch(i)
		{
		case 0:
			tempString = mqttInfo->username1;
			break;
		case 1:
			tempString = mqttInfo->username2;
			break;
		case 2:
			tempString = mqttInfo->username3;
			break;
		case 3:
			tempString = mqttInfo->username4;
			break;
		case 4:
			tempString = mqttInfo->username5;
			break;
		}
		strcpy(username[i], tempString);
	}
}


/**
 * @brief 101协议参数配置
 * @param configInfo 从配置文件获取到的配置信息
 * @param info101 101参数结构体指针
 * @return void
 */
void IEC101ParamConfig(EdgeGatewayConfig *configInfo, Configure101 *info101, char *filename)
{
	info101->num = configInfo->iec101.slaveNumber;

	if(strcmp("/dev/ttymxc2", configInfo->iec101.uartName) == 0)
		info101->portNo = 1;
	else if(strcmp("/dev/ttymxc3", configInfo->iec101.uartName) == 0)
		info101->portNo = 2;
	else if(strcmp("/dev/ttymxc4", configInfo->iec101.uartName) == 0)
		info101->portNo = 3;

	if(2400 == configInfo->iec101.bandrate)
		info101->baudRate = 0;
	else if(4800 == configInfo->iec101.bandrate)
		info101->baudRate = 1;
	else if(9600 == configInfo->iec101.bandrate)
		info101->baudRate = 2;
	else if(38400 == configInfo->iec101.bandrate)
		info101->baudRate = 3;
	else if(115200 == configInfo->iec101.bandrate)
		info101->baudRate = 4;

	info101->parity = configInfo->iec101.parity;
	info101->balanMode = configInfo->iec101.balanMode;
	info101->sourceAddr = configInfo->iec101.sourceAddr;
	info101->linkAddrSize = configInfo->iec101.linkAddrSize;
	info101->ASDUCotSize = configInfo->iec101.ASDUCotSize;
	info101->ASDUAddr = configInfo->iec101.ASDUAddr;
	info101->ASDUAddrSize = configInfo->iec101.ASDUAddrSize;
	info101->InfoAddrSize = configInfo->iec101.infoAddrSize;

	for(int i = 0; i < info101->num; i++)
	{
		info101->sModuleId[i] = configInfo->iec101.sModuleId[i];
		info101->sMsourceAddr[i] = configInfo->iec101.sModuleId[i];
	}
	info101->sMstate = 1;
	info101->sMportNo = info101->portNo;

	strcpy(filename, configInfo->iec101.dataFilename);
}


/**
 * @brief 104协议参数配置
 * @param configInfo 从配置文件获取到的配置信息
 * @param info101 101参数结构体指针
 * @return void
 */
void IEC104ParamConfig(EdgeGatewayConfig *configInfo, Configure104 *info104, char *filename)
{
	info104->num = configInfo->iec104.slaveNumber;
	strcpy(info104->ip, configInfo->iec104.localIP);
	info104->balanMode = 1;
	info104->sourceAddr = configInfo->iec104.sourceAddr;
	info104->linkAddrSize = configInfo->iec104.linkAddrSize;
	info104->ASDUCotSize = configInfo->iec104.ASDUCotSize;
	info104->ASDUAddr = configInfo->iec104.ASDUAddr;
	info104->ASDUAddrSize = configInfo->iec104.ASDUAddrSize;
	info104->InfoAddrSize = configInfo->iec104.infoAddrSize;

	for(int i = 0; i < info104->num; i++)
	{
		info104->sModuleId[i] = configInfo->iec104.sModuleId[i];
		info104->sMsourceAddr[i] = configInfo->iec104.sModuleId[i];
	}
	info104->sMstate = 1;
	info104->sMnetEn = 1;
	strcpy(info104->sMip[0], configInfo->iec104.sMip1);
	strcpy(info104->sMip[1], configInfo->iec104.sMip2);
	strcpy(info104->sMip[2], configInfo->iec104.sMip3);
	strcpy(info104->sMip[3], configInfo->iec104.sMip4);
	strcpy(info104->sMip[4], configInfo->iec104.sMip5);

	strcpy(filename, configInfo->iec104.dataFilename);
}


