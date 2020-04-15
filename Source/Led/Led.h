/**
 * @file Led.h
 * @brief Led指示灯函数程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.15
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _LED_H_
#define _LED_H_

#include <stdbool.h>

#define LED_ON		0			//LED亮
#define LED_OFF		!LED_ON		//LED灭

void IndicatorLedRunning(void);
void IndicatorLedOnOrOff(bool onOrOff);

#endif

