/**
 * @file NoiseSensor.c
 * @brief 噪声传感器通信程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.20
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
#include "NoiseSensor.h"
#include "../../Config.h"
#include "../../Modbus/ModbusInit.h"
#include "../../DataStorage/DataProcess.h"
#include "../../ProcessCommunication/ProcessSignal.h"
#include "../DeviceParamConfig.h"


static void NoiseDataParse(uint16_t *tabRegisters, int deviceId, char *filename);


/**
 * @brief 创建噪声传感器进程
 * @param void
 * @return 成功：0 失败：其他
 */
int CreatNoiseProcess(void)
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
 * @breif 噪声传感器modbus通信函数
 * @param uartInfo 串口信息结构体指针
 * @param deviceId 设备ID数组
 * @param deviceNum 设备数量
 * @param filename 数据文件名
 * @return 成功:0 失败:其他
 */
int NoiseSensor(UartInfo *uartInfo, int deviceId[], int deviceNum, char *filename)
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
    nbPoints = NOISE_REGISTERS_NUMBER;
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
			printf_debug("NoiseSensor: Semaphore_V error\n");
	}
    else
    	printf_debug("NoiseSensor: Semaphore_P error\n");

    while (1)
    {
    	for(int i = 0; i < deviceNum; i++)
    	{
			/* 设置从机ID */
			modbus_set_slave(ctx, deviceId[i]);
			modbus_read_registers(ctx, NOISE_REGISTERS_ADDRESS, NOISE_REGISTERS_NUMBER, tabRegisters);

			/* 对数据进行解析和保存 */
			if(Semaphore_P(semId) == NO_ERROR)
			{
				printf("NoiseSensor: Semaphore_P success\n");
				NoiseDataParse(tabRegisters, deviceId[i], filename);
				if(Semaphore_V(semId) == NO_ERROR)
					printf("NoiseSensor: Semaphore_V success\n");
			}
    	}

		sleep(NOISE_MODBUS_INTERVAL);
    }

    /* Free the memory */
    free(tabRegisters);

    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);

	return NO_ERROR;
}


/**
 * @breif 噪声传感器数据解析和保存
 * @param tabRegisters 数据集
 * @param deviceId 设备ID
 * @param filename 保存的文件名
 * @return 成功:0 失败:其他
 */
static void NoiseDataParse(uint16_t *tabRegisters, int deviceId, char *filename)
{
	int noiseValue = 0;
	DataInformation dataInfo;
	dataInfo.deviceId = deviceId;
	strcpy(dataInfo.mqttUserName, filename);

	noiseValue = tabRegisters[0];
	printf("noiseValue = %d\n", noiseValue);

	/* 保存数据 */
	strcpy(dataInfo.dataName, "noiseValue");
	dataInfo.dataType = INT_TYPE;
	dataInfo.intData = noiseValue;
	GetTimeStr(dataInfo.updateTime);
	SaveData(filename, &dataInfo);
}


