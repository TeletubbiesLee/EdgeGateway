#include<stdio.h>
#include <semaphore.h>

#include "mysem.h"
#define LOG_TAG    "mysem"
//#include "elog/inc/elog.h"

sem_t sem[SEM_NUM];
unsigned int sem_val[SEM_NUM];

/**
  * @brief : 初始化信号量初始值
  * @return: [-1]-失败；[0]-成功
  * @update: [2019-3-1][Nichangmeng][newly increased]
  */
static void sem_val_init(void)
{
	  /* 信号量初值初始化 */
    sem_val[AD7616_SEM] = 0;
    sem_val[WAVE_DATA_SEM] = 1;
    sem_val[WAVE_PTHREAD_START] = 0;
    sem_val[SERVER_WRITE_SEM] = 1;
    sem_val[CLIENT_WRITE_SEM] = 1;
    sem_val[WAVE_COUNT_SEM] = 1;
    sem_val[DB_HANDLE] = 1;
    sem_val[OTHER_FILE_SEM] = 0;
    sem_val[CMD_REPLY_SEM] = 1;
}

/**
  * @brief : 初始化信号量
  * @return: [-1]-失败；[0]-成功
  * @update: [2019-3-1][Nichangmeng][newly increased]
  */
int mysem_init(void)
{
	int ret = 0;

	sem_val_init();
	
//	for(int i = 0;i < SEM_NUM; ++i)
//	{
//		ret = sem_init(&sem[i], 0, sem_val[i]);
//		if(ret == -1){
//			printf("sem %d init failed\r\n", i);
//		}
//	}
	int i = 0;
	while(i < SEM_NUM)
	{
		ret = sem_init(&sem[i], 0, sem_val[i]);
		if(ret == -1){
			printf("sem %d init failed\r\n", i);
		}
		i++;
	}

	return 0;
}

/**
  * @brief : 获取一个信号量
  * @param : [sem_index]-指定信号数组的一个下标 其范围为 0 ～ (SEM_NUM - 1)
  * @return:
  * @update: [2019-3-1][Nichangmeng][newly increased]
  */
void sem_P(enum SEMAPHORE_SET sem_index)
{
	int ret = 0;

	ret = sem_wait(&sem[sem_index]);
	if(ret == -1){
		printf("sem_wait failed sem index is %d\r\n", sem_index);
	}
}

/**
  * @brief : 释放一个信号量
  * @param : [sem_index]-指定信号数组的一个下标 其范围为 0 ～ (SEM_NUM - 1)
  * @return:
  * @update: [2019-3-1][Nichangmeng][newly increased]
  */
void sem_V(enum SEMAPHORE_SET sem_index)
{
	int ret = 0;

	ret = sem_post(&sem[sem_index]);
	if(ret == -1){
		printf("sem_post failed sem index is %d\r\n", sem_index);
	}
}
