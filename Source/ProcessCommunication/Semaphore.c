/**
 * @file Semaphore.c
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
#include "Semaphore.h"
#include "../Config.h"


union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};


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

