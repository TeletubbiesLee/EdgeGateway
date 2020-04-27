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


static void MqttPublishMessage(MQTTClient *client, char *payload, int payloadLenth);
static int MqttInit(MQTTClient *client, char *accessUser);





/**
 * @breif MQTT发布消息到ThingsBoard函数
 * @param accessUser 管理凭证
 * @return void
 */
int MqttPublish(char *accessUser)
{
    MQTTClient client;
    char payload[2][128] = {"{\"temperature\":121, \"humidity\":55.3, \"active\": true}",
    						"{\"temperature1\":12, \"humidity1\":5.3, \"active1\": false}"};

    MqttInit(&client, accessUser);

    while(1)
    {
		/* TODO:从数据库中读取出数据 */

		MqttPublishMessage(&client, payload[0], strlen(payload[0]));
		MqttPublishMessage(&client, payload[1], strlen(payload[1]));

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


