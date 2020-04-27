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



/**
 * @breif 噪声传感器modbus通信函数
 * @param uartInfo 串口信息结构体指针
 * @param deviceId 设备ID数组
 * @param deviceNum 设备数量
 * @return 成功:0 失败:其他
 */
int NoiseSensor(UartInfo *uartInfo, int deviceId[], int deviceNum)
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

    while (1)
    {
    	for(int i = 0; i < deviceNum; i++)
    	{
			/* 设置从机ID */
			modbus_set_slave(ctx, deviceId[i]);
			modbus_read_registers(ctx, NOISE_REGISTERS_ADDRESS, NOISE_REGISTERS_NUMBER, tabRegisters);

			/* TODO：对数据进行解析和保存 */
			printf("value = %d\n", tabRegisters[0]);
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


