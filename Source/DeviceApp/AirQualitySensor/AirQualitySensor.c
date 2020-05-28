/**
 * @file AirQualitySensor.c
 * @brief 六合一空气质量传感器通信程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.24
 *
 * @author Lei.L
 * @version ver 1.0
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sem.h>
#include "../../Modbus/libmodbus/modbus.h"
#include "../../Config.h"
#include "../../Modbus/ModbusInit.h"
#include "AirQualitySensor.h"
#include "../../DataStorage/DataProcess.h"
#include "../../ProcessCommunication/ProcessSignal.h"
#include "../DeviceParamConfig.h"


static void AirQualityDataParse(uint16_t *tabRegisters, int deviceId, char *filename);


/**
 * @brief 创建空气质量传感器进程
 * @param void
 * @return 成功：0 失败：其他
 */
int CreatAirQualityProcess(void)
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
 * @breif 六合一空气质量传感器modbus通信函数
 * @param uartInfo 串口信息结构体指针
 * @param deviceId 设备ID数组
 * @param deviceNum 设备数量
 * @param filename 数据文件名
 * @return 成功:0 失败:其他
 */
int AirQualitySensor(UartInfo *uartInfo, int deviceId[], int deviceNum, char *filename)
{
	modbus_t *ctx = NULL;       //成功打开设备后返回的结构体指针
	uint16_t *tabRegisters = NULL;      //寄存器的空间
	int nbPoints;               //空间大小
	int semId = 0;

	if(-1 == ModbusInit(&ctx, uartInfo))		//Modbus初始化
	{
		printf_debug("ModbusInit error\n");
		return FUNCTION_FAIL;
	}

	/* 为bit和寄存器分配内存空间 */
	nbPoints = AIR_QUALITY_REGISTERS_NUMBER;
	tabRegisters = (uint16_t *) malloc(nbPoints * sizeof(uint16_t));
	memset(tabRegisters, 0, nbPoints * sizeof(uint16_t));

    semId = semget((key_t)SEMAPHORE_KEY, 1, 0666 | IPC_CREAT);

    if(Semaphore_P(semId) == NO_ERROR)
	{
		if(0 != CreateDataFile(filename))
		{
			printf_debug("CreateDataFile(\"%s\") error\n", filename);
		}
		printf("CreateDataFile(\"%s\") success\n", filename);
		if(Semaphore_V(semId) != NO_ERROR)
			printf_debug("AirQualitySensor: Semaphore_V error\n");
	}
    else
    	printf_debug("AirQualitySensor: Semaphore_P error\n");

	while (1)
	{
		for(int i = 0; i < deviceNum; i++)
		{
			/* 设置从机ID */
			modbus_set_slave(ctx, deviceId[i]);
			modbus_read_registers(ctx, AIR_QUALITY_REGISTERS_ADDRESS, AIR_QUALITY_REGISTERS_NUMBER, tabRegisters);

			/* 对数据进行解析和保存 */
			if(Semaphore_P(semId) == NO_ERROR)
			{
				printf("AirQualitySensor: Semaphore_P success\n");
				AirQualityDataParse(tabRegisters, deviceId[i], filename);
				if(Semaphore_V(semId) == NO_ERROR)
					printf("AirQualitySensor: Semaphore_V success\n");
			}
		}
		sleep(AIR_QUALITY_MODBUS_INTERVAL);
	}

	/* Free the memory */
	free(tabRegisters);

	/* Close the connection */
	modbus_close(ctx);
	modbus_free(ctx);

	return NO_ERROR;
}


/**
 * @breif 六合一空气质量传感器数据解析和保存
 * @param tabRegisters 数据集
 * @param deviceId 设备ID
 * @param filename 保存的文件名
 * @return 成功:0 失败:其他
 */
static void AirQualityDataParse(uint16_t *tabRegisters, int deviceId, char *filename)
{
	int CO2 = 0, PM2_5 = 0, PM10 = 0, humidity = 0, sRH = 0, Stem = 0;
	float TVOC = 0.0, CH2O = 0.0, temperature = 0.0;
	DataInformation dataInfo;
	dataInfo.deviceId = deviceId;
	strcpy(dataInfo.mqttUserName, filename);

	CO2 = tabRegisters[0];
	printf("CO2 = %d ppm\n", CO2);
	/* 保存数据 */
	strcpy(dataInfo.dataName, "CO2");
	dataInfo.dataType = INT_TYPE;
	dataInfo.intData = CO2;
	GetTimeStr(dataInfo.updateTime);
	SaveData(filename, &dataInfo);

	TVOC = tabRegisters[1] / 10.0;
	printf("TVOC = %.2f ug/m3\n", TVOC);
	/* 保存数据 */
	strcpy(dataInfo.dataName, "TVOC");
	dataInfo.dataType = FLOAT_TYPE;
	dataInfo.floatData = TVOC;
	GetTimeStr(dataInfo.updateTime);
	SaveData(filename, &dataInfo);

	CH2O = tabRegisters[2] / 10.0;
	printf("CH2O = %.2f ug/m3\n", CH2O);
	/* 保存数据 */
	strcpy(dataInfo.dataName, "CH2O");
	dataInfo.dataType = FLOAT_TYPE;
	dataInfo.floatData = CH2O;
	GetTimeStr(dataInfo.updateTime);
	SaveData(filename, &dataInfo);

	PM2_5 = tabRegisters[3];
	printf("PM2_5 = %d ppm\n", PM2_5);
	/* 保存数据 */
	strcpy(dataInfo.dataName, "PM2_5");
	dataInfo.dataType = INT_TYPE;
	dataInfo.intData = PM2_5;
	GetTimeStr(dataInfo.updateTime);
	SaveData(filename, &dataInfo);

	sRH = tabRegisters[4];
	humidity = -6 + 125 * sRH / (64 * 1024);
	printf("humidity = %d %%RH\n", humidity);
	/* 保存数据 */
	strcpy(dataInfo.dataName, "humidity");
	dataInfo.dataType = INT_TYPE;
	dataInfo.intData = humidity;
	GetTimeStr(dataInfo.updateTime);
	SaveData(filename, &dataInfo);

	Stem = tabRegisters[5];
	temperature = -46.85 + 175.72 * Stem / (64 * 1024);
	printf("temperature = %.2f ℃\n", temperature);
	/* 保存数据 */
	strcpy(dataInfo.dataName, "temperature");
	dataInfo.dataType = FLOAT_TYPE;
	dataInfo.floatData = temperature;
	GetTimeStr(dataInfo.updateTime);
	SaveData(filename, &dataInfo);

	PM10 = tabRegisters[6];
	printf("PM10 = %d ppm\n", PM10);
	/* 保存数据 */
	strcpy(dataInfo.dataName, "PM10");
	dataInfo.dataType = INT_TYPE;
	dataInfo.intData = PM10;
	GetTimeStr(dataInfo.updateTime);
	SaveData(filename, &dataInfo);
}







