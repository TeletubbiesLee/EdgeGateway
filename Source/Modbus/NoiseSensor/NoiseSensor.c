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
#include "../libmodbus/modbus.h"
#include "NoiseSensor.h"
#include "../../Config.h"
#include "../ModbusInit.h"
#include "../../DataStorage/DataProcess.h"

static void NoiseDataParse(uint16_t *tabRegisters, int deviceId, char *filename);

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

    if(-1 == ModbusInit(&ctx, uartInfo))		//Modbus初始化
	{
		printf_debug("ModbusInit error\n");
		return FUNCTION_FAIL;
	}

    /* 为bit和寄存器分配内存空间 */
    nbPoints = NOISE_REGISTERS_NUMBER;
    tabRegisters = (uint16_t *) malloc(nbPoints * sizeof(uint16_t));
    memset(tabRegisters, 0, nbPoints * sizeof(uint16_t));

	if(0 != CreateDataFile(filename))
	{
		printf_debug("CreateDataFile(\"%s\") error\n", filename);
	}

    while (1)
    {
    	for(int i = 0; i < deviceNum; i++)
    	{
			/* 设置从机ID */
			modbus_set_slave(ctx, deviceId[i]);
			modbus_read_registers(ctx, NOISE_REGISTERS_ADDRESS, NOISE_REGISTERS_NUMBER, tabRegisters);

			/* 对数据进行解析和保存 */
			NoiseDataParse(tabRegisters, deviceId[i], filename);
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


