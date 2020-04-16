/**
 * @file RS485.h
 * @brief 使用485通信的相关程序
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.03.30
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _RS485_H_
#define _RS485_H_

typedef enum
{
	DISABLE_485 = 0,	//使能脚电平拉高
	ENABLE_485			//使能脚电平拉低
}RS485_ENABLE_t;

int RS485_Enable(const int fd, const RS485_ENABLE_t enable);

#endif

