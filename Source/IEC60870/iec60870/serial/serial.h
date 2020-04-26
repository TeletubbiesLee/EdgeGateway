/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      serial.h
  * @brief:		  串口头文件
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2018-11-07
  * @update:    [2018-11-07][Chen][newly increased]
  */
#ifndef _SERIAL_H_
#define _SERIAL_H_
/* INCLUDE FILES -------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

/* PRIVATE VARIABLES ---------------------------------------------------------*/

/* PUBLIC VARIABLE -----------------------------------------------------------*/
typedef struct sSerialPort* SerialPort;
typedef enum 
{
    SERIAL_PORT_ERROR_NONE = 0,
    SERIAL_PORT_ERROR_INVALID_ARGUMENT = 1,
    SERIAL_PORT_ERROR_INVALID_BAUDRATE = 2,
    SERIAL_PORT_ERROR_OPEN_FAILED = 3,
    SERIAL_PORT_ERROR_UNKNOWN = 99
} SerialPortError;
typedef struct 
{
    char name[100];
    int fd;
}serial_name_fd_str;

/* PRIVATE FUNCTION PROTOTYPES -----------------------------------------------*/

/* PUBLIC FUNCTION PROTOTYPES ------------------------------------------------*/
SerialPort SerialPort_create(const uint8_t portNo, int baudRate, uint8_t dataBits, uint8_t parity, uint8_t stopBits);
void SerialPort_destroy(SerialPort self);
int SerialPort_open(SerialPort self,const uint8_t portNo);
void SerialPort_close(SerialPort self);
int  SerialPort_getBaudRate(SerialPort self);
void SerialPort_setTimeout(SerialPort self, int timeout);
void SerialPort_discardInBuffer(SerialPort self);
int SerialPort_readByte(uint8_t fd, uint8_t *pbuf, uint16_t count);
int SerialPort_write(int fd, uint8_t* buffer, int startPos, int bufSize);
SerialPortError SerialPort_getLastError(SerialPort self);
int portNo_to_name(char *name, uint8_t portNo);
int portNo_to_fd(int portNo);

#ifdef __cplusplus
}
#endif

#endif /* _SERIAL_H_ */
/* END OF FILE ----------------------------------------------------------------*/
