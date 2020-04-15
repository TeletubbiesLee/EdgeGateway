边缘网关程序框架

原则：
	1.各部分程序按不同功能分类进行存放，一种功能相关文件存放在一个文件夹，尽可能减少各部分功能对外开放的接口，做到高内聚低耦合
	2.main函数中主要为根据配置建立进程


透传功能：TransparentTransmission/
	1.接口函数：头文件包含TransparentTransmission.h，函数调用TransparentTransmission()
	2.外部联系：Config.h、DataStruct.h
	
信号处理相关：ProcessSignal/
	1.接口函数：头文件包含ProcessSignal.h，函数调用SetProcessCloseSignal()





