/**
 * @file Interface_S2J.c
 * @brief C结构体转JSON文件的对外接口程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.04.29
 *
 * @author Lei.L
 * @version ver 1.0
 */

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include "struct2json/cJSON.h"
#include "struct2json/s2j.h"
#include "Interface_S2J.h"
#include "../Config.h"


static void *json_to_struct(cJSON* json_obj);
static cJSON *struct_to_json(void* struct_obj);
static cJSON* vPort_Get_cJSON(const char *value);
static char* vPort_Print_cJSON(cJSON *item);



/**
 * @fn S2J_test
 * @brief 结构体转Json文件的测试函数
 * @return void
 */
int S2J_test(void) {
	EdgeGatewayConfig orignal_default_obj = {
			.uartToNetNumber = 1,
			.uartToNet1.ProtocolType = 2,
			.uartToNet1.uartName = "789",
			.uartToNet1.bandrate = 9600,
			.uartToNet1.uartType = 6,
			.uartToNet1.localIP = "192.168.10.0",
			.uartToNet1.localPort = 2,
			.uartToNet1.remoteIP = "192.168.11.10",
			.uartToNet1.remotePort = 2,
			.noiseNumber = 23,
			.noiseSersor1.uartName = "qweq",
			.noiseSersor1.bandrate = 115200,
			.noiseSersor1.uartType = 123,
			.noiseSersor1.slaveNumber = 45,
			.noiseSersor1.slaveID = {1, 2, 3, 4},
			.noiseSersor1.dataFilename = "852",
			.mqttAccess.mqttNumber = 123,
			.mqttAccess.username1 = "qaz",
			.mqttAccess.username2 = "tyhtr",
	};
	Struct2JsonInit();
	CreateJsonFile("EdgeGateway.json", &orignal_default_obj);

	EdgeGatewayConfig *orignal_new_obj;
	GetJsonFile("EdgeGateway.json", &orignal_new_obj);

	printf("EdgeGatewayConfig.uartToNet1.localIP = %s\n", orignal_new_obj->uartToNet1.localIP);
	printf("EdgeGatewayConfig.uartToNet1.ProtocolType = %d\n", orignal_new_obj->uartToNet1.ProtocolType);
	printf("EdgeGatewayConfig.uartToNet1.bandrate = %d\n", orignal_new_obj->uartToNet1.bandrate);
	printf("EdgeGatewayConfig.mqttAccess.mqttNumber = %d\n", orignal_new_obj->mqttAccess.mqttNumber);
	printf("EdgeGatewayConfig.mqttAccess.username2 = %s\n", orignal_new_obj->mqttAccess.username2);
	printf("EdgeGatewayConfig.noiseSersor1.slaveID[2] = %d\n", orignal_new_obj->noiseSersor1.slaveID[2]);

    s2j_delete_struct_obj(orignal_new_obj);

    return 0;
}


/**
 * @fn Struct2JsonInit
 * @brief 初始化struct2json
 * @return void
 */
void Struct2JsonInit(void)
{
	S2jHook s2jhook;
    s2jhook.malloc_fn = malloc;  //初始化内存申请函数
    s2jhook.free_fn = free;

    s2j_init(&s2jhook);   //初始化struct2json的内存申请
}


/**
 * @fn Struct2JsonInit
 * @brief orignal_obj 清除struct转json中的结构体分配的空间
 * @return void
 */
void S2J_StructFree(EdgeGatewayConfig *orignal_obj)
{
	s2j_delete_struct_obj(orignal_obj);
}


/**
 * @brief 结构体转换为json并写入到文件
 * @param fileName 要写入的文件名称
 * @param edgeGatewayStruct 边缘网关配置信息结构体
 * @return 成功：0 失败：-1
 */
int CreateJsonFile(char *filename, EdgeGatewayConfig *edgeGatewayStruct)
{
	int fd = 0;
    char* string = NULL;
    ssize_t writeNum = 0;
    int ret = NO_ERROR;

    fd = open(filename, O_RDWR | O_CREAT | O_TRUNC);
    if(fd < 0)
	{
    	printf_debug("file %s open error\n", filename);
		return OPEN_FILE_FAIL;
	}

	cJSON* structJson = struct_to_json(edgeGatewayStruct);

	string = vPort_Print_cJSON(structJson);

	writeNum = write(fd, string, strlen(string));    //依照标准格式进行写入
	if(writeNum != strlen(string))
	{
		printf_debug("write error\n");
		ret = FUNCTION_FAIL;
	}

	if(string != NULL)
	{
		memset(string, 0, strlen(string));
		free(string);
	}

    close(fd);
    if(structJson != NULL)
    {
    	s2j_delete_json_obj(structJson);
    }

    return ret;
}


/**
 * @brief 获取json文件，获得配置信息
 * @param fileName 要写入的文件名称
 * @param edgeGatewayStruct 边缘网关配置信息结构体
 * @return 成功：0 失败：其他
 */
int GetJsonFile(char *filename, EdgeGatewayConfig **edgeGatewayStruct)
{
	int fd = 0, ret = 0;
    char* string = NULL;
    struct stat fileStat;
	off_t fileSize = 0;
	size_t stringLenth = 0;
    cJSON *readJson;

    fd = open(filename, O_RDONLY);
	if(fd < 0)
    {
    	printf_debug("file %s open error\n", filename);
		return OPEN_FILE_FAIL;
    }

    //获取json文件
	fstat(fd, &fileStat);
	fileSize = fileStat.st_size;	//获取文件大小
    string = malloc(fileSize);   //申请内存
    if(NULL == string)
    {
    	printf_debug("malloc error\n");
    	ret = POINT_NULL;
    	goto JSON_RES;
    }

    stringLenth = read(fd, string, fileSize);
    if(stringLenth == -1)
    {
    	printf_debug("read error\n");
    	ret = FUNCTION_FAIL;
        goto JSON_RES;
    }

    readJson = vPort_Get_cJSON(string);
    if(NULL == readJson)
    {
    	printf_debug("vPort_Get_cJSON error\n");
    	ret = POINT_NULL;
        goto JSON_RES;
    }

    *edgeGatewayStruct = json_to_struct(readJson);

    if(readJson != NULL)
	{
		s2j_delete_json_obj(readJson);
	}

JSON_RES:
	close(fd);
	if(NULL == string)
	{
		free(string);  //释放内存
	}

    return ret;
}


/*
 * 串口转网口配置信息，由json转struct
 */
#define J2S_UartToNet(struct_obj,json_obj) do{		\
	s2j_struct_get_basic_element(struct_obj, json_obj, int, ProtocolType);	\
    s2j_struct_get_basic_element(struct_obj, json_obj, string, uartName);	\
    s2j_struct_get_basic_element(struct_obj, json_obj, int, bandrate);		\
    s2j_struct_get_basic_element(struct_obj, json_obj, int, uartType);		\
    s2j_struct_get_basic_element(struct_obj, json_obj, string, localIP);	\
    s2j_struct_get_basic_element(struct_obj, json_obj, int, localPort);		\
    s2j_struct_get_basic_element(struct_obj, json_obj, string, remoteIP);	\
    s2j_struct_get_basic_element(struct_obj, json_obj, int, remotePort);	\
}while(0)


/*
 * Modbus配置信息，由json转struct
 */
#define J2S_Modbus(struct_obj,json_obj) do{		\
	s2j_struct_get_basic_element(struct_obj, json_obj, string, uartName);		\
	s2j_struct_get_basic_element(struct_obj, json_obj, int, bandrate);			\
	s2j_struct_get_basic_element(struct_obj, json_obj, int, uartType);			\
	s2j_struct_get_basic_element(struct_obj, json_obj, int, slaveNumber);		\
	s2j_struct_get_array_element(struct_obj, json_obj, int, slaveID);			\
	s2j_struct_get_basic_element(struct_obj, json_obj, string, dataFilename);	\
}while(0)


/*
 * MQTT配置信息，由json转struct
 */
#define J2S_Mqtt(struct_obj,json_obj) do{		\
	s2j_struct_get_basic_element(struct_obj, json_obj, int, mqttNumber);	\
	s2j_struct_get_basic_element(struct_obj, json_obj, string, username1);	\
	s2j_struct_get_basic_element(struct_obj, json_obj, string, username2);	\
	s2j_struct_get_basic_element(struct_obj, json_obj, string, username3);	\
	s2j_struct_get_basic_element(struct_obj, json_obj, string, username4);	\
	s2j_struct_get_basic_element(struct_obj, json_obj, string, username5);	\
}while(0)


/*
 * 串口转网口配置信息，由struct转json
 */
#define S2J_UartToNet(json_obj,struct_obj) do{		\
	s2j_json_set_basic_element(json_obj, struct_obj, int, ProtocolType);	\
	s2j_json_set_basic_element(json_obj, struct_obj, string, uartName);		\
	s2j_json_set_basic_element(json_obj, struct_obj, int, bandrate);		\
	s2j_json_set_basic_element(json_obj, struct_obj, int, uartType);		\
	s2j_json_set_basic_element(json_obj, struct_obj, string, localIP);		\
	s2j_json_set_basic_element(json_obj, struct_obj, int, localPort);		\
	s2j_json_set_basic_element(json_obj, struct_obj, string, remoteIP);		\
	s2j_json_set_basic_element(json_obj, struct_obj, int, remotePort);		\
}while(0)


/*
 * Modbus配置信息，由struct转json
 */
#define S2J_Modbus(json_obj,struct_obj) do{		\
	s2j_json_set_basic_element(json_obj, struct_obj, string, uartName);		\
	s2j_json_set_basic_element(json_obj, struct_obj, int, bandrate);		\
	s2j_json_set_basic_element(json_obj, struct_obj, int, uartType);		\
	s2j_json_set_basic_element(json_obj, struct_obj, int, slaveNumber);		\
	s2j_json_set_array_element(json_obj, struct_obj, int, slaveID, 256);	\
	s2j_json_set_basic_element(json_obj, struct_obj, string, dataFilename);	\
}while(0)


/*
 * MQTT配置信息，由struct转json
 */
#define S2J_Mqtt(json_obj,struct_obj) do{		\
	s2j_json_set_basic_element(json_obj, struct_obj, int, mqttNumber);	\
	s2j_json_set_basic_element(json_obj, struct_obj, string, username1);\
	s2j_json_set_basic_element(json_obj, struct_obj, string, username2);\
	s2j_json_set_basic_element(json_obj, struct_obj, string, username3);\
	s2j_json_set_basic_element(json_obj, struct_obj, string, username4);\
	s2j_json_set_basic_element(json_obj, struct_obj, string, username5);\
}while(0)


/**
 * @breif JSON object to structure object
 * @param json_obj JSON object
 * @return structure object
 */
static void *json_to_struct(cJSON* json_obj)
{
    /* create structure object */
    s2j_create_struct_obj(struct_edgeGateway, EdgeGatewayConfig);

    /* deserialize data to structure object. */
    /* 串口转网口相关配置信息 */
    s2j_struct_get_basic_element(struct_edgeGateway, json_obj, int, uartToNetNumber);
    s2j_struct_get_struct_element(struct_UartToNet1, struct_edgeGateway, json_UartToNet1, json_obj, UartToNetConfig, uartToNet1);
    J2S_UartToNet(struct_UartToNet1, json_UartToNet1);
    s2j_struct_get_struct_element(struct_UartToNet2, struct_edgeGateway, json_UartToNet2, json_obj, UartToNetConfig, uartToNet2);
    J2S_UartToNet(struct_UartToNet2, json_UartToNet2);
    s2j_struct_get_struct_element(struct_UartToNet3, struct_edgeGateway, json_UartToNet3, json_obj, UartToNetConfig, uartToNet3);
    J2S_UartToNet(struct_UartToNet3, json_UartToNet3);
    s2j_struct_get_struct_element(struct_UartToNet4, struct_edgeGateway, json_UartToNet4, json_obj, UartToNetConfig, uartToNet4);
    J2S_UartToNet(struct_UartToNet4, json_UartToNet4);
    s2j_struct_get_struct_element(struct_UartToNet5, struct_edgeGateway, json_UartToNet5, json_obj, UartToNetConfig, uartToNet5);
    J2S_UartToNet(struct_UartToNet5, json_UartToNet5);

    /* 噪声传感器相关配置信息 */
    s2j_struct_get_basic_element(struct_edgeGateway, json_obj, int, noiseNumber);
    s2j_struct_get_struct_element(struct_Noise1, struct_edgeGateway, json_Noise1, json_obj, ModbusConfig, noiseSersor1);
    J2S_Modbus(struct_Noise1, json_Noise1);
    s2j_struct_get_struct_element(struct_Noise2, struct_edgeGateway, json_Noise2, json_obj, ModbusConfig, noiseSersor2);
    J2S_Modbus(struct_Noise2, json_Noise2);
    s2j_struct_get_struct_element(struct_Noise3, struct_edgeGateway, json_Noise3, json_obj, ModbusConfig, noiseSersor3);
    J2S_Modbus(struct_Noise3, json_Noise3);
    s2j_struct_get_struct_element(struct_Noise4, struct_edgeGateway, json_Noise4, json_obj, ModbusConfig, noiseSersor4);
    J2S_Modbus(struct_Noise4, json_Noise4);
    s2j_struct_get_struct_element(struct_Noise5, struct_edgeGateway, json_Noise5, json_obj, ModbusConfig, noiseSersor5);
    J2S_Modbus(struct_Noise5, json_Noise5);

    /* 六合一空气质量传感器相关配置信息 */
	s2j_struct_get_basic_element(struct_edgeGateway, json_obj, int, airNumber);
	s2j_struct_get_struct_element(struct_Air1, struct_edgeGateway, json_Air1, json_obj, ModbusConfig, airSersor1);
	J2S_Modbus(struct_Air1, json_Air1);
	s2j_struct_get_struct_element(struct_Air2, struct_edgeGateway, json_Air2, json_obj, ModbusConfig, airSersor2);
	J2S_Modbus(struct_Air2, json_Air2);
	s2j_struct_get_struct_element(struct_Air3, struct_edgeGateway, json_Air3, json_obj, ModbusConfig, airSersor3);
	J2S_Modbus(struct_Air3, json_Air3);
	s2j_struct_get_struct_element(struct_Air4, struct_edgeGateway, json_Air4, json_obj, ModbusConfig, airSersor4);
	J2S_Modbus(struct_Air4, json_Air4);
	s2j_struct_get_struct_element(struct_Air5, struct_edgeGateway, json_Air5, json_obj, ModbusConfig, airSersor5);
	J2S_Modbus(struct_Air5, json_Air5);

	/* 测温中继相关配置信息 */
	s2j_struct_get_basic_element(struct_edgeGateway, json_obj, int, tempNumber);
	s2j_struct_get_struct_element(struct_Temp1, struct_edgeGateway, json_Temp1, json_obj, ModbusConfig, tempSersor1);
	J2S_Modbus(struct_Temp1, json_Temp1);
	s2j_struct_get_struct_element(struct_Temp2, struct_edgeGateway, json_Temp2, json_obj, ModbusConfig, tempSersor2);
	J2S_Modbus(struct_Temp2, json_Temp2);
	s2j_struct_get_struct_element(struct_Temp3, struct_edgeGateway, json_Temp3, json_obj, ModbusConfig, tempSersor3);
	J2S_Modbus(struct_Temp3, json_Temp3);
	s2j_struct_get_struct_element(struct_Temp4, struct_edgeGateway, json_Temp4, json_obj, ModbusConfig, tempSersor4);
	J2S_Modbus(struct_Temp4, json_Temp4);
	s2j_struct_get_struct_element(struct_Temp5, struct_edgeGateway, json_Temp5, json_obj, ModbusConfig, tempSersor5);
	J2S_Modbus(struct_Temp5, json_Temp5);

    /* MQTT通信相关配置信息 */
    s2j_struct_get_struct_element(struct_Mqtt, struct_edgeGateway, json_Mqtt, json_obj, MqttConfig, mqttAccess);
    J2S_Mqtt(struct_Mqtt, json_Mqtt);

    /* return Student structure object pointer */
    return struct_edgeGateway;
}


/**
 * @breif Student structure object to JSON object
 * @param struct_obj structure object
 * @return JSON object
 */
static cJSON *struct_to_json(void* struct_obj)
{
	EdgeGatewayConfig *struct_edgeGateway = (EdgeGatewayConfig *)struct_obj;

    /* create Student JSON object */
    s2j_create_json_obj(json_edgeGateway);

    /* 串口转网口相关配置信息 */
    s2j_json_set_basic_element(json_edgeGateway, struct_edgeGateway, int, uartToNetNumber);
    s2j_json_set_struct_element(json_UartToNet1, json_edgeGateway, struct_UartToNet1, struct_edgeGateway, UartToNetConfig, uartToNet1);
    S2J_UartToNet(json_UartToNet1, struct_UartToNet1);
    s2j_json_set_struct_element(json_UartToNet2, json_edgeGateway, struct_UartToNet2, struct_edgeGateway, UartToNetConfig, uartToNet2);
    S2J_UartToNet(json_UartToNet2, struct_UartToNet2);
    s2j_json_set_struct_element(json_UartToNet3, json_edgeGateway, struct_UartToNet3, struct_edgeGateway, UartToNetConfig, uartToNet3);
    S2J_UartToNet(json_UartToNet3, struct_UartToNet3);
    s2j_json_set_struct_element(json_UartToNet4, json_edgeGateway, struct_UartToNet4, struct_edgeGateway, UartToNetConfig, uartToNet4);
    S2J_UartToNet(json_UartToNet4, struct_UartToNet4);
    s2j_json_set_struct_element(json_UartToNet5, json_edgeGateway, struct_UartToNet5, struct_edgeGateway, UartToNetConfig, uartToNet5);
    S2J_UartToNet(json_UartToNet5, struct_UartToNet5);

    /* 噪声传感器相关配置信息 */
    s2j_json_set_basic_element(json_edgeGateway, struct_edgeGateway, int, noiseNumber);
    s2j_json_set_struct_element(json_Modbus1, json_edgeGateway, struct_Modbus1, struct_edgeGateway, ModbusConfig, noiseSersor1);
    S2J_Modbus(json_Modbus1, struct_Modbus1);
    s2j_json_set_struct_element(json_Modbus2, json_edgeGateway, struct_Modbus2, struct_edgeGateway, ModbusConfig, noiseSersor2);
    S2J_Modbus(json_Modbus2, struct_Modbus2);
    s2j_json_set_struct_element(json_Modbus3, json_edgeGateway, struct_Modbus3, struct_edgeGateway, ModbusConfig, noiseSersor3);
    S2J_Modbus(json_Modbus3, struct_Modbus3);
    s2j_json_set_struct_element(json_Modbus4, json_edgeGateway, struct_Modbus4, struct_edgeGateway, ModbusConfig, noiseSersor4);
    S2J_Modbus(json_Modbus4, struct_Modbus4);
    s2j_json_set_struct_element(json_Modbus5, json_edgeGateway, struct_Modbus5, struct_edgeGateway, ModbusConfig, noiseSersor5);
    S2J_Modbus(json_Modbus5, struct_Modbus5);

    /* 六合一空气质量传感器相关配置信息 */
	s2j_json_set_basic_element(json_edgeGateway, struct_edgeGateway, int, airNumber);
	s2j_json_set_struct_element(json_Air1, json_edgeGateway, struct_Air1, struct_edgeGateway, ModbusConfig, airSersor1);
	S2J_Modbus(json_Air1, struct_Air1);
	s2j_json_set_struct_element(json_Air2, json_edgeGateway, struct_Air2, struct_edgeGateway, ModbusConfig, airSersor2);
	S2J_Modbus(json_Air2, struct_Air2);
	s2j_json_set_struct_element(json_Air3, json_edgeGateway, struct_Air3, struct_edgeGateway, ModbusConfig, airSersor3);
	S2J_Modbus(json_Air3, struct_Air3);
	s2j_json_set_struct_element(json_Air4, json_edgeGateway, struct_Air4, struct_edgeGateway, ModbusConfig, airSersor4);
	S2J_Modbus(json_Air4, struct_Air4);
	s2j_json_set_struct_element(json_Air5, json_edgeGateway, struct_Air5, struct_edgeGateway, ModbusConfig, airSersor5);
	S2J_Modbus(json_Air5, struct_Air5);

	/* 测温中继相关配置信息 */
	s2j_json_set_basic_element(json_edgeGateway, struct_edgeGateway, int, tempNumber);
	s2j_json_set_struct_element(json_Temp1, json_edgeGateway, struct_Temp1, struct_edgeGateway, ModbusConfig, tempSersor1);
	S2J_Modbus(json_Temp1, struct_Temp1);
	s2j_json_set_struct_element(json_Temp2, json_edgeGateway, struct_Temp2, struct_edgeGateway, ModbusConfig, tempSersor2);
	S2J_Modbus(json_Temp2, struct_Temp2);
	s2j_json_set_struct_element(json_Temp3, json_edgeGateway, struct_Temp3, struct_edgeGateway, ModbusConfig, tempSersor3);
	S2J_Modbus(json_Temp3, struct_Temp3);
	s2j_json_set_struct_element(json_Temp4, json_edgeGateway, struct_Temp4, struct_edgeGateway, ModbusConfig, tempSersor4);
	S2J_Modbus(json_Temp4, struct_Temp4);
	s2j_json_set_struct_element(json_Temp5, json_edgeGateway, struct_Temp5, struct_edgeGateway, ModbusConfig, tempSersor5);
	S2J_Modbus(json_Temp5, struct_Temp5);

    /* MQTT通信相关配置信息 */
    s2j_json_set_struct_element(json_Mqtt, json_edgeGateway, struct_Mqtt, struct_edgeGateway, MqttConfig, mqttAccess);
    S2J_Mqtt(json_Mqtt, struct_Mqtt);

    /* return Student JSON object pointer */
    return json_edgeGateway;
}


/**
 * @fn vPort_Get_cJSON
 * @brief 获取json，重新封装cJSON_Parse函数
 * @param value 指向要转换为json结构的字符串
 * @return 返回指向获取到的json指针
 *
 */
static cJSON* vPort_Get_cJSON(const char *value)
{
    return cJSON_Parse(value);
}


/**
 * @fn vPort_Print_cJSON
 * @brief 打印json，重新封装cJSON_Print函数
 * @param item 指向要打印的json结构体的指针
 * @return 返回指向打印的json结构体生成的字符串指针
 */
static char* vPort_Print_cJSON(cJSON *item)
{
    return cJSON_Print(item);
}

