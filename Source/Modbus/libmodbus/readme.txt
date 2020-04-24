libmodbus库的修改记录-Lei.L

(注：后续对libmodbus库的修改必须在此文件中添加修改日期和修改者，记录格式保持统一)
记录格式：
[日期] [修改人]
[修改文件]
	1.[修改概括]
		[具体修改代码]
	2.[修改概括]
		[具体修改代码]
	...

[日期] [修改人]
[修改文件]
	...
(注：不同日期间用空行分隔，不同文件间用空行分隔)


2020/04/23 Lei.L
modbus-rtu-private.h
	1.大约22、23行，添加两个宏定义，用于支持485：
		#define HAVE_DECL_TIOCSRS485 1
		#define HAVE_DECL_TIOCM_RTS 1

modbus-rtu.c
	1.函数modbus_rtu_set_serial_mode(modbus_t *ctx, int mode)中打约第922、923行添加两句程序：
		rs485conf.flags |= SER_RS485_RTS_ON_SEND;
        rs485conf.flags |= SER_RS485_RTS_AFTER_SEND;


