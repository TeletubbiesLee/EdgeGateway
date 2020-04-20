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
	if(LED_ON == onOrOff)
		system("echo \"0\" > /sys/class/gpio/gpio130/value");
	else if(LED_OFF == onOrOff)
		system("echo \"1\" > /sys/class/gpio/gpio130/value");
}



