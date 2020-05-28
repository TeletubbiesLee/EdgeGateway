/**
 * @file ProcessSignal.c
 * @brief 进程间信号量操作程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.05.08
 *
 * @author Lei.L
 * @version ver 1.0
 */

#include <stdio.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>
#include "Semaphore.h"
#include "../Config.h"


union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};


static void HandleSignal(int signalNum);


/**
 * @breif 初始化信号量
 * @param semId 信号量ID
 * @return 成功:0 失败:其他
 */
int SetSemValue(int semId)
{
	union semun semUnion;

	semUnion.val = 1;
	if(semctl(semId, 0, SETVAL, semUnion) == -1)
	{
		printf_debug("Failed to Init semaphore\n");
		return FUNCTION_FAIL;
	}
	return NO_ERROR;
}


/**
 * @breif 删除信号量
 * @param semId 信号量ID
 * @return 成功:0 失败:其他
 */
int DelSemValue(int semId)
{
	union semun semUnion;

	semUnion.val = 1;
	if(semctl(semId, 0, IPC_RMID, semUnion) == -1)
	{
		printf_debug("Failed to delete semaphore\n");
		return FUNCTION_FAIL;
	}
	return NO_ERROR;
}


/**
 * @breif 获取信号量
 * @param semId 信号量ID
 * @return 成功:0 失败:其他
 */
int Semaphore_P(int semId)
{
	struct sembuf semBuffer;

	semBuffer.sem_num = 0;
	semBuffer.sem_op = -1;
	semBuffer.sem_flg = SEM_UNDO;
	if(semop(semId, &semBuffer, 1) == -1)
	{
		printf_debug("Semaphore_P failed\n");
		return FUNCTION_FAIL;
	}
	return NO_ERROR;
}


/**
 * @breif 释放信号量值
 * @param semId 信号量ID
 * @return 成功:0 失败:其他
 */
int Semaphore_V(int semId)
{
	struct sembuf semBuffer;

	semBuffer.sem_num = 0;
	semBuffer.sem_op = 1;
	semBuffer.sem_flg = SEM_UNDO;
	if(semop(semId, &semBuffer, 1) == -1)
	{
		printf_debug("Semaphore_V failed\n");
		return FUNCTION_FAIL;
	}
	return NO_ERROR;
}


/**
 * @breif 设置进程关闭信号
 * @param void
 * @return void
 */
void SetProcessCloseSignal(void)
{
	signal(SIGINT, HandleSignal);			//父进程收到Ctrl C时，子进程会收到此信号
	prctl(PR_SET_PDEATHSIG, SIGINT);
	signal(SIGHUP, HandleSignal);			//父进程退出时，子进程会收到此信号
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



