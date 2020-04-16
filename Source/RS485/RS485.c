/**
 * @file RS485.c
 * @brief 使用485通信的相关程序
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.03.30
 *
 * @author Lei.L
 * @version ver 1.0
 */

#include <stdio.h>
#include <linux/serial.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "RS485.h"

/**
 * @fn RS485_Enable
 * @brief 485使能函数
 * @param fd 485设备号
 * @param enable 是否使能
 * @return 成功:0 错误:其他
 */
int RS485_Enable(const int fd, const RS485_ENABLE_t enable)
{
	struct serial_rs485 rs485conf;
	int res;

	/* Get configure from device */
	res = ioctl(fd, TIOCGRS485, &rs485conf);
	if(res < 0)
	{
		perror("Ioctl error on getting 485 configure:");
		close(fd);
		return res;
	}

	/* Set enable/disable to configure */
	if(enable)			// Enable rs485 mode
	{
		rs485conf.flags |= SER_RS485_ENABLED;
	}
	else        		// Disable rs485 mode
	{
		rs485conf.flags &= ~(SER_RS485_ENABLED);
	}

	rs485conf.delay_rts_before_send = 0x00000004;

	/* Set configure to device */
	res = ioctl(fd, TIOCSRS485, &rs485conf);
	if(res < 0)
	{
		perror("Ioctl error on setting 485 configure:");
		close(fd);
	}

	return res;
}
