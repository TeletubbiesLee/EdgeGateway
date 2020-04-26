/**
 * @file modbus-485.h
 * @brief modbus使用485通信的相关程序
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.03.30
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _MODBUS_485_H_
#define _MODBUS_485_H_

typedef enum
{
	DISABLE_485 = 0,
	ENABLE_485
}RS485_ENABLE_t;

int rs485_enable(const int fd, const RS485_ENABLE_t enable);

#endif

