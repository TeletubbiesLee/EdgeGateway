边缘网关程序框架-Lei.L

原则：
	1.对整个程序中的文件夹功能进行记录，包括但不限于接口函数与外部引用。接口函数为该文件夹对其他文件夹程序提供的接口，外部引用为该文件夹引用的其他文件夹的内容
	2.各部分程序按不同功能分类进行存放，一种功能相关文件存放在一个文件夹，尽可能减少各部分功能对外开放的接口与联系，做到高内聚低耦合


程序文件夹介绍：
指示灯：Led/
	1.接口函数：头文件包含Led.h，函数调用IndicatorLedRunning()和IndicatorLedOnOrOff()
	
Modbus通信相关：Modbus/
	六合一空气质量传感器：AirQualitySensor/
		1.接口函数：头文件包含AirQualitySensor.h，函数调用AirQualitySensor()
		2.外部引用：DataStruct.h、Config.h、modbus.h、ModbusInit.h
	modbus第三方库：libmodbus/
		1.接口函数：头文件包含modbus.h、modbus-config.h
	噪声传感器：NoiseSensor/
		1.接口函数：头文件包含NoiseSensor.h，函数调用NoiseSensor()
		2.外部引用：DataStruct.h、Config.h、modbus.h、ModbusInit.h
	双杰测温中继：SOJO/
		1.接口函数：
		2.外部引用：

MQTT通信相关：MQTT/
	paho mqtt-c第三方库：paho_mqtt_c/
		1.接口函数：头文件包含MqttPublish.h，函数调用MqttPublish()
		2.外部引用：Config.h

信号处理相关：ProcessSignal/
	1.接口函数：头文件包含ProcessSignal.h，函数调用SetProcessCloseSignal()

485相关：RS485/
	1.接口函数：头文件包含RS485.h，函数调用RS485_Enable()
	2.外部引用：DataStruct.h、Config.h
	
透传功能：TransparentTransmission/
	1.接口函数：头文件包含TransparentTransmission.h，函数调用TransparentTransmission()
	2.外部引用：Config.h、DataStruct.h、RS485.h






