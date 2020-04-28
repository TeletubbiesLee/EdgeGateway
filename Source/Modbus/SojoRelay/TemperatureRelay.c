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
#include "TemperatureRelay.h"
#include "../libmodbus/modbus.h"
#include "../../Config.h"
#include "../ModbusInit.h"
#include "../../DataStorage/DataProcess.h"


static void SOJO_TemperatureDataProcess(uint16_t registerData[], int arrayNumber, float temperature[]);


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
    float temperature[TEMP_RELAY_REGISTERS_NUMBER] = {0};		//存放温度数据
    int tempValue = 0;			//临时值

    if(-1 == ModbusInit(&ctx, uartInfo))		//Modbus初始化
	{
		printf_debug("ModbusInit error\n");
		return FUNCTION_FAIL;
	}

    /* 为bit和寄存器分配内存空间 */
    nbPoints = TEMP_RELAY_REGISTERS_NUMBER;
    tabRegisters = (uint16_t *) malloc(nbPoints * sizeof(uint16_t));
    memset(tabRegisters, 0, nbPoints * sizeof(uint16_t));

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

			/* TODO：对数据进行解析和保存 */
			SOJO_TemperatureDataProcess(tabRegisters, TEMP_RELAY_REGISTERS_NUMBER, temperature);
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
 * @return void
 * @updata: [2020-01-07][Gang][creat]
 */
static void SOJO_TemperatureDataProcess(uint16_t registerData[], int arrayNumber, float temperature[])
{
	int i = 0;
	int tempValue = 0;				//存放临时值

	for(i = 0; i < arrayNumber; i++)
	{
		if(registerData[i] & (1 << 15))
		{
			tempValue = ~registerData[i] + 1;
			tempValue &= (1 << 15);
		}
		else
		{
			tempValue = registerData[i];
		}
		temperature[i] = tempValue / 10.0;
	}
	printf("temperature[%d] = %.2f\n", i, temperature[i]);
}


