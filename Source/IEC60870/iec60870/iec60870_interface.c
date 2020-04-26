/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      iec60870_interface.c
  * @brief:		通用对外接口头文件
  * @version:   V1.0.0
  * @author:    txl
  * @date:      2020-04-02
  * @update:    [2020-04-02][txl][newly increased]
  */

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>
#include <time.h>
#include "./port/iec_interface.h"
#include "./common/common.h"
#include "mysem/mysem.h"
#include "./point/dataSwitch_point_table.h"
#include "iec60870_interface.h"

static struct sIecParam_t sParam101;
static struct sMasterParam *mParam101;
static struct sIecParam_t sParam104;
static struct sMasterParam *mParam104;

struct tagOverallCfg g_tagOverallCfg;
long int startTime;

int getCurrentTime(ds_privateTime_t pTime);

static const char *ipAddr[] =
{
	"192.168.1.221",
	"192.168.1.222",
};

void iec101_init(uint8_t num)
{
	time_t ti;
	struct tm * pTM;
	time(&ti);
	pTM = localtime(&ti);
	printf("year  : %d\r\n", pTM->tm_year);
	printf("month : %d\r\n", pTM->tm_mon);
	printf("day   : %d\r\n", pTM->tm_mday);
	printf("week  : %d\r\n", pTM->tm_wday);
	printf("hour  : %d\r\n", pTM->tm_hour);
	printf("minute: %d\r\n", pTM->tm_min);
	printf("sec   : %d\r\n", pTM->tm_sec);

	struct timeval tv;
	gettimeofday(&tv,NULL);
	startTime = tv.tv_sec*1000 + tv.tv_usec/1000;


	mysem_init();			//初始化信号量
	sParam101.stype = 3;

	sParam101.portNo = 2; 		/*串口号 0—2*/
	sParam101.baudRate = 2;		/*波特率 0: 2400 4800 9600 38400 115200*/
	sParam101.parity = 0;	    /*奇偶校验 0无 1奇 2偶*/

	sParam101.ip = "192.168.1.1";   /*IP地址(点分十进制)*/		//本机IP
	sParam101.LocalNetPort = 1;	/*本机网口 1 2*/
	sParam101.ModuleSN = 0;	    /*模块ID号*/
	sParam101.encrypt = 0;		/*是否加密 0不加密 1加密*/
	sParam101.balanMode = 0;		/*平衡or非平衡 1平衡*/
	sParam101.sourceAddr = 1; 	/*链路地址*/
	sParam101.linkAddrSize = 2;	/*链路地址长度 1 2*/
	sParam101.ASDUCotSize = 2;	/*传送原因长度*/
	sParam101.ASDUAddr = 1;		/*ASDU地址*/
	sParam101.ASDUAddrSize = 2;	/*ASDU地址长度 1 2*/
	sParam101.InfoAddrSize = 2;	/*信息体地址长度 2 3*/ //101为2 104为3

	g_tagOverallCfg.fun.get_currentTime_info = getCurrentTime;	//当前时间初始化
	g_tagOverallCfg.def.switch_OFF = 1;

	sParam101.allInfoDisk = &g_tagOverallCfg; //全局配置结构
	uint8_t state = 1;
	mParam101 = (struct sMasterParam *)malloc(num * sizeof(struct sMasterParam));
	if (mParam101 == NULL)
	{
		printf("101 init error\r\n");
		return;
	}
	if(iec_init(&sParam101) == 3)
	{
		int i = 0;
		while(i < num)
		{
			((struct sMasterParam *)(mParam101 + i))->module_id = i + 1;                         /* 模块ID */
			((struct sMasterParam *)(mParam101 + i))->sourceAddr = i + 1; 	                    /* 链路地址 */
//			struct tagSwitchQuantityIn communication_state;  /* 通讯状态 */
//			communication_state.pValue = &state;
			((struct sMasterParam *)(mParam101 + i))->communication_state.pValue = &state;
			((struct sMasterParam *)(mParam101 + i))->YX_len = 150;                            /* 遥信长度 */
			((struct sMasterParam *)(mParam101 + i))->YC_len = 100;                            /* 遥测长度 */
			((struct sMasterParam *)(mParam101 + i))->p_YC_data = (tagAnalogQuantityIn_t)malloc(100 * sizeof(struct tagAnalogQuantityIn));
			int j = 0;
			while (j < ((struct sMasterParam *)(mParam101 + i))->YC_len)
			{
				((tagAnalogQuantityIn_t)(((struct sMasterParam *)(mParam101 + i))->p_YC_data + j))->pValue = (float *)malloc(sizeof(float));
				if (((tagAnalogQuantityIn_t)(((struct sMasterParam *)(mParam101 + i))->p_YC_data + j))->pValue == NULL)
				{
					printf("error\r\n");
				}
				memset(((tagAnalogQuantityIn_t)(((struct sMasterParam *)(mParam101 + i))->p_YC_data + j))->pValue, 0, 1);
				j++;
			}

			((struct sMasterParam *)(mParam101 + i))->YK_len = 11;                            /* 遥控长度 */
			((struct sMasterParam *)(mParam101 + i))->portNo = 3; 		                    /* 串口号0关闭1开启 */
			((struct sMasterParam *)(mParam101 + i))->netEn = 0; 		                    /* 网口使用0关闭1开启 */
			((struct sMasterParam *)(mParam101 + i))->ip = ipAddr[i];           			/*IP地址(点分十进制)*/   //设备IP
			((struct sMasterParam *)(mParam101 + i))->yk_type = 1;		                    /* 遥控单双点 */

			MasterModleParam(((struct sMasterParam *)(mParam101 + i)));					//从站链路地址为1
			i++;
		}
	}

	if(iec_start(3) == TRUE)
	{
		printf("success\r\n");
		return;
	}
}

void iec104_init(uint8_t num)
{
	time_t ti;
	struct tm * pTM;
	time(&ti);
	pTM = localtime(&ti);
	printf("year  : %d\r\n", pTM->tm_year);
	printf("month : %d\r\n", pTM->tm_mon);
	printf("day   : %d\r\n", pTM->tm_mday);
	printf("week  : %d\r\n", pTM->tm_wday);
	printf("hour  : %d\r\n", pTM->tm_hour);
	printf("minute: %d\r\n", pTM->tm_min);
	printf("sec   : %d\r\n", pTM->tm_sec);


	struct timeval tv;
	gettimeofday(&tv,NULL);
	startTime = tv.tv_sec*1000 + tv.tv_usec/1000;

	mysem_init();			//初始化信号量
	sParam104.stype = 4; 		/*协议类型 0代表s101 1代表s104*/

	sParam104.portNo = 0; 		/*串口号 0—2*/
	sParam104.baudRate = 0;		/*波特率 0: 2400 4800 9600 38400 115200*/
	sParam104.parity = 0;	    /*奇偶校验 0无 1奇 2偶*/

	sParam104.ip = "192.168.1.1";   /*IP地址(点分十进制)*/		//本机IP
	sParam104.LocalNetPort = 1;	/*本机网口 1 2*/
	sParam104.ModuleSN = 0;	    /*模块ID号*/
	sParam104.encrypt = 0;		/*是否加密 0不加密 1加密*/
	sParam104.balanMode = 1;		/*平衡or非平衡 1平衡*/
	sParam104.sourceAddr = 1; 	/*链路地址*/
	sParam104.linkAddrSize = 2;	/*链路地址长度 1 2*/
	sParam104.ASDUCotSize = 2;	/*传送原因长度*/
	sParam104.ASDUAddr = 1;		/*ASDU地址*/
	sParam104.ASDUAddrSize = 2;	/*ASDU地址长度 1 2*/
	sParam104.InfoAddrSize = 3;	/*信息体地址长度 2 3*/ //101为2 104为3

	g_tagOverallCfg.fun.get_currentTime_info = getCurrentTime;	//当前时间初始化
	g_tagOverallCfg.def.switch_OFF = 1;

	sParam104.allInfoDisk = &g_tagOverallCfg; //全局配置结构
	uint8_t state = 1;
	mParam104 = (struct sMasterParam *)malloc(num * sizeof(struct sMasterParam));
	if (mParam104 == NULL)
	{
		printf("104 init error\r\n");
		return;
	}

	if(iec_init(&sParam104) == 4)
	{
		int i = 0;
		while(i < num)
		{
			((struct sMasterParam *)(mParam104 + i))->module_id = i + 1;                         /* 模块ID */
			((struct sMasterParam *)(mParam104 + i))->sourceAddr = i + 1; 	                    /* 链路地址 */
			((struct sMasterParam *)(mParam104 + i))->communication_state.pValue = &state;
			((struct sMasterParam *)(mParam104 + i))->YX_len = 150;                            /* 遥信长度 */
			((struct sMasterParam *)(mParam104 + i))->YC_len = 100;                            /* 遥测长度 */
			((struct sMasterParam *)(mParam104 + i))->p_YC_data = (tagAnalogQuantityIn_t)malloc(100 * sizeof(struct tagAnalogQuantityIn));
			int j = 0;
			while (j < ((struct sMasterParam *)(mParam104 + i))->YC_len)
			{
				((tagAnalogQuantityIn_t)(((struct sMasterParam *)(mParam104 + i))->p_YC_data + j))->pValue = (float *)malloc(sizeof(float));
				memset(((tagAnalogQuantityIn_t)(((struct sMasterParam *)(mParam104 + i))->p_YC_data + j))->pValue, 0, 1);

				j++;
			}
			((struct sMasterParam *)(mParam104 + i))->YK_len = 11;                            /* 遥控长度 */
			((struct sMasterParam *)(mParam104 + i))->portNo = 0; 		                    /* 串口号0关闭1开启 */
			((struct sMasterParam *)(mParam104 + i))->netEn = 1; 		                    /* 网口使用0关闭1开启 */
			((struct sMasterParam *)(mParam104 + i))->ip = ipAddr[i];           		/*IP地址(点分十进制)*/   //设备IP
			((struct sMasterParam *)(mParam104 + i))->yk_type = 1;		                    /* 遥控单双点 */

			MasterModleParam(((struct sMasterParam *)(mParam104 + i)));					//从站链路地址为1
			i++;
		}
	}

	if(iec_start(4) == TRUE)
	{
		return;
	}
}

/**
  * @brief :
  * @param : id 从机地址
  * @param : type 选择101 104协议
  * @param : num 读取遥测偏移量
  * @return:
  * @updata:
  */
float ReadYcData(uint8_t id, IECType iecType, YcDataType ycType)
{
	if (iecType == IEC101)
	{
		return (*((tagAnalogQuantityIn_t)(((struct sMasterParam *)(mParam101 + (id - 1)))->p_YC_data + ycType))->pValue);
	}
	else if(iecType == IEC104)
	{
		return (*((tagAnalogQuantityIn_t)(((struct sMasterParam *)(mParam104 + (id - 1)))->p_YC_data + ycType))->pValue);
	}
	else
	{
		printf("read YC error\r\n");
	}
}

int getCurrentTime(ds_privateTime_t pTime)
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	long int ti = (long int)((tv.tv_sec*1000) + (tv.tv_usec/1000)) - startTime;
	pTime->time_ms = (uint64_t)ti;

	time_t curentTime;
	struct tm * pTM;
	time(&curentTime);
	pTM = localtime(&curentTime);
	pTime->time_cp56.year = (uint8_t)(pTM->tm_year - 100);
	pTime->time_cp56.month = (uint8_t)(pTM->tm_mon + 1);
	pTime->time_cp56.dayofWeek = (uint8_t)((pTM->tm_wday << 4) | pTM->tm_mday);
	pTime->time_cp56.hour = (uint8_t)pTM->tm_hour;
	pTime->time_cp56.minute = (uint8_t)pTM->tm_min;
	pTime->time_cp56.msecondH = (uint8_t)pTM->tm_sec;
	pTime->time_cp56.msecondL = (uint8_t)(tv.tv_usec / 1000);
}
