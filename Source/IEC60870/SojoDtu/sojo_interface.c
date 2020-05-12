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
  * @param : iecType 选择101 104协议
  * @param : num 从机个数
  * @return:
  * @updata:
  */
void iec_startup(IECType iecType, uint8_t num)
{
	ParameterConfiguration *para = NULL;
	if(iecType == IEC101)
	{
		iec101_init(para);
	}
	else if(iecType == IEC104)
	{
		iec104_init(para);
	}
	else
	{
		printf("startup fail!\r\n");
	}
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
