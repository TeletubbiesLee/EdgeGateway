/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      cs104SlaveApp.h
  * @brief:		104app交互处理
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2018-11-09
  * @update:    [2018-11-09][Chen][newly increased]
  */
#ifndef	_CS104_SLAVE_APP_H_
#define _CS104_SLAVE_APP_H_


/* INCLUDE FILES -------------------------------------------------------------*/
#include "cs104SlaveConfig.h"
#include "stdint.h"

#ifndef  FALSE
#define  FALSE     0
#define  TRUE      !FALSE
#endif

/* PUBLIC VARIABLE -----------------------------------------------------------*/

/* DEFINE --------------------------------------------------------------------*/

/* 链路层接收状态 */

/* Struct  -------------------------------------------------------------------*/
#pragma pack(push,1)
enum TATUS
{
		START = 0,	                               // 寻找起始标志状态
		HEAD,	                                   // 寻找报文头
		CONTINUE                                  // 接收后续报文
}Status;// 接收状态
typedef struct 
{
    uint8_t Milliseconds_L;   // D7-D0
    uint8_t Milliseconds_H;   // D7-D0
    uint8_t Minutes;					// D5-D0
    uint8_t Hours;						// D4-D0
    uint8_t DayofWeekMonth;		// WEEK(D7-D5)MONTH(D4-D0)
    uint8_t Months;						// D3-D0
    uint8_t Years;						// D6-D0
}_CS104_SLAVE_CP56Time2a_t;

typedef struct 
{
    struct STATUS
    {
        uint8_t Length;
        union 
        {
            uint8_t priority;	
            uint8_t Lock_ID;
        }symbol;
    }status;

    struct HEAD
    {
        uint8_t TypeID;     
        uint8_t VSQ;  
        uint8_t COT_L;
        uint8_t COT_H;
        uint8_t PubAddr_L;
        uint8_t PubAddr_H;
    }Head;

    union 
    {
        uint8_t buff[256-sizeof(struct STATUS)-sizeof(struct HEAD)];
        
        struct 
        {
            uint8_t InfoAddr_L;
            uint8_t InfoAddr_M;
            uint8_t InfoAddr_H;
            uint8_t QOI; 
        }C_100;

        struct 
        {
            uint8_t InfoAddr_L;
            uint8_t InfoAddr_M;
            uint8_t InfoAddr_H;	
            _CS104_SLAVE_CP56Time2a_t CP56Time2a;
        }C_103;

        struct 
        {
            uint8_t InfoAddr_L;
            uint8_t InfoAddr_M;
            uint8_t InfoAddr_H;	
            uint8_t QRP; 
        }C_105;

        struct 
        {
            uint8_t InfoAddr_L;
            uint8_t InfoAddr_M;
            uint8_t InfoAddr_H;	
            uint8_t COI; 
        }C_70;

        struct 
        {
            uint8_t InfoAddr_L;
            uint8_t InfoAddr_M;
            uint8_t InfoAddr_H;
            uint8_t FBP_L;
            uint8_t FBP_H;				
        }C_104;

        union 
        {
            struct 
            {
                struct 
                {
                    uint8_t InfoAddr_L;
                    uint8_t InfoAddr_M;
                    uint8_t InfoAddr_H;	
                    uint8_t Value; 
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD))/4];
            }SQ0;
            struct 
            {
                uint8_t InfoAddr_L;
                uint8_t InfoAddr_M;
                uint8_t InfoAddr_H;
                struct 
                {
                    uint8_t Value; 
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-3)/1];
            }SQ1;
        }C_1;

        union 
        {
            struct 
            {
                struct 
                {
                    uint8_t InfoAddr_L;
                    uint8_t InfoAddr_M;
                    uint8_t InfoAddr_H;	
                    uint8_t Value; 
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD))/4];
            }SQ0;
            struct 
            {
                uint8_t InfoAddr_L;
                uint8_t InfoAddr_M; 
                uint8_t InfoAddr_H;	
                struct 
                {
                    uint8_t Value; 
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-3)/1];
            }SQ1;
        }C_3;

        union 
        {
            struct 
            {
                struct 
                {
                    uint8_t InfoAddr_L;
                    uint8_t InfoAddr_M;
                    uint8_t InfoAddr_H;
                    uint8_t Value; 
                    _CS104_SLAVE_CP56Time2a_t CP56Time2a;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD))/11];
            }SQ0;
            struct 
            {
                uint8_t InfoAddr_L;
                uint8_t InfoAddr_M;
                uint8_t InfoAddr_H;	
                struct 
                {
                    uint8_t Value; 
                    _CS104_SLAVE_CP56Time2a_t CP56Time2a;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-3)/8];
            }SQ1;
        }C_30;

        union 
        {
            struct 
            {
                struct 
                {
                    uint8_t InfoAddr_L;
                    uint8_t InfoAddr_M;
                    uint8_t InfoAddr_H;	
                    uint8_t Value; 
                    _CS104_SLAVE_CP56Time2a_t CP56Time2a;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD))/11];
            }SQ0;
            struct 
            {
                uint8_t InfoAddr_L;
                uint8_t InfoAddr_M;
                uint8_t InfoAddr_H;	
                struct 
                {
                    uint8_t Value; 
                    _CS104_SLAVE_CP56Time2a_t CP56Time2a;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-3)/8];
            }SQ1;
        }C_31;

        union 
        {
            struct 
            {
                struct 
                {
                    uint8_t InfoAddr_L;
                    uint8_t InfoAddr_M;
                    uint8_t InfoAddr_H;
                    uint16_t Value;
                    uint8_t QDS;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD))/8];
            }SQ0;
            struct 
            {
                uint8_t InfoAddr_L;
                uint8_t InfoAddr_M;
                uint8_t InfoAddr_H;	
                struct 
                {
                    uint16_t Value;									
                    uint8_t QDS;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-3)/5];
            }SQ1;
        }C_9;

        union 
        {
            struct 
            {
                struct 
                {
                    uint8_t InfoAddr_L;
                    uint8_t InfoAddr_M;
                    uint8_t InfoAddr_H;
                    uint16_t Value;
                    uint8_t QDS;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD))/8];
            }SQ0;
            struct 
            {
                uint8_t InfoAddr_L;
                uint8_t InfoAddr_M;
                uint8_t InfoAddr_H;
                struct 
                {
                    uint16_t Value;
                    uint8_t QDS;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-3)/5];
            }SQ1;
        }C_11;

        union 
        {
            struct 
            {
                struct 
                {
                    uint8_t InfoAddr_L;
                    uint8_t InfoAddr_M;
                    uint8_t InfoAddr_H;
                    uint32_t Value;	
                    uint8_t QDS;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD))/8];
            }SQ0;

            struct 
            {
                uint8_t InfoAddr_L;
                uint8_t InfoAddr_M;
                uint8_t InfoAddr_H;
                struct 
                {
                    uint32_t Value;	
                    uint8_t QDS;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-3)/5];
            }SQ1;
        }C_13;

        struct 
        {
            uint8_t InfoAddr_L;
            uint8_t InfoAddr_M;
            uint8_t InfoAddr_H;
            uint8_t SCO_L;
            uint8_t SCO_H;
        }C_45;	

        struct 
        {
            uint8_t InfoAddr_L;
            uint8_t InfoAddr_M;
            uint8_t InfoAddr_H;	
            uint8_t DCO_L;
            uint8_t DCO_H;
        }C_46;	

        struct 
        {
            uint8_t Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD))]; 
        }C_42;

        struct 
        {
            uint8_t InfoAddr_L;
            uint8_t InfoAddr_M;
            uint8_t InfoAddr_H;	
            uint8_t SN_L;
            uint8_t SN_H;
        }C_200;

        union 
        {
            struct 
            {
                uint8_t InfoAddr_L;
                uint8_t InfoAddr_M;
                uint8_t InfoAddr_H;
            }Down;

            struct 
            {
                uint8_t InfoAddr_L;
                uint8_t InfoAddr_M;
                uint8_t InfoAddr_H;	
                uint8_t SN1_L;
                uint8_t SN1_H;
                uint8_t SN2_L;
                uint8_t SN2_H;
                uint8_t SN3_L;
                uint8_t SN3_H;
            }Up;
        }C_201;

        union 
        {
            struct 
            {
                uint8_t SN1_L;
                uint8_t SN1_H;
                uint16_t Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-2)/4];		
            }Down;

            struct 
            {
                uint8_t SN_L;
                uint8_t SN_H;	
                uint8_t PI_L;
                uint8_t PI_H;
                struct 
                {
                    uint8_t InfoAddr_L;
                    uint8_t InfoAddr_M;
                    uint8_t InfoAddr_H;
                    uint8_t Tag;
                    uint8_t Len;
                    uint32_t Value;	
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-4)/9];
            }Up;
        }C_202;

        union 
        {
            uint8_t SN_L;
            uint8_t SN_H;	
            uint8_t PI_L;
            uint8_t PI_H;
            struct 
            {
                uint8_t InfoAddr_L;
                uint8_t InfoAddr_M;
                uint8_t InfoAddr_H;
                uint8_t Tag;
                uint8_t Len;
                uint32_t Value;	
            }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-4)/9];
        }C_203;

        struct 
        {
            uint8_t Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD))]; 
        }C_210;
    }Data;
}CS104_SLAVE_PASDU;

 struct CS104_SLAVE_PASDU_104
{
     struct PASDUHead_104
    {   
        uint8_t	TypeID; // 类型标识
        uint8_t	VSQ;    // 可变结构限定词
     
        // 传送原因
        uint16_t COT;

        // 应用服务数据单元公共地址
        uint16_t PubAddr;

        // 信息对象地址
        uint16_t InfoAddr;
    }Head;
    uint8_t Data[CS104_SLAVE_ASDUSIZE - sizeof(struct PASDUHead_104)];
};

 struct CS104_SLAVE_SendData
{
    uint8_t TxLen;
    uint8_t TxData[CS104_SLAVE_ASDUSIZE];
};  
		
/* 组处理结构 -----------------------------------------------------------------*/ 
struct CS104_SLAVE_APPINFO
{	
    uint8_t Connect; // 网络连接状态	

    uint8_t  YXTypeID;           // 遥信类型标识
    uint8_t  YCTypeID;           // 遥测类型标识	
	
	uint8_t  SDataEn;            // 备份发送使能
    int16_t  CurSData;          // 未被确认的I帧备份缓冲区位置标识
    int16_t  SDataIn;           // 未被确认的I帧备份缓冲区入指针
	int16_t  SDataOut;           // 未被确认的I帧备份缓冲区出指针
	
  	uint8_t  RxdBuf[2*CS104_SLAVE_APDUSIZE]; // 接收缓冲区
    uint8_t  TxdBuf[2*CS104_SLAVE_APDUSIZE]; // 发送缓冲区

  	uint32_t  Data1Flag; 
    uint32_t  AllDataFlag; // 总召状态标志
    uint32_t  AppNextFlag; // 续发数据标志

  	uint8_t TxLen;     

  	struct CS104_SLAVE_PASDU_104 TxMsg;  // 应用层发送数据指针
//    struct CS104_SLAVE_PASDU_104 RxMsg; // 应用层接收数据指针
	
    struct CS104_SLAVE_SendData SData[CS104_SLAVE_K + 1]; // 未被确认的I帧备份

    struct PGroupTrn
    {		
        uint16_t InfoAddr;
        uint8_t TypeID;
        uint8_t COT;
        uint8_t GroupNo;
    }GroupTrn;// 分组处理结构体
	
    CS104_SLAVE_PASDU *StoreDATA1IN;              // 待发送缓冲区1级数据入指针
    CS104_SLAVE_PASDU *StoreDATA1OUT;             // 待发送缓冲区1级数据出指针
    CS104_SLAVE_PASDU StoreDATA1Buf[CS104_SLAVE_STOREDATA1NUM];     	// 1级数据待发送缓冲区
    CS104_SLAVE_PASDU *StoreDATA2IN;              // 待发送缓冲区1级数据入指针
    CS104_SLAVE_PASDU *StoreDATA2OUT;             // 待发送缓冲区1级数据出指针
    CS104_SLAVE_PASDU StoreDATA2Buf[CS104_SLAVE_STOREDATA2NUM];     	// 2级数据待发送缓冲区
    CS104_SLAVE_PASDU ST_Temp;              // 存储数据处理

    CS104_SLAVE_PASDU TX_Temp;              //发送数据处理
    CS104_SLAVE_PASDU RX_Temp;              //接受数据处理

    uint16_t AllData_yx_sendaddr;
    uint16_t AllData_yc_sendaddr;    
};

struct CS104_SLAVE_PAPCI
{
    uint8_t	StartCode;					//启动码
    uint8_t	Length;						//长度
    uint16_t NS;						//控制域发送
    uint16_t NR;						//控制域接收
};

struct CS104_SLAVE_LINKINFO
{
    uint8_t Connect; // 网络连接状态
    uint8_t DataType[CS104_SLAVE_K]; // 接收数据类型，I格式帧
    uint8_t StopSendFlag; //停止发送标志(未被确认的I格式帧达到K个，停止发送)		

    uint8_t RxdBuf[2*CS104_SLAVE_FRAMEBUFSIZE]; // 接收缓冲区
    uint8_t TxdBuf[2*CS104_SLAVE_FRAMEBUFSIZE]; // 发送缓冲区

    uint16_t NR; // 接收包计数
    uint16_t NS; // 发送包计数
    uint16_t NRACK; // 上次确认的接受序列号

    uint16_t PeerNoAckNum; // 对方未发送确认的I格式帧数目	  

    uint16_t RxdHead; // 处理数据的头指针
    uint16_t RxdTail;		
    uint16_t RxdLength;

    uint16_t FrameHead; // 组帧时数据头在发送缓冲区的位置
    uint16_t TxdHead;
    uint16_t TxdTail;

    // 参数输入
    uint16_t Tick0;
    uint16_t Tick1;
    uint16_t Tick2;
    uint16_t Tick3;

    uint32_t  ClockCounter;  // 程序运行周期控制
		
    enum DLRXDSTATUS
    {
            RXDSTART = 0,	                               // 寻找起始标志状态
            RXDHEAD,	                                   // 寻找报文头
            RXDCONTINUE                                  // 接收后续报文
    }RxdStatus;// 接收状态
        
    struct PTick
    {
        uint8_t	Flag; // 按位使用
        uint8_t	FlagBak; // 备份
        uint64_t timeBeginT0;                   /*t0 30s 建立连接的超时*/
        uint64_t timeEndT0;		                /*t0 30s 建立连接的超时*/
        uint64_t timeBeginT1[CS104_SLAVE_K*2];  /*t1 15s 发送或测试 APDU 的超时*/
        uint64_t timeEndT1[CS104_SLAVE_K*2];    /*t1 15s 发送或测试 APDU 的超时*/
        uint64_t timeBeginT2;	                /*t2 10s 无数据报文时确认的超时，t2<t1*/
        uint64_t timeEndT2;		                /*t2 10s 无数据报文时确认的超时，t2<t1*/
        uint64_t timeBeginT3;	                /*t3 20s 长期空闲状态下发送测试帧的超时*/
        uint64_t timeEndT3;		                /*t3 20s 长期空闲状态下发送测试帧的超时*/
        uint64_t timeResetBegin;                /*复位进程*/
        uint64_t timeResetEnd;
    }Tick;				
};

struct CS104_SLAVE_AppMsg
{
    uint16_t Cmd;
    uint16_t ConNum;
    uint8_t Len;
    uint8_t* pData;
};

struct sCS104_Slave_Pad
{
    uint8_t   Port;            //端口
    uint8_t   DevId;		 //设备号
    uint8_t   ModuleSN;		 	 //模块ID号
    uint8_t   Encrypt;	     // 加密

    uint8_t   CotSize;      // 传输原因字节长度
    uint8_t   PubAddrSize;  // 链路公共地址字节长度
    uint8_t   LinkAddrSize; // 链路地址占用字节  
    uint8_t   InfoAddrSize; // 信息体地址字节长度

    uint8_t  YCNum;        // 遥测量总数
    uint8_t  YXNum;        // 遥信量总数
    uint8_t  YKNum;        // 遥控量总数
    uint8_t  DDNum;        // 电度量总数

    uint16_t  LinkAddress;  // 链路地址,链路地址可与公共地址相同 	  
    uint16_t  PubAddress;   // 公共地址，可与链路地址相同，即：同一链路上的设备(子站)地址

    uint16_t  YX_AllNum;    // 遥信总数
    uint16_t  YX_FirstAddr; // 遥信首地址
    uint16_t  YX_FrameNum;	// 遥信每组数量
    uint16_t  YC_AllNum;		// 遥测总数
    uint16_t  YC_FirstAddr; // 遥信首地址
    uint16_t  YC_FrameNum;  // 遥测每组数量

    uint32_t  TickValue[4]; // IEC104 4个延时时钟
    uint64_t   TimeOutReset;		//复位进程超时时间
	uint8_t    resetFlag;			//复位进程标志
	uint8_t    updateFlag;		//加密升级结束标志
    uint32_t   ClockTimers;  // 调用超出次数，程序执行一次，控制运行周期
};
#pragma pack(pop)
/* PUBLIC VARIABLES ----------------------------------------------------------*/
extern struct sCS104_Slave_Pad CS104_Slave_Pad[CS104_SLAVE_VOLUMES]; // DL/T634_5101规约参数

/* PUBLIC FUNCTION DECLARATION -----------------------------------------------*/
uint8_t CS104_Slave_Reply(uint8_t pdrv, uint8_t *pbuf);
uint8_t CS104_Slave_Appinit(uint8_t pdrv);
uint8_t CS104_Slave_Clock(uint8_t pdrv);

#endif /* END _CS104_SLAVE_APP_H_*/

/* END OF FILE ---------------------------------------------------------------*/


