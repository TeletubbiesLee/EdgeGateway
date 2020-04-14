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


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include "Uart.h"
#include "../Config.h"


static int SpeedArray[] = {
	B921600, B460800, B230400, B115200, B57600, B38400,
    B19200, B9600, B4800, B2400, B1200, B300,
};
static int BandrateArray[] = {
	921600, 460800, 230400, 115200, 57600, 38400,
    19200, 9600, 4800, 2400, 1200, 300,
};


/**
 * @breif 打开串口设备
 * @param device 串口设备的字符串名称
 * @param bandrate 波特率
 * @return 设备文件描述符或-1
 */
int UartInit(char *device, int bandrate)
{
	int uartFd = -1;
	int ret = 0;

	uartFd = OpenDevice(device);
	if (uartFd > 0)
	{
		SetSpeed(uartFd, bandrate);
	}
	else
	{
		printf_debug("Error opening %s\n", device);
		return FUNCTION_FAIL;
	}

	ret = SetParity(uartFd, UART_DATA_BITS_NUM, UART_STOP_BITS_NUM, UART_PARITY_NUM);
	if (FUNCTION_FAIL == ret)
	{
		printf_debug("Set Parity Error\n");
		close(uartFd);
		return FUNCTION_FAIL;
	}

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
 	if (-1 == fd)
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
 * @breif 设置串口传输速度
 * @param fd 串口设备的文件描述符
 * @param speed 波特率
 * @return void
 */
void SetSpeed(int fd, int speed)
{
	int i = 0;
	int status = 0;
	struct termios opt;

	tcgetattr(fd, &opt);

	for(i = 0; i < sizeof(SpeedArray) / sizeof(int); i++)
    {
		if(speed == BandrateArray[i])
        {
			tcflush(fd, TCIOFLUSH);
			cfsetispeed(&opt, SpeedArray[i]);
			cfsetospeed(&opt, SpeedArray[i]);
			status = tcsetattr(fd, TCSANOW, &opt);
			if(status != 0)
			{
				printf_debug("tcsetattr fd1\n");
			}
			return;
		}
		tcflush(fd, TCIOFLUSH);
  	 }

	if (i == 12)
    {
		printf_debug("\tSorry, please set the correct baud rate!\n");
		PrintUartUsage();
	}
}


/**
 * @brief 设置串口数据位，停止位和效验位
 * @param fd 打开的串口文件句柄*
 * @param dataBits 数据位，取值为：7 or 8
 * @param stopBits 停止位，取值为：1 or 2
 * @param parity 校验类型，取值为：N E O S
 * @return 成功:0 错误:-1
 */
int SetParity(int fd, int dataBits, int stopBits, int parity)
{
	struct termios options;
	if (tcgetattr(fd, &options) != 0)
	{
		printf_debug("SetupSerial 1\n");
		return FUNCTION_FAIL;
	}
	options.c_cflag &= ~CSIZE;
	switch (dataBits) /*设置数据位数*/
	{
		case 7:
			options.c_cflag |= CS7;
			break;
		case 8:
			options.c_cflag |= CS8;
			break;
		default:
			printf_debug("Unsupported data size\n");
			return FUNCTION_FAIL;
	}

	switch (parity)
	{
		case 'n':
		case 'N':
			options.c_cflag &= ~PARENB;   /* Clear parity enable */
			options.c_iflag &= ~INPCK;     /* Enable parity checking */
			break;
		case 'o':
		case 'O':
			options.c_cflag |= (PARODD | PARENB);  /* 设置为奇效验*/
			options.c_iflag |= INPCK;             /* Disnable parity checking */
			break;
		case 'e':
		case 'E':
			options.c_cflag |= PARENB;     /* Enable parity */
			options.c_cflag &= ~PARODD;   /* 转换为偶效验*/
			options.c_iflag |= INPCK;       /* Disnable parity checking */
			break;
		case 'S':
		case 's':  /*as no parity*/
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
			break;
		default:
			printf_debug("Unsupported parity\n");
			return FUNCTION_FAIL;
	}
 	/* 设置停止位*/
  	switch (stopBits)
	{
		case 1:
			options.c_cflag &= ~CSTOPB;
			break;
		case 2:
			options.c_cflag |= CSTOPB;
			break;
		default:
			printf_debug("Unsupported stop bits\n");
			return FUNCTION_FAIL;
 	}
  	/* Set input parity option */
  	if (parity != 'n')
    	options.c_iflag |= INPCK;
  	options.c_cc[VTIME] = 150; // 15 seconds
    options.c_cc[VMIN] = 0;

	options.c_lflag &= ~(ECHO | ICANON);

  	tcflush(fd, TCIFLUSH);		/* Update the options and do it NOW */
  	if (tcsetattr(fd, TCSANOW, &options) != 0)
	{
  		printf_debug("SetupSerial 3\n");
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
