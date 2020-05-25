/**
 * @file SaveConfig.c
 * @brief 保存配置信息的对外接口程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.05.21
 *
 * @author Lei.L
 * @version ver 1.0
 */

#include <stdio.h>
#include <string.h>
#include "SaveConfig.h"

/**
 * @fn GetConfig
 * @brief 获取配置信息的函数，调用之后，可以直接从g_EdgeGatewayConfig指针中获取数据
 * @param void
 * @return void
 */
void GetConfig(void)
{
	GetJsonFile(JSON_CONFIG_FILENAME, &g_EdgeGatewayConfig);
}


/**
 * @fn StructFree
 * @brief 释放g_EdgeGatewayConfig分配的内存空间
 * @param void
 * @return void
 */
void StructFree(void)
{
	S2J_StructFree(g_EdgeGatewayConfig);
}


/**
 * @fn SaveUart2NetConfig
 * @brief 保存串口转网口的透传功能的配置信息到文件
 * @param uart2NetInfo 透传功能的配置信息的指针
 * @return void
 */
void SaveUart2NetConfig(Uart2NetInfo *uart2NetInfo)
{
	if(g_EdgeGatewayConfig == NULL)
	{
		printf("g_EdgeGatewayConfig is NULL, please call GetConfig() before\n");
		return;
	}

	g_EdgeGatewayConfig->uartToNetNumber = uart2NetInfo->uartToNetNumber;
	for(int i = 0; i < uart2NetInfo->uartToNetNumber; i++)
	{
		switch(i)
		{
		case 0:
			g_EdgeGatewayConfig->uartToNet1.ProtocolType = uart2NetInfo->uartToNet[i].ProtocolType;
			strcpy(g_EdgeGatewayConfig->uartToNet1.uartName, uart2NetInfo->uartToNet[i].uartName);
			g_EdgeGatewayConfig->uartToNet1.bandrate = uart2NetInfo->uartToNet[i].bandrate;
			g_EdgeGatewayConfig->uartToNet1.uartType = uart2NetInfo->uartToNet[i].uartType;
			strcpy(g_EdgeGatewayConfig->uartToNet1.localIP, uart2NetInfo->uartToNet[i].localIP);
			g_EdgeGatewayConfig->uartToNet1.localPort = uart2NetInfo->uartToNet[i].localPort;
			strcpy(g_EdgeGatewayConfig->uartToNet1.remoteIP, uart2NetInfo->uartToNet[i].remoteIP);
			g_EdgeGatewayConfig->uartToNet1.remotePort = uart2NetInfo->uartToNet[i].remotePort;
			break;
		case 1:
			g_EdgeGatewayConfig->uartToNet2.ProtocolType = uart2NetInfo->uartToNet[i].ProtocolType;
			strcpy(g_EdgeGatewayConfig->uartToNet2.uartName, uart2NetInfo->uartToNet[i].uartName);
			g_EdgeGatewayConfig->uartToNet2.bandrate = uart2NetInfo->uartToNet[i].bandrate;
			g_EdgeGatewayConfig->uartToNet2.uartType = uart2NetInfo->uartToNet[i].uartType;
			strcpy(g_EdgeGatewayConfig->uartToNet2.localIP, uart2NetInfo->uartToNet[i].localIP);
			g_EdgeGatewayConfig->uartToNet2.localPort = uart2NetInfo->uartToNet[i].localPort;
			strcpy(g_EdgeGatewayConfig->uartToNet2.remoteIP, uart2NetInfo->uartToNet[i].remoteIP);
			g_EdgeGatewayConfig->uartToNet2.remotePort = uart2NetInfo->uartToNet[i].remotePort;
			break;
		case 2:
			g_EdgeGatewayConfig->uartToNet3.ProtocolType = uart2NetInfo->uartToNet[i].ProtocolType;
			strcpy(g_EdgeGatewayConfig->uartToNet3.uartName, uart2NetInfo->uartToNet[i].uartName);
			g_EdgeGatewayConfig->uartToNet3.bandrate = uart2NetInfo->uartToNet[i].bandrate;
			g_EdgeGatewayConfig->uartToNet3.uartType = uart2NetInfo->uartToNet[i].uartType;
			strcpy(g_EdgeGatewayConfig->uartToNet3.localIP, uart2NetInfo->uartToNet[i].localIP);
			g_EdgeGatewayConfig->uartToNet3.localPort = uart2NetInfo->uartToNet[i].localPort;
			strcpy(g_EdgeGatewayConfig->uartToNet3.remoteIP, uart2NetInfo->uartToNet[i].remoteIP);
			g_EdgeGatewayConfig->uartToNet3.remotePort = uart2NetInfo->uartToNet[i].remotePort;
			break;
		case 3:
			g_EdgeGatewayConfig->uartToNet4.ProtocolType = uart2NetInfo->uartToNet[i].ProtocolType;
			strcpy(g_EdgeGatewayConfig->uartToNet4.uartName, uart2NetInfo->uartToNet[i].uartName);
			g_EdgeGatewayConfig->uartToNet4.bandrate = uart2NetInfo->uartToNet[i].bandrate;
			g_EdgeGatewayConfig->uartToNet4.uartType = uart2NetInfo->uartToNet[i].uartType;
			strcpy(g_EdgeGatewayConfig->uartToNet4.localIP, uart2NetInfo->uartToNet[i].localIP);
			g_EdgeGatewayConfig->uartToNet4.localPort = uart2NetInfo->uartToNet[i].localPort;
			strcpy(g_EdgeGatewayConfig->uartToNet4.remoteIP, uart2NetInfo->uartToNet[i].remoteIP);
			g_EdgeGatewayConfig->uartToNet4.remotePort = uart2NetInfo->uartToNet[i].remotePort;
			break;
		case 4:
			g_EdgeGatewayConfig->uartToNet5.ProtocolType = uart2NetInfo->uartToNet[i].ProtocolType;
			strcpy(g_EdgeGatewayConfig->uartToNet5.uartName, uart2NetInfo->uartToNet[i].uartName);
			g_EdgeGatewayConfig->uartToNet5.bandrate = uart2NetInfo->uartToNet[i].bandrate;
			g_EdgeGatewayConfig->uartToNet5.uartType = uart2NetInfo->uartToNet[i].uartType;
			strcpy(g_EdgeGatewayConfig->uartToNet5.localIP, uart2NetInfo->uartToNet[i].localIP);
			g_EdgeGatewayConfig->uartToNet5.localPort = uart2NetInfo->uartToNet[i].localPort;
			strcpy(g_EdgeGatewayConfig->uartToNet5.remoteIP, uart2NetInfo->uartToNet[i].remoteIP);
			g_EdgeGatewayConfig->uartToNet5.remotePort = uart2NetInfo->uartToNet[i].remotePort;
			break;
		}
	}

	CreateJsonFile(JSON_CONFIG_FILENAME, g_EdgeGatewayConfig);
}


/**
 * @fn SaveNoiseConfig
 * @brief 保存噪声传感器的配置信息到文件
 * @param noiseInfo 噪声传感器的配置信息的指针
 * @return void
 */
void SaveNoiseConfig(ModbusInfo *noiseInfo)
{
	if(g_EdgeGatewayConfig == NULL)
	{
		printf("g_EdgeGatewayConfig is NULL, please call GetConfig() before\n");
		return;
	}

	g_EdgeGatewayConfig->noiseNumber = noiseInfo->sersorNumber;
	for(int i = 0; i < noiseInfo->sersorNumber; i++)
	{
		switch(i)
		{
		case 0:
			strcpy(g_EdgeGatewayConfig->noiseSersor1.uartName, noiseInfo->sersor[i].uartName);
			g_EdgeGatewayConfig->noiseSersor1.bandrate = noiseInfo->sersor[i].bandrate;
			g_EdgeGatewayConfig->noiseSersor1.uartType = noiseInfo->sersor[i].uartType;
			g_EdgeGatewayConfig->noiseSersor1.slaveNumber = noiseInfo->sersor[i].slaveNumber;
			for(int j = 0; j < noiseInfo->sersor[i].slaveNumber; j++)
			{
				g_EdgeGatewayConfig->noiseSersor1.slaveID[j] = noiseInfo->sersor[i].slaveID[j];
			}
			strcpy(g_EdgeGatewayConfig->noiseSersor1.dataFilename, noiseInfo->sersor[i].dataFilename);
			break;
		case 1:
			strcpy(g_EdgeGatewayConfig->noiseSersor2.uartName, noiseInfo->sersor[i].uartName);
			g_EdgeGatewayConfig->noiseSersor2.bandrate = noiseInfo->sersor[i].bandrate;
			g_EdgeGatewayConfig->noiseSersor2.uartType = noiseInfo->sersor[i].uartType;
			g_EdgeGatewayConfig->noiseSersor2.slaveNumber = noiseInfo->sersor[i].slaveNumber;
			for(int j = 0; j < noiseInfo->sersor[i].slaveNumber; j++)
			{
				g_EdgeGatewayConfig->noiseSersor2.slaveID[j] = noiseInfo->sersor[i].slaveID[j];
			}
			strcpy(g_EdgeGatewayConfig->noiseSersor2.dataFilename, noiseInfo->sersor[i].dataFilename);
			break;
		case 2:
			strcpy(g_EdgeGatewayConfig->noiseSersor3.uartName, noiseInfo->sersor[i].uartName);
			g_EdgeGatewayConfig->noiseSersor3.bandrate = noiseInfo->sersor[i].bandrate;
			g_EdgeGatewayConfig->noiseSersor3.uartType = noiseInfo->sersor[i].uartType;
			g_EdgeGatewayConfig->noiseSersor3.slaveNumber = noiseInfo->sersor[i].slaveNumber;
			for(int j = 0; j < noiseInfo->sersor[i].slaveNumber; j++)
			{
				g_EdgeGatewayConfig->noiseSersor3.slaveID[j] = noiseInfo->sersor[i].slaveID[j];
			}
			strcpy(g_EdgeGatewayConfig->noiseSersor3.dataFilename, noiseInfo->sersor[i].dataFilename);
			break;
		case 3:
			strcpy(g_EdgeGatewayConfig->noiseSersor4.uartName, noiseInfo->sersor[i].uartName);
			g_EdgeGatewayConfig->noiseSersor4.bandrate = noiseInfo->sersor[i].bandrate;
			g_EdgeGatewayConfig->noiseSersor4.uartType = noiseInfo->sersor[i].uartType;
			g_EdgeGatewayConfig->noiseSersor4.slaveNumber = noiseInfo->sersor[i].slaveNumber;
			for(int j = 0; j < noiseInfo->sersor[i].slaveNumber; j++)
			{
				g_EdgeGatewayConfig->noiseSersor4.slaveID[j] = noiseInfo->sersor[i].slaveID[j];
			}
			strcpy(g_EdgeGatewayConfig->noiseSersor4.dataFilename, noiseInfo->sersor[i].dataFilename);
			break;
		case 4:
			strcpy(g_EdgeGatewayConfig->noiseSersor5.uartName, noiseInfo->sersor[i].uartName);
			g_EdgeGatewayConfig->noiseSersor5.bandrate = noiseInfo->sersor[i].bandrate;
			g_EdgeGatewayConfig->noiseSersor5.uartType = noiseInfo->sersor[i].uartType;
			g_EdgeGatewayConfig->noiseSersor5.slaveNumber = noiseInfo->sersor[i].slaveNumber;
			for(int j = 0; j < noiseInfo->sersor[i].slaveNumber; j++)
			{
				g_EdgeGatewayConfig->noiseSersor5.slaveID[j] = noiseInfo->sersor[i].slaveID[j];
			}
			strcpy(g_EdgeGatewayConfig->noiseSersor5.dataFilename, noiseInfo->sersor[i].dataFilename);
			break;
		}
	}

	CreateJsonFile(JSON_CONFIG_FILENAME, g_EdgeGatewayConfig);
}


/**
 * @fn SaveAirConfig
 * @brief 保存空气质量传感器的配置信息到文件
 * @param airInfo 空气质量传感器的配置信息的指针
 * @return void
 */
void SaveAirConfig(ModbusInfo *airInfo)
{
	if(g_EdgeGatewayConfig == NULL)
	{
		printf("g_EdgeGatewayConfig is NULL, please call GetConfig() before\n");
		return;
	}

	g_EdgeGatewayConfig->airNumber = airInfo->sersorNumber;
	for(int i = 0; i < airInfo->sersorNumber; i++)
	{
		switch(i)
		{
		case 0:
			strcpy(g_EdgeGatewayConfig->airSersor1.uartName, airInfo->sersor[i].uartName);
			g_EdgeGatewayConfig->airSersor1.bandrate = airInfo->sersor[i].bandrate;
			g_EdgeGatewayConfig->airSersor1.uartType = airInfo->sersor[i].uartType;
			g_EdgeGatewayConfig->airSersor1.slaveNumber = airInfo->sersor[i].slaveNumber;
			for(int j = 0; j < airInfo->sersor[i].slaveNumber; j++)
			{
				g_EdgeGatewayConfig->airSersor1.slaveID[j] = airInfo->sersor[i].slaveID[j];
			}
			strcpy(g_EdgeGatewayConfig->airSersor1.dataFilename, airInfo->sersor[i].dataFilename);
			break;
		case 1:
			strcpy(g_EdgeGatewayConfig->airSersor2.uartName, airInfo->sersor[i].uartName);
			g_EdgeGatewayConfig->airSersor2.bandrate = airInfo->sersor[i].bandrate;
			g_EdgeGatewayConfig->airSersor2.uartType = airInfo->sersor[i].uartType;
			g_EdgeGatewayConfig->airSersor2.slaveNumber = airInfo->sersor[i].slaveNumber;
			for(int j = 0; j < airInfo->sersor[i].slaveNumber; j++)
			{
				g_EdgeGatewayConfig->airSersor2.slaveID[j] = airInfo->sersor[i].slaveID[j];
			}
			strcpy(g_EdgeGatewayConfig->airSersor2.dataFilename, airInfo->sersor[i].dataFilename);
			break;
		case 2:
			strcpy(g_EdgeGatewayConfig->airSersor3.uartName, airInfo->sersor[i].uartName);
			g_EdgeGatewayConfig->airSersor3.bandrate = airInfo->sersor[i].bandrate;
			g_EdgeGatewayConfig->airSersor3.uartType = airInfo->sersor[i].uartType;
			g_EdgeGatewayConfig->airSersor3.slaveNumber = airInfo->sersor[i].slaveNumber;
			for(int j = 0; j < airInfo->sersor[i].slaveNumber; j++)
			{
				g_EdgeGatewayConfig->airSersor3.slaveID[j] = airInfo->sersor[i].slaveID[j];
			}
			strcpy(g_EdgeGatewayConfig->airSersor3.dataFilename, airInfo->sersor[i].dataFilename);
			break;
		case 3:
			strcpy(g_EdgeGatewayConfig->airSersor4.uartName, airInfo->sersor[i].uartName);
			g_EdgeGatewayConfig->airSersor4.bandrate = airInfo->sersor[i].bandrate;
			g_EdgeGatewayConfig->airSersor4.uartType = airInfo->sersor[i].uartType;
			g_EdgeGatewayConfig->airSersor4.slaveNumber = airInfo->sersor[i].slaveNumber;
			for(int j = 0; j < airInfo->sersor[i].slaveNumber; j++)
			{
				g_EdgeGatewayConfig->airSersor4.slaveID[j] = airInfo->sersor[i].slaveID[j];
			}
			strcpy(g_EdgeGatewayConfig->airSersor4.dataFilename, airInfo->sersor[i].dataFilename);
			break;
		case 4:
			strcpy(g_EdgeGatewayConfig->airSersor5.uartName, airInfo->sersor[i].uartName);
			g_EdgeGatewayConfig->airSersor5.bandrate = airInfo->sersor[i].bandrate;
			g_EdgeGatewayConfig->airSersor5.uartType = airInfo->sersor[i].uartType;
			g_EdgeGatewayConfig->airSersor5.slaveNumber = airInfo->sersor[i].slaveNumber;
			for(int j = 0; j < airInfo->sersor[i].slaveNumber; j++)
			{
				g_EdgeGatewayConfig->airSersor5.slaveID[j] = airInfo->sersor[i].slaveID[j];
			}
			strcpy(g_EdgeGatewayConfig->airSersor5.dataFilename, airInfo->sersor[i].dataFilename);
			break;
		}
	}

	CreateJsonFile(JSON_CONFIG_FILENAME, g_EdgeGatewayConfig);
}


/**
 * @fn SaveTempConfig
 * @brief 保存测温中继配置信息到文件
 * @param tempInfo 测温中继配置信息的指针
 * @return void
 */
void SaveTempConfig(ModbusInfo *tempInfo)
{
	if(g_EdgeGatewayConfig == NULL)
	{
		printf("g_EdgeGatewayConfig is NULL, please call GetConfig() before\n");
		return;
	}

	g_EdgeGatewayConfig->tempNumber = tempInfo->sersorNumber;
	for(int i = 0; i < tempInfo->sersorNumber; i++)
	{
		switch(i)
		{
		case 0:
			strcpy(g_EdgeGatewayConfig->tempSersor1.uartName, tempInfo->sersor[i].uartName);
			g_EdgeGatewayConfig->tempSersor1.bandrate = tempInfo->sersor[i].bandrate;
			g_EdgeGatewayConfig->tempSersor1.uartType = tempInfo->sersor[i].uartType;
			g_EdgeGatewayConfig->tempSersor1.slaveNumber = tempInfo->sersor[i].slaveNumber;
			for(int j = 0; j < tempInfo->sersor[i].slaveNumber; j++)
			{
				g_EdgeGatewayConfig->tempSersor1.slaveID[j] = tempInfo->sersor[i].slaveID[j];
			}
			strcpy(g_EdgeGatewayConfig->tempSersor1.dataFilename, tempInfo->sersor[i].dataFilename);
			break;
		case 1:
			strcpy(g_EdgeGatewayConfig->tempSersor2.uartName, tempInfo->sersor[i].uartName);
			g_EdgeGatewayConfig->tempSersor2.bandrate = tempInfo->sersor[i].bandrate;
			g_EdgeGatewayConfig->tempSersor2.uartType = tempInfo->sersor[i].uartType;
			g_EdgeGatewayConfig->tempSersor2.slaveNumber = tempInfo->sersor[i].slaveNumber;
			for(int j = 0; j < tempInfo->sersor[i].slaveNumber; j++)
			{
				g_EdgeGatewayConfig->tempSersor2.slaveID[j] = tempInfo->sersor[i].slaveID[j];
			}
			strcpy(g_EdgeGatewayConfig->tempSersor2.dataFilename, tempInfo->sersor[i].dataFilename);
			break;
		case 2:
			strcpy(g_EdgeGatewayConfig->tempSersor3.uartName, tempInfo->sersor[i].uartName);
			g_EdgeGatewayConfig->tempSersor3.bandrate = tempInfo->sersor[i].bandrate;
			g_EdgeGatewayConfig->tempSersor3.uartType = tempInfo->sersor[i].uartType;
			g_EdgeGatewayConfig->tempSersor3.slaveNumber = tempInfo->sersor[i].slaveNumber;
			for(int j = 0; j < tempInfo->sersor[i].slaveNumber; j++)
			{
				g_EdgeGatewayConfig->tempSersor3.slaveID[j] = tempInfo->sersor[i].slaveID[j];
			}
			strcpy(g_EdgeGatewayConfig->tempSersor3.dataFilename, tempInfo->sersor[i].dataFilename);
			break;
		case 3:
			strcpy(g_EdgeGatewayConfig->tempSersor4.uartName, tempInfo->sersor[i].uartName);
			g_EdgeGatewayConfig->tempSersor4.bandrate = tempInfo->sersor[i].bandrate;
			g_EdgeGatewayConfig->tempSersor4.uartType = tempInfo->sersor[i].uartType;
			g_EdgeGatewayConfig->tempSersor4.slaveNumber = tempInfo->sersor[i].slaveNumber;
			for(int j = 0; j < tempInfo->sersor[i].slaveNumber; j++)
			{
				g_EdgeGatewayConfig->tempSersor4.slaveID[j] = tempInfo->sersor[i].slaveID[j];
			}
			strcpy(g_EdgeGatewayConfig->tempSersor4.dataFilename, tempInfo->sersor[i].dataFilename);
			break;
		case 4:
			strcpy(g_EdgeGatewayConfig->tempSersor5.uartName, tempInfo->sersor[i].uartName);
			g_EdgeGatewayConfig->tempSersor5.bandrate = tempInfo->sersor[i].bandrate;
			g_EdgeGatewayConfig->tempSersor5.uartType = tempInfo->sersor[i].uartType;
			g_EdgeGatewayConfig->tempSersor5.slaveNumber = tempInfo->sersor[i].slaveNumber;
			for(int j = 0; j < tempInfo->sersor[i].slaveNumber; j++)
			{
				g_EdgeGatewayConfig->tempSersor5.slaveID[j] = tempInfo->sersor[i].slaveID[j];
			}
			strcpy(g_EdgeGatewayConfig->tempSersor5.dataFilename, tempInfo->sersor[i].dataFilename);
			break;
		}
	}

	CreateJsonFile(JSON_CONFIG_FILENAME, g_EdgeGatewayConfig);
}


/**
 * @fn SaveTempConfig
 * @brief 保存101配置信息到文件
 * @param iec101 101配置信息的指针
 * @return void
 */
void SaveIec101Config(IEC101Config *iec101)
{
	strcpy(g_EdgeGatewayConfig->iec101.uartName, iec101->uartName);
	g_EdgeGatewayConfig->iec101.bandrate = iec101->bandrate;
	g_EdgeGatewayConfig->iec101.parity = iec101->parity;
	g_EdgeGatewayConfig->iec101.uartType = iec101->uartType;
	g_EdgeGatewayConfig->iec101.balanMode = iec101->balanMode;
	g_EdgeGatewayConfig->iec101.sourceAddr = iec101->sourceAddr;
	g_EdgeGatewayConfig->iec101.linkAddrSize = iec101->linkAddrSize;
	g_EdgeGatewayConfig->iec101.ASDUCotSize = iec101->ASDUCotSize;
	g_EdgeGatewayConfig->iec101.ASDUAddr = iec101->ASDUAddr;
	g_EdgeGatewayConfig->iec101.ASDUAddrSize = iec101->ASDUAddrSize;
	g_EdgeGatewayConfig->iec101.infoAddrSize = iec101->infoAddrSize;
	g_EdgeGatewayConfig->iec101.slaveNumber = iec101->slaveNumber;
	for(int i = 0; i < iec101->slaveNumber; i++)
	{
		g_EdgeGatewayConfig->iec101.sModuleId[i] = iec101->sModuleId[i];
		g_EdgeGatewayConfig->iec101.sMsourceAddr[i] = iec101->sMsourceAddr[i];
	}
	g_EdgeGatewayConfig->iec101.sMstate = iec101->sMstate;
	g_EdgeGatewayConfig->iec101.sMportNo = iec101->sMportNo;
	strcpy(g_EdgeGatewayConfig->iec101.dataFilename, iec101->dataFilename);

	CreateJsonFile(JSON_CONFIG_FILENAME, g_EdgeGatewayConfig);
}


/**
 * @fn SaveTempConfig
 * @brief 保存104配置信息到文件
 * @param iec104 104配置信息的指针
 * @return void
 */
void SaveIec104Config(IEC104Config *iec104)
{
	strcpy(g_EdgeGatewayConfig->iec104.localIP, iec104->localIP);
	g_EdgeGatewayConfig->iec104.balanMode = iec104->balanMode;
	g_EdgeGatewayConfig->iec104.sourceAddr = iec104->sourceAddr;
	g_EdgeGatewayConfig->iec104.linkAddrSize = iec104->linkAddrSize;
	g_EdgeGatewayConfig->iec104.ASDUCotSize = iec104->ASDUCotSize;
	g_EdgeGatewayConfig->iec104.ASDUAddr = iec104->ASDUAddr;
	g_EdgeGatewayConfig->iec104.ASDUAddrSize = iec104->ASDUAddrSize;
	g_EdgeGatewayConfig->iec104.infoAddrSize = iec104->infoAddrSize;
	g_EdgeGatewayConfig->iec104.slaveNumber = iec104->slaveNumber;
	for(int i = 0; i < iec104->slaveNumber; i++)
	{
		g_EdgeGatewayConfig->iec104.sModuleId[i] = iec104->sModuleId[i];
		g_EdgeGatewayConfig->iec104.sMsourceAddr[i] = iec104->sMsourceAddr[i];
	}
	g_EdgeGatewayConfig->iec104.sMstate = iec104->sMstate;
	g_EdgeGatewayConfig->iec104.sMnetEn = iec104->sMnetEn;
	strcpy(g_EdgeGatewayConfig->iec104.sMip1, iec104->sMip1);
	strcpy(g_EdgeGatewayConfig->iec104.sMip2, iec104->sMip2);
	strcpy(g_EdgeGatewayConfig->iec104.sMip3, iec104->sMip3);
	strcpy(g_EdgeGatewayConfig->iec104.sMip4, iec104->sMip4);
	strcpy(g_EdgeGatewayConfig->iec104.sMip5, iec104->sMip5);

	strcpy(g_EdgeGatewayConfig->iec104.dataFilename, iec104->dataFilename);

	CreateJsonFile(JSON_CONFIG_FILENAME, g_EdgeGatewayConfig);
}


/**
 * @fn SaveTempConfig
 * @brief 保存MQTT配置信息到文件
 * @param mqttAccess MQTT配置信息的指针
 * @return void
 */
void SaveMqttConfig(MqttConfig *mqttAccess)
{
	g_EdgeGatewayConfig->mqttAccess.mqttNumber = mqttAccess->mqttNumber;
	strcpy(g_EdgeGatewayConfig->mqttAccess.clondIP, mqttAccess->clondIP);
	strcpy(g_EdgeGatewayConfig->mqttAccess.username1, mqttAccess->username1);
	strcpy(g_EdgeGatewayConfig->mqttAccess.username2, mqttAccess->username2);
	strcpy(g_EdgeGatewayConfig->mqttAccess.username3, mqttAccess->username3);
	strcpy(g_EdgeGatewayConfig->mqttAccess.username4, mqttAccess->username4);
	strcpy(g_EdgeGatewayConfig->mqttAccess.username5, mqttAccess->username5);

	CreateJsonFile(JSON_CONFIG_FILENAME, g_EdgeGatewayConfig);
}





