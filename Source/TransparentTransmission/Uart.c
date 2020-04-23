/**
 * @file Uart.c
 * @brief Linux系统中串口设备相关操作函数文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.02.17
 *
 * @author Lei.L
 * @version ver 1.0
 */


#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include "Uart.h"
#include "../Config.h"
#include "../RS485/RS485.h"
#include "../DataStruct.h"


static struct termios newtio, oldtio;

/**
 * @breif 打开串口设备
 * @param device 串口设备的字符串名称
 * @param bandrate 波特率
 * @return 设备文件描述符或-1
 */
int UartInit(char *device, int bandrate, int type)
{
	int uartFd = -1;
	int ret = 0;

	uartFd = OpenDevice(device);
	if(uartFd < 0)
	{
		printf_debug("Error opening %s\n", device);
		return FUNCTION_FAIL;
	}

	if(RS485_TYPE == type)			//485接收时，将使能引脚电平拉低
		RS485_Enable(uartFd, ENABLE_485);

	ret = SetPort(uartFd, bandrate, UART_DATA_BITS_NUM, UART_STOP_BITS_NUM, UART_PARITY_NUM);
	if (FUNCTION_FAIL == ret)
	{
		printf_debug("Set Parity Error\n");
		close(uartFd);
		return FUNCTION_FAIL;
	}
	write(uartFd, "SOJO", strlen("SOJO"));

	return uartFd;
}


/**
 * @breif 打开串口设备
 * @param dev 串口设备的字符串名称
 * @return 设备文件描述符或-1
 */
int OpenDevice(char *dev)
{
	int fd = open(dev, O_RDWR);         //阻塞模式打开 | O_NOCTTY | O_NDELAY | O_NONBLOCK
 	if (fd < 0)
    {
 		printf_debug("Can't Open Serial Port: %s.\n", dev);
   		return FUNCTION_FAIL;
	}
    else
    {
        return fd;
    }	
}


/**
 * @breif 设置串口传输速度，数据位，校验位，停止位等
 * @param fd 串口设备的文件描述符
 * @param nSpeed 波特率
 * @param nBits	数据位
 * @param nEvent 校验位
 * @param nStop 停止位
 * @return 成功：0 失败：-1
 */
int SetPort(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
	memset(&oldtio, 0, sizeof(oldtio));
	/* save the old serial port configuration */
	if(tcgetattr(fd, &oldtio) != 0) {
		printf_debug("SetPort tcgetattr");
		return FUNCTION_FAIL;
	}

	memset(&newtio, 0, sizeof(newtio));
	/* ignore modem control lines and enable receiver */
	newtio.c_cflag |= CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;
	/* set character size */
	switch (nBits) {
		case 8:
			newtio.c_cflag |= CS8;
			break;
		case 7:
			newtio.c_cflag |= CS7;
			break;
		case 6:
			newtio.c_cflag |= CS6;
			break;
		case 5:
			newtio.c_cflag |= CS5;
			break;
		default:
			newtio.c_cflag |= CS8;
			break;
	}
	/* set the parity */
	switch (nEvent) {
		case 'o':
		case 'O':
			newtio.c_cflag |= PARENB;
			newtio.c_cflag |= PARODD;
			newtio.c_iflag |= (INPCK | ISTRIP);
			break;
		case 'e':
		case 'E':
			newtio.c_cflag |= PARENB;
			newtio.c_cflag &= ~PARODD;
			newtio.c_iflag |= (INPCK | ISTRIP);
			break;
		case 'n':
		case 'N':
			newtio.c_cflag &= ~PARENB;
			break;
		default:
			newtio.c_cflag &= ~PARENB;
			break;
	}
	/* set the stop bits */
	switch (nStop) {
		case 1:
			newtio.c_cflag &= ~CSTOPB;
			break;
		case 2:
			newtio.c_cflag |= CSTOPB;
			break;
		default:
			newtio.c_cflag &= ~CSTOPB;
			break;
	}
	/* set output and input baud rate */
	switch (nSpeed) {
		case 0:
			cfsetospeed(&newtio, B0);
			cfsetispeed(&newtio, B0);
			break;
		case 50:
			cfsetospeed(&newtio, B50);
			cfsetispeed(&newtio, B50);
			break;
		case 75:
			cfsetospeed(&newtio, B75);
			cfsetispeed(&newtio, B75);
			break;
		case 110:
			cfsetospeed(&newtio, B110);
			cfsetispeed(&newtio, B110);
			break;
		case 134:
			cfsetospeed(&newtio, B134);
			cfsetispeed(&newtio, B134);
			break;
		case 150:
			cfsetospeed(&newtio, B150);
			cfsetispeed(&newtio, B150);
			break;
		case 200:
			cfsetospeed(&newtio, B200);
			cfsetispeed(&newtio, B200);
			break;
		case 300:
			cfsetospeed(&newtio, B300);
			cfsetispeed(&newtio, B300);
			break;
		case 600:
			cfsetospeed(&newtio, B600);
			cfsetispeed(&newtio, B600);
			break;
		case 1200:
			cfsetospeed(&newtio, B1200);
			cfsetispeed(&newtio, B1200);
			break;
		case 1800:
			cfsetospeed(&newtio, B1800);
			cfsetispeed(&newtio, B1800);
			break;
		case 2400:
			cfsetospeed(&newtio, B2400);
			cfsetispeed(&newtio, B2400);
			break;
		case 4800:
			cfsetospeed(&newtio, B4800);
			cfsetispeed(&newtio, B4800);
			break;
		case 9600:
			cfsetospeed(&newtio, B9600);
			cfsetispeed(&newtio, B9600);
			break;
		case 19200:
			cfsetospeed(&newtio, B19200);
			cfsetispeed(&newtio, B19200);
			break;
		case 38400:
			cfsetospeed(&newtio, B38400);
			cfsetispeed(&newtio, B38400);
			break;
		case 57600:
			cfsetospeed(&newtio, B57600);
			cfsetispeed(&newtio, B57600);
			break;
		case 115200:
			cfsetospeed(&newtio, B115200);
			cfsetispeed(&newtio, B115200);
			break;
		case 230400:
			cfsetospeed(&newtio, B230400);
			cfsetispeed(&newtio, B230400);
			break;
		default:
			cfsetospeed(&newtio, B115200);
			cfsetispeed(&newtio, B115200);
			break;
	}
	/* set timeout in deciseconds for non-canonical read */
	newtio.c_cc[VTIME] = 0;
	/* set minimum number of characters for non-canonical read */
	newtio.c_cc[VMIN] = 0;
	/* flushes data received but not read */
	tcflush(fd, TCIFLUSH);
	/* set the parameters associated with the terminal from
		the termios structure and the change occurs immediately */
	if((tcsetattr(fd, TCSANOW, &newtio))!=0)
	{
		printf_debug("SetPort tcsetattr");
		return FUNCTION_FAIL;
	}

	return NO_ERROR;
}


/**
 * @brief 打印串口的使用方法
 * @return void
 */
void PrintUartUsage()
{
    printf("Usage: program_name option [ dev... ] \n");
    printf("\t-h  --help     Display this usage information.\n"
            "\t-d  --device   The device ttyS[0-3] or ttySCMA[0-1]\n"
	    	"\t-b  --baudrate Set the baud rate you can select\n"
	    	"\t               [230400, 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300]\n"
            "\t-s  --string   Write the device data\n");
    return;
}
