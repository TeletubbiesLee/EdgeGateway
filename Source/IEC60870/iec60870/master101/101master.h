#ifndef _101_MASTER_
#define _101_MASTER_

#include <stdio.h>
#include <stdint.h>
//#include "dataSwitch.h"
#include "../port/iec_interface.h"

/* 搜索报文标志 */
enum CS101_MASTER_SEARCH_FLAG
{
    CS101_M_SEARTCH_START,              //查找启动标志
    CS101_M_RECEIVE_MESS,               //接收后续报文
    CS101_M_FRAME_VERIFY,               //帧校验
};

/* 遥控单双点 */
enum CS101_MASTER_YK_SINGLE_DOUBLE
{
    CS101_M_YK_SINGLE_HAND,                        /* 单点 */
    CS101_M_YK_DOUBLE_HAND,                        /* 双点 */
};

/* 分合操作 */
enum CS101_MASTER_YK_OPEN_CLOSE
{
    CS101_M_YK_OPEN_HAND,                          /* 分 */
    CS101_M_YK_CLOSE_HAND,                         /* 合 */
};

/* 固有参数宏定义 */
#define CS101_MASTER_HEARTBEAT_TIME         10000               /* 心跳测试时间ms */
#define CS101_MASTER_CALL_DATA_TIME         500                 /* 召唤二级数据时间ms */
#define CS101_MASTER_RETRY_TIME             1000                /* 重发时间ms */
#define CS101_MASTER_RETRY_NUMBER           3                   /* 重发次数 */
#define CS101_MASTER_AWAIT_TIME             500                 /* 链路等待时间ms */
#define CS101_MASTER_ALL_CALL_TIME          20000               /* 循环总召时间ms 60*/
#define CS101_MASTER_TIMING_TIME            10000               /* 链路重连时间ms */

/* 数据组包标志 */
#define CS101_M_PACK_REQUEST_LINK_FLAG      0x00000001          /* 请求链路 */
#define CS101_M_PACK_RESET_LINK_FLAG        0x00000002          /* 复位链路 */
#define CS101_M_PACK_CLOCK_SYNC_FLAG        0x00000004          /* 时钟同步 */
#define CS101_M_PACK_CLOCK_READ_FLAG        0x00000008          /* 时钟读取 */
#define CS101_M_PACK_RESET_COURSE_FLAG      0x00000010          /* 复位进程 */
#define CS101_M_PACK_ALWAYS_CALLED_FLAG     0x00000020          /* 总召唤 */
#define CS101_M_PACK_YK_HANDLE_FLAG         0x00000040          /* 遥控操作 */
#define CS101_M_PACK_STAIR_DATA_FLAG        0x00000080          /* 访问一级数据 */
#define CS101_M_PACK_SECOND_DATA_FLAG       0x00000100          /* 访问二级数据 */
#define CS101_M_PACK_LINK_ACCEPT_FLAG       0x00000200          /* 发送认可 */
#define CS101_M_PACK_LINK_STATE_FLAG        0x00000400          /* 发送链路状态 */
#define CS101_M_PACK_LINK_TEST_FLAG         0x00000800          /* 心跳测试 */

/* 三遥起始地址 */
#define CS101_MASTER_YX_START_ADDR           0x0001
#define CS101_MASTER_YC_START_ADDR           0x4001
#define CS101_MASTER_YK_START_ADDR           0x6001

/* 每个端口连接最大从站个数 */
#define CS101_MASTER_SLAVE_MAX     16

/* 101模式选择 */
#define CS101_MASTER_BALANCE_MODE            1       /* 平衡模式 */
#define CS101_MASTER_NOBALANCE_MODE          0       /* 非平衡模式 */

#define CS101_MASTER_TIME_ENABLE             1       /* 定时器启动 */
#define CS101_MASTER_TIME_UNABLE             0       /* 定时器停止 */

#define CS101_MASTER_LINK_ADDR_MAX_LEN       2       //链路地址最大长度
#define CS101_MASTER_ASDU_ADDR_MAX_LEN       2       //公共地址最大长度
#define CS101_MASTER_COT_MAX_LEN             2       //COT最大长度
#define CS101_MASTER_INFO_ADDR_LEN           2       //信息体对象地址长度
#define CS101_MASTER_INFO_OBJECT_LEN         256 - 7 - CS101_MASTER_LINK_ADDR_MAX_LEN - CS101_MASTER_ASDU_ADDR_MAX_LEN - CS101_MASTER_COT_MAX_LEN   //信息体对象长度

#define CS101_MASTER_RX_QUEUE_LEN            512     //接收队列长度
#define CS101_MASTER_RX_FRAME_MAX_LEN        512     //接收帧最大长度
#define CS101_MASTER_TX_FRAME_MAX_LEN        512     //发送帧最大长度

#define CS101_MASTER_FIXED_START             0x10    //固定帧启动字符
#define CS101_MASTER_VARIABLE_START          0x68    //可变帧启动字符
#define CS101_MASTER_FRAME_FINISH            0x16    //帧结束字符

/* 链路标志  */
#define CS101_MASTER_LINK_FLAG_BITS          0x00000fff      //链路标志位
#define CS101_MASTER_LINK_REQUEST_FLAG       0x00000001      //请求链路
#define CS101_MASTER_LINK_RESET_FLAG         0x00000002      //复位链路
#define CS101_MASTER_LINK_ESTABLISH_FLAG     0x00000004      //链路建立
#define CS101_MASTER_LINK_INIT_END_FLAG      0x00000008      //初始化结束
#define CS101_MASTER_LINK_WAIT_ACK_FLAG      0x00000010      //等待链路应答
#define CS101_MASTER_LINK_WAIT_JUMP_FLAG     0x00000020      //链路等待跳转
#define CS101_MASTER_LINK_CLOCK_SYNC_FLAG    0x00000040      //初始化时钟同步

/* 遥控标志 */  
#define CS101_MASTER_YK_FLAG_BITS            0x00fff000      //遥控标志位
#define CS101_MASTER_YK_TWO_POINT_FLAG       0x00001000      //遥控双点标志
#define CS101_MASTER_YK_SELECT_FLAG          0x00002000      //遥控选择标志
#define CS101_MASTER_YK_EXECUTE_FLAG         0x00004000      //遥控执行标志
#define CS101_MASTER_YK_FINAL_FLAG           0x00008000      //遥控终止标志
#define CS101_MASTER_YK_OPEN_FLAG            0x00010000      //遥控分
#define CS101_MASTER_YK_CLOSE_FLAG           0x00020000      //遥控合
#define CS101_MASTER_YK_ACT_AFFIRM_FLAG      0x00040000      //激活确认
#define CS101_MASTER_YK_FINAL_AFFIRM_FLAG    0x00080000      //终止确认
#define CS101_MASTER_YK_EXE_END_FLAG         0x00100000      //执行结束
#define CS101_MASTER_YK_ERROR_FLAG           0x00200000      //遥控错误
#define CS101_MASTER_YK_ING_FLAG             0x00400000      //该回路正在遥控

/* 时钟同步和读取 */
#define CS101_MASTER_CLOCK_SYNCH_CLOCK_FLAG  0x01000000      //时钟同步和读取

/* 遥控选择，执行 */
#define CS101_MASTER_YK_EXECUTE              0x00    //遥控执行
#define CS101_MASTER_YK_SELECT               0x80    //遥控选择

/* 控制域定义 */
#define  CS101_MASTER_CONTROL_FCB_FIRST      0x01    // FCB第一次接收
//前4位
#define  CS101_MASTER_CONTROL_FCV            0x10    // 帧计数有效位 
#define  CS101_MASTER_CONTROL_DFC            0x10    // 数据流控制
#define  CS101_MASTER_CONTROL_FCB            0x20    // 帧计数位
#define  CS101_MASTER_CONTROL_ACD            0x20    // 要求访问位 
#define  CS101_MASTER_CONTROL_PRM            0x40    // 启动报文位
#define  CS101_MASTER_CONTROL_DIR            0x80    // 传输方向位
//主站向子站传输功能码定义
#define   CS101_MASTER_CONTROL_M_FC_0        0       // 复位远方链路
#define   CS101_MASTER_CONTROL_M_FC_2        2       // 链路测试
#define   CS101_MASTER_CONTROL_M_FC_3        3	    // 发送确认命令
#define   CS101_MASTER_CONTROL_M_FC_4        4	    // 发送不确认命令
#define   CS101_MASTER_CONTROL_M_FC_8        8	    // 访问请求
#define   CS101_MASTER_CONTROL_M_FC_9        9	    // 请求链路
#define   CS101_MASTER_CONTROL_M_FC_10       10	    // 请求响应，请求一级用户数据
#define   CS101_MASTER_CONTROL_M_FC_11       11	    // 请求响应，请求二级用户数据

//子站向主站传输功能码定义 
#define   CS101_MASTER_CONTROL_S_FC_0        0       // 认可
#define   CS101_MASTER_CONTROL_S_FC_1        1       // 否定认可
#define   CS101_MASTER_CONTROL_S_FC_8        8       // 响应用户数据
#define   CS101_MASTER_CONTROL_S_FC_9        9       // 无所请求的用户数据
#define   CS101_MASTER_CONTROL_S_FC_11       11      // 链路状态

/* 类型标识TI */
#define CS101_MASTER_TI_M_SP_NA_1            1       //单点信息
#define CS101_MASTER_TI_M_DP_NA_1            3       //双点信息
#define CS101_MASTER_TI_M_ME_NA_1            9       //测量值，归一化值
#define CS101_MASTER_TI_M_ME_NB_1            11      //测量值，标度化值
#define CS101_MASTER_TI_M_ME_NC_1            13      //测量值，短浮点数
#define CS101_MASTER_TI_M_SP_TB_1            30      //带时标的单点信息
#define CS101_MASTER_TI_M_DP_TB_1            31      //带时标的双点信息
#define CS101_MASTER_TI_M_FT_NA_1            42      //故障事件信息
#define CS101_MASTER_TI_M_IT_NB_1            206     //累计量，短浮点数
#define CS101_MASTER_TI_M_IT_TC_1            207     //带时标的累计量
#define CS101_MASTER_TI_C_SC_NA_1            45      //单点命令
#define CS101_MASTER_TI_C_SC_NB_1            46      //双点命令
#define CS101_MASTER_TI_M_EI_NA_1            70      //初始化结束
#define CS101_MASTER_TI_C_IC_NA_1            100     //站总召唤命令
#define CS101_MASTER_TI_C_CI_NA_1            101     //电能量召唤命令
#define CS101_MASTER_TI_C_CS_NA_1            103     //时钟同步命令
#define CS101_MASTER_TI_C_TS_NA_1            104     //测试命令
#define CS101_MASTER_TI_C_RP_NA_1            105     //复位进程命令
#define CS101_MASTER_TI_C_SR_NA_1            200     //切换定值区号
#define CS101_MASTER_TI_C_RR_NA_1            201     //读定值区号
#define CS101_MASTER_TI_C_RS_NA_1            202     //读参数和定值
#define CS101_MASTER_TI_C_WS_NA_1            203     //写参数和定值
#define CS101_MASTER_TI_F_FR_NA_1            210     //文件传输
#define CS101_MASTER_TI_F_SR_NA_1            211     //软件升级

/* 传送原因COT */
#define CS101_MASTER_COT_PER_CYC             1       //周期、循环
#define CS101_MASTER_COT_BACK                2       //背景扫描
#define CS101_MASTER_COT_SPONT               3       //突发（自发）
#define CS101_MASTER_COT_INIT                4       //初始化
#define CS101_MASTER_COT_REQ                 5       //请求或被请求
#define CS101_MASTER_COT_ACT                 6       //激活
#define CS101_MASTER_COT_ACTCON              7       //激活确认
#define CS101_MASTER_COT_DEACT               8       //停止激活
#define CS101_MASTER_COT_DEACTCON            9       //停止激活确认
#define CS101_MASTER_COT_ACTTERM             10      //激活终止
#define CS101_MASTER_COT_FILE                13      //文件传输
#define CS101_MASTER_COT_INTROGEN            20      //响应站召唤
#define CS101_MASTER_COT_REQCOGEN            37      //响应电能量总召唤

/* 接收队列 */
typedef struct 
{
    uint8_t buff[CS101_MASTER_RX_QUEUE_LEN];
    uint32_t in;
    uint32_t out;
    uint32_t seek_out;
}CS101_MASTER_RX_QUEUE_t;

/* 接收帧缓冲区 */
typedef struct 
{
    uint8_t buff[CS101_MASTER_RX_FRAME_MAX_LEN];
    uint32_t len;
}CS101_MASTER_RX_FRAME_t;

/* 发送帧缓冲区 */
typedef struct 
{
    uint8_t buff[CS101_MASTER_TX_FRAME_MAX_LEN];
    uint32_t len;
}CS101_MASTER_TX_FRAME_t;

/* 链路地址 */
typedef struct 
{
    uint8_t addr[CS101_MASTER_LINK_ADDR_MAX_LEN];
    uint8_t len;
}CS101_MASTER_LINK_ADDR_t;

/* 公共地址 */
typedef struct 
{
    uint8_t addr[CS101_MASTER_ASDU_ADDR_MAX_LEN];
    uint8_t len;
}CS101_MASTER_ASDU_ADDR_t;

/* 传送原因 */
typedef struct 
{
    uint8_t cot[CS101_MASTER_COT_MAX_LEN];
    uint8_t len;
}CS101_MASTER_COT_t;

/* 时标结构 */
typedef struct 
{
    uint8_t milliseconds_L;     // 毫秒低八位D7-D0
    uint8_t milliseconds_H;     // 毫秒高八位D7-D0
    uint8_t minutes;		    // 分钟D5-D0
    uint8_t hours;				// 小时D4-D0
    uint8_t day_of_week;		// 日和星期WEEK(D7-D5)MONTH(D4-D0)
    uint8_t months;				// 月D3-D0
    uint8_t years;				// 年D6-D0
}CS101_MASTER_CP56Time2a_t;

/* 年月日时分秒时间结构 */
typedef struct 
{
    uint16_t msec;              // 毫秒
    uint8_t sec;		        // 秒
    uint8_t min;                // 分
    uint8_t hours;				// 小时
    uint8_t day;		        // 日
    uint8_t week;               // 星期
    uint8_t months;				// 月
    uint8_t years;				// 年
}CS101_MASTER_PAR_TIME_t;

/* 101主站时间结构 */
typedef struct
{
    CS101_MASTER_CP56Time2a_t CP56Time2a;                                /* CP56Time2a 时间 */
    CS101_MASTER_PAR_TIME_t time_par;                                        /* 标准时间 */
    uint64_t time_ms;
}CS101_MASTER_TIME_t;

/* 遥信结构 */
typedef struct 
{
    uint16_t addr;
    uint8_t value;
}CS101_MASTER_YX_DATA_t;

/* SOE结构 */
typedef struct 
{
    uint16_t addr;
    uint8_t value;
    CS101_MASTER_CP56Time2a_t CP56Time2a;
}CS101_MASTER_SOE_DATA_t;

/* 遥测结构 */
typedef struct 
{
    uint16_t addr;
    float value;
}CS101_MASTER_YC_DATA_t;

/* ASDU结构 */
typedef struct 
{
    uint8_t TI;             //类型标识  
    uint8_t VSQ;            //可变结构限定词
    CS101_MASTER_COT_t COT;              //传送原因
    CS101_MASTER_ASDU_ADDR_t ASDU_addr;  //应用服务数据单元公共地址

    union
    {
        uint8_t info_buff[CS101_MASTER_INFO_OBJECT_LEN];
        struct
        {
            uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
            uint8_t QOI;                        //召唤限定词
        }C_100;                                 //召唤命令         

        struct
        {
            uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
            CS101_MASTER_CP56Time2a_t CP56Time2a;
        }C_103;                                 //时钟同步命令

        struct
        {
            uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
            uint8_t QRP;                        //复位进程命令限定词
        }C_105;                                 //复位进程命令

        struct
        {
            uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
            uint8_t COI;                        //初始化原因
        }C_70;                                  //初始化结束

        struct
        {
            uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
            uint8_t FBP_L;
            uint8_t FBP_H;			            //固定测试图像	
        }C_104;                                 //测试命令

        union
        {
            struct
            {
                struct
                {
                    uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
                    uint8_t value; 
                }array[CS101_MASTER_INFO_OBJECT_LEN / (CS101_MASTER_INFO_ADDR_LEN + 1)];
            }SQ0;

            struct
            {
                uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
                struct
                {
                    uint8_t value; 
                }array[(CS101_MASTER_INFO_OBJECT_LEN - CS101_MASTER_INFO_ADDR_LEN) / 1];
            }SQ1;
        }C_1;                                   //单点信息

        union
        {
            struct
            {
                struct
                {
                    uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
                    uint8_t value; 
                }array[CS101_MASTER_INFO_OBJECT_LEN / (CS101_MASTER_INFO_ADDR_LEN + 1)];
            }SQ0;

            struct
            {
                uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
                struct
                {
                    uint8_t value; 
                }array[(CS101_MASTER_INFO_OBJECT_LEN - CS101_MASTER_INFO_ADDR_LEN) / 1];
            }SQ1;
        }C_3;                                   //双点信息

        union
        {
            struct
            {
                struct
                {
                    uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
                    uint8_t value; 
                    CS101_MASTER_CP56Time2a_t CP56Time2a;
                }array[CS101_MASTER_INFO_OBJECT_LEN / (CS101_MASTER_INFO_ADDR_LEN + 8)];
            }SQ0;

            struct
            {
                uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址	
                struct
                {
                    uint8_t value; 
                    CS101_MASTER_CP56Time2a_t CP56Time2a;
                }array[(CS101_MASTER_INFO_OBJECT_LEN - CS101_MASTER_INFO_ADDR_LEN) / 8];
            }SQ1;
        }C_30;                                  //带时标的单点信息

        union
        {
            struct
            {
                struct
                {
                    uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
                    uint8_t value; 
                    CS101_MASTER_CP56Time2a_t CP56Time2a;
                }array[CS101_MASTER_INFO_OBJECT_LEN / (CS101_MASTER_INFO_ADDR_LEN + 8)];
            }SQ0;

            struct
            {
                uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址	
                struct
                {
                    uint8_t value; 
                    CS101_MASTER_CP56Time2a_t CP56Time2a;
                }array[(CS101_MASTER_INFO_OBJECT_LEN - CS101_MASTER_INFO_ADDR_LEN) / 8];
            }SQ1;
        }C_31;                              //带时标的双点信息

        union
        {
            struct
            {
                struct
                {
                    uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
                    uint8_t value[2];
                    uint8_t QDS;
                }array[CS101_MASTER_INFO_OBJECT_LEN / (CS101_MASTER_INFO_ADDR_LEN + 3)];
            }SQ0;

            struct
            {
                uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址	
                struct
                {
                    uint8_t value[2];									
                    uint8_t QDS;
                }array[(CS101_MASTER_INFO_OBJECT_LEN - CS101_MASTER_INFO_ADDR_LEN) / 3];
            }SQ1;
        }C_9;                               //归一化值

        union
        {
            struct
            {
                struct
                {
                    uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
                    uint8_t value[2];
                    uint8_t QDS;
                }array[CS101_MASTER_INFO_OBJECT_LEN / (CS101_MASTER_INFO_ADDR_LEN + 3)];
            }SQ0;

            struct
            {
                uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
                struct
                {
                    uint8_t value[2];
                    uint8_t QDS;
                }array[(CS101_MASTER_INFO_OBJECT_LEN - CS101_MASTER_INFO_ADDR_LEN) / 3];
            }SQ1;
        }C_11;                              //标度化值

        union
        {
            struct
            {
                struct
                {
                    uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
                    uint8_t value[4];	
                    uint8_t QDS;
                }array[CS101_MASTER_INFO_OBJECT_LEN / (CS101_MASTER_INFO_ADDR_LEN + 5)];
            }SQ0;

            struct
            {
                uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
                struct
                {
                    uint8_t value[4];	
                    uint8_t QDS;
                }array[(CS101_MASTER_INFO_OBJECT_LEN - CS101_MASTER_INFO_ADDR_LEN) / 5];
            }SQ1;
        }C_13;                          //短浮点数

        struct
        {
            uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
            uint8_t SCO;
        }C_45;	                        //遥控单点命令

        struct
        {
            uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
            uint8_t DCO;
        }C_46;	                        //遥控双点命令

        struct
        {
            uint8_t array[CS101_MASTER_INFO_OBJECT_LEN]; 
        }C_42;                          //故障值信息

        struct
        {
            uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
            uint8_t SN_L;
            uint8_t SN_H;
        }C_200;                         //切换定值区

        union
        {
            struct
            {
                uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
            }control;                   //控制方向

            struct
            {
                uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
                uint8_t SN1_L;
                uint8_t SN1_H;
                uint8_t SN2_L;
                uint8_t SN2_H;
                uint8_t SN3_L;
                uint8_t SN3_H;
            }monitor;                   //监视方向
        }C_201;                         //读定值区号

        union
        {
            struct
            {
                uint8_t SN1_L;
                uint8_t SN1_H;
                uint32_t array[(CS101_MASTER_INFO_OBJECT_LEN - 2) / 4];		
            }Down;

            struct
            {
                uint8_t SN_L;
                uint8_t SN_H;	
                uint8_t PI_L;
                uint8_t PI_H;
                struct
                {
                    uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
                    uint8_t tag;
                    uint8_t len;
                    uint32_t value;	
                }array[(CS101_MASTER_INFO_OBJECT_LEN - 4) / (CS101_MASTER_INFO_ADDR_LEN + 6)];
            }Up;
        }C_202;                         //读参数和定值

        union
        {
            uint8_t SN_L;
            uint8_t SN_H;	
            uint8_t PI_L;
            uint8_t PI_H;
            struct
            {
                uint8_t info_addr[CS101_MASTER_INFO_ADDR_LEN];   //信息对象地址
                uint8_t tag;
                uint8_t len;
                uint32_t value;	
            }array[(CS101_MASTER_INFO_OBJECT_LEN - 4) / (CS101_MASTER_INFO_ADDR_LEN + 6)];
        }C_203;                         //写参数和定值

        struct
        {
            uint8_t array[CS101_MASTER_INFO_OBJECT_LEN / 1];
        }C_210;                         //文件传输
    }info_data;
    uint8_t info_len;                   //信息体长度
}CS101_MASTER_ASDU_t;

/* 定时器结构 */
typedef struct
{
    uint8_t enable_flag;            /* 定时器使能标志 */
    uint32_t timing_time;           /* 定时时间 */
    uint64_t start_time;            /* 启动时间 */
}CS101_MASTER_TIMER_t;

/* 遥控数据结构 */
typedef struct
{
    uint8_t yk_single_double;       /* 遥控单双点 */
    uint16_t yk_addr;               /* 遥控地址 */
    uint16_t yk_sta;                /* 遥控状态 */
}CS101_MASTER_YK_DATA_t;

/* 101物理层公共结构 */
typedef struct 
{
    uint8_t port_number; 		    /* 串口号 0—2 */
    int fd;                         /* 串口文件描述符 */
    CS101_MASTER_RX_QUEUE_t rx_queue;        /* 接收队列 */
    uint32_t search_flag;           /* 搜索报文标志 */
    uint32_t seek_count;            /* 搜索报文超时计时 */
    uint32_t seek_time;             /* 搜索报文超时时间 */
}CS101_MASTER_PHYSICS_t;

/* 101链路层结构 */
typedef struct
{
    uint16_t module_id;                     /* 模块ID */
    uint8_t port_number; 		            /* 串口号 0—2 */
    uint8_t mode_selection;                 /* 模式选择 */
    CS101_MASTER_LINK_ADDR_t link_addr;     /* 链路地址 */
    CS101_MASTER_RX_FRAME_t rx_frame;       /* 接收帧缓冲区 */
    CS101_MASTER_TX_FRAME_t tx_frame;       /* 发送帧缓冲区 */
    CS101_MASTER_TX_FRAME_t tx_re_frame;    /* 重发帧缓冲区 */
    CS101_MASTER_ASDU_t ASDU;               /* ASDU */
    uint8_t masterFCB;                      /* 主站FCB位状态 */
    uint8_t subsiteFCB;                     /* 子站FCB位状态 */
    CS101_MASTER_TIME_t time_101;           /* 101主站时间 */
    uint32_t link_flag;                     /* D0~D7链路标志D8~D19遥控标志*/
    CS101_MASTER_YK_DATA_t yk_data;         /* 遥控数据 */
    uint32_t pack_flag;                     /* 数据组包标志 */
    CS101_MASTER_TIMER_t heartbeat_time;    /* 心跳测试定时器*/
    CS101_MASTER_TIMER_t call_data_time;    /* 召唤二级数据定时器 */
    CS101_MASTER_TIMER_t retry_time;        /* 重发定时器 */
    CS101_MASTER_TIMER_t await_time;        /* 等待定时器 */
    CS101_MASTER_TIMER_t all_call_time;     /* 循环总召定时器 */
    uint32_t retry_num_con;                 /* 重法次数计数 */
    uint32_t retry_number;                  /* 重法次数计数 */
}CS101_MASTER_LINK_t;

/* 101主站应用数据结构 */
typedef struct
{
    uint16_t module_id;                         /* 模块ID */
    struct tagSwitchQuantityIn communication_state;  /* 通讯状态 */
    uint16_t YX_len;                            /* 遥信长度 */
    tagSwitchQuantityIn_t p_YX_data;            /* 遥信数据 */
    uint16_t YC_len;                            /* 遥测长度 */
    tagAnalogQuantityIn_t p_YC_data;            /* 遥测数据 */
    uint16_t YK_len;                            /* 遥控长度 */
    tagControlOutputIn_t p_YK_data;             /* 遥控数据 */
}CS101_MASTER_APPDATA_t;

/* 101主站端口配置初始化结构 */
typedef struct
{
    uint8_t port_number; 		    /* 串口号 0—2 */
    int fd;                         /* 串口文件描述符 */
    uint8_t mode;                   /* 模式 */
    uint8_t link_addr_len;          /* 链路地址长度 */
    uint8_t cot_len;                /* 传送原因长度 */
    uint16_t ASDU_addr;             /* ASDU地址 */
    uint8_t ASDU_len;               /* ASDU地址长度 */
}CS101_MASTER_COMCFG_t;

/* 101主站应用结构 */
typedef struct
{
    uint8_t port_number; 		                /* 串口号 0—2 */
    uint16_t link_addr;                         /* 链路地址 */
    uint16_t module_id;                         /* 模块ID */
    uint8_t YK_single_double;                   /* 遥控单双点 */
    struct tagSwitchQuantityIn communication_state;  /* 通讯状态 */
    uint16_t YX_len;                            /* 遥信长度 */
    tagSwitchQuantityIn_t p_YX_data;            /* 遥信数据 */
    uint16_t YC_len;                            /* 遥测长度 */
    tagAnalogQuantityIn_t p_YC_data;            /* 遥测数据 */
    uint16_t YK_len;                            /* 遥控长度 */
    tagControlOutputIn_t p_YK_data;             /* 遥控数据 */
}CS101_MASTER_APPCFG_t;

int cs101_master_comconfig_init(int fd, IecParam sParam);
int cs101_master_appconfig_init(sMasterParam_t sParam);
int cs101_master_start(void);
int cs101_master_YK_receive(tagControlCfg_t p_control);

#endif /* _101_MASTER_ */

