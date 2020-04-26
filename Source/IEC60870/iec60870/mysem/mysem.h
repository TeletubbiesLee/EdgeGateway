#ifndef __MY_SEM_H__
#define __MY_SEM_H__


/* ---------------------信号量集合成员控制---------------------------- */

enum SEMAPHORE_SET{
    AD7616_SEM = 0,         /* ad7616 线程启动顺序信号 */
    WAVE_DATA_SEM,          /* 实现原始数据队列原子操作 */
    WAVE_PTHREAD_START,     /* 录波线程启动线程入口参数保存 */
    WAVE_COUNT_SEM,         /* 录波文件名序号 */
    SERVER_WRITE_SEM,       /* server write信号 */
    CLIENT_WRITE_SEM,       /* client write信号 */
    DB_HANDLE,              /* 数据池操作 */
    OTHER_FILE_SEM,         /* 其他文件线程写数据 */
    CMD_REPLY_SEM,          /* 命令执行状态 */

    SEM_NUM,                /* 定义信号量的个数 */
};


/* ---------------------信号量集函数接口------------------------------ */

int mysem_init(void);
void sem_P(enum SEMAPHORE_SET sem_num);   //获取
void sem_V(enum SEMAPHORE_SET sem_num);   //释放

#endif
