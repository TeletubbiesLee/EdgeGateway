/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      sojo_interface.h
  * @brief:		双杰对外接口头文件
  * @version:   V1.0.0
  * @author:    txl
  * @date:      2020-04-08
  * @update:    [2020-04-08][txl][newly increased]
  */

#ifndef	_SOJO_INTERFACE_H_
#define _SOJO_INTERFACE_H_

#include "stdint.h"
#include "sojo_point_table_enum.h"
#include "../../IEC60870/iec60870_configure.h"

void iec101_startup(Configure101 *para);
void iec104_startup(Configure104 *para);

float ReadDataYc(uint8_t id, IECType iecType, YcDataType ycType);

#endif
