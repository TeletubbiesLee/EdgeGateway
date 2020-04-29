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

#include <stdio.h>
#include "struct2json/s2j.h"
#include "Interface_S2J.h"


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

typedef struct TagModbusConfig{
	char uartName[20];
	int bandrate;
	int uartType;
	int slaveNumber;
	int slaveID[256];
	char dataFilename[30];
}ModbusConfig;

typedef struct TagMqttConfig{
	int mqttNumber;
	char username1[20];
	char username2[20];
//	char username3[20];
//	char username4[20];
//	char username5[20];
}MqttConfig;

typedef struct TagEdgeGatewayConfig{
	int uartToNetNumber;
	UartToNetConfig uartToNet1;
//	UartToNetConfig uartToNet2;
//	UartToNetConfig uartToNet3;
//	UartToNetConfig uartToNet4;
//	UartToNetConfig uartToNet5;
	int noiseNumber;
	ModbusConfig noiseSersor1;
//	ModbusConfig noiseSersor2;
//	ModbusConfig noiseSersor3;
//	ModbusConfig noiseSersor4;
//	ModbusConfig noiseSersor5;
//	int airNumber;
//	ModbusConfig airSersor1;
//	ModbusConfig airSersor2;
//	ModbusConfig airSersor3;
//	ModbusConfig airSersor4;
//	ModbusConfig airSersor5;
//	int tempNumber;
//	ModbusConfig tempSersor1;
//	ModbusConfig tempSersor2;
//	ModbusConfig tempSersor3;
//	ModbusConfig tempSersor4;
//	ModbusConfig tempSersor5;
	MqttConfig mqttAccess;
}EdgeGatewayConfig;

int S2J(void) {
	EdgeGatewayConfig orignal_student_obj = {};

    /* serialize Student structure object */
    cJSON *json_student = struct_to_json(&orignal_student_obj);

    printf("%s\n", vPort_Print_cJSON(json_student));
    /* deserialize Student structure object */
    EdgeGatewayConfig *converted_student_obj = json_to_struct(json_student);

    /* compare Student structure object */
    if(memcmp(&orignal_student_obj, converted_student_obj, sizeof(EdgeGatewayConfig))) {
        printf("Converted failed!\n");
    } else {
        printf("Converted OK!\n");
    }

    s2j_delete_json_obj(json_student);
    s2j_delete_struct_obj(converted_student_obj);

    return 0;
}

/**
 * Student JSON object to structure object
 *
 * @param json_obj JSON object
 *
 * @return structure object
 */
void *json_to_struct(cJSON* json_obj)
{
    /* create Student structure object */
    s2j_create_struct_obj(struct_student, EdgeGatewayConfig);

    /* deserialize data to Student structure object. */
    s2j_struct_get_basic_element(struct_student, json_obj, int, uartToNetNumber);
    s2j_struct_get_struct_element(struct_UartToNet, struct_student, json_UartToNet, json_obj, UartToNetConfig, uartToNet1);
    s2j_struct_get_basic_element(struct_UartToNet, json_UartToNet, int, ProtocolType);
    s2j_struct_get_basic_element(struct_UartToNet, json_UartToNet, string, uartName);
    s2j_struct_get_basic_element(struct_UartToNet, json_UartToNet, int, bandrate);
    s2j_struct_get_basic_element(struct_UartToNet, json_UartToNet, int, uartType);
    s2j_struct_get_basic_element(struct_UartToNet, json_UartToNet, string, localIP);
    s2j_struct_get_basic_element(struct_UartToNet, json_UartToNet, int, localPort);
    s2j_struct_get_basic_element(struct_UartToNet, json_UartToNet, string, remoteIP);
    s2j_struct_get_basic_element(struct_UartToNet, json_UartToNet, int, remotePort);

    s2j_struct_get_basic_element(struct_student, json_obj, int, noiseNumber);
    s2j_struct_get_struct_element(struct_Modbus, struct_student, json_Modbus, json_obj, ModbusConfig, noiseSersor1);
    s2j_struct_get_basic_element(struct_Modbus, json_Modbus, string, uartName);
    s2j_struct_get_basic_element(struct_Modbus, json_Modbus, int, bandrate);
    s2j_struct_get_basic_element(struct_Modbus, json_Modbus, int, uartType);
    s2j_struct_get_basic_element(struct_Modbus, json_Modbus, int, slaveNumber);
    s2j_struct_get_array_element(struct_Modbus, json_Modbus, int, slaveID);
    s2j_struct_get_basic_element(struct_Modbus, json_Modbus, string, dataFilename);

    s2j_struct_get_struct_element(struct_Mqtt, struct_student, json_Mqtt, json_obj, MqttConfig, mqttAccess);
    s2j_struct_get_basic_element(struct_Mqtt, json_Mqtt, int, mqttNumber);
    s2j_struct_get_basic_element(struct_Mqtt, json_Mqtt, string, username1);
    s2j_struct_get_basic_element(struct_Mqtt, json_Mqtt, string, username2);



#if 0
    s2j_struct_get_array_element(struct_student, json_obj, int, score);
    s2j_struct_get_basic_element(struct_student, json_obj, string, name);
    s2j_struct_get_basic_element(struct_student, json_obj, double, weight);
    // another xxx_ex api, add default value and more secure
    s2j_struct_get_array_element_ex(struct_student, json_obj, int, score, 256, 0);
    s2j_struct_get_basic_element_ex(struct_student, json_obj, string, name, "John");
    s2j_struct_get_basic_element_ex(struct_student, json_obj, double, weight, 0);


    /* deserialize data to Student.Hometown structure object. */
    s2j_struct_get_struct_element(struct_hometown, struct_student, json_hometown, json_obj, Hometown, hometown);
    s2j_struct_get_basic_element(struct_hometown, json_hometown, string, name);
#endif
    /* return Student structure object pointer */
    return struct_student;
}


/**
 * Student structure object to JSON object
 *
 * @param struct_obj structure object
 *
 * @param JSON object
 */
cJSON *struct_to_json(void* struct_obj)
{
	EdgeGatewayConfig *struct_student = (EdgeGatewayConfig *)struct_obj;

    /* create Student JSON object */
    s2j_create_json_obj(json_student);

    s2j_json_set_basic_element(json_student, struct_student, int, uartToNetNumber);
    s2j_json_set_struct_element(json_UartToNet, json_student, struct_UartToNet, struct_student, UartToNetConfig, uartToNet1);
    s2j_json_set_basic_element(json_UartToNet, struct_UartToNet, int, ProtocolType);
    s2j_json_set_basic_element(json_UartToNet, struct_UartToNet, string, uartName);
    s2j_json_set_basic_element(json_UartToNet, struct_UartToNet, int, bandrate);
    s2j_json_set_basic_element(json_UartToNet, struct_UartToNet, int, uartType);
    s2j_json_set_basic_element(json_UartToNet, struct_UartToNet, string, localIP);
    s2j_json_set_basic_element(json_UartToNet, struct_UartToNet, int, localPort);
    s2j_json_set_basic_element(json_UartToNet, struct_UartToNet, string, remoteIP);
    s2j_json_set_basic_element(json_UartToNet, struct_UartToNet, int, remotePort);

    s2j_json_set_basic_element(json_student, struct_student, int, noiseNumber);
    s2j_json_set_struct_element(json_Modbus, json_student, struct_Modbus, struct_student, ModbusConfig, noiseSersor1);
    s2j_json_set_basic_element(json_Modbus, struct_Modbus, string, uartName);
    s2j_json_set_basic_element(json_Modbus, struct_Modbus, int, bandrate);
    s2j_json_set_basic_element(json_Modbus, struct_Modbus, int, uartType);
    s2j_json_set_basic_element(json_Modbus, struct_Modbus, int, slaveNumber);
    s2j_json_set_array_element(json_Modbus, struct_Modbus, int, slaveID, 256);
    s2j_json_set_basic_element(json_Modbus, struct_Modbus, string, dataFilename);

    s2j_json_set_struct_element(json_Mqtt, json_student, struct_Mqtt, struct_student, MqttConfig, mqttAccess);
    s2j_json_set_basic_element(json_Mqtt, struct_Mqtt, int, mqttNumber);
    s2j_json_set_basic_element(json_Mqtt, struct_Mqtt, string, username1);
    s2j_json_set_basic_element(json_Mqtt, struct_Mqtt, string, username2);

#if 0
    /* serialize data to Student JSON object. */
    s2j_json_set_basic_element(json_student, struct_student, int, id);
    s2j_json_set_basic_element(json_student, struct_student, double, weight);
    s2j_json_set_array_element(json_student, struct_student, int, score, 256);
    s2j_json_set_basic_element(json_student, struct_student, string, name);

    /* serialize data to Student.Hometown JSON object. */
    s2j_json_set_struct_element(json_hometown, json_student, struct_hometown, struct_student, Hometown, hometown);
    s2j_json_set_basic_element(json_hometown, struct_hometown, string, name);
#endif
    /* return Student JSON object pointer */
    return json_student;
}


/**
 * @fn vPort_Get_cJSON
 * @brief 获取json，重新封装cJSON_Parse函数
 * @param value 指向要转换为json结构的字符串
 * @return 返回指向获取到的json指针
 *
 */
cJSON* vPort_Get_cJSON(const char *value)
{
    return cJSON_Parse(value);
}


/**
 * @fn vPort_Print_cJSON
 * @brief 打印json，重新封装cJSON_Print函数
 * @param item 指向要打印的json结构体的指针
 * @return 返回指向打印的json结构体生成的字符串指针
 */
char* vPort_Print_cJSON(cJSON *item)
{
    return cJSON_Print(item);
}

