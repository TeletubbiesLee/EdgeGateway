/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      sojo_interface.c
  * @brief:		双杰对外接口文件
  * @version:   V1.0.0
  * @author:    txl
  * @date:      2020-04-08
  * @update:    [2020-04-08][txl][newly increased]
  */

#include <stdio.h>
#include "../iec60870/iec60870_interface.h"
#include "../iec60870/iec60870_configure.h"
#include "sojo_interface.h"

/**
  * @brief :
  * @param : para 101配置参数结构体
  * @return:
  * @updata:
  */
void iec101_startup(Configure101 *para)
{
	if (para == NULL)
	{
		return;
	}
	iec101_init(para);
}
/**
  * @brief :
  * @param : para 104配置参数结构体
  * @return:
  * @updata:
  */
void iec104_startup(Configure104 *para)
{
	if (para == NULL)
	{
		return;
	}
	iec104_init(para);
}
/**
  * @brief :
  * @param : id 从机id
  * @param : type 选择101 104协议
  * @param : ycType 遥测号
  * @return:
  * @updata:
  */
float ReadDataYc(uint8_t id, IECType iecType, YcDataType ycType)
{
	return (ReadYcData(id, iecType, ycType));
}
