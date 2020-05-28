/**
  * @copyright  Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      dataSwitch_format_common.h
  * @brief:     The format common of the project.
  * @version:   ver 1.1
  * @author:    Mr.J
  * @date:      2018-10-15
  * @update:    
  */

#ifndef _DATA_SWITCH_FORMAT_COMMON_H_
#define _DATA_SWITCH_FORMAT_COMMON_H_

/* INCLUDES FILES ------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>

/* STRUCTURE DEFINITION ------------------------------------------------------------------------------------------------------------------------------------------*/
struct tagCommonMoldIn
{
    uint16_t addr;             // 地址
    void *pValue;            // 值
};
typedef struct tagCommonMoldIn *tagCommonMoldIn_t;

struct tagSwitchQuantityIn
{
    uint16_t addr;             // 地址
    uint8_t *pValue;            // 值
};
typedef struct tagSwitchQuantityIn *tagSwitchQuantityIn_t;

struct tagAnalogQuantityIn
{
    uint16_t addr;             // 地址
    float *pValue;            // 值
};
typedef struct tagAnalogQuantityIn *tagAnalogQuantityIn_t;

struct tagControlOutputIn
{
    uint16_t addr;             // 地址
    float *pValue;            // 值
};
typedef struct tagControlOutputIn *tagControlOutputIn_t;

struct tagRelayOutputIn
{
    uint16_t addr;             // 地址
    uint8_t *pValue;            // 值
};
typedef struct tagRelayOutputIn *tagRelayOutputIn_t;

struct tagValueParaIn
{
    uint16_t addr;             // 地址
    float *pValue;            // 值
};
typedef struct tagValueParaIn *tagValueParaIn_t;

#endif /* _DATA_SWITCH_FORMAT_COMMON_H_ */

/* FILE END  ------------------------------------------------------------------------*/
