/**
 * @file Led.c
 * @brief Led指示灯函数程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.15
 *
 * @author Lei.L
 * @version ver 1.0
 */


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include "Led.h"


/**
 * @breif 程序运行指示灯闪烁
 * @param void
 * @return void
 */
void IndicatorLedRunning(void)
{
	IndicatorLedOnOrOff(LED_ON);
	sleep(LED_DELAY_INTERVAL);
	IndicatorLedOnOrOff(LED_OFF);
	sleep(LED_DELAY_INTERVAL);
}


/**
 * @breif 程序运行指示灯亮或者灭
 * @param void
 * @return void
 */
void IndicatorLedOnOrOff(bool onOrOff)
{
	int fd = open(LED130_GPIO_VAL, O_RDWR);

	if(LED_ON == onOrOff)
		write(fd, "0", sizeof("0"));
	else if(LED_OFF == onOrOff)
		write(fd, "1", sizeof("1"));

	close(fd);
}



