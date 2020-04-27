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


#include "../Config.h"
#include "DataProcess.h"


/**
 * @breif 创建数据文件
 * @param filename 数据文件的名字
 * @return void
 */
int CreateDataFile(char *tableName)
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
 * @param filename 存放数据的文件名
 * @param dataInfo 保存数据的结构体指针
 * @return void
 */
int InsertData(char *tableName, DataInformation *dataInfo)
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
 * @param filename 存放数据的文件名
 * @param dataName 要删除数据的数据名
 * @return void
 */
int DeleteData(char *tableName, DataInformation *dataInfo)
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
 * @param filename 存放数据的文件名
 * @param dataInfo 保存数据的结构体指针
 * @return void
 */
int UpdateData(char *tableName, DataInformation *dataInfo)
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
 * @param filename 存放数据的文件名
 * @param dataInfo 保存数据的结构体指针
 * @return void
 */
int SelectData(char *tableName, DataInformation *dataInfo)
{
	int ret = NO_ERROR;
	ret = selectRecord(tableName, dataInfo);
	if (ret != NO_ERROR)
	{
		ret = FUNCTION_FAIL;
	}
	return ret;
}

void test(void)
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
	ret = CreateDataFile(tableName);
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
	ret = InsertData(tableName, dataInfo);
	if (ret != NO_ERROR)
	{
		return;
	}
	printf("SelectData\r\n");
	ret = SelectData(tableName, dataInfo);
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
	ret = InsertData(tableName, dataInfo);
	if (ret != NO_ERROR)
	{
		return;
	}
	printf("SelectData\r\n");
	ret = SelectData(tableName, dataInfo);
	if (ret != NO_ERROR)
	{
		return;
	}
	printf("dataType = %d intData = %d\n",dataInfo->dataType,dataInfo->intData);
	printf("UpdateData\r\n");
	dataInfo->dataType = FLOAT_TYPE;
	dataInfo->floatData = 7.941234;
	ret = UpdateData(tableName, dataInfo);
	if (ret != NO_ERROR)
	{
		return;
	}
	printf("SelectData\r\n");
	ret = SelectData(tableName, dataInfo);
	if (ret != NO_ERROR)
	{
		return;
	}
	printf("dataType = %d floatData = %f\n",dataInfo->dataType,dataInfo->floatData);
	printf("DeleteData\r\n");
	strcpy(dataInfo->dataName, "temp");
	dataInfo->deviceId = 1;
	ret = DeleteData(tableName, dataInfo);
	if (ret != NO_ERROR)
	{
		return;
	}
}

