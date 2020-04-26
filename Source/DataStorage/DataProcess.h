/**
 * @file DataProcess.h
 * @brief 数据处理程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.26
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _DATA_PROCESS_H_
#define _DATA_PROCESS_H_

#include <stdbool.h>

#define DATA_NAME_STRING_LENTH	40
#define TIME_STRING_LENTH		20


/*
 * 数据类型
 */
typedef enum TagDataType
{
	BIT_TYPE = 0,
	INT_TYPE,
	FLOAT_TYPE,
}DataType;


/*
 * 数据的相关信息结构体
 */
typedef struct TagDataInformation
{
	char dataName[DATA_NAME_STRING_LENTH];
	int deviceId;
	int dataType;
	bool bitData;
	int intData;
	float floatData;
	char updateTime[TIME_STRING_LENTH];		//2020-04-26 11:42:20
	char mqttUserName[DATA_NAME_STRING_LENTH];

}DataInformation;


#endif


