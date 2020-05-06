/**
 * @file Config.h
 * @brief 配置相关的头文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.13
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdio.h>


#define printf_debug(fmt,...) do{printf("%s: %s[line %d]: "fmt, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);}while(0)


#define PROTOCOL_MAX_PROCESS	5			//每种协议最大支持数
#define SLAVE_MAX_NUMBER		256			//从机最大数


/*
 * 程序错误码
 */
typedef enum {
	FUNCTION_FAIL = -1,		//函数执行错误
	NO_ERROR = 0,			//没有错误
	POINT_NULL,				//指针为空
	OPEN_FILE_FAIL,			//打开文件失败
	MALLOC_FAIL,			//分配堆内存失败
	FEW_ARGUMENTS,			//参数太少
	ERROR_ARGUMENTS,		//参数错误

}ErrorCode;


#endif
