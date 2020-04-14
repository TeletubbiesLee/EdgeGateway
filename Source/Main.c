/**
 * @file Main.c
 * @brief 边缘网关main函数程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.13
 *
 * @author Lei.L
 * @version ver 1.0
 */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>

static int do_abort = 0;
int Func(int i);
void HandleSignal(int signo);

/**
 * 	@brief: main函数
 */
int main(int argc, char *argv[])
{
	pid_t pid = 0;

	for(int i = 0; i < 3; i++)
	{
		if((pid = fork()) == 0)
		{
			printf("child %d\n", i);
			signal(SIGHUP, HandleSignal);
			prctl(PR_SET_PDEATHSIG, SIGHUP);
			Func(i);
			return 0;
		}
		else
		{
			printf("parent\n");
		}

	}

	while(1)
	{
		printf("pid = %d, parent\n", getpid());
		sleep(1);
	}
	printf("pid = %d, exit\n", getpid());
	return 0;
}


int Func(int i)
{
	while(!do_abort)
	{
		printf("pid = %d, i = %d\n", getpid(), i);
		sleep(1);
	}
	printf("pid = %d, exit\n", getpid());
	return 0;
}

void HandleSignal(int signo)
{
	if(signo == SIGHUP)
	{
		printf("chile recv SIGHUP\n");
		do_abort = 1;
	}
}



