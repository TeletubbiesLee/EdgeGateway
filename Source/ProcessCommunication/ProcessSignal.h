/**
 * @file ProcessSignal.h
 * @brief 进程间信号量操作程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.05.08
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#define SEMAPHORE_KEY	1234

int SetSemValue(int semId);
int DelSemValue(int semId);
int Semaphore_P(int semId);
int Semaphore_V(int semId);
void SetProcessCloseSignal(void);

#endif
