/**
 * @file DataProcess.c
 * @brief 数据处理程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.26
 *
 * @author Lei.L
 * @version ver 1.0
 */


#include <stdlib.h>
#include <string.h>
#include "SqliteInterface.h"
#include "../Config.h"
#include "DataProcess.h"


static int CreateTableFile(char *tableName);
static int InsertTableData(char *tableName, DataInformation *dataInfo);
static int DeleteTableData(char *tableName, DataInformation *dataInfo);
static int UpdateTableData(char *tableName, DataInformation *dataInfo);
static int SelectTableData(char *tableName, DataInformation *dataInfo);



/**
 * @breif 创建数据文件
 * @param filename 数据文件的名字
 * @return 成功：0 失败：其他
 */
int CreateDataFile(char *filename)
{
	return CreateTableFile(filename);
}


/**
 * @breif 保存数据
 * @param filename 数据文件的名字
 * @param dataInfo 保存数据的结构体指针
 * @return 成功：0 失败：其他
 */
int SaveData(char *filename, DataInformation *dataInfo)
{
	DeleteTableData(filename, dataInfo);
	return InsertTableData(filename, dataInfo);
}


/**
 * @breif 删除数据
 * @param filename 数据文件的名字
 * @param dataInfo 保存数据的结构体指针
 * @return 成功：0 失败：其他
 */
int DeleteData(char *filename, DataInformation *dataInfo)
{
	return DeleteTableData(filename, dataInfo);
}


/**
 * @breif 读取数据
 * @param filename 数据文件的名字
 * @param dataInfo 保存数据的结构体指针
 * @return 成功：0 失败：其他
 */
int ReadData(char *filename, DataInformation *dataInfo)
{
	return SelectTableData(filename, dataInfo);
}


/**
 * @breif 轮询读取数据
 * @param filename 数据文件的名字
 * @param dataInfo 保存数据的结构体指针
 * @return 成功：0 失败：其他
 */
int PollData(char *filename, DataInformation *dataInfo)
{
	/* TODO:数据库轮询读取数据 */


	return NO_ERROR;
}


/**
 * @brief 获取当前时间字符串
 * @param timeStr 保存时间字符串
 * @return 成功:0 错误:-1
 */
int GetTimeStr(char *timeStr)
{
    time_t rawTime;
    struct tm *info;

    time(&rawTime);     //获取当前时间
    info = localtime(&rawTime);

    strftime(timeStr, 20, "%Y-%m-%d %H:%M:%S", info);    //时间字符串格式化

    return NO_ERROR;
}


/**
 * @breif 创建数据库的表
 * @param tableName 数据库表的名字
 * @return 成功：0 失败：其他
 */
static int CreateTableFile(char *tableName)
{
	int ret = NO_ERROR;
	//打开数据库
	ret = openSqlite(DATA_LIB_NAME);
	if (ret != NO_ERROR)
	{
		ret = FUNCTION_FAIL;
		return ret;
	}
	//创建表格
	ret = createTable(tableName);

	if (ret != NO_ERROR)
	{
		ret = FUNCTION_FAIL;
	}

	return ret;
}


/**
 * @breif 插入数据
 * @param tableName 数据库表的名字
 * @param dataInfo 保存数据的结构体指针
 * @return 成功：0 失败：其他
 */
static int InsertTableData(char *tableName, DataInformation *dataInfo)
{
	int ret = NO_ERROR;
	ret = insertRecord(tableName, dataInfo);
	if (ret != NO_ERROR)
	{
		ret = FUNCTION_FAIL;
	}
	return ret;
}


/**
 * @breif 删除数据
 * @param tableName 数据库表的名字
 * @param dataName 要删除数据的数据名
 * @return 成功：0 失败：其他
 */
static int DeleteTableData(char *tableName, DataInformation *dataInfo)
{
	int ret = NO_ERROR;
	ret = deleteRecord(tableName, dataInfo);
	if (ret != NO_ERROR)
	{
		ret = FUNCTION_FAIL;
	}
	return ret;
}


/**
 * @breif 修改数据
 * @param tableName 数据库表的名字
 * @param dataInfo 保存数据的结构体指针
 * @return 成功：0 失败：其他
 */
static int UpdateTableData(char *tableName, DataInformation *dataInfo)
{
	int ret = NO_ERROR;
	ret = updateRecord(tableName, dataInfo);
	if (ret != NO_ERROR)
	{
		ret = FUNCTION_FAIL;
	}
	return ret;
}


/**
 * @breif 查找数据，返回读取到数据的值
 * @param tableName 数据库表的名字
 * @param dataInfo 保存数据的结构体指针
 * @return 成功：0 失败：其他
 */
static int SelectTableData(char *tableName, DataInformation *dataInfo)
{
	int ret = NO_ERROR;
	ret = selectRecord(tableName, dataInfo);
	if (ret != NO_ERROR)
	{
		ret = FUNCTION_FAIL;
	}
	return ret;
}


/**
 * @breif 数据库相关函数测试
 * @param void
 * @return void
 */
void DataBaseTest(void)
{
	int ret = NO_ERROR;
	char *tableName = "deviceTable";
	DataInformation *dataInfo = NULL;
	dataInfo = (DataInformation *)malloc(sizeof(DataInformation));
	if (dataInfo == NULL)
	{
		return;
	}
	printf("CreateDataFile\r\n");
	ret = CreateTableFile(tableName);
	if (ret != NO_ERROR)
	{
		printf("CreateDataFile error\r\n");
		return;
	}
	printf("InsertData\r\n");
	strcpy(dataInfo->dataName, "temp");
	dataInfo->deviceId = 1;
	dataInfo->dataType = BIT_TYPE;
	dataInfo->bitData = true;
	strcpy(dataInfo->updateTime, "2020-04-27 8:34:52");
	strcpy(dataInfo->mqttUserName, "mqtt -b 123456");
	ret = InsertTableData(tableName, dataInfo);
	if (ret != NO_ERROR)
	{
		return;
	}
	printf("SelectData\r\n");
	ret = SelectTableData(tableName, dataInfo);
	if (ret != NO_ERROR)
	{
		return;
	}
	printf("dataType = %d bitData = %d\n",dataInfo->dataType,dataInfo->bitData);
	printf("InsertData\r\n");
	strcpy(dataInfo->dataName, "pm2.5");
	dataInfo->deviceId = 2;
	dataInfo->dataType = INT_TYPE;
	dataInfo->intData = 7;
	strcpy(dataInfo->updateTime, "2020-04-30 8:34:52");
	strcpy(dataInfo->mqttUserName, "mqtt -b 123456");
	ret = InsertTableData(tableName, dataInfo);
	if (ret != NO_ERROR)
	{
		return;
	}
	printf("SelectData\r\n");
	ret = SelectTableData(tableName, dataInfo);
	if (ret != NO_ERROR)
	{
		return;
	}
	printf("dataType = %d intData = %d\n",dataInfo->dataType,dataInfo->intData);
	printf("UpdateData\r\n");
	dataInfo->dataType = FLOAT_TYPE;
	dataInfo->floatData = 7.941234;
	ret = UpdateTableData(tableName, dataInfo);
	if (ret != NO_ERROR)
	{
		return;
	}
	printf("SelectData\r\n");
	ret = SelectTableData(tableName, dataInfo);
	if (ret != NO_ERROR)
	{
		return;
	}
	printf("dataType = %d floatData = %f\n",dataInfo->dataType,dataInfo->floatData);
	printf("DeleteData\r\n");
	strcpy(dataInfo->dataName, "temp");
	dataInfo->deviceId = 1;
	ret = DeleteTableData(tableName, dataInfo);
	if (ret != NO_ERROR)
	{
		return;
	}
}

