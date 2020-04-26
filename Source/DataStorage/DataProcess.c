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
int CreateDataFile(char *filename)
{
	/* TODO:创建数据库、数据库中的表、以及表中的列等 */

	return NO_ERROR;
}


/**
 * @breif 插入数据
 * @param filename 存放数据的文件名
 * @param dataInfo 保存数据的结构体指针
 * @return void
 */
int InsertData(char *filename, DataInformation *dataInfo)
{
	/* TODO:将数据信息插入到数据库中 */

	return NO_ERROR;
}


/**
 * @breif 删除数据
 * @param filename 存放数据的文件名
 * @param dataName 要删除数据的数据名
 * @return void
 */
int DeleteData(char *filename, char *dataName)
{
	/* TODO:在数据库中删除数据 */

	return NO_ERROR;
}


/**
 * @breif 修改数据
 * @param filename 存放数据的文件名
 * @param dataInfo 保存数据的结构体指针
 * @return void
 */
int UpdateData(char *filename, DataInformation *dataInfo)
{
	/* TODO:将新的数据保存到数据库中 */

	return NO_ERROR;
}


/**
 * @breif 查找数据，返回读取到数据的值
 * @param filename 存放数据的文件名
 * @param dataInfo 保存数据的结构体指针
 * @return void
 */
int SelectData(char *filename, DataInformation *dataInfo)
{
	/* TODO:读取数据，并保存到结构体中 */

	return NO_ERROR;
}


