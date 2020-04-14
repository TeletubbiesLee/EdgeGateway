/**
 * @file TransparentTransmission.c
 * @brief 透传功能程序对外接口程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.14
 *
 * @author Lei.L
 * @version ver 1.0
 */

#include "TransparentTransmission.h"
#include "../Config.h"

/**
 * @breif 透传功能
 * @param type 透传功能的类型：UDP_TO_UART TCP_CLIENT_TO_UART TCP_SERVER_TO_UART
 * @param uartInfo 串口信息结构体指针
 * @param networkInfo 网口信息结构体指针
 * @return 成功0或失败-1
 */
int TransparentTransmission(int type, UartInfo *uartInfo, NetworkInfo *networkInfo)
{
	if(type == UDP_TO_UART)
	{
		UDP2Uart(uartInfo, networkInfo);
	}
	else if(type == TCP_CLIENT_TO_UART)
	{
		TCP_Server2Uart(uartInfo, networkInfo);		//设备外接TCP客户端和串口，嵌入式设备对应内部程序为TCP服务器端转串口
	}
	else if(type == TCP_SERVER_TO_UART)
	{
		TCP_Client2Uart(uartInfo, networkInfo);		//设备外接TCP服务器端和串口，嵌入式设备对应内部程序为TCP客户端转串口
	}
	else
	{
		printf_debug("error: arguments \'%d\' is wrong to program!\n", type);
		return ERROR_ARGUMENTS;
	}
	return NO_ERROR;
}
