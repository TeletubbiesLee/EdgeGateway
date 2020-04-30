/**
 * @file Interface_S2J.h
 * @brief C结构体转JSON文件的对外接口程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.29
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _INTERFACE_S2J_H_
#define _INTERFACE_S2J_H_


/*
 * 边缘网关系统信息
 */
typedef struct TagSystemInfomation{
	char deviceName[40];
	char deviceModel[20];
	char serialNumber[20];
	char firmwareVersion[10];
	char bootloaderVersion[10];
}SystemInfomation;


/*
 * 串口转网口透传功能相关配置信息
 */
typedef struct TagUartToNetConfig{
    int ProtocolType;
    char uartName[20];
    int bandrate;
    int uartType;
    char localIP[20];
    int localPort;
    char remoteIP[20];
    int remotePort;
}UartToNetConfig;


/*
 * Modbus通信相关配置信息
 */
typedef struct TagModbusConfig{
	char uartName[20];
	int bandrate;
	int uartType;
	int slaveNumber;
	int slaveID[256];
	char dataFilename[30];
}ModbusConfig;


/*
 * 101通信相关配置信息
 */
typedef struct TagIEC101Config{
	char uartName[20];
	int bandrate;
	int uartType;
	int slaveNumber;
	int slaveID[256];
	char dataFilename[30];
}IEC101Config;


/*
 * 104通信相关配置信息
 */
typedef struct TagIEC104Config{
	char localIP[20];
	int localPort;
	int slaveNumber;
	char slaveIP1[20];
	char slaveIP2[20];
	char slaveIP3[20];
	char slaveIP4[20];
	char slaveIP5[20];
}IEC104Config;


/*
 * mqtt通信相关配置信息
 */
typedef struct TagMqttConfig{
	int mqttNumber;
	char clondIP[20];
	char username1[20];
	char username2[20];
	char username3[20];
	char username4[20];
	char username5[20];
}MqttConfig;


/*
 * 边缘网关配置信息结构体
 */
typedef struct TagEdgeGatewayConfig{
	SystemInfomation systemInfo;
	int uartToNetNumber;
	UartToNetConfig uartToNet1;
	UartToNetConfig uartToNet2;
	UartToNetConfig uartToNet3;
	UartToNetConfig uartToNet4;
	UartToNetConfig uartToNet5;
	int noiseNumber;
	ModbusConfig noiseSersor1;
	ModbusConfig noiseSersor2;
	ModbusConfig noiseSersor3;
	ModbusConfig noiseSersor4;
	ModbusConfig noiseSersor5;
	int airNumber;
	ModbusConfig airSersor1;
	ModbusConfig airSersor2;
	ModbusConfig airSersor3;
	ModbusConfig airSersor4;
	ModbusConfig airSersor5;
	int tempNumber;
	ModbusConfig tempSersor1;
	ModbusConfig tempSersor2;
	ModbusConfig tempSersor3;
	ModbusConfig tempSersor4;
	ModbusConfig tempSersor5;
	IEC101Config iec101;
	IEC104Config iec104;
	MqttConfig mqttAccess;
}EdgeGatewayConfig;

#define JSON_CONFIG_FILENAME "EdgeGateway.json"

extern EdgeGatewayConfig *g_EdgeGatewayConfig;
extern EdgeGatewayConfig g_DefaultConfig;

void Struct2JsonInit(void);
int CreateJsonFile(char *filename, EdgeGatewayConfig *edgeGatewayStruct);
int GetJsonFile(char *filename, EdgeGatewayConfig **edgeGatewayStruct);
void S2J_StructFree(EdgeGatewayConfig *orignal_obj);
void ConfigPrintf(EdgeGatewayConfig *edgeGatewayConfig);
int S2J_test(void);

#endif

