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
#include "../libmodbus/modbus.h"
#include "../../Config.h"
#include "../ModbusInit.h"
#include "AirQualitySensor.h"


static void AirQualityDataParse(uint16_t *tabRegisters);


/**
 * @breif 六合一空气质量传感器modbus通信函数
 * @param uartInfo 串口信息结构体指针
 * @return 成功:0 失败:其他
 */
int AirQualitySensor(UartInfo *uartInfo)
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
	nbPoints = AIR_QUALITY_REGISTERS_NUMBER;
	tabRegisters = (uint16_t *) malloc(nbPoints * sizeof(uint16_t));
	memset(tabRegisters, 0, nbPoints * sizeof(uint16_t));

	while (1)
	{
		/* 设置从机ID */
		modbus_set_slave(ctx, AIR_QUALITY_SERVER_ID);
		modbus_read_registers(ctx, AIR_QUALITY_REGISTERS_ADDRESS, AIR_QUALITY_REGISTERS_NUMBER, tabRegisters);

		/* TODO：对数据进行解析和保存 */
		AirQualityDataParse(tabRegisters);

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
 * @breif 六合一空气质量传感器数据解析
 * @param tabRegisters 数据集
 * @return 成功:0 失败:其他
 */
static void AirQualityDataParse(uint16_t *tabRegisters)
{
	int CO2 = 0, PM2_5 = 0, PM10 = 0, humidity = 0, sRH = 0, Stem = 0;
	float TVOC = 0.0, CH2O = 0.0, temperature = 0.0;

	CO2 = tabRegisters[0];
	printf("CO2 = %d ppm\n", CO2);

	TVOC = tabRegisters[1] / 10.0;
	printf("TVOC = %.2f ug/m3\n", TVOC);

	CH2O = tabRegisters[2] / 10.0;
	printf("CH2O = %.2f ug/m3\n", CH2O);

	PM2_5 = tabRegisters[3];
	printf("PM2_5 = %d ppm\n", PM2_5);

	sRH = tabRegisters[4];
	humidity = -6 + 125 * sRH / (64 * 1024);
	printf("humidity = %d %%RH\n", humidity);

	Stem = tabRegisters[5];
	temperature = -46.85 + 175.72 * Stem / (64 * 1024);
	printf("temperature = %.2f ℃\n", temperature);

	PM10 = tabRegisters[6];
	printf("PM10 = %d ppm\n", PM10);
}


