/**
 * @file Main.c
 * @brief 边缘网关main函数程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.13
 *
 * @author Lei.L
 * @version ver 1.0
 */
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/sem.h>
#include "Led/Led.h"
#include "ProcessSignal/ProcessSignal.h"
#include "TransparentTransmission/TransparentTransmission.h"
#include "DeviceApp/NoiseSensor/NoiseSensor.h"
#include "DeviceApp/AirQualitySensor/AirQualitySensor.h"
#include "DeviceApp/SojoRelay/TemperatureRelay.h"
#include "MQTT/MqttPublish.h"
#include "ParserConfig/Interface_S2J.h"
#include "ProcessCommunication/Semaphore.h"
#include "DataStruct.h"
#include "Config.h"
#include "DeviceApp/SojoDtu/SojoDtu.h"



static void FreePointArray(void *pointArray[], int num);
static int CreatNet2UartProcess(void);
static int CreatNoiseProcess(void);
static int CreatAirQualityProcess(void);
static int CreatSojoRelayProcess(void);
static int CreatMqttProcess(void);
static int CreatIec101Process(void);
static int CreatIec104Process(void);
static int CreatWebProcess(void);
static void TrsptTrsmsParamConfig(EdgeGatewayConfig *configInfo, int *processNum, int type[], UartInfo *uart[], NetworkInfo *eth[]);
static void ModbusParamConfig(EdgeGatewayConfig *configInfo, int *processNum, char *username[], UartInfo *uart[], int deviceNum[], int *deviceId[], int sersorType);
static void MqttParamConfig(EdgeGatewayConfig *configInfo, int *processNum, char *username[]);
static void IEC101ParamConfig(EdgeGatewayConfig *configInfo, Configure101 *info101, char *filename);
static void IEC104ParamConfig(EdgeGatewayConfig *configInfo, Configure104 *info104, char *filename);


/**
 * 	@brief: main函数
 */
int main(int argc, char *argv[])
{
	int semId = 0;

	/* 解析配置文件，获取配置信息  */
	GetJsonFile(JSON_CONFIG_FILENAME, &g_EdgeGatewayConfig);

	/* 初始化信号量 */
	semId = semget((key_t)SEMAPHORE_KEY, 1, 0666 | IPC_CREAT);
	SetSemValue(semId);


	CreatNet2UartProcess();

	CreatNoiseProcess();

	CreatAirQualityProcess();

	CreatSojoRelayProcess();

	CreatMqttProcess();

	CreatIec101Process();

	CreatIec104Process();

	CreatWebProcess();

	/* 创建其他功能的进程 */

	/****************/


	/* 父进程创建完子进程后，执行的任务 */
	while(1)
	{
		IndicatorLedRunning();		//程序运行指示灯
	}

	IndicatorLedOnOrOff(LED_OFF);	//程序退出，灯关闭
	printf("EdgeGateway (pid:%d) exit\n", getpid());

	S2J_StructFree(g_EdgeGatewayConfig);
	DelSemValue(semId);


	return 0;
}


/**
 * @brief 释放指针数组中的空间
 * @param void
 * @return v
 */
static void FreePointArray(void *pointArray[], int num)
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
 * @brief 创建透传进程
 * @param void
 * @return 成功：0 失败：其他
 */
static int CreatNet2UartProcess(void)
{
	if(g_EdgeGatewayConfig == NULL)
	{
		printf_debug("g_EdgeGatewayConfig is NULL!\n");
		return POINT_NULL;
	}

	/* 透传功能需要的配置信息 */
	bool isError = true;
	pid_t pid = 0;
	int trsptTrsmsProcessNum = 0;			//透传功能进程数
	int trsptTrsmsType[PROTOCOL_MAX_PROCESS] = {0};
	UartInfo *trsptTrsmsUart[PROTOCOL_MAX_PROCESS] = {0};
	NetworkInfo *trsptTrsmsEth[PROTOCOL_MAX_PROCESS] = {0};
	for(int i = 0; i < PROTOCOL_MAX_PROCESS; i++)
	{
		if((trsptTrsmsUart[i] = malloc(sizeof(UartInfo))) == NULL)
		{
			printf_debug("trsptTrsmsUart[%d] malloc error\n", i);
			isError = false;
			break;
		}
		if((trsptTrsmsEth[i] = malloc(sizeof(NetworkInfo))) == NULL)
		{
			printf_debug("trsptTrsmsEth[%d] malloc error\n", i);
			isError = false;
			break;
		}
	}

	if(false == isError)
	{
		FreePointArray((void**)trsptTrsmsUart, PROTOCOL_MAX_PROCESS);
		FreePointArray((void**)trsptTrsmsEth, PROTOCOL_MAX_PROCESS);
		return MALLOC_FAIL;
	}

	TrsptTrsmsParamConfig(g_EdgeGatewayConfig, &trsptTrsmsProcessNum, trsptTrsmsType, trsptTrsmsUart, trsptTrsmsEth);
	/* 创建透传功能进程 */
	for(int i = 0; i < trsptTrsmsProcessNum; i++)
	{
		if((pid = fork()) == 0)
		{
			SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

			printf("TransparentTransmission (pid:%d) creat\n", getpid());
			TransparentTransmission(trsptTrsmsType[i], trsptTrsmsUart[i], trsptTrsmsEth[i]);		//透传功能
			printf("TransparentTransmission (pid:%d) exit\n", getpid());

			return NO_ERROR;
		}
	}

	FreePointArray((void**)trsptTrsmsUart, PROTOCOL_MAX_PROCESS);
	FreePointArray((void**)trsptTrsmsEth, PROTOCOL_MAX_PROCESS);
	return NO_ERROR;
}


/**
 * @brief 创建噪声传感器进程
 * @param void
 * @return 成功：0 失败：其他
 */
static int CreatNoiseProcess(void)
{
	if(g_EdgeGatewayConfig == NULL)
	{
		printf_debug("g_EdgeGatewayConfig is NULL!\n");
		return POINT_NULL;
	}

	/* 噪声传感器需要的配置信息 */
	bool isError = true;
	pid_t pid = 0;
	int noiseProcessNum = 0;
	char *noiseUsername[PROTOCOL_MAX_PROCESS] = {0};
	UartInfo *noiseUart[PROTOCOL_MAX_PROCESS] = {0};
	int noiseDeviceNum[PROTOCOL_MAX_PROCESS] = {0};
	int *noiseDeviceId[PROTOCOL_MAX_PROCESS] = {0};
	for(int i = 0; i < PROTOCOL_MAX_PROCESS; i++)
	{
		if((noiseUsername[i] = malloc(sizeof(char) * 30)) == NULL)
		{
			printf_debug("noiseUsername[%d] malloc error\n", i);
			isError = false;
			break;
		}
		if((noiseUart[i] = malloc(sizeof(UartInfo))) == NULL)
		{
			printf_debug("noiseUart[%d] malloc error\n", i);
			isError = false;
			break;
		}
		if((noiseDeviceId[i] = malloc(sizeof(int) * SLAVE_MAX_NUMBER)) == NULL)
		{
			printf_debug("noiseDeviceId[%d] malloc error\n", i);
			isError = false;
			break;
		}
	}

	if(false == isError)
	{
		FreePointArray((void**)noiseUsername, PROTOCOL_MAX_PROCESS);
		FreePointArray((void**)noiseUart, PROTOCOL_MAX_PROCESS);
		FreePointArray((void**)noiseDeviceId, PROTOCOL_MAX_PROCESS);
		return MALLOC_FAIL;
	}

	ModbusParamConfig(g_EdgeGatewayConfig, &noiseProcessNum, noiseUsername, noiseUart, noiseDeviceNum, noiseDeviceId, NOISE_SERSOR);
	/* 创建噪声传感器通信进程 */
	for(int i = 0; i < noiseProcessNum; i++)
	{
		if((pid = fork()) == 0)
		{
			SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

			printf("NoiseSensor (pid:%d) creat\n", getpid());
			NoiseSensor(noiseUart[i], noiseDeviceId[i], noiseDeviceNum[i], noiseUsername[i]);
			printf("NoiseSensor (pid:%d) exit\n", getpid());

			return NO_ERROR;
		}
	}
	FreePointArray((void**)noiseUsername, PROTOCOL_MAX_PROCESS);
	FreePointArray((void**)noiseUart, PROTOCOL_MAX_PROCESS);
	FreePointArray((void**)noiseDeviceId, PROTOCOL_MAX_PROCESS);
	return NO_ERROR;
}


/**
 * @brief 创建空气质量传感器进程
 * @param void
 * @return 成功：0 失败：其他
 */
static int CreatAirQualityProcess(void)
{
	if(g_EdgeGatewayConfig == NULL)
	{
		printf_debug("g_EdgeGatewayConfig is NULL!\n");
		return POINT_NULL;
	}

	/* 六合一空气质量传感器需要的配置信息 */
	bool isError = true;
	pid_t pid = 0;
	int airQualityProcessNum = 0;
	char *airQualityUsername[PROTOCOL_MAX_PROCESS] = {0};
	UartInfo *airQualitySensor[PROTOCOL_MAX_PROCESS] = {0};
	int airQualityDeviceNum[PROTOCOL_MAX_PROCESS] = {0};
	int *airQualityDeviceId[PROTOCOL_MAX_PROCESS] = {0};
	for(int i = 0; i < PROTOCOL_MAX_PROCESS; i++)
	{
		if((airQualityUsername[i] = malloc(sizeof(char) * 30)) == NULL)
		{
			printf_debug("airQualityUsername[%d] malloc error\n", i);
			isError = false;
			break;
		}
		if((airQualitySensor[i] = malloc(sizeof(UartInfo))) == NULL)
		{
			printf_debug("airQualitySensor[%d] malloc error\n", i);
			isError = false;
			break;
		}
		if((airQualityDeviceId[i] = malloc(sizeof(int) * SLAVE_MAX_NUMBER)) == NULL)
		{
			printf_debug("airQualityDeviceId[%d] malloc error\n", i);
			isError = false;
			break;
		}
	}

	if(false == isError)
	{
		FreePointArray((void**)airQualityUsername, PROTOCOL_MAX_PROCESS);
		FreePointArray((void**)airQualitySensor, PROTOCOL_MAX_PROCESS);
		FreePointArray((void**)airQualityDeviceId, PROTOCOL_MAX_PROCESS);
		return MALLOC_FAIL;
	}

	ModbusParamConfig(g_EdgeGatewayConfig, &airQualityProcessNum, airQualityUsername, airQualitySensor, airQualityDeviceNum, airQualityDeviceId, AIR_QUALITY_SERSOR);
	/* 创建六合一空气质量传感器通信进程 */
	for(int i = 0; i < airQualityProcessNum; i++)
	{
		if((pid = fork()) == 0)
		{
			SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

			printf("AirQualitySensor (pid:%d) creat\n", getpid());
			AirQualitySensor(airQualitySensor[i], airQualityDeviceId[i], airQualityDeviceNum[i], airQualityUsername[i]);
			printf("AirQualitySensor (pid:%d) exit\n", getpid());

			return NO_ERROR;
		}
	}
	FreePointArray((void**)airQualityUsername, PROTOCOL_MAX_PROCESS);
	FreePointArray((void**)airQualitySensor, PROTOCOL_MAX_PROCESS);
	FreePointArray((void**)airQualityDeviceId, PROTOCOL_MAX_PROCESS);
	return NO_ERROR;
}


/**
 * @brief 创建测温接收中继进程
 * @param void
 * @return 成功：0 失败：其他
 */
static int CreatSojoRelayProcess(void)
{
	if(g_EdgeGatewayConfig == NULL)
	{
		printf_debug("g_EdgeGatewayConfig is NULL!\n");
		return POINT_NULL;
	}

	/* 双杰测温中继需要的配置信息 */
	bool isError = true;
	pid_t pid = 0;
	int sojoRelayProcessNum = 0;
	char *sojoRelayUsername[PROTOCOL_MAX_PROCESS] = {0};
	UartInfo *sojoRelaySensor[PROTOCOL_MAX_PROCESS] = {0};
	int sojoRelayDeviceNum[PROTOCOL_MAX_PROCESS] = {0};
	int *sojoRelayDeviceId[PROTOCOL_MAX_PROCESS] = {0};
	for(int i = 0; i < PROTOCOL_MAX_PROCESS; i++)
	{
		if((sojoRelayUsername[i] = malloc(sizeof(char) * 30)) == NULL)
		{
			printf_debug("sojoRelayUsername[%d] malloc error\n", i);
			isError = false;
			break;
		}
		if((sojoRelaySensor[i] = malloc(sizeof(UartInfo))) == NULL)
		{
			printf_debug("sojoRelaySensor[%d] malloc error\n", i);
			isError = false;
			break;
		}
		if((sojoRelayDeviceId[i] = malloc(sizeof(int) * SLAVE_MAX_NUMBER)) == NULL)
		{
			printf_debug("sojoRelayDeviceId[%d] malloc error\n", i);
			isError = false;
			break;
		}
	}

	if(false == isError)
	{
		FreePointArray((void**)sojoRelayUsername, PROTOCOL_MAX_PROCESS);
		FreePointArray((void**)sojoRelaySensor, PROTOCOL_MAX_PROCESS);
		FreePointArray((void**)sojoRelayDeviceId, PROTOCOL_MAX_PROCESS);
		return MALLOC_FAIL;
	}

	ModbusParamConfig(g_EdgeGatewayConfig, &sojoRelayProcessNum, sojoRelayUsername, sojoRelaySensor, sojoRelayDeviceNum, sojoRelayDeviceId, SOJO_RELAY);
	/* 创建双杰测温中继通信进程 */
	for(int i = 0; i < sojoRelayProcessNum; i++)
	{
		if((pid = fork()) == 0)
		{
			SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

			printf("TemperatureRelay (pid:%d) creat\n", getpid());
			TemperatureRelay(sojoRelaySensor[i], sojoRelayDeviceId[i], sojoRelayDeviceNum[i], sojoRelayUsername[i]);
			printf("TemperatureRelay (pid:%d) exit\n", getpid());

			return NO_ERROR;
		}
	}
	FreePointArray((void**)sojoRelayUsername, PROTOCOL_MAX_PROCESS);
	FreePointArray((void**)sojoRelaySensor, PROTOCOL_MAX_PROCESS);
	FreePointArray((void**)sojoRelayDeviceId, PROTOCOL_MAX_PROCESS);
	return NO_ERROR;
}


/**
 * @brief 创建Mqtt上传数据的进程
 * @param void
 * @return 成功：0 失败：其他
 */
static int CreatMqttProcess(void)
{
	if(g_EdgeGatewayConfig == NULL)
	{
		printf_debug("g_EdgeGatewayConfig is NULL!\n");
		return POINT_NULL;
	}

	/* MQTT发布数据需要的配置信息 */
	bool isError = true;
	pid_t pid = 0;
	int mqttProcessNum = 0;
	char *userName[PROTOCOL_MAX_PROCESS] = {0};
	for(int i = 0; i < PROTOCOL_MAX_PROCESS; i++)
	{
		if((userName[i] = malloc(sizeof(char) * 30)) == NULL)
		{
			printf_debug("userName[%d] malloc error\n", i);
			isError = false;
			break;
		}
	}

	if(false == isError)
	{
		FreePointArray((void**)userName, PROTOCOL_MAX_PROCESS);
		return MALLOC_FAIL;
	}

	MqttParamConfig(g_EdgeGatewayConfig, &mqttProcessNum, userName);
	/* 创建MQTT通信进程 */
	for(int i = 0; i < mqttProcessNum; i++)
	{
		if((pid = fork()) == 0)
		{
			SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

			printf("MqttPublish (pid:%d) creat\n", getpid());
			MqttPublish(userName[i]);							//MQTT发布信息
			printf("MqttPublish (pid:%d) exit\n", getpid());

			for(int i = 0; i < PROTOCOL_MAX_PROCESS; i++)
			{
				if(userName[i] != NULL)
				{
					free(userName[i]);
					userName[i] = NULL;
				}
			}
			return NO_ERROR;
		}
	}
	FreePointArray((void**)userName, PROTOCOL_MAX_PROCESS);
	return NO_ERROR;
}


/**
 * @brief 创建101协议的进程
 * @param void
 * @return 成功：0 失败：其他
 */
static int CreatIec101Process(void)
{
	if(g_EdgeGatewayConfig == NULL)
	{
		printf_debug("g_EdgeGatewayConfig is NULL!\n");
		return POINT_NULL;
	}

	/* IEC101协议需要的配置信息 */
	pid_t pid = 0;
	Configure101 info101;
	char iec101Filename[30] = {0};
	IEC101ParamConfig(g_EdgeGatewayConfig, &info101, iec101Filename);
	/* 创建101进程 */
	if(info101.num > 0 && (pid = fork()) == 0)
	{
		SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

		printf("IEC101 (pid:%d) creat\n", getpid());
		SojoDtu_IEC101(&info101, iec101Filename);
		printf("IEC101 (pid:%d) exit\n", getpid());

		return NO_ERROR;
	}
	return NO_ERROR;
}


/**
 * @brief 创建104协议的进程
 * @param void
 * @return 成功：0 失败：其他
 */
static int CreatIec104Process(void)
{
	if(g_EdgeGatewayConfig == NULL)
	{
		printf_debug("g_EdgeGatewayConfig is NULL!\n");
		return POINT_NULL;
	}

	/* IEC104协议需要的配置信息 */
	bool isError = true;
	pid_t pid = 0;
	Configure104 info104;
	char iec104Filename[30] = {0};
	for(int i = 0; i < IEC104_SLAVE_MAX; i++)
	{
		if((info104.sMip[i] = malloc(sizeof(char) * 20)) == NULL)
		{
			printf_debug("info104.sMip[%d] malloc error\n", i);
			isError = false;
			break;
		}
	}

	if(false == isError)
	{
		FreePointArray((void**)info104.sMip, PROTOCOL_MAX_PROCESS);
		return MALLOC_FAIL;
	}

	IEC104ParamConfig(g_EdgeGatewayConfig, &info104, iec104Filename);
	/* 创建104进程 */
	if(info104.num > 0 && (pid = fork()) == 0)
	{
		SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

		printf("IEC104 (pid:%d) creat\n", getpid());
		SojoDtu_IEC104(&info104, iec104Filename);
		printf("IEC104 (pid:%d) exit\n", getpid());

		return NO_ERROR;
	}
	FreePointArray((void**)info104.sMip, PROTOCOL_MAX_PROCESS);
	return NO_ERROR;
}


/**
 * @brief 创建Web网页的进程
 * @param void
 * @return 成功：0 失败：其他
 */
static int CreatWebProcess(void)
{
	if(g_EdgeGatewayConfig == NULL)
	{
		printf_debug("g_EdgeGatewayConfig is NULL!\n");
		return POINT_NULL;
	}

	/* 创建嵌入式网页进程 */
	pid_t pid = 0;
	if((pid = fork()) == 0)
	{
		SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

		printf("Web (pid:%d) creat\n", getpid());
		system("./GoAhead/bin/goahead -v --home ./GoAhead/bin/ /home/root/GoAhead 192.168.10.10");
		printf("Web (pid:%d) exit\n", getpid());

		return NO_ERROR;
	}
	return NO_ERROR;
}


/**
 * @brief 透传功能参数配置
 * @param configInfo 从配置文件获取到的配置信息
 * @param processNum 进程数
 * @param type 协议类型数组
 * @param uart 串口信息结构体数组
 * @param eth 网口信息结构体数组
 * @return void
 */
static void TrsptTrsmsParamConfig(EdgeGatewayConfig *configInfo, int *processNum, int type[], UartInfo *uart[], NetworkInfo *eth[])
{
	UartToNetConfig *uart2Net = NULL;

	*processNum = configInfo->uartToNetNumber;

	for(int i = 0; i < *processNum; i++)
	{
		switch(i)
		{
			case 0:
				uart2Net = &configInfo->uartToNet1;
				break;
			case 1:
				uart2Net = &configInfo->uartToNet2;
				break;
			case 2:
				uart2Net = &configInfo->uartToNet3;
				break;
			case 3:
				uart2Net = &configInfo->uartToNet4;
				break;
			case 4:
				uart2Net = &configInfo->uartToNet5;
				break;
		}
		type[i] = uart2Net->ProtocolType;
		strcpy(uart[i]->uartName, uart2Net->uartName);
		uart[i]->bandrate = uart2Net->bandrate;
		uart[i]->uartType = uart2Net->uartType;
		strcpy(eth[i]->localAddress, uart2Net->localIP);
		eth[i]->localPort = uart2Net->localPort;
		strcpy(eth[i]->remoteAddress, uart2Net->remoteIP);
		eth[i]->remotePort = uart2Net->remotePort;
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
static void ModbusParamConfig(EdgeGatewayConfig *configInfo, int *processNum, char *username[], UartInfo *uart[], int deviceNum[], int *deviceId[], int sersorType)
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
static void MqttParamConfig(EdgeGatewayConfig *configInfo, int *processNum, char *username[])
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
static void IEC101ParamConfig(EdgeGatewayConfig *configInfo, Configure101 *info101, char *filename)
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
static void IEC104ParamConfig(EdgeGatewayConfig *configInfo, Configure104 *info104, char *filename)
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









































/**
 *               ii.                                         ;9ABH,
 *              SA391,                                    .r9GG35&G
 *              &#ii13Gh;                               i3X31i;:,rB1
 *              iMs,:,i5895,                         .5G91:,:;:s1:8A
 *               33::::,,;5G5,                     ,58Si,,:::,sHX;iH1
 *                Sr.,:;rs13BBX35hh11511h5Shhh5S3GAXS:.,,::,,1AG3i,GG
 *                .G51S511sr;;iiiishS8G89Shsrrsh59S;.,,,,,..5A85Si,h8
 *               :SB9s:,............................,,,.,,,SASh53h,1G.
 *            .r18S;..,,,,,,,,,,,,,,,,,,,,,,,,,,,,,....,,.1H315199,rX,
 *          ;S89s,..,,,,,,,,,,,,,,,,,,,,,,,....,,.......,,,;r1ShS8,;Xi
 *        i55s:.........,,,,,,,,,,,,,,,,.,,,......,.....,,....r9&5.:X1
 *       59;.....,.     .,,,,,,,,,,,...        .............,..:1;.:&s
 *      s8,..;53S5S3s.   .,,,,,,,.,..      i15S5h1:.........,,,..,,:99
 *      93.:39s:rSGB@A;  ..,,,,.....    .SG3hhh9G&BGi..,,,,,,,,,,,,.,83
 *      G5.G8  9#@@@@@X. .,,,,,,.....  iA9,.S&B###@@Mr...,,,,,,,,..,.;Xh
 *      Gs.X8 S@@@@@@@B:..,,,,,,,,,,. rA1 ,A@@@@@@@@@H:........,,,,,,.iX:
 *     ;9. ,8A#@@@@@@#5,.,,,,,,,,,... 9A. 8@@@@@@@@@@M;    ....,,,,,,,,S8
 *     X3    iS8XAHH8s.,,,,,,,,,,...,..58hH@@@@@@@@@Hs       ...,,,,,,,:Gs
 *    r8,        ,,,...,,,,,,,,,,.....  ,h8XABMMHX3r.          .,,,,,,,.rX:
 *   :9, .    .:,..,:;;;::,.,,,,,..          .,,.               ..,,,,,,.59
 *  .Si      ,:.i8HBMMMMMB&5,....                    .            .,,,,,.sMr
 *  SS       :: h@@@@@@@@@@#; .                     ...  .         ..,,,,iM5
 *  91  .    ;:.,1&@@@@@@MXs.                            .          .,,:,:&S
 *  hS ....  .:;,,,i3MMS1;..,..... .  .     ...                     ..,:,.99
 *  ,8; ..... .,:,..,8Ms:;,,,...                                     .,::.83
 *   s&: ....  .sS553B@@HX3s;,.    .,;13h.                            .:::&1
 *    SXr  .  ...;s3G99XA&X88Shss11155hi.                             ,;:h&,
 *     iH8:  . ..   ,;iiii;,::,,,,,.                                 .;irHA
 *      ,8X5;   .     .......                                       ,;iihS8Gi
 *         1831,                                                 .,;irrrrrs&@
 *           ;5A8r.                                            .:;iiiiirrss1H
 *             :X@H3s.......                                .,:;iii;iiiiirsrh
 *              r#h:;,...,,.. .,,:;;;;;:::,...              .:;;;;;;iiiirrss1
 *             ,M8 ..,....,.....,,::::::,,...         .     .,;;;iiiiiirss11h
 *             8B;.,,,,,,,.,.....          .           ..   .:;;;;iirrsss111h
 *            i@5,:::,,,,,,,,.... .                   . .:::;;;;;irrrss111111
 *            9Bi,:,,,,......                        ..r91;;;;;iirrsss1ss1111
 *
 *								狗头保佑，永无BUG！
 */






