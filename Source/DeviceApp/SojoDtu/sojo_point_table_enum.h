/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      sojo_point_table_enum.h
  * @brief:		双杰点表文件
  * @version:   V1.0.0
  * @author:    txl
  * @date:      2020-04-08
  * @update:    [2020-04-08][txl][newly increased]
  */

#ifndef	_SOJO_POINT_TABLE_INTERFACE_H_
#define _SOJO_POINT_TABLE_INTERFACE_H_

typedef enum TagIECType{
	IEC101 = 1,
	IEC104,
}IECType;

typedef enum TagYcDataType{
	YCDATA_F = 0,                      // 频率
	YCDATA_Ia,                         // A相电流
	YCDATA_Ib,                         // B相电流
	YCDATA_Ic,                         // C相电流
	YCDATA_I0,                         // 零序电流
	YCDATA_cIa = 5,
	YCDATA_cIb,
	YCDATA_cIc,
	YCDATA_Uab,                        // 线电压Uab
	YCDATA_Ucb,                        // 线电压Ucb
	YCDATA_Uac = 10,                        // 线电压Uac
	YCDATA_U0,                         // 零序电压
	YCDATA_UAB,                        // 线电压UAB
	YCDATA_UCB,                        // 线电压UCB
	YCDATA_P,                          // 有功功率
	YCDATA_Q = 15,                          // 无功功率
	YCDATA_S,                          // 视在功率
	YCDATA_PF,                         // 功率因数
	YCDATA_DC1,                        // 直流电压
	YCDATA_DC2                         // 直流电压2
}YcDataType;

#endif
