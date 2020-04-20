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
#include <errno.h>
#include "../libmodbus/modbus.h"
#include "../libmodbus/modbus-config.h"
#include "NoiseSensor.h"
#include "../../Config.h"
#include "../../RS485/RS485.h"



/**
 * @breif 噪声传感器modbus通信函数
 * @param uartInfo 串口信息结构体指针
 * @return 成功:0 失败:其他
 */
int NoiseSensor(int type, UartInfo *uartInfo)
{
    modbus_t *ctx = NULL;       //成功打开设备后返回的结构体指针
    uint16_t *tabRegisters = NULL;      //寄存器的空间
    int nbPoints;               //空间大小


    /* 根据Modbus_RTU的类型建立连接 */
    ctx = modbus_new_rtu(uartInfo->uartName, uartInfo->bandrate, UART_PARITY, UART_DATA_BIT, UART_STOP_BIT);
    if (NULL == ctx)
    {
    	printf_debug("Unable to allocate libmodbus context\n");
        return POINT_NULL;
    }

    modbus_set_debug(ctx, TRUE);        //设置Dubug模式
    modbus_set_error_recovery(ctx, MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL);

    /* 设置从机ID */
    modbus_set_slave(ctx, NOISE_SERVER_ID);

    /* 建立连接 */
    if (-1 == modbus_connect(ctx)) {
    	printf_debug("Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return FUNCTION_FAIL;
    }
    printf("Connection Successful!\r\n");

    /* 为bit和寄存器分配内存空间 */
    nbPoints = NOISE_REGISTERS_NUMBER;
    tabRegisters = (uint16_t *) malloc(nbPoints * sizeof(uint16_t));
    memset(tabRegisters, 0, nbPoints * sizeof(uint16_t));

    /* 设置硬件接口为RS232还是RS485 */
    SetSerialInterfaceValue(type);

    while (1)
    {
		modbus_read_registers(ctx, NOISE_REGISTERS_ADDRESS, NOISE_REGISTERS_NUMBER, tabRegisters);
		printf("value = %d\n", tabRegisters[0]);

		/* TODO：对数据进行解析和保存 */

		sleep(NOISE_MODBUS_INTERVAL);
    }

    /* Free the memory */
    free(tabRegisters);

    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);

	return NO_ERROR;
}


