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
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/sem.h>
#include "Led/Led.h"
#include "ProcessSignal/ProcessSignal.h"
#include "TransparentTransmission/TransparentTransmission.h"
#include "Modbus/NoiseSensor/NoiseSensor.h"
#include "Modbus/AirQualitySensor/AirQualitySensor.h"
#include "Modbus/SojoRelay/TemperatureRelay.h"
#include "MQTT/MqttPublish.h"
#include "ParserConfig/Interface_S2J.h"
#include "ProcessCommunication/Semaphore.h"
#include "DataStruct.h"
#include "Config.h"


static void TrsptTrsmsParamConfig(EdgeGatewayConfig *configInfo, int *processNum, int type[], UartInfo *uart[], NetworkInfo *eth[]);
static void ModbusParamConfig(EdgeGatewayConfig *configInfo, int *processNum, char *username[], UartInfo *uart[], int deviceNum[], int *deviceId[], int sersorType);
static void MqttParamConfig(EdgeGatewayConfig *configInfo, int *processNum, char *username[]);


/**
 * 	@brief: main函数
 */
int main(int argc, char *argv[])
{
	pid_t pid = 0;			//子进程的进程ID号
	int semId = 0;

	/* 透传功能需要的配置信息 */
	int trsptTrsmsProcessNum = 0;			//透传功能进程数
	int trsptTrsmsType[PROTOCOL_MAX_PROCESS] = {0};
	UartInfo *trsptTrsmsUart[PROTOCOL_MAX_PROCESS] = {0};
	NetworkInfo *trsptTrsmsEth[PROTOCOL_MAX_PROCESS] = {0};
	for(int i = 0; i < PROTOCOL_MAX_PROCESS; i++)
	{
		trsptTrsmsUart[i] = malloc(sizeof(UartInfo));
		trsptTrsmsEth[i] = malloc(sizeof(NetworkInfo));
	}

	/* 噪声传感器需要的配置信息 */
	int noiseProcessNum = 0;
	char *noiseUsername[PROTOCOL_MAX_PROCESS] = {0};
	UartInfo *noiseUart[PROTOCOL_MAX_PROCESS] = {0};
	int noiseDeviceNum[PROTOCOL_MAX_PROCESS] = {0};
	int *noiseDeviceId[PROTOCOL_MAX_PROCESS] = {0};
	for(int i = 0; i < PROTOCOL_MAX_PROCESS; i++)
	{
		noiseUsername[i] = malloc(sizeof(char) * 30);
		noiseUart[i] = malloc(sizeof(UartInfo));
		noiseDeviceId[i] = malloc(sizeof(int) * SLAVE_MAX_NUMBER);
	}

	/* 六合一空气质量传感器需要的配置信息 */
	int airQualityProcessNum = 0;
	char *airQualityUsername[PROTOCOL_MAX_PROCESS] = {0};
	UartInfo *airQualitySensor[PROTOCOL_MAX_PROCESS] = {0};
	int airQualityDeviceNum[PROTOCOL_MAX_PROCESS] = {0};
	int *airQualityDeviceId[PROTOCOL_MAX_PROCESS] = {0};
	for(int i = 0; i < PROTOCOL_MAX_PROCESS; i++)
	{
		airQualityUsername[i] = malloc(sizeof(char) * 30);
		airQualitySensor[i] = malloc(sizeof(UartInfo));
		airQualityDeviceId[i] = malloc(sizeof(int) * SLAVE_MAX_NUMBER);
	}

	/* 双杰测温中继需要的配置信息 */
	int sojoRelayProcessNum = 0;
	char *sojoRelayUsername[PROTOCOL_MAX_PROCESS] = {0};
	UartInfo *sojoRelaySensor[PROTOCOL_MAX_PROCESS] = {0};
	int sojoRelayDeviceNum[PROTOCOL_MAX_PROCESS] = {0};
	int *sojoRelayDeviceId[PROTOCOL_MAX_PROCESS] = {0};
	for(int i = 0; i < PROTOCOL_MAX_PROCESS; i++)
	{
		sojoRelayUsername[i] = malloc(sizeof(char) * 30);
		sojoRelaySensor[i] = malloc(sizeof(UartInfo));
		sojoRelayDeviceId[i] = malloc(sizeof(int) * SLAVE_MAX_NUMBER);
	}

	/* MQTT发布数据需要的配置信息 */
	int mqttProcessNum = 0;
	char *userName[PROTOCOL_MAX_PROCESS] = {0};
	for(int i = 0; i < PROTOCOL_MAX_PROCESS; i++)
	{
		userName[i] = malloc(sizeof(char) * 30);
	}


	/* 解析配置文件，获取配置信息  */
	GetJsonFile(JSON_CONFIG_FILENAME, &g_EdgeGatewayConfig);

	/* 将获取的配置信息进行赋值 */
	TrsptTrsmsParamConfig(g_EdgeGatewayConfig, &trsptTrsmsProcessNum, trsptTrsmsType, trsptTrsmsUart, trsptTrsmsEth);
	ModbusParamConfig(g_EdgeGatewayConfig, &noiseProcessNum, noiseUsername, noiseUart, noiseDeviceNum, noiseDeviceId, NOISE_SERSOR);
	ModbusParamConfig(g_EdgeGatewayConfig, &airQualityProcessNum, airQualityUsername, airQualitySensor, airQualityDeviceNum, airQualityDeviceId, AIR_QUALITY_SERSOR);
	ModbusParamConfig(g_EdgeGatewayConfig, &sojoRelayProcessNum, sojoRelayUsername, sojoRelaySensor, sojoRelayDeviceNum, sojoRelayDeviceId, SOJO_RELAY);
	MqttParamConfig(g_EdgeGatewayConfig, &mqttProcessNum, userName);

	/* 初始化信号量 */
	semId = semget((key_t)SEMAPHORE_KEY, 1, 0666 | IPC_CREAT);
	SetSemValue(semId);

	/* 创建透传功能进程 */
	for(int i = 0; i < trsptTrsmsProcessNum; i++)
	{
		if((pid = fork()) == 0)
		{
			SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

			printf("TransparentTransmission (pid:%d) creat\n", getpid());
			//TransparentTransmission(trsptTrsmsType[i], trsptTrsmsUart[i], trsptTrsmsEth[i]);		//透传功能
			printf("TransparentTransmission (pid:%d) exit\n", getpid());

			return 0;
		}
	}

	/* 创建噪声传感器通信进程 */
	for(int i = 0; i < noiseProcessNum; i++)
	{
		if((pid = fork()) == 0)
		{
			SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

			printf("NoiseSensor (pid:%d) creat\n", getpid());
			//NoiseSensor(noiseUart[i], noiseDeviceId[i], noiseDeviceNum[i], noiseUsername[i]);
			printf("NoiseSensor (pid:%d) exit\n", getpid());

			return 0;
		}
	}

	/* 创建六合一空气质量传感器通信进程 */
	for(int i = 0; i < airQualityProcessNum; i++)
	{
		if((pid = fork()) == 0)
		{
			SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

			printf("AirQualitySensor (pid:%d) creat\n", getpid());
			//AirQualitySensor(airQualitySensor[i], airQualityDeviceId[i], airQualityDeviceNum[i], airQualityUsername[i]);
			printf("AirQualitySensor (pid:%d) exit\n", getpid());

			return 0;
		}
	}

	/* 创建双杰测温中继通信进程 */
	for(int i = 0; i < sojoRelayProcessNum; i++)
	{
		if((pid = fork()) == 0)
		{
			SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

			printf("TemperatureRelay (pid:%d) creat\n", getpid());
			//TemperatureRelay(sojoRelaySensor[i], sojoRelayDeviceId[i], sojoRelayDeviceNum[i], sojoRelayUsername[i]);
			printf("TemperatureRelay (pid:%d) exit\n", getpid());

			return 0;
		}
	}

	/* 创建MQTT通信进程 */
	for(int i = 0; i < mqttProcessNum; i++)
	{
		if((pid = fork()) == 0)
		{
			SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

			printf("MqttPublish (pid:%d) creat\n", getpid());
			//MqttPublish(userName[i]);							//MQTT发布信息
			printf("MqttPublish (pid:%d) exit\n", getpid());

			return 0;
		}
	}

	/* 创建101进程 */
	if((pid = fork()) == 0)
	{
		SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

		printf("IEC101 (pid:%d) creat\n", getpid());
		/* TODO:在此处添加101的对外接口函数 */
		printf("IEC101 (pid:%d) exit\n", getpid());

		return 0;
	}

	/* 创建104进程 */
	if((pid = fork()) == 0)
	{
		SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

		printf("IEC104 (pid:%d) creat\n", getpid());
		/* TODO:在此处添加104协议的对外接口函数 */
		printf("IEC104 (pid:%d) exit\n", getpid());

		return 0;
	}

	/* 创建嵌入式网页进程 */
	if((pid = fork()) == 0)
	{
		SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

		printf("Web (pid:%d) creat\n", getpid());
		/* TODO:在此处添加嵌入式网页的对外接口函数 */
		printf("Web (pid:%d) exit\n", getpid());

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

	/* 释放内存空间 */
	for(int i = 0; i < PROTOCOL_MAX_PROCESS; i++)
	{
		free(trsptTrsmsUart[i]);
		free(trsptTrsmsEth[i]);
		free(noiseUsername[i]);
		free(noiseUart[i]);
		free(noiseDeviceId[i]);
		free(airQualityUsername[i]);
		free(airQualitySensor[i]);
		free(airQualityDeviceId[i]);
		free(sojoRelayUsername[i]);
		free(sojoRelaySensor[i]);
		free(sojoRelayDeviceId[i]);
		free(userName[i]);
	}
	DelSemValue(semId);


	return 0;
}


/**
 * @brief 透传功能参数配置
 * @param configInfo 从配置文件获取到的配置信息
 * @param processNum 进程数
 * @param type 协议类型数组
 * @param uart 串口信息结构体数组
 * @param eth 网口信息结构体数组
 * @return void
 */
static void TrsptTrsmsParamConfig(EdgeGatewayConfig *configInfo, int *processNum, int type[], UartInfo *uart[], NetworkInfo *eth[])
{
	UartToNetConfig *uart2Net = NULL;

	*processNum = configInfo->uartToNetNumber;

	for(int i = 0; i < *processNum; i++)
	{
		switch(i)
		{
			case 0:
				uart2Net = &configInfo->uartToNet1;
				break;
			case 1:
				uart2Net = &configInfo->uartToNet2;
				break;
			case 2:
				uart2Net = &configInfo->uartToNet3;
				break;
			case 3:
				uart2Net = &configInfo->uartToNet4;
				break;
			case 4:
				uart2Net = &configInfo->uartToNet5;
				break;
		}
		type[i] = uart2Net->ProtocolType;
		strcpy(uart[i]->uartName, uart2Net->uartName);
		uart[i]->bandrate = uart2Net->bandrate;
		uart[i]->uartType = uart2Net->uartType;
		strcpy(eth[i]->localAddress, uart2Net->localIP);
		eth[i]->localPort = uart2Net->localPort;
		strcpy(eth[i]->remoteAddress, uart2Net->remoteIP);
		eth[i]->remotePort = uart2Net->remotePort;
	}
}


/**
 * @brief 透传功能参数配置
 * @param configInfo 从配置文件获取到的配置信息
 * @param processNum 进程数
 * @param username 保存数据的数据表名
 * @param uart 串口信息结构体数组
 * @param deviceNum 设备数
 * @param deviceId 设备号
 * @param sersorType 传感器类型
 * @return void
 */
static void ModbusParamConfig(EdgeGatewayConfig *configInfo, int *processNum, char *username[], UartInfo *uart[], int deviceNum[], int *deviceId[], int sersorType)
{
	ModbusConfig **modbusInfo = NULL;
	ModbusConfig *noiseInfo = NULL, *airQulityInfo = NULL, *sojoRelay = NULL;

	switch(sersorType)
	{
	case NOISE_SERSOR:
		modbusInfo = &noiseInfo;
		break;
	case AIR_QUALITY_SERSOR:
		modbusInfo = &airQulityInfo;
		break;
	case SOJO_RELAY:
		modbusInfo = &sojoRelay;
		break;
	}

	*processNum = configInfo->noiseNumber;

	for(int i = 0; i < *processNum; i++)
	{
		switch(i)
		{
		case 0:
			noiseInfo = &configInfo->noiseSersor1;
			airQulityInfo = &configInfo->airSersor1;
			sojoRelay = &configInfo->tempSersor1;
			break;
		case 1:
			noiseInfo = &configInfo->noiseSersor2;
			airQulityInfo = &configInfo->airSersor2;
			sojoRelay = &configInfo->tempSersor2;
			break;
		case 2:
			noiseInfo = &configInfo->noiseSersor3;
			airQulityInfo = &configInfo->airSersor3;
			sojoRelay = &configInfo->tempSersor3;
			break;
		case 3:
			noiseInfo = &configInfo->noiseSersor4;
			airQulityInfo = &configInfo->airSersor4;
			sojoRelay = &configInfo->tempSersor4;
			break;
		case 4:
			noiseInfo = &configInfo->noiseSersor5;
			airQulityInfo = &configInfo->airSersor5;
			sojoRelay = &configInfo->tempSersor5;
			break;
		}

		strcpy(username[i], (*modbusInfo)->dataFilename);
		uart[i]->bandrate = (*modbusInfo)->bandrate;
		uart[i]->uartType = (*modbusInfo)->uartType;
		strcpy(uart[i]->uartName, (*modbusInfo)->uartName);
		deviceNum[i] = (*modbusInfo)->slaveNumber;
		for(int j = 0; j < deviceNum[i]; j++)
		{
			deviceId[i][j] = (*modbusInfo)->slaveID[j];
		}
	}
}


/**
 * @brief 透传功能参数配置
 * @param configInfo 从配置文件获取到的配置信息
 * @param processNum 进程数
 * @param username 保存数据的数据表名
 * @return void
 */
static void MqttParamConfig(EdgeGatewayConfig *configInfo, int *processNum, char *username[])
{
	MqttConfig *mqttInfo = NULL;
	char *tempString = NULL;

	mqttInfo = &configInfo->mqttAccess;
	*processNum = mqttInfo->mqttNumber;
	for(int i = 0; i < *processNum; i++)
	{
		switch(i)
		{
		case 0:
			tempString = mqttInfo->username1;
			break;
		case 1:
			tempString = mqttInfo->username2;
			break;
		case 2:
			tempString = mqttInfo->username3;
			break;
		case 3:
			tempString = mqttInfo->username4;
			break;
		case 4:
			tempString = mqttInfo->username5;
			break;
		}
		strcpy(username[i], tempString);
	}
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






