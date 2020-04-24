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
#include "Led/Led.h"
#include "TransparentTransmission/TransparentTransmission.h"
#include "ProcessSignal/ProcessSignal.h"
#include "Modbus/NoiseSensor/NoiseSensor.h"
#include "DataStruct.h"


/**
 * 	@brief: main函数
 */
int main(int argc, char *argv[])
{
	pid_t pid = 0;			//子进程的进程ID号

	/* 透传功能需要的配置信息 */
	int trsptTrsmsProcessNum = 0;			//透传功能进程数
	int trsptTrsmsType[2] = {TCP_SERVER_TO_UART, TCP_CLIENT_TO_UART};
	UartInfo trsptTrsmsUart[2] = {{"/dev/ttymxc3", 9600, RS485_TYPE}, {"/dev/ttymxc4", 9600, RS485_TYPE}};
	NetworkInfo trsptTrsmsEth[2] = {{"192.168.10.10", 6666, "iot.shangshan.info", 41001},
									{"127.0.0.1", 5555, "192.168.10.11", 3333}};

	/* 噪声传感器需要的配置信息 */
	UartInfo noiseUart = {"/dev/ttymxc4", 9600, RS485_TYPE};

	/* 六合一空气质量传感器需要的配置信息 */
	//UartInfo airQualitySensor = {"/dev/ttymxc3", 9600, RS485_TYPE};


	/* 解析配置文件，获取配置信息  */

	/*********END***********/


	/* 创建透传功能进程 */
	for(int i = 0; i < trsptTrsmsProcessNum; i++)
	{
		if((pid = fork()) == 0)
		{
			SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

			printf("TransparentTransmission (pid:%d) creat\n", getpid());
			TransparentTransmission(trsptTrsmsType[i], &trsptTrsmsUart[i], &trsptTrsmsEth[i]);		//透传功能
			printf("TransparentTransmission (pid:%d) exit\n", getpid());

			return 0;
		}
	}

	/* 创建噪声传感器通信进程 */
	if((pid = fork()) == 0)
	{
		SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

		printf("NoiseSensor (pid:%d) creat\n", getpid());
		NoiseSensor(&noiseUart);							//噪声传感器
		printf("NoiseSensor (pid:%d) exit\n", getpid());

		return 0;
	}


	/* 创建其他功能的进程 */

	/****************/


	/* 父进程创建完子进程后，执行的任务 */
	while(1)
	{
		IndicatorLedRunning();		//程序运行指示灯
	}

	IndicatorLedOnOrOff(LED_OFF);	//程序退出，灯关闭
	printf("EdgeGateway (pid:%d) exit\n", getpid());

	return 0;
}














































/**
 *               ii.                                         ;9ABH,
 *              SA391,                                    .r9GG35&G
 *              &#ii13Gh;                               i3X31i;:,rB1
 *              iMs,:,i5895,                         .5G91:,:;:s1:8A
 *               33::::,,;5G5,                     ,58Si,,:::,sHX;iH1
 *                Sr.,:;rs13BBX35hh11511h5Shhh5S3GAXS:.,,::,,1AG3i,GG
 *                .G51S511sr;;iiiishS8G89Shsrrsh59S;.,,,,,..5A85Si,h8
 *               :SB9s:,............................,,,.,,,SASh53h,1G.
 *            .r18S;..,,,,,,,,,,,,,,,,,,,,,,,,,,,,,....,,.1H315199,rX,
 *          ;S89s,..,,,,,,,,,,,,,,,,,,,,,,,....,,.......,,,;r1ShS8,;Xi
 *        i55s:.........,,,,,,,,,,,,,,,,.,,,......,.....,,....r9&5.:X1
 *       59;.....,.     .,,,,,,,,,,,...        .............,..:1;.:&s
 *      s8,..;53S5S3s.   .,,,,,,,.,..      i15S5h1:.........,,,..,,:99
 *      93.:39s:rSGB@A;  ..,,,,.....    .SG3hhh9G&BGi..,,,,,,,,,,,,.,83
 *      G5.G8  9#@@@@@X. .,,,,,,.....  iA9,.S&B###@@Mr...,,,,,,,,..,.;Xh
 *      Gs.X8 S@@@@@@@B:..,,,,,,,,,,. rA1 ,A@@@@@@@@@H:........,,,,,,.iX:
 *     ;9. ,8A#@@@@@@#5,.,,,,,,,,,... 9A. 8@@@@@@@@@@M;    ....,,,,,,,,S8
 *     X3    iS8XAHH8s.,,,,,,,,,,...,..58hH@@@@@@@@@Hs       ...,,,,,,,:Gs
 *    r8,        ,,,...,,,,,,,,,,.....  ,h8XABMMHX3r.          .,,,,,,,.rX:
 *   :9, .    .:,..,:;;;::,.,,,,,..          .,,.               ..,,,,,,.59
 *  .Si      ,:.i8HBMMMMMB&5,....                    .            .,,,,,.sMr
 *  SS       :: h@@@@@@@@@@#; .                     ...  .         ..,,,,iM5
 *  91  .    ;:.,1&@@@@@@MXs.                            .          .,,:,:&S
 *  hS ....  .:;,,,i3MMS1;..,..... .  .     ...                     ..,:,.99
 *  ,8; ..... .,:,..,8Ms:;,,,...                                     .,::.83
 *   s&: ....  .sS553B@@HX3s;,.    .,;13h.                            .:::&1
 *    SXr  .  ...;s3G99XA&X88Shss11155hi.                             ,;:h&,
 *     iH8:  . ..   ,;iiii;,::,,,,,.                                 .;irHA
 *      ,8X5;   .     .......                                       ,;iihS8Gi
 *         1831,                                                 .,;irrrrrs&@
 *           ;5A8r.                                            .:;iiiiirrss1H
 *             :X@H3s.......                                .,:;iii;iiiiirsrh
 *              r#h:;,...,,.. .,,:;;;;;:::,...              .:;;;;;;iiiirrss1
 *             ,M8 ..,....,.....,,::::::,,...         .     .,;;;iiiiiirss11h
 *             8B;.,,,,,,,.,.....          .           ..   .:;;;;iirrsss111h
 *            i@5,:::,,,,,,,,.... .                   . .:::;;;;;irrrss111111
 *            9Bi,:,,,,......                        ..r91;;;;;iirrsss1ss1111
 *
 *								狗头保佑，永无BUG！
 */






