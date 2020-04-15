/**
 * @file ProcessSignal.c
 * @brief 进程信号设置函数程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.15
 *
 * @author Lei.L
 * @version ver 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>
#include "ProcessSignal.h"


static void HandleSignal(int signalNum);


/**
 * @breif 设置进程关闭信号
 * @param void
 * @return void
 */
void SetProcessCloseSignal(void)
{
	signal(SIGINT, HandleSignal);
	prctl(PR_SET_PDEATHSIG, SIGINT);
	signal(SIGHUP, HandleSignal);
	prctl(PR_SET_PDEATHSIG, SIGHUP);
}


/**
 * @breif 信号处理函数
 * @param signalNum 信号值
 * @return void
 */
static void HandleSignal(int signalNum)
{
	if(signalNum == SIGINT || signalNum == SIGHUP)
	{
		printf("Process (pid:%d) recv SIGHUP or SIGINT, exit\n", getpid());
		exit(0);
	}
}

