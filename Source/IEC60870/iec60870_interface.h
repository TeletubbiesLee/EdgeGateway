﻿/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      iec60870_interface.h
  * @brief:		通用对外接口头文件
  * @version:   V1.0.0
  * @author:    txl
  * @date:      2020-04-02
  * @update:    [2020-04-02][txl][newly increased]
  */

#ifndef	_IEC60870_INTERFACE_H_
#define _IEC60870_INTERFACE_H_

#include "stdint.h"
#include "../DeviceApp/SojoDtu/sojo_point_table_enum.h"
#include "iec60870_configure.h"

void iec101_init(Configure101 *para);
void iec104_init(Configure104 *para);
float ReadYcData(uint8_t id, IECType iecType, YcDataType ycType);

#endif
