/**
 * @file ModbusInit.c
 * @brief modbus初始化相关程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.24
 *
 * @author Lei.L
 * @version ver 1.0
 */

#include <errno.h>
#include "ModbusInit.h"
#include "../Config.h"

/**
 * @breif 噪声传感器modbus通信函数
 * @param ctx modbus_t结构体指针的地址
 * @param uartInfo 串口信息结构体指针
 * @return 成功:0  失败:-1
 */
int ModbusInit(modbus_t **ctx, UartInfo *uartInfo)
{
	/* 根据Modbus_RTU的类型建立连接 */
	*ctx = modbus_new_rtu(uartInfo->uartName, uartInfo->bandrate, UART_PARITY, UART_DATA_BIT, UART_STOP_BIT);
	if (NULL == *ctx)
	{
		printf_debug("Unable to allocate libmodbus context\n");
		return POINT_NULL;
	}
#if MODBUS_TX_RX_DEBUG
	modbus_set_debug(*ctx, TRUE);        //设置Dubug模式
#endif
	modbus_set_error_recovery(*ctx, MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL);

	/* 建立连接 */
	if (-1 == modbus_connect(*ctx)) {
		printf_debug("Connection failed: %s\n", modbus_strerror(errno));
		modbus_free(*ctx);
		return FUNCTION_FAIL;
	}
	printf("Connection Successful!\r\n");

	/* 设置Modbus为使用RS485 */
	if(RS485_TYPE == uartInfo->uartType)
	{
		if(-1 == modbus_rtu_set_serial_mode(*ctx, MODBUS_RTU_RS485))
		{
			printf_debug("modbus_rtu_set_serial_mode() set RS485 error\n");
			return FUNCTION_FAIL;
		}
		printf("%s()-modbus enable %s 485\n", __FUNCTION__, uartInfo->uartName);
	}
	return NO_ERROR;
}

