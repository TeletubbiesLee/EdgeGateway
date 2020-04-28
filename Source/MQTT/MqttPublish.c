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
#include "paho_mqtt_c/MQTTClient.h"
#include "MqttPublish.h"
#include "../Config.h"
#include "../DataStorage/DataProcess.h"


static void MqttPublishMessage(MQTTClient *client, char *payload, int payloadLenth);
static int MqttInit(MQTTClient *client, char *accessUser);
static int ReadDataPoll(char *filename, char *payload);



/**
 * @breif MQTT发布消息到ThingsBoard函数
 * @param accessUser 管理凭证
 * @return void
 */
int MqttPublish(char *accessUser)
{
    MQTTClient client;
    char payload[128] = {0};

    MqttInit(&client, accessUser);

    while(1)
    {
		/* 读取出数据 */
    	ReadDataPoll(accessUser, payload);

		MqttPublishMessage(&client, payload, strlen(payload));

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


/**
 * @breif 轮询读取数据
 * @param filename 数据文件名
 * @param payload 负载消息
 * @param payloadLenth 消息长度
 * @return 成功：0 失败 -1
 */
static int ReadDataPoll(char *filename, char *payload)
{
	int ret = NO_ERROR;
	DataInformation dataInfo;

	if(0 == PollData(filename, &dataInfo))
	{
		switch (dataInfo.dataType)
		{
		case BIT_TYPE:
			sprintf(payload, "{\"%s_%d\":%s}", dataInfo.dataName, dataInfo.deviceId, (dataInfo.bitData?"true":"flase"));
			break;
		case INT_TYPE:
			sprintf(payload, "{\"%s_%d\":%d}", dataInfo.dataName, dataInfo.deviceId, dataInfo.intData);
			break;
		case FLOAT_TYPE:
			sprintf(payload, "{\"%s_%d\":%.3f}", dataInfo.dataName, dataInfo.deviceId, dataInfo.floatData);
			break;
		default:
			printf_debug("dataInfo.dataType value error\n");
			ret = FUNCTION_FAIL;
			break;
		}
	}
	else
	{
		printf_debug("PollData error\n");
		ret = FUNCTION_FAIL;
	}
	return ret;
}


