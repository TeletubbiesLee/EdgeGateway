/**
 * @file WebProcess.c
 * @brief 嵌入式网页进程相关程序
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.05.28
 *
 * @author Lei.L
 * @version ver 1.0
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "../Config.h"
#include "../ProcessCommunication/ProcessSignal.h"
#include "WebProcess.h"
#include "../TransparentTransmission/Net.h"


/**
 * @brief 创建Web网页的进程
 * @param void
 * @return 成功：0 失败：其他
 */
int CreatWebProcess(void)
{
	/* 创建嵌入式网页进程 */
	pid_t pid = 0;
	char *eth = "eth1";
	char ipAddress[20] = {0};
	char webCmdString[100] = "./GoAhead/bin/goahead -v --home ./GoAhead/bin/ /home/root/GoAhead ";

	GetIpAddress(eth, ipAddress);
	strcat(webCmdString, ipAddress);

	if((pid = fork()) == 0)
	{
		SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

		printf("Web (pid:%d) creat\n", getpid());
		printf("system(\"%s\")\n", webCmdString);
		system(webCmdString);
		printf("Web (pid:%d) exit\n", getpid());

		return NO_ERROR;
	}
	return NO_ERROR;
}



