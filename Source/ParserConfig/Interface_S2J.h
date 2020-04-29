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

#include "struct2json/cJSON.h"

void *json_to_struct(cJSON* json_obj);
cJSON *struct_to_json(void* struct_obj);
cJSON* vPort_Get_cJSON(const char *value);
char* vPort_Print_cJSON(cJSON *item);
int S2J(void);

#endif

