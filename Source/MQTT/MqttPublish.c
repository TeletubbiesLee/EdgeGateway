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
#include "paho_mqtt_c/MQTTClient.h"
#include "MqttPublish.h"
#include "../Config.h"



/**
 * @breif MQTT发布消息函数
 * @param void
 * @return void
 */
int MqttPublish(void)
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;
    char payload[2][128] = {"{\"temperature\":121, \"humidity\":55.3, \"active\": true}",
    						"{\"temperature1\":12, \"humidity1\":5.3, \"active1\": false}"};

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = "A1_TEST_TOKEN";

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    printf("Connect Successful!\n");

    int i = 2;
	while(i--)
	{
		pubmsg.payload = payload[i];
		pubmsg.payloadlen = (int)strlen(payload[i]);
		pubmsg.qos = QOS;
		pubmsg.retained = 0;
		MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
		printf("Waiting for up to %d seconds for publication of %s\n"
				"on topic %s for client with ClientID: %s\n",
				(int)(TIMEOUT/1000), payload[i], TOPIC, CLIENTID);
		rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
		printf("Message with delivery token %d delivered\n", token);
	}

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}


