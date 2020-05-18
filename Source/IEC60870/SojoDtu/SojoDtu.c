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
#include <unistd.h>
#include "sojo_interface.h"
#include "sojo_point_table_enum.h"
#include "SojoDtu.h"
#include "../../Config.h"


/**
 * @breif 双杰DTU的101通信
 * @param void
 * @return 成功：0 失败：-1
 */
int SojoDtu_IEC101(Configure101 *info101)
{
	float num = 0.0;

	iec101_startup(info101);

	/* TODO:通过101协议读取设备数据，并解析保存到数据库中 */
	while(1)
	{
		for(int i = 1; i <= info101->num; i++)
		{
			sleep(30);
			printf("sleep end\n");
			num = ReadDataYc(i, IEC101, YCDATA_DC1);
			printf("ReadDataYc end\n");
			printf("IEC101: device %d : YCDATA_DC1 = %f\n", i, num);
			num = ReadDataYc(i, IEC101, YCDATA_DC2);
			printf("IEC101: device %d : YCDATA_DC2 = %f\n", i, num);
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
int SojoDtu_IEC104(Configure104 *info104)
{
	float num = 0.0;

	iec104_startup(info104);

	/* TODO:通过104协议读取设备数据，并解析保存到数据库中 */
	while(1)
	{
		for(int i = 1; i <= info104->num; i++)
		{
			sleep(30);
			num = ReadDataYc(i, IEC104, YCDATA_DC1);
			printf("IEC104: device %d : YCDATA_DC1 = %f\n", i, num);
			num = ReadDataYc(i, IEC104, YCDATA_DC2);
			printf("IEC104: device %d : YCDATA_DC2 = %f\n", i, num);
		}
		sleep(IEC104_INTERVAL);
	}
	return NO_ERROR;

}


