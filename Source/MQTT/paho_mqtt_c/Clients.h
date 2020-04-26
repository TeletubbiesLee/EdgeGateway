/*******************************************************************************
 * Copyright (c) 2009, 2018 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution. 
 *
 * The Eclipse Public License is available at 
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Ian Craggs - add SSL support
 *    Ian Craggs - fix for bug 413429 - connectionLost not called
 *    Ian Craggs - change will payload to binary
 *    Ian Craggs - password to binary
 *    Ian Craggs - MQTT 5 support
 *******************************************************************************/

#if !defined(CLIENTS_H)
#define CLIENTS_H

#include <time.h>
#if defined(OPENSSL)
#if defined(WIN32) || defined(WIN64)
#include <winsock2.h>
#endif
#include <openssl/ssl.h>
#endif
#include "MQTTClient.h"
#include "LinkedList.h"
#include "MQTTClientPersistence.h"


/**
 * Stored publication data to minimize copying
 */
typedef struct
{
	char *topic;
	int topiclen;
	char* payload;
	int payloadlen;
	int refcount;
} Publications;

/**
 * Client publication message data
 */
typedef struct
{
	int qos;
	int retain;
	int msgid;
	int MQTTVersion;
	MQTTProperties properties;
	Publications *publish;
	time_t lastTouch;		/**> used for retry and expiry */
	char nextMessageType;	/**> PUBREC, PUBREL, PUBCOMP */
	int len;				/**> length of the whole structure+data */
} Messages;

/**
 * Client will message data
 */
typedef struct
{
	char *topic;
	int payloadlen;
	void *payload;
	int retained;
	int qos;
} willMessages;

typedef struct
{
	int socket;
	time_t lastSent;
	time_t lastReceived;
	time_t lastPing;
#if defined(OPENSSL)
	SSL* ssl;
	SSL_CTX* ctx;
#endif
	int websocket; /**< socket has been upgraded to use web sockets */
	char *websocket_key;
} networkHandles;


/* connection states */
/** no connection in progress, see connected value */
#define NOT_IN_PROGRESS  0x0
/** TCP connection in progress */
#define TCP_IN_PROGRESS  0x1
/** SSL connection in progress */
#define SSL_IN_PROGRESS  0x2
/** Websocket connection in progress */
#define WEBSOCKET_IN_PROGRESS   0x3
/** TCP completed, waiting for MQTT ACK */
#define WAIT_FOR_CONNACK 0x4
/** Disconnecting */
#define DISCONNECTING    -2

/**
 * Data related to one client
 */
typedef struct
{
	char* clientID;					      /**< the string id of the client */
	const char* username;					/**< MQTT v3.1 user name */
	int passwordlen;              /**< MQTT password length */
	const void* password;					/**< MQTT v3.1 binary password */
	unsigned int cleansession : 1;	/**< MQTT V3 clean session flag */
	unsigned int cleanstart : 1;		/**< MQTT V5 clean start flag */
	unsigned int connected : 1;		/**< whether it is currently connected */
	unsigned int good : 1; 			  /**< if we have an error on the socket we turn this off */
	unsigned int ping_outstanding : 1;
	signed int connect_state : 4;
	networkHandles net;
	int msgID;
	int keepAliveInterval;
	int retryInterval;
	int maxInflightMessages;
	willMessages* will;
	List* inboundMsgs;
	List* outboundMsgs;				/**< in flight */
	List* messageQueue;
	unsigned int qentry_seqno;
	void* phandle;  /* the persistence handle */
	MQTTClient_persistence* persistence; /* a persistence implementation */
	void* context; /* calling context - used when calling disconnect_internal */
	int MQTTVersion;
	int sessionExpiry; /**< MQTT 5 session expiry */
#if defined(OPENSSL)
	MQTTClient_SSLOptions *sslopts;
	SSL_SESSION* session;    /***< SSL session pointer for fast handhake */
#endif
} Clients;

int clientIDCompare(void* a, void* b);
int clientSocketCompare(void* a, void* b);

/**
 * Configuration data related to all clients
 */
typedef struct
{
	const char* version;
	List* clients;
} ClientStates;

#endif
