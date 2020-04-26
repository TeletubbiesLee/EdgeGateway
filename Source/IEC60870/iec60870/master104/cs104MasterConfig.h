/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2017-2018. All right reserved.
  * @file:      cs104MasterConfig.h
  * @brief:     104协议类定义.
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2019-03-18
  * @update:    [2019-03-18][Chen][newly increased]
  */
#ifndef	_CS104_MASTER_CONFIG_H_
#define _CS104_MASTER_CONFIG_H_
/* PUBLIC VARIABLE -----------------------------------------------------------*/
enum
{
  CS104_MASTER_DISK0 = 0,
	CS104_MASTER_DISK1,
  CS104_MASTER_DISK2,
  CS104_MASTER_DISK3,
  CS104_MASTER_DISK4,
  CS104_MASTER_DISK5,
  CS104_MASTER_DISK6,
  CS104_MASTER_DISK7,
  CS104_MASTER_DISK8,
  CS104_MASTER_DISK9,
  CS104_MASTER_DISK10,
  CS104_MASTER_DISK11,
  CS104_MASTER_DISK12,
  CS104_MASTER_DISK13,
  CS104_MASTER_DISK14,
  CS104_MASTER_DISK15,
	CS104_MASTER_VOLUMES
};

/* Number of volumes (logical drives) to be used.*/
#define  CS104_MASTER_STOREDATA1NUM	8
#define  CS104_MASTER_STOREDATA2NUM	4

//LINK_FLAG
// EVENT
#define  CS104_MASTER_DLFLAG                     0x00000020
#define  CS104_MASTER_APPFLAG                    0x00000040
#define  CS104_MASTER_APPTIMEFLAG                0x00000080
#define  CS104_MASTER_SCHEDULE                   0x00000002
#define  CS104_MASTER_NEXTFRAME                  0x00000001
#define  CS104_MASTER_SENDOVER                   0x00000004

/* 应用层特定参数 */
#undef    CS104_MASTER_COTByte	                 // 定义传输原因字节数
#undef    CS104_MASTER_PUBADDRBYTE				 // 定义公共地址字节数
#define   CS104_MASTER_BROADCASTADDR	         0xFFFF
#define   CS104_MASTER_INFOADDR3BYTE

//数据状态
#define  CS104_MASTER_USTART                    0x00000001/*U帧启动链路*/
#define	 CS104_MASTER_USTOP	                    0x00000002/*U帧停止链路*/
#define  CS104_MASTER_UTEST	                    0x00000004/*U帧测试链路*/
#define  CS104_MASTER_CLOCKSYN	                0x00000008/*时钟同步命令*/
#define  CS104_MASTER_ALLDATA                   0x00000010/*总召命令*/
#define  CS104_MASTER_ELEDATA                   0x00000020/*电度总召命令*/
#define  CS104_MASTER_RESETPRO                  0x00000040/*复位进程命令*/
#define  CS104_MASTER_TEST                      0x00000080/*测试命令*/
#define  CS104_MASTER_SWICHAREA                 0x00000100/*切换定值区命令*/
#define  CS104_MASTER_RDAREA                    0x00000200/*读定值区号命令*/
#define  CS104_MASTER_RDSETIG                   0x00000400/*读定值参数命令*/
#define  CS104_MASTER_WRSETIG                   0x00000800/*写定值参数命令*/
#define  CS104_MASTER_FILETRANS                 0x00001000/*文件传输*/
#define  CS104_MASTER_SOFTUP                    0x00002000/*软件升级*/
#define  CS104_MASTER_REMOTECTL                 0x00004000/*远方遥控*/
//基本规约配置
/* Define Parameter ----------------------------------------------------------*/
#define  CS104_MASTER_ASDUSIZE                    249
#define  CS104_MASTER_LPDUSIZE	                 (255)	  // 链路层发送报文的最大长度
#define  CS104_MASTER_APDUSIZE	                  253       // 应用层收发缓冲区的最大长度
#define  CS104_MASTER_FRAMEBUFSIZE	              256       // 链路层接收缓冲区的最大长度

/*************************************************************************************************************************************/
//DL/T634.5104_2002规约
/* IEC104超时计时器 -----------------------------------------------------------*/
/* t0: 主站端和子站或终端设备建立一次TCP连接的最大允许时间，主站端和子站或终端设备之
 *     间的TCP链接在实际运行中可能经常进行关闭和重建，这发生在以下4种情况；
 *     (1) 主站端和子站或终端设备之间的I格式报文出现丢失、错序或者发送U格式报文得不
 *         到应答时，双方均可主动关闭TCP链接，然后进行重新建立链接；
 *     (2) 主站系统重新启动后将与各个子站重新建立TCP链接；
 *     (3) 子站或设备终端通电后或由于自恢复而重新启动后，将重新连接；
 *     (4) 子站或设备终端收到主站端的RESET_PROCESS(复位远方终端)信号后，将关闭链接
 *         并重新初始化，然后重新连接，每次建立连接时，子站或终端设备都调用socket的
 *         listen()函数进行侦听，主站端调用socket的connect()函数进行连接，如果在t0
 *         时间内未能成功建立链接，可能网络发生故障，主站端应该向运行人员发出警告信息；
 * t1: 发送方发送一个I格式报文后，必须在t1时间内得到接收方的认可，否则发送方认为TCP
 *     连接出现问题并重新建立连接；
 * t2: 接收方在接收到I格式报文后，若经过t2时间未再收到新的I格式报文，则必须向发送方
 *     发送S格式测试帧对已经接收到的I格式报文进行认可，t2必须小于t1（t2 < t1）；
 * t3: 调度端、子站或终端每接收一帧I帧、S帧或者U帧将重新触发计时器t3，若在t3未接收到
 *     任何报文，将向对方发送测试链路帧，t3要大于t1（t3 > t1）。
 *
 * 注：所有超时值的最大范围：1到255s。
 * 默认情况下:
 *            t0 = 30s
 *            t1 = 15s
 *            t2 = 5s
 *            t3 = 20s
 * ---------------------------------------------------------------------------*/
#define  CS104_MASTER_RUNCYCLE                          DLT_104MASTER_MS
#define  CS104_MASTER_T0                                (30000/CS104_MASTER_RUNCYCLE)
#define  CS104_MASTER_T1                                (15000/CS104_MASTER_RUNCYCLE) // No Ack CloseTCP
#define  CS104_MASTER_T2                                (5000/CS104_MASTER_RUNCYCLE)  // Send S
#define  CS104_MASTER_T3                                (20000/CS104_MASTER_RUNCYCLE) // Send Test U

/* 基本内容定义 --------------------------------------------------------------*/
// PTick
#define  CS104_MASTER_T0FLAG		                 0x0001
#define  CS104_MASTER_T1FLAG		                 0x0002
#define  CS104_MASTER_T2FLAG		                 0x0004
#define  CS104_MASTER_T3FLAG		                 0x0008

/* -----------------------------------------------------------------------------
 * IEC60870-5-104规定两个参数K和W：
 * K: 取值范围为1到32767，表示发送方在K个I格式报文未得到对方的确认时，将停止数据传送。
 * W: 取值范围为1到32767，表示接收方最迟在接收了W个I格式报文后应该发出认可。
 * IEC60870-5-104规定K和W的默认值分别为12个APDU和8个APDU。
 * ---------------------------------------------------------------------------*/
#define  CS104_MASTER_K                                 12
#define  CS104_MASTER_W                                 4
/* 链路层帧修饰词***************************************************************/
#define  CS104_MASTER_STARTCODE68	                    0x68	// 起始码

/* 链路层 --------------------------------------------------------------------*/
#define  CS104_MASTER_S_FRAME                    0x01
#define  CS104_MASTER_U_FRAME                    0x03
#define  CS104_MASTER_U_STARTDTACT               0x07
#define  CS104_MASTER_U_STARTDTCON               0x0B
#define  CS104_MASTER_U_STOPDTACT                0x13
#define  CS104_MASTER_U_STOPDTCON                0x23
#define  CS104_MASTER_U_TESTFRACT                0x43
#define  CS104_MASTER_U_TESTFRCON                0x83

/* 链路层到应用层的命令(WORD)Link_Command内容 */
#define  CS104_MASTER_LINK_CALLDATA		                 1	  // I数据针
#define  CS104_MASTER_LINK_APPCON		                   2	  // S确认针
#define  CS104_MASTER_LINK_WORK		                     3    // 链路连接
#define  CS104_MASTER_LINK_NOWORK                      4    // 链路未连接

//链路层主站向子站传输功能码定义
#define  CS104_MASTER_M_FUN0             0           // 复位远方链路
#define  CS104_MASTER_M_FUN2             2           // 测试链路功能
#define  CS104_MASTER_M_FUN3             3	          // 发送确认命令
#define  CS104_MASTER_M_FUN4             4	          // 发送不确认命令
#define  CS104_MASTER_M_FUN8             8	          // 请求响应确定访问状态
#define  CS104_MASTER_M_FUN9             9	          // 召唤链路状态
#define  CS104_MASTER_M_FUN10            10	        // 召唤1级用户数据
#define  CS104_MASTER_M_FUN11            11          // 召唤2级用户数据

#define  CS104_MASTER_LINK_RESETRDL              0         // 复位远方链路
#define  CS104_MASTER_LINK_TESTDL                2         // 测试链路功能
#define  CS104_MASTER_LINK_SENDCON               3         // 发送确认命令
#define  CS104_MASTER_LINK_SENDNOCON             4         // 发送不确认命令
#define  CS104_MASTER_LINK_REQACD                8         // 请求响应确定访问状态
#define  CS104_MASTER_LINK_REQSTATUS             9         // 召唤链路状态
#define  CS104_MASTER_LINK_REQDATA1              10        // 召唤1级用户数据
#define  CS104_MASTER_LINK_REQDATA2              11        // 召唤2级用户数据

//*链路层子站向主站传输功能码定义
#define  CS104_MASTER_S_FUN0              0            // 确认
#define  CS104_MASTER_S_FUN1              1            // 确认链路忙未接收报文
#define  CS104_MASTER_S_FUN8              8            // 以数据响应请求帧
#define  CS104_MASTER_S_FUN9              9            // 无所召唤的数据
#define  CS104_MASTER_S_FUN11             11           // 响应链路状态或回答请求帧


/* TI类型标识 ********************************************************************************/
//监视方向过程信息
#define  CS104_MASTER_M_SP_NA_1              1            // 单点信息
#define  CS104_MASTER_M_DP_NA_1              3            // 双点信息
#define  CS104_MASTER_M_ME_NA_1              9            // 测量值，归一化值
#define  CS104_MASTER_M_ME_NB_1              11           // 测量值，标度化值
#define  CS104_MASTER_M_ME_NC_1              13           // 测量值，短浮点数
#define  CS104_MASTER_M_SP_TB_1              30           // 带时标的单点信息
#define  CS104_MASTER_M_DP_TB_1              31           // 带时标的双点信息
#define  CS104_MASTER_M_FT_NA_1              42           // 故障事件信息
#define  CS104_MASTER_M_IT_NB_1              206          // 累计量，短浮点数
#define  CS104_MASTER_M_IT_TC_1              207          // 带时标累计量，短浮点数

//控制方向过程信息
#define  CS104_MASTER_C_SC_NA_1              45            // 单点命令
#define  CS104_MASTER_C_SC_NB_1              46            // 双点命令

//监视方向的系统命令
#define  CS104_MASTER_M_EI_NA_1              70            // 初始化结束

//控制方向的系统命令
#define  CS104_MASTER_C_IC_NA_1              100            // 站总召唤命令
#define  CS104_MASTER_C_CI_NA_1              101            // 电能量召唤命令
#define  CS104_MASTER_C_CS_NA_1              103            // 时间同步命令
#define  CS104_MASTER_C_TS_NA_1              104            // 测试命令
#define  CS104_MASTER_C_RP_NA_1              105            // 复位进程命令
#define  CS104_MASTER_C_SR_NA_1              200            // 切换定值区
#define  CS104_MASTER_C_RR_NA_1              201            // 读定值区号
#define  CS104_MASTER_C_RS_NA_1              202            // 读参数和定值
#define  CS104_MASTER_C_WS_NA_1              203            // 写参数和定值
#define  CS104_MASTER_F_FR_NA_1              210            // 文件传输
#define  CS104_MASTER_F_SR_NA_1              211            // 软件升级

/* COT传送原因 ********************************************************************************/
#define  CS104_MASTER_COT_YES              0            // 肯定认可
#define  CS104_MASTER_COT_NO               0x40         // 否定认可
#define  CS104_MASTER_COT_CYC              1            // 周期循环
#define  CS104_MASTER_COT_BACK             2            // 背景扫描
#define  CS104_MASTER_COT_SPONT            3            // 突发
#define  CS104_MASTER_COT_INIT             4            // 初始化
#define  CS104_MASTER_COT_REQ              5            // 请求或被请求
#define  CS104_MASTER_COT_ACT              6            // 激活
#define  CS104_MASTER_COT_ACTCON           7            // 激活确认
#define  CS104_MASTER_COT_DEACT            8            // 停止激活
#define  CS104_MASTER_COT_DEACTCON         9            // 停止激活确认
#define  CS104_MASTER_COT_ACCTTERM         10           // 激活终止
#define  CS104_MASTER_COT_FILE             13           // 文件传输
#define  CS104_MASTER_COT_INTROGEN         20           // 响应站召唤
#define  CS104_MASTER_COT_REQCOGEN         37           // 响应电能召唤
#define  CS104_MASTER_COT_TIERR            (44|CS104_MASTER_COT_NO)          // 类型标志错误
#define  CS104_MASTER_COT_COTERR           (45|CS104_MASTER_COT_NO)           // 传送原因错误
#define  CS104_MASTER_COT_ASUDERR          (46|CS104_MASTER_COT_NO)           // 应用单元地址错误
#define  CS104_MASTER_COT_ADDRERR          (47|CS104_MASTER_COT_NO)           // 信息体地址错误

/* 发送优先级 ********************************************************************************/
//1级数据
#define  CS104_MASTER_M_EI_NA_P              1            // 初始化结束
#define  CS104_MASTER_C_IC_NA_PF             2            // 站总召唤命令
#define  CS104_MASTER_C_SC_NA_P              3            // 单点命令,定值命令
#define  CS104_MASTER_M_SP_TB_P              4           // 带时标的单点信息
#define  CS104_MASTER_C_IC_NA_P              5            // 站总召唤命令，定值召唤

//2级数据
#define  CS104_MASTER_M_FT_NA_P              0x81           // 故障事件信息
#define  CS104_MASTER_C_CS_NA_P              0x82            // 时间同步命令
#define  CS104_MASTER_C_TS_NA_P              0x83            // 测试命令
#define  CS104_MASTER_M_ME_NC_P              0x84           // 测量值，短浮点数
#define  CS104_MASTER_C_RP_NA_P              0x85            // 复位进程命令
#define  CS104_MASTER_F_FR_NA_P              0x86            // 文件召唤
#define  CS104_MASTER_F_FW_NA_P              0x87            // 文件传输
#define  CS104_MASTER_C_CI_NA_P              0x88            // 电能量召唤命令

/* 自定义 ********************************************************************************/
#define  CS104_MASTER_I_SWITCHAREA           1           // 切换定值区命令
#define  CS104_MASTER_I_READAREA             2           // 读定值区号命令
#define  CS104_MASTER_I_READSETTING          3           // 读定值参数命令
#define  CS104_MASTER_I_WRITESETTING         4           // 写定值参数命令
#endif /* END _CS104_MASTER_CONFIG_H_ */
    

/* END OF FILE ---------------------------------------------------------------*/
