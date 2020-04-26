/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2017-2018. All right reserved.
  * @file:      cs101SlaveConfig.h
  * @brief:     101协议类定义.
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2018-11-07
  * @update:    [2018-11-07][Chen][newly increased]
  */
#ifndef	_CS101_SLAVE_CONFIG_H_
#define _CS101_SLAVE_CONFIG_H_

/* SPECIAL VARIABLE -----------------------------------------------------------*/
#define CS101_SLAVE_COMMUNICATION  1   //针对通讯板//链路重新链接需要复位主站链路
#define CS101_SLAVE_E5  0   //针对无数据确认回复E5

/* PUBLIC VARIABLE -----------------------------------------------------------*/
enum
{
  CS101_SLAVE_DISK0 = 0,
	CS101_SLAVE_DISK1,
	CS101_SLAVE_DISK2,
  CS101_SLAVE_DISK3,
	CS101_SLAVE_DISK_VOLUMES
};
/* Number of volumes (logical drives) to be used.*/
#define CS101_SLAVE_LPDUSIZE  256
#define CS101_SLAVE_STOREDATA1NUM	8
#define CS101_SLAVE_STOREDATA2NUM	4

#define  CS101_SLAVE_NUMOF_MAXRETRY                0x03 // 默认DL/T634.5101_2002规约链路建立最多连接次数

//LINK_FLAG
#define  CS101_SLAVE_REQSTATUS                  0x00000001
#define  CS101_SLAVE_RECONFIRM                  0x00000002
#define  CS101_SLAVE_ASKSTATUS                  0x00000004
#define  CS101_SLAVE_ASKRELINK                  0x00000008

#define  CS101_SLAVE_SENDABLE                  	0x00100000// 平衡
#define  CS101_SLAVE_REDATA1                  	0x00400000// 非平衡
#define  CS101_SLAVE_REDATA2                  	0x00800000// 非平衡

#define  CS101_SLAVE_FIRST_ALL_CALLSTART       	0x10000000
#define  CS101_SLAVE_FIRST_ALL_CALLEND        	0x20000000
#define  CS101_SLAVE_FIRST_RlaConCode         	0x40000000
#define  CS101_SLAVE_INITEND                  	0x80000000

//1级数据状?
#define  CS101_SLAVE_HAVESTDATA1                0x00000001
#define  CS101_SLAVE_HAVEINITEND                0x00000002
#define  CS101_SLAVE_HAVERESET                  0x00000004
#define  CS101_SLAVE_CALLALLDATA                0x00000010
#define  CS101_SLAVE_HAVESOE                    0x00000020
#define  CS101_SLAVE_HAVECOS                    0x00000040

/*************************************************************************************************************************************/
//DL/T634.5101_2002规约

/* 链路层帧修饰词***************************************************************/
#define  CS101_SLAVE_STARTCODE10	                     0x10	// 起始码
#define  CS101_SLAVE_STARTCODE68	                     0x68	// 起始码
#define  CS101_SLAVE_ENDCODE                           0x16	// 终止码

/* 链路层控制域定义************************************************************/
//前4位
#define  CS101_SLAVE_FUNCODE                       0x0F // 功能码
#define  CS101_SLAVE_FCV                           0x10 // 帧计数有效位
#define  CS101_SLAVE_DFC                           0x10 // 数据流控制
#define  CS101_SLAVE_FCB                           0x20 // 帧计数位
#define  CS101_SLAVE_ACD                           0x20 // 要求访问位
#define  CS101_SLAVE_PRM                           0x40 // 启动报文位
#define  CS101_SLAVE_DIR                           0x80 // 传输方向位

//链路层主站向子站传输功能码定义
#define   CS101_SLAVE_M_FUN0             	0           // 复位远方链路
#define   CS101_SLAVE_M_FUN2              2           // 测试链路功能
#define   CS101_SLAVE_M_FUN3              3	          // 发送确认命令
#define   CS101_SLAVE_M_FUN4             	4	          // 发送不确认命令
#define   CS101_SLAVE_M_FUN8              8	          // 请求响应确定访问状态
#define   CS101_SLAVE_M_FUN9             	9	          // 召唤链路状态
#define   CS101_SLAVE_M_FUN10             10	        // 召唤1级用户数据
#define   CS101_SLAVE_M_FUN11             11          // 召唤2级用户数据

//*链路层子站向主站传输功能码定义
#define   CS101_SLAVE_S_FUN0              0            // 确认
#define   CS101_SLAVE_S_FUN1              1            // 确认链路忙未接收报文
#define   CS101_SLAVE_S_FUN8              8            // 以数据响应请求帧
#define   CS101_SLAVE_S_FUN9              9            // 无所召唤的数据
#define   CS101_SLAVE_S_FUN11             11           // 响应链路状态或回答请求帧
 
/* TI类型标识 ********************************************************************************/
//监视方向过程信息
#define   CS101_SLAVE_M_SP_NA_1              1            // 单点信息
#define   CS101_SLAVE_M_DP_NA_1              3            // 双点信息
#define   CS101_SLAVE_M_ME_NA_1              9            // 测量值，归一化值
#define   CS101_SLAVE_M_ME_NB_1              11           // 测量值，标度化值
#define   CS101_SLAVE_M_ME_NC_1              13           // 测量值，短浮点数
#define   CS101_SLAVE_M_SP_TB_1              30           // 带时标的单点信息
#define   CS101_SLAVE_M_DP_TB_1              31           // 带时标的双点信息
#define   CS101_SLAVE_M_FT_NA_1              42           // 故障事件信息
#define   CS101_SLAVE_M_IT_NB_1              206          // 累计量，短浮点数
#define   CS101_SLAVE_M_IT_TC_1              207          // 带时标累计量，短浮点数

//控制方向过程信息
#define   CS101_SLAVE_C_SC_NA_1              45            // 单点命令
#define   CS101_SLAVE_C_SC_NB_1              46            // 双点命令

//监视方向的系统命令
#define   CS101_SLAVE_M_EI_NA_1              70            // 初始化结束

//控制方向的系统命令
#define   CS101_SLAVE_C_IC_NA_1              100            // 站总召唤命令
#define   CS101_SLAVE_C_CI_NA_1              101            // 电能量召唤命令
#define   CS101_SLAVE_C_CS_NA_1              103            // 时间同步命令
#define   CS101_SLAVE_C_TS_NA_1              104            // 测试命令
#define   CS101_SLAVE_C_RP_NA_1              105            // 复位进程命令
#define   CS101_SLAVE_C_SR_NA_1              200            // 切换定值区
#define   CS101_SLAVE_C_RR_NA_1              201            // 读定值区号
#define   CS101_SLAVE_C_RS_NA_1              202            // 读参数和定值
#define   CS101_SLAVE_C_WS_NA_1              203            // 写参数和定值
#define   CS101_SLAVE_F_FR_NA_1              210            // 文件传输
#define   CS101_SLAVE_F_SR_NA_1              211            // 软件升级

/* COT传送原因 ********************************************************************************/
#define   CS101_SLAVE_COT_YES              0            // 肯定认可
#define   CS101_SLAVE_COT_NO               0x40         // 否定认可
#define   CS101_SLAVE_COT_CYC              1            // 周期循环
#define   CS101_SLAVE_COT_BACK             2            // 背景扫描
#define   CS101_SLAVE_COT_SPONT            3            // 突发
#define   CS101_SLAVE_COT_INIT             4            // 初始化
#define   CS101_SLAVE_COT_REQ              5            // 请求或被请求
#define   CS101_SLAVE_COT_ACT              6            // 激活
#define   CS101_SLAVE_COT_ACTCON           7            // 激活确认
#define   CS101_SLAVE_COT_DEACT            8            // 停止激活
#define   CS101_SLAVE_COT_DEACTCON         9            // 停止激活确认
#define   CS101_SLAVE_COT_ACCTTERM         10           // 激活终止
#define   CS101_SLAVE_COT_FILE             13           // 文件传输
#define   CS101_SLAVE_COT_INTROGEN         20           // 响应站召唤
#define   CS101_SLAVE_COT_REQCOGEN         37           // 响应电能召唤
#define   CS101_SLAVE_COT_TIERR            (44|CS101_SLAVE_COT_NO)           // 类型标志错误
#define   CS101_SLAVE_COT_COTERR           (45|CS101_SLAVE_COT_NO)           // 传送原因错误
#define   CS101_SLAVE_COT_ASUDERR          (46|CS101_SLAVE_COT_NO)           // 应用单元地址错误
#define   CS101_SLAVE_COT_ADDRERR          (47|CS101_SLAVE_COT_NO)           // 信息体地址错误

/* 发送优先级 ********************************************************************************/
//1级数据
#define   CS101_SLAVE_M_EI_NA_P              1            // 初始化结束
#define   CS101_SLAVE_C_IC_NA_PF             2            // 站总召唤命令
#define   CS101_SLAVE_C_SC_NA_P              3            // 单点命令,定值命令
#define   CS101_SLAVE_M_SP_TB_P              4           // 带时标的单点信息
#define   CS101_SLAVE_C_IC_NA_P              5            // 站总召唤命令，定值召唤

//2级数据
#define   CS101_SLAVE_M_FT_NA_P              0x81           // 故障事件信息
#define   CS101_SLAVE_C_CS_NA_P              0x82            // 时间同步命令
#define   CS101_SLAVE_C_TS_NA_P              0x83            // 测试命令
#define   CS101_SLAVE_M_ME_NC_P              0x84           // 测量值，短浮点数
#define   CS101_SLAVE_C_RP_NA_P              0x85            // 复位进程命令
#define   CS101_SLAVE_F_FR_NA_P              0x86            // 文件召唤
#define   CS101_SLAVE_F_FW_NA_P              0x87            // 文件传输
#define   CS101_SLAVE_C_CI_NA_P              0x88            // 电能量召唤命令

#endif /* END _CS101_SLAVE_CONFIG_H_ */
    

/* END OF FILE ---------------------------------------------------------------*/
