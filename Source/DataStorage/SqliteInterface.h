/**
 * @file SqliteInterface.h
 * @brief 数据库对外接口头文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.26
 *
 * @author txl
 * @version ver 1.0
 */

#ifndef _SQLITE_INTERFACE_H_
#define _SQLITE_INTERFACE_H_

#include <stdbool.h>
#include "./sqlite/sqlite3.h"
#include "DataProcess.h"

//SQL语句长度
#define  SQL_SIZE  256


typedef struct TagPollDataInformation
{
	char id[4];
	char dataName[DATA_NAME_STRING_LENTH];
	char deviceId[8];
	char dataType[4];
	char dataValue[20];
	char updateTime[TIME_STRING_LENTH];
	char mqttUserName[DATA_NAME_STRING_LENTH];

}PollDataInformation;

int openSqlite(char *fileName);
int createTable(char *tableName);
int insertRecord(char *tableName, DataInformation *dataInfo);
int deleteRecord(char *tableName, DataInformation *dataInfo);
int updateRecord(char *tableName, DataInformation *dataInfo);
int selectRecord(char *tableName, DataInformation *dataInfo);
int pollAllRecord(char *tableName, PollDataInformation **allData, int *rowNum);
void freePollDataInformation(PollDataInformation *allData);
#endif


