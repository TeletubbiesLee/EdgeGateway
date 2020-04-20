/**
 * @file modbus-config.h
 * @brief modbus配置头文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.02.17
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _MODBUS_CONFIG_H_
#define _MODBUS_CONFIG_H_


/* Constants defined by configure.ac */
#define HAVE_INTTYPES_H @HAVE_INTTYPES_H@
#define HAVE_STDINT_H @HAVE_STDINT_H@

#ifdef HAVE_INTTYPES_H
    #include <inttypes.h>
#endif

#ifdef HAVE_STDINT_H
    #ifndef _MSC_VER
        #include <stdint.h>
    #else
        #include "stdint.h"
    #endif
#endif

#define BUG_REPORT(_cond, _format, _args ...) \
    printf("\nLine %d: assertion error for '%s': " _format "\n", __LINE__, # _cond, ## _args);

#define ASSERT_TRUE(_cond, _format, __args...) {  \
            if (_cond) {                                  \
                printf("OK\n");                           \
            } else {                                      \
                BUG_REPORT(_cond, _format, ## __args);    \
                goto close;                               \
            }                                             \
        };


#define UART_PARITY         			'N'			//串口数据奇偶校验位
#define UART_DATA_BIT       			8			//串口数据位
#define UART_STOP_BIT       			1			//串口停止位


/*
 * modbus类型枚举
 */
enum {
    MODBUS_TCP,
    MODBUS_TCP_PI,
    MODBUS_RTU
};


/*
 * modbus功能码
 */
enum {
    COIL_BITS = 1,
    DISCRETE_INPUTS,
    HOLDING_REGISTER,
    INPUT_REGISTER
};


#endif /* _MODBUS_CONFIG_H_ */
