/**
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
#include "../SojoDtu/sojo_point_table_enum.h"

void iec101_init(uint8_t num);
void iec104_init(uint8_t num);
float ReadYcData(uint8_t id, IECType iecType, YcDataType ycType);

#endif
