/**
 * @file MqttPublish.c
 * @brief MQTT发布消息程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.24
 *
 * @author Lei.L
 * @version ver 1.0
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sem.h>
#include "paho_mqtt_c/MQTTClient.h"
#include "MqttPublish.h"
#include "../Config.h"
#include "../DataStorage/DataProcess.h"
#include "../ProcessCommunication/ProcessSignal.h"
#include "../DeviceApp/DeviceParamConfig.h"


static void MqttPublishMessage(MQTTClient *client, char *payload, int payloadLenth);
static int MqttInit(MQTTClient *client, char *accessUser);


/**
 * @brief 创建Mqtt上传数据的进程
 * @param void
 * @return 成功：0 失败：其他
 */
int CreatMqttProcess(void)
{
	if(g_EdgeGatewayConfig == NULL)
	{
		printf_debug("g_EdgeGatewayConfig is NULL!\n");
		return POINT_NULL;
	}

	/* MQTT发布数据需要的配置信息 */
	bool isError = true;
	pid_t pid = 0;
	int mqttProcessNum = 0;
	char *userName[PROTOCOL_MAX_PROCESS] = {0};
	for(int i = 0; i < PROTOCOL_MAX_PROCESS; i++)
	{
		if((userName[i] = malloc(sizeof(char) * 30)) == NULL)
		{
			printf_debug("userName[%d] malloc error\n", i);
			isError = false;
			break;
		}
	}

	if(false == isError)
	{
		FreePointArray((void**)userName, PROTOCOL_MAX_PROCESS);
		return MALLOC_FAIL;
	}

	MqttParamConfig(g_EdgeGatewayConfig, &mqttProcessNum, userName);
	/* 创建MQTT通信进程 */
	for(int i = 0; i < mqttProcessNum; i++)
	{
		if((pid = fork()) == 0)
		{
			SetProcessCloseSignal();		//父进程关闭之后，子进程也全部关闭

			printf("MqttPublish (pid:%d) creat\n", getpid());
			MqttPublish(userName[i]);							//MQTT发布信息
			printf("MqttPublish (pid:%d) exit\n", getpid());

			for(int i = 0; i < PROTOCOL_MAX_PROCESS; i++)
			{
				if(userName[i] != NULL)
				{
					free(userName[i]);
					userName[i] = NULL;
				}
			}
			return NO_ERROR;
		}
	}
	FreePointArray((void**)userName, PROTOCOL_MAX_PROCESS);
	return NO_ERROR;
}


/**
 * @breif MQTT发布消息到ThingsBoard函数
 * @param accessUser 管理凭证
 * @return void
 */
int MqttPublish(char *accessUser)
{
    MQTTClient client;
	int semId = 0;
	int ret = 0, i = 0;
	db_list_t *payList = NULL;
	db_lnode_t* current;

    MqttInit(&client, accessUser);

    semId = semget((key_t)SEMAPHORE_KEY, 1, 0666 | IPC_CREAT);

    CreateDataFile(accessUser);

    while(1)
    {
		/* 读取出数据 */
		if(Semaphore_P(semId) == NO_ERROR)
		{
	    	ret = PollData(accessUser, &payList);
			if(Semaphore_V(semId) != NO_ERROR)
				printf_debug("MQTT: Semaphore_V error\n");
		}
		else
			printf_debug("MQTT: Semaphore_P error\n");

		if(0 == ret)
		{
			i = 0;
			for(current = payList->head; i++ < payList->limit_size; current = current->next)
			{
				printf("%s - payload : %s\n", accessUser, (char*)current->data);
				MqttPublishMessage(&client, current->data, strlen(current->data));
			}
		}
		db_list_destory(payList);

		sleep(MQTT_PUBLISH_INTERVAL);
    }

    MQTTClient_disconnect(client, 10000);
    printf("Disconnect Successful!\n");
    MQTTClient_destroy(&client);

    return NO_ERROR;
}


/**
 * @breif MQTT发布消息函数
 * @param client MQTT客户端结构体指针
 * @param payload 负载消息
 * @param payloadLenth 消息长度
 * @return void
 */
static int MqttInit(MQTTClient *client, char *accessUser)
{
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

	MQTTClient_create(client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.username = accessUser;

	if ((rc = MQTTClient_connect(*client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
		printf_debug("Failed to connect, return code %d\n", rc);
		return FUNCTION_FAIL;
	}
	printf("Connect Successful!\n");

	return NO_ERROR;
}


/**
 * @breif MQTT发布消息函数
 * @param client MQTT客户端结构体指针
 * @param payload 负载消息
 * @param payloadLenth 消息长度
 * @return void
 */
static void MqttPublishMessage(MQTTClient *client, char *payload, int payloadLenth)
{
    MQTTClient_deliveryToken token;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;

    pubmsg.payload = payload;
	pubmsg.payloadlen = payloadLenth;
	pubmsg.qos = QOS;
	pubmsg.retained = 0;
	MQTTClient_publishMessage(*client, TOPIC, &pubmsg, &token);
	printf("Waiting for up to %d seconds for publication of %s\n"
			"on topic %s for client with ClientID: %s\n",
			(int)(TIMEOUT/1000), payload, TOPIC, CLIENTID);
	MQTTClient_waitForCompletion(*client, token, TIMEOUT);
	printf("Message with delivery token %d delivered\n", token);

}




