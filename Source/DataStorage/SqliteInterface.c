/**
 * @file SqliteInterface.c
 * @brief 数据库对外接口
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.26
 *
 * @author txl
 * @version ver 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "DataProcess.h"
#include "SqliteInterface.h"
#include "./sqlite/sqlite3.h"

sqlite3 *db;

/**
 * @breif 字符串转换浮点型
 * @param str 字符串
 * @return 浮点型数
 */
float stringToFloat(char *str)
{
/*
   flag 用于标记小数点位置
   flag1 用于标记字符串的正负
   result拥有存储结果
   p 用于保存当前以访问多少位
   q 用于保存共有几位小数
*/
    int i, n, p, flag, q, flag1;
    float result;

    n = strlen(str);
    result = 0;
    p = 1;
    q = 1;
    flag = 0;
    flag1 = 1;
    for (i = n - 1; i >= 0; i--)
    {
    	//用于跳过最右端多余的0
        if (i == (n - 1))
        {
            while (str[i] == '0')
            {
                i--;
            }
        }
        if (str[i] == '.')
        {
            flag = 1;
            continue;
        }
        if (flag == 0)
        {
            q *= 10;
        }
        if (i == 0 && (str[i] == '+' || str[i] == '-'))
        {
            if (str[i] == '-')
            {
                flag1 = -1;
            }
            continue;
        }
        result += p * (str[i] -'0');
        p *= 10;
    }
    return flag1*result/q;
}
/**
 * @breif 打开数据库
 * @param 无
 * @return void
 */
int openSqlite(char *fileName)
{
	db = NULL;
	int ret = sqlite3_open(fileName,&db);
	if(ret)
	{
		fprintf(stderr,"can't open database: %s\n",sqlite3_errmsg(db));
		sqlite3_close(db);
		return ret;
	}
	else
	{
		printf("You have opened a sqlite3 database successfully!\n");
	}
	return ret;
}
/**
 * @breif 创建表
 * @param filename 表名
 * @return void
 */
int createTable(char *tableName)
{
	char sql[SQL_SIZE] = "";				//sql语句
	char *zErrMsg = 0;

	if (db == NULL)
	{
		return SQLITE_ERROR;
	}
	sprintf(sql, "CREATE TABLE %s(ID INTEGER PRIMARY KEY,dataName VARCHAR(20),deviceId INTERER,dataType INTERER,dataValue REAL,updateTime VARCHAR(20),mqttUserName VARCHAR(20));", tableName);
	if(SQLITE_OK != sqlite3_exec(db,sql,0,0,&zErrMsg))
	{
		printf("operate failed: %s\n",zErrMsg);
		return SQLITE_ERROR;
	}
	return SQLITE_OK;
}
/**
 * @breif 插入数据
 * @param filename 存放数据的文件名
 * @param dataInfo 数据接口
 * @return void
 */
int insertRecord(char *tableName, DataInformation *dataInfo)
{
	char sql[SQL_SIZE] = "";				//sql语句
	char *zErrMsg = 0;
	if (db == NULL)
	{
		return SQLITE_ERROR;
	}
	char *dataName = dataInfo->dataName;
	int deviceId = dataInfo->deviceId;
	int dataType = dataInfo->dataType;
	float bitData;
	if (dataInfo->bitData)
	{
		bitData = 1.000000;
	}
	else
	{
		bitData = 0.000000;
	}
	float intData = (float)dataInfo->intData;
	float floatData = dataInfo->floatData;
	char *updateTime = dataInfo->updateTime;		//2020-04-26 11:42:20
	char *mqttUserName = dataInfo->mqttUserName;
	if (dataType == BIT_TYPE)
	{
		sprintf(sql, "INSERT INTO '%s' VALUES(NULL,'%s',%d,%d,%f,'%s','%s');", tableName, dataName, deviceId, dataType, bitData, updateTime, mqttUserName);
	}
	else if(dataType == INT_TYPE)
	{
		sprintf(sql, "INSERT INTO '%s' VALUES(NULL,'%s',%d,%d,%f,'%s','%s');", tableName, dataName, deviceId, dataType, intData, updateTime, mqttUserName);
	}
	else if(dataType == FLOAT_TYPE)
	{
		sprintf(sql, "INSERT INTO '%s' VALUES(NULL,'%s',%d,%d,%f,'%s','%s');", tableName, dataName, deviceId, dataType, floatData, updateTime, mqttUserName);
	}
	else
	{
		printf("data type error!\n");
		return SQLITE_ERROR;
	}

	if(SQLITE_OK != sqlite3_exec(db,sql,0,0,&zErrMsg))
	{
		printf("operate failed: %s\n",zErrMsg);
		return SQLITE_ERROR;
	}
	return SQLITE_OK;
}
/**
 * @breif 删除数据
 * @param tableName 表名
 * @param dataInfo 数据接口
 * @return void
 */
int deleteRecord(char *tableName, DataInformation *dataInfo)
{
	char sql[SQL_SIZE] = "";
	char *zErrMsg;
	if (db == NULL)
	{
		return SQLITE_ERROR;
	}
	char *dataName = dataInfo->dataName;
	int deviceId = dataInfo->deviceId;
	sprintf(sql, "DELETE FROM %s WHERE dataName='%s' AND deviceId=%d;",tableName, dataName, deviceId);
	if(SQLITE_OK != sqlite3_exec(db,sql,0,0,&zErrMsg))
	{
		printf("operate failed: %s\n",zErrMsg);
		return SQLITE_ERROR;
	}
	return SQLITE_OK;
}
/**
 * @breif 更改数据
 * @param tableName 表名
 * @param dataInfo 数据接口
 * @return void
 */
int updateRecord(char *tableName, DataInformation *dataInfo)
{
	char sql[SQL_SIZE] = "";
	char *zErrMsg;
	if (db == NULL)
	{
		return SQLITE_ERROR;
	}
	char *dataName = dataInfo->dataName;
	int deviceId = dataInfo->deviceId;
	int dataType = dataInfo->dataType;
	float dataValue;
	if (dataType == BIT_TYPE)
	{
		if (dataInfo->bitData)
		{
			dataValue = 1.000000;
		}
		else
		{
			dataValue = 0.000000;
		}
	}
	else if(dataType == INT_TYPE)
	{
		dataValue = (float)dataInfo->intData;
	}
	else if(dataType == FLOAT_TYPE)
	{
		dataValue = dataInfo->floatData;
	}
	else
	{
		printf("data type error!\n");
		return SQLITE_ERROR;
	}
	sprintf(sql, "UPDATE %s SET dataType=%d WHERE dataName='%s' AND deviceId=%d;", tableName, dataType, dataName, deviceId);
	if(SQLITE_OK != sqlite3_exec(db,sql,0,0,&zErrMsg))
	{
		printf("operate failed: %s\n",zErrMsg);
		return SQLITE_ERROR;
	}
	sprintf(sql, "UPDATE %s SET dataValue=%f WHERE dataName='%s' AND deviceId=%d;", tableName, dataValue, dataName, deviceId);
	if(SQLITE_OK != sqlite3_exec(db,sql,0,0,&zErrMsg))
	{
		printf("operate failed: %s\n",zErrMsg);
		return SQLITE_ERROR;
	}
	return SQLITE_OK;
}
/**
 * @breif 查找数据
 * @param tableName 表名
 * @param dataInfo 数据接口
 * @return void
 */
int selectRecord(char *tableName, DataInformation *dataInfo)
{
	int nrow = 0,ncolumn = 0;
	int i;
	char **azResult=0;
	char sql[SQL_SIZE] = "";
	char *zErrMsg;
	if (db == NULL)
	{
		return SQLITE_ERROR;
	}
	char *dataName = dataInfo->dataName;
	int deviceId = dataInfo->deviceId;
	int dataType;

	sprintf(sql, "SELECT * FROM %s WHERE dataName='%s' AND deviceId=%d;", tableName, dataName, deviceId);
	if(SQLITE_OK != sqlite3_get_table(db,sql,&azResult,&nrow,&ncolumn,&zErrMsg))
	{
		printf("operate failed: %s\n",zErrMsg);
	}

	for (i = 0; i < ( nrow + 1) * ncolumn; i++)
	{
		printf("azResult[%d] = %s\n",i,azResult[i]);
	}
	int bitData = 0;
	dataInfo->dataType = atoi(azResult[10]);
	dataType = dataInfo->dataType;
	if (dataType == BIT_TYPE)
	{
		dataInfo->bitData = atoi(azResult[11]);
		if (bitData == 0)
		{
			dataInfo->bitData = true;
		}
		else
		{
			dataInfo->bitData = false;
		}
	}
	else if(dataType == INT_TYPE)
	{
		dataInfo->intData = atoi(azResult[11]);
	}
	else if(dataType == FLOAT_TYPE)
	{
		dataInfo->floatData = (float)stringToFloat(azResult[11]);
	}
	else
	{
		printf("data type error!\n");
		return SQLITE_ERROR;
	}

	sqlite3_free_table(azResult);
	return SQLITE_OK;
}
/**
 * @breif 查找所有数据
 * @param tableName 表名
 * @param allData 数据存放结构体
 * @param rowNum 行数
 * @return void
 */
int pollAllRecord(char *tableName, PollDataInformation *allData, int *rowNum)
{
	int nrow = 0,ncolumn = 0;
	int i, j;
	char **azResult=0;
	char sql[SQL_SIZE] = "";
	char *zErrMsg;
	if (db == NULL)
	{
		return SQLITE_ERROR;
	}

	sprintf(sql, "SELECT * FROM %s;", tableName);
	if(SQLITE_OK != sqlite3_get_table(db,sql,&azResult,&nrow,&ncolumn,&zErrMsg))
	{
		printf("operate failed: %s\n",zErrMsg);
	}
	allData = (PollDataInformation *)malloc(nrow * sizeof(PollDataInformation));
	if (allData == NULL)
	{
		printf("PollDataInformation malloc fail!\n");
		return SQLITE_ERROR;
	}
	memset(allData, 0, nrow * sizeof(PollDataInformation));
	*rowNum = nrow;
	for (i = 0; i < nrow; i++)
	{
		strncpy(allData[i].dataName, azResult[ncolumn*i], strlen(azResult[ncolumn*i]) + 1);
		strncpy(allData[i].deviceId, azResult[ncolumn*i+1], strlen(azResult[ncolumn*i+1]) + 1);
		strncpy(allData[i].dataType, azResult[ncolumn*i+2], strlen(azResult[ncolumn*i+2]) + 1);
		strncpy(allData[i].bitData, azResult[ncolumn*i+3], strlen(azResult[ncolumn*i+3]) + 1);
		strncpy(allData[i].intData, azResult[ncolumn*i+4], strlen(azResult[ncolumn*i+4]) + 1);
		strncpy(allData[i].floatData, azResult[ncolumn*i+5], strlen(azResult[ncolumn*i+5]) + 1);
		strncpy(allData[i].updateTime, azResult[ncolumn*i+6], strlen(azResult[ncolumn*i+6]) + 1);
		strncpy(allData[i].mqttUserName, azResult[ncolumn*i+7], strlen(azResult[ncolumn*i+7]) + 1);
	}
	sqlite3_free_table(azResult);
	return SQLITE_OK;
}
/**
 * @breif 释放内存
 * @param allData 数据存放结构体
 * @return void
 */
void freePollDataInformation(PollDataInformation *allData)
{
	free(allData);
	allData = NULL;
}
