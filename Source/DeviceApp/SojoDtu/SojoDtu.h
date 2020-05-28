/**
 * @file SojoDtu.h
 * @brief 双杰DTU通信程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.27
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _SOJO_DTU_H_
#define _SOJO_DTU_H_

#include "../../IEC60870/iec60870_configure.h"

#define IEC101_INTERVAL		10
#define IEC104_INTERVAL		10

int CreatIec101Process(void);
int CreatIec104Process(void);
int SojoDtu_IEC101(Configure101 *info101, char *filename);
int SojoDtu_IEC104(Configure104 *info104, char *filename);


#endif


