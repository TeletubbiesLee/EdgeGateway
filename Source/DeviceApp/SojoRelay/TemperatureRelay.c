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
#include "../../ProcessCommunication/Semaphore.h"


static void SOJO_TemperatureDataProcess(uint16_t registerData[], int arrayNumber, int deviceId, char *filename);


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


