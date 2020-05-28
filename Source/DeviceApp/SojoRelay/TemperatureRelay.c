/**
 * @file TemperatureRelay.c
 * @brief 双杰测温接收中继程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.26
 *
 * @author Lei.L
 * @version ver 1.0
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sem.h>
#include "TemperatureRelay.h"
#include "../../Modbus/libmodbus/modbus.h"
#include "../../Config.h"
#include "../../Modbus/ModbusInit.h"
#include "../../DataStorage/DataProcess.h"
#include "../../ProcessCommunication/ProcessSignal.h"
#include "../DeviceParamConfig.h"


static void SOJO_TemperatureDataProcess(uint16_t registerData[], int arrayNumber, int deviceId, char *filename);


/**
 * @brief 创建测温接收中继进程
 * @param void
 * @return 成功：0 失败：其他
 */
int CreatSojoRelayProcess(void)
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
 * @breif 噪声传感器modbus通信函数
 * @param uartInfo 串口信息结构体指针
 * @param deviceId 设备ID数组
 * @param deviceNum 设备数量
 * @param filename 数据文件名
 * @return 成功:0 失败:其他
 */
int TemperatureRelay(UartInfo *uartInfo, int deviceId[], int deviceNum, char *filename)
{
    modbus_t *ctx = NULL;       //成功打开设备后返回的结构体指针
    uint16_t *tabRegisters = NULL;      //寄存器的空间
    int nbPoints;               //空间大小
    int tempValue = 0;			//临时值
	int semId = 0;

    if(-1 == ModbusInit(&ctx, uartInfo))		//Modbus初始化
	{
		printf_debug("ModbusInit error\n");
		return FUNCTION_FAIL;
	}

    /* 为bit和寄存器分配内存空间 */
    nbPoints = TEMP_RELAY_REGISTERS_NUMBER;
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
			printf_debug("TemperatureRelay: Semaphore_V error\n");
	}
    else
    	printf_debug("TemperatureRelay: Semaphore_P error\n");

    while (1)
    {
    	for(int i = 0; i < deviceNum; i++)
    	{
			/* 设置从机ID */
			modbus_set_slave(ctx, deviceId[i]);
			for(int j = 0; j < TEMP_RELAY_REGISTERS_NUMBER; j += tempValue)
			{
				tempValue = (TEMP_RELAY_REGISTERS_NUMBER - j) > 125 ? 125 : TEMP_RELAY_REGISTERS_NUMBER - j;
				modbus_read_registers(ctx, TEMP_RELAY_REGISTERS_ADDRESS + j, tempValue, &tabRegisters[j]);
			}

			/* 对数据进行解析和保存 */
			if(Semaphore_P(semId) == NO_ERROR)
			{
				printf("TemperatureRelay: Semaphore_P success\n");
				SOJO_TemperatureDataProcess(tabRegisters, TEMP_RELAY_REGISTERS_NUMBER, deviceId[i], filename);
				if(Semaphore_V(semId) == NO_ERROR)
					printf("TemperatureRelay: Semaphore_V success\n");
			}
    	}
		sleep(TEMP_RELAY_MODBUS_INTERVAL);
    }

    /* Free the memory */
    free(tabRegisters);

    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);

	return NO_ERROR;
}


/**
 * @fn SOJO_TemperatureDataProcess
 * @brief 双杰温度数据处理
 * @param registerData 用于存放数据
 * @param arrayNumber 数据个数
 * @param deviceId 设备ID
 * @param filename 保存的文件名
 * @return void
 * @updata: [2020-01-07][Gang][creat]
 */
static void SOJO_TemperatureDataProcess(uint16_t registerData[], int arrayNumber, int deviceId, char *filename)
{
	int i = 0;
	float temperature = 0.0;
	char tempString[20] = {0};
	DataInformation dataInfo;
	dataInfo.deviceId = deviceId;
	strcpy(dataInfo.mqttUserName, filename);

	for(i = 0; i < arrayNumber; i++)
	{
		if(0 <= registerData[i] && registerData[i] < 2000)
		{
			temperature = registerData[i] / 10.0;
		}
		else if(2000 <= registerData[i] && registerData[i] <= 2500)
		{
			temperature = (registerData[i] - 2000) / -10.0;
		}
		else
		{
			temperature = registerData[i];
		}
		printf("temperature[%d] = %.2f\n", i, temperature);

		/* 保存数据 */
		sprintf(tempString, "temperature%d", i);
		strcpy(dataInfo.dataName, tempString);
		dataInfo.dataType = FLOAT_TYPE;
		dataInfo.floatData = temperature;
		GetTimeStr(dataInfo.updateTime);
		SaveData(filename, &dataInfo);
	}

}


