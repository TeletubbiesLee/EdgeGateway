/**
 * @file SojoDtu.c
 * @brief 双杰DTU通信程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.27
 *
 * @author Lei.L
 * @version ver 1.0
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/sem.h>
#include "sojo_interface.h"
#include "sojo_point_table_enum.h"
#include "SojoDtu.h"
#include "../../Config.h"
#include "../../DataStorage/DataProcess.h"
#include "../../ProcessCommunication/ProcessSignal.h"
#include "../DeviceParamConfig.h"


static void SojoDtu_SaveData(char *filename, IECType iecType, int id);


/**
 * @brief 创建101协议的进程
 * @param void
 * @return 成功：0 失败：其他
 */
int CreatIec101Process(void)
{
	if(g_EdgeGatewayConfig == NULL)
	{
		printf_debug("g_EdgeGatewayConfig is NULL!\n");
		return POINT_NULL;
	}

	/* IEC101协议需要的配置信息 */
	pid_t pid = 0;
	Configure101 info101;
	char iec101Filename[30] = {0};
	IEC101ParamConfig(g_EdgeGatewayConfig, &info101, iec101Filename);
	/* 创建101进程 */
	if(info101.num > 0 && (pid = fork()) == 0)
	{
		SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

		printf("IEC101 (pid:%d) creat\n", getpid());
		SojoDtu_IEC101(&info101, iec101Filename);
		printf("IEC101 (pid:%d) exit\n", getpid());

		return NO_ERROR;
	}
	return NO_ERROR;
}


/**
 * @brief 创建104协议的进程
 * @param void
 * @return 成功：0 失败：其他
 */
int CreatIec104Process(void)
{
	if(g_EdgeGatewayConfig == NULL)
	{
		printf_debug("g_EdgeGatewayConfig is NULL!\n");
		return POINT_NULL;
	}

	/* IEC104协议需要的配置信息 */
	bool isError = true;
	pid_t pid = 0;
	Configure104 info104;
	char iec104Filename[30] = {0};
	for(int i = 0; i < IEC104_SLAVE_MAX; i++)
	{
		if((info104.sMip[i] = malloc(sizeof(char) * 20)) == NULL)
		{
			printf_debug("info104.sMip[%d] malloc error\n", i);
			isError = false;
			break;
		}
	}

	if(false == isError)
	{
		FreePointArray((void**)info104.sMip, PROTOCOL_MAX_PROCESS);
		return MALLOC_FAIL;
	}

	IEC104ParamConfig(g_EdgeGatewayConfig, &info104, iec104Filename);
	/* 创建104进程 */
	if(info104.num > 0 && (pid = fork()) == 0)
	{
		SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

		printf("IEC104 (pid:%d) creat\n", getpid());
		SojoDtu_IEC104(&info104, iec104Filename);
		printf("IEC104 (pid:%d) exit\n", getpid());

		return NO_ERROR;
	}
	FreePointArray((void**)info104.sMip, PROTOCOL_MAX_PROCESS);
	return NO_ERROR;
}


/**
 * @breif 双杰DTU的101通信
 * @param void
 * @return 成功：0 失败：-1
 */
int SojoDtu_IEC101(Configure101 *info101, char *filename)
{
	int semId = 0;

	iec101_startup(info101);

	semId = semget((key_t)SEMAPHORE_KEY, 1, 0666 | IPC_CREAT);

	if(Semaphore_P(semId) == NO_ERROR)
	{
		if(0 != CreateDataFile(filename))
		{
			printf_debug("CreateDataFile(\"%s\") error\n", filename);
		}
		printf("CreateDataFile(\"%s\") success\n", filename);
		if(Semaphore_V(semId) != NO_ERROR)
			printf_debug("SojoDtu_IEC101: Semaphore_V error\n");
	}
	else
		printf_debug("SojoDtu_IEC101: Semaphore_P error\n");

	while(1)
	{
		for(int i = 1; i <= info101->num; i++)
		{
			sleep(30);
			printf("SojoDtu_IEC101: sleep end\n");
			/* 对数据进行解析和保存 */
			if(Semaphore_P(semId) == NO_ERROR)
			{
				printf("SojoDtu_IEC101: Semaphore_P success\n");
				SojoDtu_SaveData(filename, IEC101, i);
				if(Semaphore_V(semId) == NO_ERROR)
					printf("SojoDtu_IEC101: Semaphore_V success\n");
			}
		}
		sleep(IEC101_INTERVAL);
	}
	return NO_ERROR;

}


/**
 * @breif 双杰DTU的104通信
 * @param void
 * @return 成功：0 失败：-1
 */
int SojoDtu_IEC104(Configure104 *info104, char *filename)
{
	int semId = 0;

	iec104_startup(info104);

	semId = semget((key_t)SEMAPHORE_KEY, 1, 0666 | IPC_CREAT);

	if(Semaphore_P(semId) == NO_ERROR)
	{
		if(0 != CreateDataFile(filename))
		{
			printf_debug("CreateDataFile(\"%s\") error\n", filename);
		}
		printf("CreateDataFile(\"%s\") success\n", filename);
		if(Semaphore_V(semId) != NO_ERROR)
			printf_debug("SojoDtu_IEC104: Semaphore_V error\n");
	}
	else
		printf_debug("SojoDtu_IEC104: Semaphore_P error\n");

	while(1)
	{
		for(int i = 1; i <= info104->num; i++)
		{
			sleep(30);
			printf("SojoDtu_IEC104: sleep end\n");
			/* 对数据进行解析和保存 */
			if(Semaphore_P(semId) == NO_ERROR)
			{
				printf("SojoDtu_IEC104: Semaphore_P success\n");
				SojoDtu_SaveData(filename, IEC104, i);
				if(Semaphore_V(semId) == NO_ERROR)
					printf("SojoDtu_IEC104: Semaphore_V success\n");
			}
		}
		sleep(IEC104_INTERVAL);
	}
	return NO_ERROR;

}


/**
 * @breif 双杰DTU的数据保存
 * @param filename 文件名
 * @param iecType 协议类型，101或104
 * @param id ID号
 * @return void
 */
static void SojoDtu_SaveData(char *filename, IECType iecType, int id)
{
	float num = 0.0;
	DataInformation dataInfo;

	dataInfo.deviceId = id;
	dataInfo.dataType = FLOAT_TYPE;
	strcpy(dataInfo.mqttUserName, filename);

	num = ReadDataYc(id, iecType, YCDATA_DC1);
	strcpy(dataInfo.dataName, "YCDATA_DC1");
	dataInfo.floatData = num;
	GetTimeStr(dataInfo.updateTime);
	SaveData(filename, &dataInfo);
	printf("%s: %s - %f\n", filename, dataInfo.dataName, dataInfo.floatData);

	num = ReadDataYc(id, iecType, YCDATA_DC2);
	strcpy(dataInfo.dataName, "YCDATA_DC2");
	dataInfo.floatData = num;
	GetTimeStr(dataInfo.updateTime);
	SaveData(filename, &dataInfo);
	printf("%s: %s - %f\n", filename, dataInfo.dataName, dataInfo.floatData);
}





