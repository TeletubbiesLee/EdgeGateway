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

#include "sojo_point_table_enum.h"
#include "stdint.h"

void iec_startup(IECType iecType, uint8_t num);

float ReadDataYc(uint8_t id, IECType iecType, YcDataType ycType);

#endif
