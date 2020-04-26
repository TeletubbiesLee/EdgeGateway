/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      cs101SlaveApp.c
  * @brief:		101app交互处理
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2018-11-09
  * @update:    [2018-11-09][Chen][newly increased]
  */
#define LOG_TAG    "CS101_Slave_App "
/* INCLUDE FILES -------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../port/iec_interface.h"
#include "cs101SlaveApp.h"
#include "cs101SlaveDisk.h"
//#include "encrypt_disk.h"						//加密模块
//#include "elog.h"
/* PRIVATE VARIABLES ---------------------------------------------------------*/

#pragma pack(push,1)
typedef struct
{
    uint8_t Milliseconds_L;   // D7-D0
    uint8_t Milliseconds_H;   // D7-D0
    uint8_t Minutes;					// D5-D0
    uint8_t Hours;						// D4-D0
    uint8_t DayofWeekMonth;		// WEEK(D7-D5)MONTH(D4-D0)
    uint8_t Months;						// D3-D0
    uint8_t Years;						// D6-D0
}CS101_SLAVE_CP56Time2a_t;

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
        uint8_t	buff[256-sizeof(struct STATUS)-sizeof(struct HEAD)];
        struct
        {
            uint8_t InfoAddr_L;
            uint8_t InfoAddr_H;
            uint8_t QOI;
        }C_100;

        struct
        {
            uint8_t InfoAddr_L;
            uint8_t InfoAddr_H;
            CS101_SLAVE_CP56Time2a_t CP56Time2a;
        }C_103;

        struct
        {
            uint8_t InfoAddr_L;
            uint8_t InfoAddr_H;
            uint8_t QRP;
        }C_105;

        struct
        {
            uint8_t InfoAddr_L;
            uint8_t InfoAddr_H;
            uint8_t COI;
        }C_70;

        struct
        {
            uint8_t InfoAddr_L;
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
                    uint8_t InfoAddr_H;
                    uint8_t Value;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD))/3];
            }SQ0;

            struct
            {
                uint8_t InfoAddr_L;
                uint8_t InfoAddr_H;
                struct
                {
                    uint8_t Value;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-2)/1];
            }SQ1;
        }C_1;

        union
        {
            struct
            {
                struct
                {
                    uint8_t InfoAddr_L;
                    uint8_t InfoAddr_H;
                    uint8_t Value;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD))/3];
            }SQ0;

            struct
            {
                uint8_t InfoAddr_L;
                uint8_t InfoAddr_H;
                struct
                {
                    uint8_t Value;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-2)/1];
            }SQ1;
        }C_3;

        union
        {
            struct
            {
                struct
                {
                    uint8_t InfoAddr_L;
                    uint8_t InfoAddr_H;
                    uint8_t Value;
                    CS101_SLAVE_CP56Time2a_t CP56Time2a;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD))/10];
            }SQ0;

            struct
            {
                uint8_t InfoAddr_L;
                uint8_t InfoAddr_H;
                struct
                {
                    uint8_t Value;
                    CS101_SLAVE_CP56Time2a_t CP56Time2a;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-2)/8];
            }SQ1;
        }C_30;

        union
        {
            struct
            {
                struct
                {
                    uint8_t InfoAddr_L;
                    uint8_t InfoAddr_H;
                    uint8_t Value;
                    CS101_SLAVE_CP56Time2a_t CP56Time2a;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD))/10];
            }SQ0;

            struct
            {
                uint8_t InfoAddr_L;
                uint8_t InfoAddr_H;
                struct
                {
                    uint8_t Value;
                    CS101_SLAVE_CP56Time2a_t CP56Time2a;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-2)/8];
            }SQ1;
        }C_31;

        union
        {
            struct
            {
                struct
                {
                    uint8_t InfoAddr_L;
                    uint8_t InfoAddr_H;
                    uint32_t Value;
                    uint8_t QDS;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD))/7];
            }SQ0;

            struct
            {
                uint8_t InfoAddr_L;
                uint8_t InfoAddr_H;
                struct
                {
                    uint32_t Value;
                    uint8_t QDS;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-2)/5];
            }SQ1;
        }C_9;

        union
        {
            struct
            {
                struct
                {
                    uint8_t InfoAddr_L;
                    uint8_t InfoAddr_H;
                    uint32_t Value;
                    uint8_t QDS;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD))/7];
            }SQ0;

            struct
            {
                uint8_t InfoAddr_L;
                uint8_t InfoAddr_H;
                struct
                {
                    uint32_t Value_L;
                    uint8_t QDS;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-2)/5];
            }SQ1;
        }C_11;

        union
        {
            struct
            {
                struct
                {
                    uint8_t InfoAddr_L;
                    uint8_t InfoAddr_H;
                    uint32_t Value;
                    uint8_t QDS;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD))/7];
            }SQ0;

            struct
            {
                uint8_t InfoAddr_L;
                uint8_t InfoAddr_H;
                struct
                {
                    uint32_t Value;
                    uint8_t QDS;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-2)/5];
            }SQ1;
        }C_13;

        struct
        {
            uint8_t InfoAddr_L;
            uint8_t InfoAddr_H;
            uint8_t SCO_L;
            uint8_t SCO_H;
        }C_45;

        struct
        {
            uint8_t InfoAddr_L;
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
            uint8_t InfoAddr_H;
            uint8_t SN_L;
            uint8_t SN_H;
        }C_200;

        union
        {
            struct
            {
                uint8_t InfoAddr_L;
                uint8_t InfoAddr_H;
            }Down;

            struct
            {
                uint8_t InfoAddr_L;
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
                uint32_t Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-2)/4];
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
                    uint8_t InfoAddr_H;
                    uint8_t Tag;
                    uint8_t Len;
                    uint32_t Value;
                }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-4)/8];
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
                uint8_t InfoAddr_H;
                uint8_t Tag;
                uint8_t Len;
                uint32_t Value;
            }Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-4)/8];
        }C_203;

        struct
        {
            uint8_t Array[(256-sizeof(struct STATUS)-sizeof(struct HEAD)-4)/1];
        }C_210;
    }Data;
}DLT634_5101SLAVE_PASDU;

typedef struct
{
//link
	enum
	{
        RXDSTART = 0,	        // 寻找起始标志状态
        RXDHEAD,	            // 寻找报文头
        RXDCONTINUE             // 接收后续报文
	}RxdStatus;           		// 接收状态

	uint16_t TxdHead;	      				// 发送缓冲区头指针
	uint16_t TxdTail;	     				// 发送缓冲区尾指针
	uint8_t TxdBuf[CS101_SLAVE_LPDUSIZE];     	// 发送缓冲区

	uint16_t RxdHead;	      				// 接收数据区头指针
    uint16_t RxdTail;       				// 接收数据区尾指针
	uint16_t RxdLength;     				// 接收一帧数据长度
	uint8_t RxdBuf[CS101_SLAVE_LPDUSIZE];			// 接收缓冲区

	uint16_t TxdTail_Pri;
	uint8_t TxdBuf_Pri[CS101_SLAVE_LPDUSIZE]; // 发送缓冲区，暂存发送的数据，重发时用

	uint16_t TxdTail_Special;
	uint8_t TxdBuf_Special[CS101_SLAVE_LPDUSIZE]; // 发送缓冲区，暂存发送的数据，特殊数据链路重建时发送

//app
	DLT634_5101SLAVE_PASDU *StoreDATA1IN;              //待发送缓冲区1级数据入指针
	DLT634_5101SLAVE_PASDU *StoreDATA1OUT;             //待发送缓冲区1级数据出指针
	DLT634_5101SLAVE_PASDU StoreDATA1Buf[CS101_SLAVE_STOREDATA1NUM];     	// 1级数据待发送缓冲区
	DLT634_5101SLAVE_PASDU *StoreDATA2IN;              //待发送缓冲区2级数据入指针
	DLT634_5101SLAVE_PASDU *StoreDATA2OUT;             //待发送缓冲区2级数据出指针
	DLT634_5101SLAVE_PASDU StoreDATA2Buf[CS101_SLAVE_STOREDATA2NUM];     	// 2级数据待发送缓冲区
	DLT634_5101SLAVE_PASDU ST_Temp;              //存储数据处理

	DLT634_5101SLAVE_PASDU TX_Temp;              //发送数据处理
	DLT634_5101SLAVE_PASDU RX_Temp;              //接受数据处理

	uint32_t LinkFlag;              //回复链路数据标志
	uint32_t Data1Flag;				//回复1级数据标志
	uint32_t Data2Flag;				//回复2级数据标志

	uint32_t AllDataFlag;			//总召状态标志

	uint32_t ClockCounter;  			//程序运行周期控制
	uint32_t TimeOutTick_Pri; 			//超时节拍计时器
	uint32_t RetryCount;				//超时记数器

	uint32_t TimeOutTick_AskSta;       //链路重发启动链路间隔
    uint32_t TimeOutTick_AskCount;     //链路重发启动链路次数

    uint32_t TimeOutLink;       //链路无数据交互

    uint64_t timePriBegin;
    uint64_t timePriEnd;	/*重发超时计数*/
    uint64_t timeAskBegin;
    uint64_t timeAskEnd;	/*链路请求超时计数*/
    uint64_t timeResetBegin;/*复位进程*/
    uint64_t timeResetEnd;
    
    uint8_t RlaConCode;	        // 接收的控制码FCB位记录
	uint8_t FCBNoTurnNum;       // FCB未翻转计数
    uint8_t SendConCode;	    // 接收的控制码FCB位记录

	uint16_t AllData_yx_sendaddr; //
    uint16_t AllData_yc_sendaddr; //
}sCS101_Slave_AppInfo;
#pragma pack(pop)

sCS101_Slave_AppInfo  *CS101_Slave_App = NULL;

/* PUBLIC VARIABLE -----------------------------------------------------------*/
sCS101_Slave_Pad 		 CS101_Slave_Pad[CS101_SLAVE_DISK_VOLUMES];

/* PRIVATE FUNCTION PROTOTYPES -----------------------------------------------*/
/**
  * @brief : sum of CKS.
  * @param : [pdrv]
  * @param : [pBuf]
  * @return: sum
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static uint8_t CS101_Slave_CKS(uint8_t pdrv ,uint8_t *pBuf)
{
    uint16_t sum = 0;
    uint16_t j =0;
    uint8_t *p = 0;
    uint8_t i = 0;

    if (*(pBuf) == CS101_SLAVE_STARTCODE10)
    {
        for (j = 0; j < (CS101_Slave_Pad[pdrv].LinkAddrSize + 1); j++)
        {
            sum += *(pBuf + 1 + j);
        }
    }
    else if(*(pBuf) == CS101_SLAVE_STARTCODE68)
    {
        p = pBuf+4;
        i = *(pBuf+1);

        while (i--)
        {
            sum += (*p++);
        }
    }

    return (uint8_t)(sum&0xff);
}

/**
  * @brief : check iec 68.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static uint8_t CS101_Slave_CheckFram68Valid(uint8_t pdrv)
{
    uint8_t *pBuf = 0;

    pBuf = (CS101_Slave_App[pdrv].RxdBuf + CS101_Slave_App[pdrv].RxdHead);

	  // 报头校验
    if ((pBuf[0] != pBuf[3]) || (pBuf[1] != pBuf[2]))
    {
        return(0);
    }

	  // 公共地址校验
    if (CS101_Slave_Pad[pdrv].LinkAddrSize == 1)
    {
        if ((pBuf[5] != CS101_Slave_Pad[pdrv].SourceAddr) && (pBuf[5] != 0xff))
        {
            return(0);
        }
    }
    else
    {
        if (((pBuf[5]|(pBuf[6]<<8)) != CS101_Slave_Pad[pdrv].SourceAddr) && ((pBuf[5]|(pBuf[6]<<8)) != 0xffff))
        {
            return (0);
        }
    }

    return (1);
}

/**
  * @brief : check iec 10.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static uint8_t CS101_Slave_CheckFram10Valid(uint8_t pdrv)
{
    uint8_t *pBuf = 0;
    uint16_t addr = 0;

    pBuf = &CS101_Slave_App[pdrv].RxdBuf[CS101_Slave_App[pdrv].RxdHead];

	  // 固定帧长数据的校验和检测
    if (pBuf[2+CS101_Slave_Pad[pdrv].LinkAddrSize] != (CS101_Slave_CKS(pdrv, pBuf)&0xFF))
    {
        return (0);
    }

	  // 固定帧长数据的结束帧检测
    if (pBuf[3+CS101_Slave_Pad[pdrv].LinkAddrSize] != CS101_SLAVE_ENDCODE)
    {
        return (0);
    }

	  // 固定帧数据公共地址检测
    if (CS101_Slave_Pad[pdrv].LinkAddrSize == 1)
    {
        addr = pBuf[2];
    }
    else
    {
        addr = pBuf[2] | ((pBuf[3]<<8));
    }

    if (addr != CS101_Slave_Pad[pdrv].SourceAddr)
    {
        return (0);
    }

    return (1);
}

/**
  * @brief : send to cache.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS101_Slave_StoreIN(uint8_t pdrv,DLT634_5101SLAVE_PASDU *buf)		//解析主端 发送命令
{
	DLT634_5101SLAVE_PASDU *temp;
	DLT634_5101SLAVE_PASDU tempbuf;
	//  log_w(" CS101_Slave_App[pdrv].StoreDATA1IN:0x%x",CS101_Slave_App[pdrv].StoreDATA1IN);
	//  log_w(" CS101_Slave_App[pdrv].StoreDATA1OUT:0x%x",CS101_Slave_App[pdrv].StoreDATA1OUT);
	if(buf->status.symbol.priority<CS101_SLAVE_M_FT_NA_P)	//待发一级数据缓冲
	{
		temp = CS101_Slave_App[pdrv].StoreDATA1IN;
		if(++temp >= CS101_Slave_App[pdrv].StoreDATA1Buf + CS101_SLAVE_STOREDATA1NUM)
		{
			temp = CS101_Slave_App[pdrv].StoreDATA1Buf;
		}

		if(temp == CS101_Slave_App[pdrv].StoreDATA1OUT)
		{
			return(FALSE);
		}

		temp = CS101_Slave_App[pdrv].StoreDATA1OUT;
		while(temp != CS101_Slave_App[pdrv].StoreDATA1IN)
		{
			if(((temp->status.symbol.priority > buf->status.symbol.priority)||(temp->status.symbol.priority == 0))&&(temp != CS101_Slave_App[pdrv].StoreDATA1OUT))
			{
				memcpy(&tempbuf,temp,sizeof(DLT634_5101SLAVE_PASDU));
				memcpy(temp,buf,sizeof(DLT634_5101SLAVE_PASDU));
				memcpy(buf,&tempbuf,sizeof(DLT634_5101SLAVE_PASDU));
			}
			if(++temp >= CS101_Slave_App[pdrv].StoreDATA1Buf + CS101_SLAVE_STOREDATA1NUM)
			{
				temp = CS101_Slave_App[pdrv].StoreDATA1Buf;
			}
		}

		memcpy(temp,buf,sizeof(DLT634_5101SLAVE_PASDU));
		if(++CS101_Slave_App[pdrv].StoreDATA1IN >= CS101_Slave_App[pdrv].StoreDATA1Buf + CS101_SLAVE_STOREDATA1NUM)
		{
			CS101_Slave_App[pdrv].StoreDATA1IN = CS101_Slave_App[pdrv].StoreDATA1Buf;
		}
	}
	else												//待发二级数据缓冲
	{
		temp = CS101_Slave_App[pdrv].StoreDATA2IN;
		if(++temp >= CS101_Slave_App[pdrv].StoreDATA2Buf + CS101_SLAVE_STOREDATA2NUM)
		{
			temp = CS101_Slave_App[pdrv].StoreDATA2Buf;
		}

		if(temp == CS101_Slave_App[pdrv].StoreDATA2OUT)
		{
			return(FALSE);
		}

		temp = CS101_Slave_App[pdrv].StoreDATA2OUT;
		while(temp != CS101_Slave_App[pdrv].StoreDATA2IN)
		{
			if(((temp->status.symbol.priority > buf->status.symbol.priority)||(temp->status.symbol.priority == 0))&&(temp != CS101_Slave_App[pdrv].StoreDATA2OUT))
			{
				memcpy(&tempbuf,temp,sizeof(DLT634_5101SLAVE_PASDU));
				memcpy(temp,buf,sizeof(DLT634_5101SLAVE_PASDU));
				memcpy(buf,&tempbuf,sizeof(DLT634_5101SLAVE_PASDU));
			}
			if(++temp >= CS101_Slave_App[pdrv].StoreDATA2Buf + CS101_SLAVE_STOREDATA2NUM)
			{
				temp = CS101_Slave_App[pdrv].StoreDATA2Buf;
			}
		}

		memcpy(temp,buf,sizeof(DLT634_5101SLAVE_PASDU));
		if(++CS101_Slave_App[pdrv].StoreDATA2IN >= CS101_Slave_App[pdrv].StoreDATA2Buf + CS101_SLAVE_STOREDATA2NUM)
		{
			CS101_Slave_App[pdrv].StoreDATA2IN = CS101_Slave_App[pdrv].StoreDATA2Buf;
		}
	}
//	 log_w(" CS101_Slave_App[pdrv].StoreDATA1IN:0x%x",CS101_Slave_App[pdrv].StoreDATA1IN);
//	 log_w(" CS101_Slave_App[pdrv].StoreDATA1OUT:0x%x",CS101_Slave_App[pdrv].StoreDATA1OUT);
	return(TRUE);
}

/**
  * @brief : analysis.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS101_Slave_AppProcess(uint8_t pdrv)
{
	//整理成02版报文，便于控制和传输
	memset((uint8_t *)&CS101_Slave_App[pdrv].RX_Temp,0,sizeof(DLT634_5101SLAVE_PASDU));
	CS101_Slave_App[pdrv].RX_Temp.status.Length = CS101_Slave_App[pdrv].RxdBuf[1] -1 -CS101_Slave_Pad[pdrv].LinkAddrSize;		//应用数据单元长度
	memcpy((uint8_t *)&CS101_Slave_App[pdrv].RX_Temp.Head.TypeID,&CS101_Slave_App[pdrv].RxdBuf[5+CS101_Slave_Pad[pdrv].LinkAddrSize],CS101_Slave_App[pdrv].RX_Temp.status.Length);		//给应用数据单元赋值
	if(CS101_Slave_Pad[pdrv].ASDUCotSize == 1)
	{
		CS101_Slave_App[pdrv].RX_Temp.Head.COT_H = 0;
		memcpy((uint8_t *)&CS101_Slave_App[pdrv].RX_Temp.Head.PubAddr_L,&CS101_Slave_App[pdrv].RxdBuf[5+CS101_Slave_Pad[pdrv].LinkAddrSize+2+CS101_Slave_Pad[pdrv].ASDUCotSize]
		,CS101_Slave_App[pdrv].RX_Temp.status.Length-2-CS101_Slave_Pad[pdrv].ASDUCotSize);
		CS101_Slave_App[pdrv].RX_Temp.status.Length = CS101_Slave_App[pdrv].RX_Temp.status.Length - CS101_Slave_Pad[pdrv].ASDUCotSize + 2;
	}
	if(CS101_Slave_Pad[pdrv].ASDUAddrSize == 1)
	{
		CS101_Slave_App[pdrv].RX_Temp.Head.PubAddr_H = 0;
		memcpy((uint8_t *)&CS101_Slave_App[pdrv].RX_Temp.Data.buff[0],&CS101_Slave_App[pdrv].RxdBuf[5+CS101_Slave_Pad[pdrv].LinkAddrSize+2+CS101_Slave_Pad[pdrv].ASDUCotSize+CS101_Slave_Pad[pdrv].ASDUAddrSize]
		,CS101_Slave_App[pdrv].RX_Temp.status.Length-2-CS101_Slave_Pad[pdrv].ASDUCotSize-CS101_Slave_Pad[pdrv].ASDUAddrSize);
		CS101_Slave_App[pdrv].RX_Temp.status.Length = CS101_Slave_App[pdrv].RX_Temp.status.Length - CS101_Slave_Pad[pdrv].ASDUAddrSize + 2;
	}
	CS101_Slave_App[pdrv].RX_Temp.status.Length += sizeof(CS101_Slave_App[pdrv].RX_Temp.status);
	if((CS101_Slave_App[pdrv].RX_Temp.Head.PubAddr_L|(CS101_Slave_App[pdrv].RX_Temp.Head.PubAddr_H<<8)) == CS101_Slave_Pad[pdrv].ASDUAddr)	//信息对象地址
    {
        switch (CS101_Slave_App[pdrv].RX_Temp.Head.TypeID)						//TI类型标识
        {
			//控制方向过程信息
			case CS101_SLAVE_C_SC_NA_1: // 单点遥控命令
			case CS101_SLAVE_C_SC_NB_1: // 双点遥控命令
				CS101_Slave_C_SC(pdrv,(uint8_t *)&CS101_Slave_App[pdrv].RX_Temp);
					break;

			//控制方向命令信息
			case CS101_SLAVE_C_IC_NA_1: // 总召唤或分组召唤
				if((CS101_Slave_App[pdrv].RX_Temp.Data.C_100.InfoAddr_H <<8 |
					CS101_Slave_App[pdrv].RX_Temp.Data.C_100.InfoAddr_L) != 0)					//地址为0 总召唤
				{
					CS101_Slave_App[pdrv].RX_Temp.status.symbol.priority = CS101_SLAVE_C_IC_NA_P;
					CS101_Slave_App[pdrv].RX_Temp.Head.COT_L = CS101_SLAVE_COT_ADDRERR;
					CS101_Slave_StoreIN(pdrv,&CS101_Slave_App[pdrv].RX_Temp);
				}
				else if(!((CS101_Slave_App[pdrv].RX_Temp.Head.COT_L|(CS101_Slave_App[pdrv].RX_Temp.Head.COT_H<<8)) == CS101_SLAVE_COT_ACT ||
					(CS101_Slave_App[pdrv].RX_Temp.Head.COT_L|(CS101_Slave_App[pdrv].RX_Temp.Head.COT_H<<8)) == CS101_SLAVE_COT_DEACT))				//判断是否激活
				{
					CS101_Slave_App[pdrv].RX_Temp.status.symbol.priority = CS101_SLAVE_C_IC_NA_P;
					CS101_Slave_App[pdrv].RX_Temp.Head.COT_L = CS101_SLAVE_COT_COTERR;
					CS101_Slave_StoreIN(pdrv,&CS101_Slave_App[pdrv].RX_Temp);
				}
				else					//总召唤进入
				{
					if((CS101_Slave_App[pdrv].RX_Temp.Head.COT_L|(CS101_Slave_App[pdrv].RX_Temp.Head.COT_H<<8)) == CS101_SLAVE_COT_ACT)
					{
						CS101_Slave_App[pdrv].Data1Flag |= CS101_SLAVE_CALLALLDATA;
						CS101_Slave_App[pdrv].AllDataFlag = 0;
						CS101_Slave_C_IC(pdrv,(uint8_t *)&CS101_Slave_App[pdrv].RX_Temp);	//总召唤
					}
					else if((CS101_Slave_App[pdrv].RX_Temp.Head.COT_L|(CS101_Slave_App[pdrv].RX_Temp.Head.COT_H<<8)) == CS101_SLAVE_COT_DEACT)
					{
						CS101_Slave_App[pdrv].Data1Flag &= (~CS101_SLAVE_CALLALLDATA);
						CS101_Slave_App[pdrv].AllDataFlag = 0;
						CS101_Slave_App[pdrv].RX_Temp.status.symbol.priority = CS101_SLAVE_C_IC_NA_P;
						CS101_Slave_App[pdrv].RX_Temp.Head.COT_L = CS101_SLAVE_COT_DEACTCON;
						CS101_Slave_StoreIN(pdrv,&CS101_Slave_App[pdrv].RX_Temp);
					}
				}
				break;

			case CS101_SLAVE_C_CI_NA_1: // 电度总召唤或分组召唤
					break;

			case CS101_SLAVE_C_CS_NA_1: // 对时命令
				if((CS101_Slave_App[pdrv].RX_Temp.Data.C_100.InfoAddr_H <<8 |
					CS101_Slave_App[pdrv].RX_Temp.Data.C_100.InfoAddr_L) != 0)	//判断信息对象地址默认为0
				{
					CS101_Slave_App[pdrv].RX_Temp.status.symbol.priority = CS101_SLAVE_C_CS_NA_P;
					CS101_Slave_App[pdrv].RX_Temp.Head.COT_L = CS101_SLAVE_COT_ADDRERR;
					CS101_Slave_StoreIN(pdrv,&CS101_Slave_App[pdrv].RX_Temp);
				}
				else
				{CS101_Slave_C_CS(pdrv,(uint8_t *)&CS101_Slave_App[pdrv].RX_Temp);}
					break;

			case CS101_SLAVE_C_TS_NA_1: // 测试命令
				CS101_Slave_App[pdrv].RX_Temp.status.symbol.priority = CS101_SLAVE_C_TS_NA_P;
				CS101_Slave_App[pdrv].RX_Temp.Head.COT_L = CS101_SLAVE_COT_ACTCON;
				CS101_Slave_StoreIN(pdrv,&CS101_Slave_App[pdrv].RX_Temp);
				break;

			case CS101_SLAVE_C_RP_NA_1: // 复位进程命令
				if((CS101_Slave_App[pdrv].RX_Temp.Data.C_100.InfoAddr_H <<8 |
					CS101_Slave_App[pdrv].RX_Temp.Data.C_100.InfoAddr_L) != 0)
				{
					CS101_Slave_App[pdrv].RX_Temp.status.symbol.priority = CS101_SLAVE_C_RP_NA_P;
					CS101_Slave_App[pdrv].RX_Temp.Head.COT_L = CS101_SLAVE_COT_ADDRERR;
					CS101_Slave_StoreIN(pdrv,&CS101_Slave_App[pdrv].RX_Temp);
				}
				else if(!((CS101_Slave_App[pdrv].RX_Temp.Head.COT_L|(CS101_Slave_App[pdrv].RX_Temp.Head.COT_H<<8)) == CS101_SLAVE_COT_ACT ||
					(CS101_Slave_App[pdrv].RX_Temp.Head.COT_L|(CS101_Slave_App[pdrv].RX_Temp.Head.COT_H<<8)) == CS101_SLAVE_COT_DEACT))
				{
					CS101_Slave_App[pdrv].RX_Temp.status.symbol.priority = CS101_SLAVE_C_RP_NA_P;
					CS101_Slave_App[pdrv].RX_Temp.Head.COT_L = CS101_SLAVE_COT_COTERR;
					CS101_Slave_StoreIN(pdrv,&CS101_Slave_App[pdrv].RX_Temp);
				}
				else
				{
					CS101_Slave_App[pdrv].Data1Flag |= CS101_SLAVE_HAVERESET;
					CS101_Slave_C_RP(pdrv,(uint8_t *)&CS101_Slave_App[pdrv].RX_Temp);
				}
					break;

			case CS101_SLAVE_C_SR_NA_1: // 切换定值区
			case CS101_SLAVE_C_RR_NA_1: // 读定值区号
			case CS101_SLAVE_C_RS_NA_1: // 读参数和定值命令
			case CS101_SLAVE_C_WS_NA_1: // 写参数和定值命令
				CS101_Slave_C_SR(pdrv,(uint8_t *)&CS101_Slave_App[pdrv].RX_Temp);
					break;

			case CS101_SLAVE_F_FR_NA_1: // 文件传输
				CS101_Slave_F_FR(pdrv,(uint8_t *)&CS101_Slave_App[pdrv].RX_Temp);
				break;

			case CS101_SLAVE_F_SR_NA_1: // 软件升级
				CS101_Slave_F_SR(pdrv,(uint8_t *)&CS101_Slave_App[pdrv].RX_Temp);
				break;

			default: // 类型标识有错误或不支持
				CS101_Slave_App[pdrv].RX_Temp.status.symbol.priority = CS101_SLAVE_M_EI_NA_P;
				CS101_Slave_App[pdrv].RX_Temp.Head.COT_L = CS101_SLAVE_COT_TIERR;
				CS101_Slave_StoreIN(pdrv,&CS101_Slave_App[pdrv].RX_Temp);
					break;
        }
    }
    else
    {
        CS101_Slave_App[pdrv].RX_Temp.status.symbol.priority = CS101_SLAVE_M_EI_NA_P;
        CS101_Slave_App[pdrv].RX_Temp.Head.COT_L = CS101_SLAVE_COT_ASUDERR;
        CS101_Slave_StoreIN(pdrv,&CS101_Slave_App[pdrv].RX_Temp);
    }
}

/**
  * @brief : write data to misi.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static uint8_t CS101_Slave_WriteDataToMISI(uint8_t pdrv)
{
    uint16_t sendlen = 0;

    sendlen = CS101_Slave_WriteData(pdrv,CS101_Slave_App[pdrv].TxdBuf + CS101_Slave_App[pdrv].TxdHead, CS101_Slave_App[pdrv].TxdTail - CS101_Slave_App[pdrv].TxdHead);

    CS101_Slave_App[pdrv].TxdHead += sendlen;
    if (CS101_Slave_App[pdrv].TxdHead >= CS101_Slave_App[pdrv].TxdTail) // 该次任务数据已经发完
    {
        CS101_Slave_App[pdrv].TxdHead = 0;
        CS101_Slave_App[pdrv].TxdTail = 0;
        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}

/**
  * @brief : 68 analysis.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_DecodeFrame68(uint8_t pdrv)
{
    uint8_t control;
    control = CS101_Slave_App[pdrv].RxdBuf[4];
    if (control & CS101_SLAVE_PRM) // 子站作为从动站接收主站链路报文。
    {
        if ((!(control&CS101_SLAVE_FCV)) || ((control&CS101_SLAVE_FCV)&&((control&CS101_SLAVE_FCB) != ((CS101_Slave_App[pdrv].RlaConCode) & 0x20))) || (!(CS101_Slave_App[pdrv].LinkFlag & CS101_SLAVE_FIRST_RlaConCode)))
        {
            if(CS101_Slave_App[pdrv].LinkFlag & CS101_SLAVE_INITEND)		//初始化链路结束标志
            {
                if (control & CS101_SLAVE_FCV) // FCV有效而且FCB变位
                {
                    CS101_Slave_App[pdrv].RlaConCode = CS101_Slave_App[pdrv].RxdBuf[4]; // 保存最新的功能码，这里只用到FCB
                    CS101_Slave_App[pdrv].FCBNoTurnNum = CS101_SLAVE_NUMOF_MAXRETRY; // 设置重发次数，指下次收到主站报文FCB未翻转时重发缓冲区内的报文。
                    CS101_Slave_App[pdrv].LinkFlag |= CS101_SLAVE_FIRST_RlaConCode;
                }
                if((control & CS101_SLAVE_FUNCODE) == CS101_SLAVE_M_FUN3)
                {
                    CS101_Slave_App[pdrv].LinkFlag |= CS101_SLAVE_RECONFIRM;
                }
                CS101_Slave_AppProcess(pdrv);								//68帧处理App
            }
        }
        else  // FCB 未翻转
        {
            if(CS101_Slave_Pad[pdrv].BalanMode)
            {
                CS101_Slave_App[pdrv].LinkFlag |= CS101_SLAVE_RECONFIRM;
            }
            else
            {
                if (CS101_Slave_App[pdrv].FCBNoTurnNum)
                {
                    CS101_Slave_App[pdrv].FCBNoTurnNum--;
                    memcpy(CS101_Slave_App[pdrv].TxdBuf + CS101_Slave_App[pdrv].TxdTail, CS101_Slave_App[pdrv].TxdBuf_Pri, CS101_Slave_App[pdrv].TxdTail_Pri);
                    CS101_Slave_App[pdrv].TxdTail += CS101_Slave_App[pdrv].TxdTail_Pri;
                    CS101_Slave_WriteDataToMISI(pdrv);
                }
            }
            return;
        }
    }
    else  // Prm=0；子站作为启动站接收主站链路报文
    {
    }
}

/**
  * @brief : status reset.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_StatusReset(uint8_t pdrv)
{
    CS101_Slave_App[pdrv].LinkFlag = 0;
    CS101_Slave_App[pdrv].Data1Flag = 0;
    CS101_Slave_App[pdrv].Data2Flag = 0;
    CS101_Slave_App[pdrv].SendConCode = 0;

    CS101_Slave_App[pdrv].StoreDATA1IN = CS101_Slave_App[pdrv].StoreDATA1Buf;
    CS101_Slave_App[pdrv].StoreDATA1OUT = CS101_Slave_App[pdrv].StoreDATA1Buf;
    memset(CS101_Slave_App[pdrv].StoreDATA1Buf,0,sizeof(CS101_Slave_App[pdrv].StoreDATA1Buf));
    CS101_Slave_App[pdrv].StoreDATA2IN = CS101_Slave_App[pdrv].StoreDATA2Buf;
    CS101_Slave_App[pdrv].StoreDATA2OUT = CS101_Slave_App[pdrv].StoreDATA2Buf;
    memset(CS101_Slave_App[pdrv].StoreDATA2Buf,0,sizeof(CS101_Slave_App[pdrv].StoreDATA2Buf));
}

/**
  * @brief : 10 analysis.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_DecodeFrame10(uint8_t pdrv)
{
    uint8_t stop = 0;
    uint8_t control = 0;
    static uint8_t initflag[CS101_SLAVE_DISK_VOLUMES] = {0};

    control = CS101_Slave_App[pdrv].RxdBuf[1];							//控制域C
    if (control & CS101_SLAVE_PRM) // 子站作为从动站接收主站的命令
    {
        if ((!(control&CS101_SLAVE_FCV)) || ((control&CS101_SLAVE_FCV)&&((control&CS101_SLAVE_FCB) != ((CS101_Slave_App[pdrv].RlaConCode) & 0x20))) || (!(CS101_Slave_App[pdrv].LinkFlag & CS101_SLAVE_FIRST_RlaConCode)))
        {
            if (control & CS101_SLAVE_FCV) // FCV有效，FCB翻转
            {
                CS101_Slave_App[pdrv].RlaConCode = CS101_Slave_App[pdrv].RxdBuf[1];
                CS101_Slave_App[pdrv].FCBNoTurnNum = CS101_SLAVE_NUMOF_MAXRETRY;
				CS101_Slave_App[pdrv].LinkFlag |= CS101_SLAVE_FIRST_RlaConCode;
            }
            switch (control & CS101_SLAVE_FUNCODE)
            {
                case CS101_SLAVE_M_FUN2: // 心跳测试
                    CS101_Slave_App[pdrv].LinkFlag |= CS101_SLAVE_RECONFIRM;
                    break;

                case CS101_SLAVE_M_FUN9: // 召唤链路状态
					CS101_Slave_App[pdrv].LinkFlag |= CS101_SLAVE_REQSTATUS;
								    /* 可以避免答非所问的过程，尽快建立链路 */
                    while (((CS101_Slave_App[pdrv].RxdTail - CS101_Slave_App[pdrv].RxdHead) >= CS101_Slave_Pad[pdrv].FixFrmLength) && (stop == 0)) // 收到召唤链路状态命令时，清掉后续相同命令。
                    {
                        if (memcmp(CS101_Slave_App[pdrv].RxdBuf, CS101_Slave_App[pdrv].RxdBuf + CS101_Slave_App[pdrv].RxdHead, CS101_Slave_Pad[pdrv].FixFrmLength) == 0)
                        {
                            CS101_Slave_App[pdrv].RxdHead += CS101_Slave_Pad[pdrv].FixFrmLength;
                        }
                        else
                        {
                            stop = 1;
                        }
                    }
                    #if CS101_SLAVE_COMMUNICATION
                    CS101_Slave_App[pdrv].RX_Temp.status.Length = sizeof(CS101_Slave_App[pdrv].TX_Temp.status) + sizeof(CS101_Slave_App[pdrv].TX_Temp.Head) + sizeof(CS101_Slave_App[pdrv].TX_Temp.Data.C_105);
                    CS101_Slave_App[pdrv].RX_Temp.Head.TypeID = CS101_SLAVE_C_RP_NA_1;
                    CS101_Slave_App[pdrv].RX_Temp.Head.VSQ = 1;
                    CS101_Slave_App[pdrv].RX_Temp.Head.COT_L = CS101_SLAVE_COT_ACT;
                    CS101_Slave_App[pdrv].RX_Temp.Head.PubAddr_H = (uint8_t)((CS101_Slave_Pad[pdrv].ASDUAddr>>8)&0xff);
                    CS101_Slave_App[pdrv].RX_Temp.Head.PubAddr_L = (uint8_t)(CS101_Slave_Pad[pdrv].ASDUAddr&0xff);
                    CS101_Slave_App[pdrv].RX_Temp.Data.C_105.QRP = 1;
                    #endif
                    break;

                case CS101_SLAVE_M_FUN0: // 复位链路
                    CS101_Slave_StatusReset(pdrv);

                    CS101_Slave_App[pdrv].LinkFlag |= CS101_SLAVE_INITEND;//初始化结束标志
                    if(CS101_Slave_Pad[pdrv].BalanMode)
                    {
                        CS101_Slave_App[pdrv].LinkFlag |= CS101_SLAVE_RECONFIRM;
                        CS101_Slave_App[pdrv].LinkFlag |= CS101_SLAVE_ASKSTATUS;
                    }
                    else
                    {
                        CS101_Slave_App[pdrv].LinkFlag |= CS101_SLAVE_RECONFIRM;
                        if(initflag[pdrv] == 0)
                        {
                            initflag[pdrv] = 1;
                            CS101_Slave_App[pdrv].Data1Flag |= CS101_SLAVE_HAVEINITEND;
                        }
                    }
                    break;

                case CS101_SLAVE_M_FUN10: // 功能码-10，召唤用户一级数据
                    // 应用层调用
                    if (!(CS101_Slave_App[pdrv].LinkFlag&CS101_SLAVE_INITEND))
                    {
                        return;
                    }
                    CS101_Slave_App[pdrv].LinkFlag |= CS101_SLAVE_REDATA1;
                    break;

                case CS101_SLAVE_M_FUN11: // 功能码-11，召唤用户二级数据
                    // 应用层调用
                    if (!(CS101_Slave_App[pdrv].LinkFlag&CS101_SLAVE_INITEND))
                    {
                        return;
                    }
                    CS101_Slave_App[pdrv].LinkFlag |= CS101_SLAVE_REDATA2;
                    break;

                default:
                    break;
            }
            return;
        }
        else // FCB未翻转
        {
            if(CS101_Slave_Pad[pdrv].BalanMode)
            {
                CS101_Slave_App[pdrv].LinkFlag |= CS101_SLAVE_RECONFIRM;
            }
            else if (CS101_Slave_App[pdrv].FCBNoTurnNum)
            {
                CS101_Slave_App[pdrv].FCBNoTurnNum--;
                memcpy(CS101_Slave_App[pdrv].TxdBuf + CS101_Slave_App[pdrv].TxdTail, CS101_Slave_App[pdrv].TxdBuf_Pri, CS101_Slave_App[pdrv].TxdTail_Pri);
                CS101_Slave_App[pdrv].TxdTail += CS101_Slave_App[pdrv].TxdTail_Pri;
                CS101_Slave_WriteDataToMISI(pdrv);
            }
        }
    }
    else // Prm=0 平衡模式子站作为启动站接收到主站的命令。
    {
        switch (control & CS101_SLAVE_FUNCODE)
        {
            case CS101_SLAVE_S_FUN0:	// 确认
                CS101_Slave_App[pdrv].TimeOutTick_Pri = 0; // 链路有数据就可以不重发
                CS101_Slave_App[pdrv].LinkFlag |= CS101_SLAVE_SENDABLE;
                break;

            case CS101_SLAVE_S_FUN11: // 应答链路
                CS101_Slave_StatusReset(pdrv);

                CS101_Slave_App[pdrv].LinkFlag |= CS101_SLAVE_ASKRELINK;
                if(initflag[pdrv] == 0)
                {
                    initflag[pdrv] = 1;
                    CS101_Slave_App[pdrv].Data1Flag |= CS101_SLAVE_HAVEINITEND;
                }

                CS101_Slave_App[pdrv].LinkFlag |= CS101_SLAVE_INITEND;//初始化结束标志
                break;

            default:
                break;
        }
    }
}

/**
  * @brief : Search Valid Frame.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void cs101Slave_SearchValidFrame(uint8_t pdrv)
{
    uint8_t stop = 0;
    uint8_t length = 0;

    while ((CS101_Slave_App[pdrv].RxdHead < CS101_Slave_App[pdrv].RxdTail) && (!stop))						//尾值为读取的字节数
    {
        if (CS101_Slave_App[pdrv].RxdStatus == RXDSTART)													//寻找起始标志状态
        {
            while ((CS101_Slave_App[pdrv].RxdBuf[CS101_Slave_App[pdrv].RxdHead] != CS101_SLAVE_STARTCODE10)
                    && (CS101_Slave_App[pdrv].RxdBuf[CS101_Slave_App[pdrv].RxdHead] != CS101_SLAVE_STARTCODE68)
                    && (CS101_Slave_App[pdrv].RxdHead < CS101_Slave_App[pdrv].RxdTail))						//判断首字节是否不为0x10 0x68
            {
                CS101_Slave_App[pdrv].RxdHead++;
            }

            if (CS101_Slave_App[pdrv].RxdHead < CS101_Slave_App[pdrv].RxdTail) 								// 找到启动字符，并将报文与缓冲区对齐。
            {
                CS101_Slave_App[pdrv].RxdStatus = RXDHEAD;													//寻找报文头
                if (CS101_Slave_App[pdrv].RxdHead != 0)
                {
                    memcpy(CS101_Slave_App[pdrv].RxdBuf,CS101_Slave_App[pdrv].RxdBuf+CS101_Slave_App[pdrv].RxdHead,CS101_Slave_App[pdrv].RxdTail-CS101_Slave_App[pdrv].RxdHead);//对齐数据到缓冲区
                    CS101_Slave_App[pdrv].RxdTail -= CS101_Slave_App[pdrv].RxdHead;		//报文尾为字节数
                    CS101_Slave_App[pdrv].RxdHead = 0;									//报文头清0
                }
            }
        }

        CS101_Slave_App[pdrv].RxdLength = CS101_Slave_App[pdrv].RxdTail - CS101_Slave_App[pdrv].RxdHead;	//报文长度
        if (CS101_Slave_App[pdrv].RxdStatus == RXDHEAD) // 报文够一帧则处理
        {
            switch (CS101_Slave_App[pdrv].RxdBuf[CS101_Slave_App[pdrv].RxdHead])
            {
                case CS101_SLAVE_STARTCODE68:
                    if (CS101_Slave_App[pdrv].RxdLength >= 5+CS101_Slave_Pad[pdrv].LinkAddrSize) // 启动字符到链路地址共6字节。
                    {
                        if (!CS101_Slave_CheckFram68Valid(pdrv)) // 判断是否报文头前4字节校验		本地地址校验
                        {
                            CS101_Slave_App[pdrv].RxdHead++;
                            CS101_Slave_App[pdrv].RxdStatus = RXDSTART;					//寻找起始标志状态
                        }
                        else
                        {
                            CS101_Slave_App[pdrv].RxdStatus = RXDCONTINUE;				//接收后续报文
                        }
                    }
                    else
                    {
                        stop = 1;
                    }
                    break;

                case CS101_SLAVE_STARTCODE10:
                    if (CS101_Slave_App[pdrv].RxdLength >= 4 + CS101_Slave_Pad[pdrv].LinkAddrSize)// 启动字符到链路地址共5字节。 最少5个字节 ，一般6个字节
                    {
                        if (!CS101_Slave_CheckFram10Valid(pdrv))
                        {
                            CS101_Slave_App[pdrv].RxdHead++;
                            CS101_Slave_App[pdrv].RxdStatus = RXDSTART;
                        }
                        else
                        {
                            CS101_Slave_App[pdrv].RxdStatus = RXDCONTINUE;
                        }
                    }
                    else
                    {
                        stop = 1;
                    }
                    break;
            }
        }
        if (CS101_Slave_App[pdrv].RxdStatus == RXDCONTINUE)			//处理报文
        {
            switch (CS101_Slave_App[pdrv].RxdBuf[CS101_Slave_App[pdrv].RxdHead])
            {
            case CS101_SLAVE_STARTCODE68:
                length = CS101_Slave_App[pdrv].RxdBuf[CS101_Slave_App[pdrv].RxdHead+1];			//报文长度L
                if (CS101_Slave_App[pdrv].RxdLength >= length+6) // 报文收全，进行处理
                {
                    if ((CS101_Slave_App[pdrv].RxdBuf[CS101_Slave_App[pdrv].RxdHead+length+4] == (CS101_Slave_CKS(pdrv, CS101_Slave_App[pdrv].RxdBuf+CS101_Slave_App[pdrv].RxdHead)&0xFF))//校验正确  判断校验位
                            && (CS101_Slave_App[pdrv].RxdBuf[CS101_Slave_App[pdrv].RxdHead+length+4+1] == CS101_SLAVE_ENDCODE)) // 结束码正确
 					{
                        if ( CS101_Slave_App[pdrv].RxdHead > 0 )
                        {
                            memcpy(CS101_Slave_App[pdrv].RxdBuf,CS101_Slave_App[pdrv].RxdBuf+CS101_Slave_App[pdrv].RxdHead,CS101_Slave_App[pdrv].RxdTail-CS101_Slave_App[pdrv].RxdHead);//报文与缓冲区对齐
                            CS101_Slave_App[pdrv].RxdTail -= CS101_Slave_App[pdrv].RxdHead;
                            CS101_Slave_App[pdrv].RxdHead = 0;
                        }
												/* 头指针后移到报文后 */
                        CS101_Slave_App[pdrv].RxdHead += (CS101_Slave_App[pdrv].RxdBuf[1]+6);
                        CS101_Slave_App[pdrv].RxdStatus = RXDSTART;

                        CS101_Slave_DecodeFrame68(pdrv);			//68报文解析
                    }
                    else
                    {
                        CS101_Slave_App[pdrv].RxdHead += 6;
                        CS101_Slave_App[pdrv].RxdStatus = RXDSTART;
                    }
                }
                else // 报文未收全，不处理，继续等待数据。
                {
                    stop = 1;
                }
                break;

            case CS101_SLAVE_STARTCODE10:
                if (CS101_Slave_App[pdrv].RxdHead > 0)
                {
                    memcpy(CS101_Slave_App[pdrv].RxdBuf,CS101_Slave_App[pdrv].RxdBuf+CS101_Slave_App[pdrv].RxdHead,CS101_Slave_App[pdrv].RxdTail-CS101_Slave_App[pdrv].RxdHead);
                    CS101_Slave_App[pdrv].RxdTail -= CS101_Slave_App[pdrv].RxdHead;
                    CS101_Slave_App[pdrv].RxdHead = 0;
                }
                CS101_Slave_App[pdrv].RxdHead += 4 + CS101_Slave_Pad[pdrv].LinkAddrSize;
                CS101_Slave_App[pdrv].RxdStatus = RXDSTART;

                CS101_Slave_DecodeFrame10(pdrv);					//10报文解析
                break;
            }
        }
    }
}

/**
  * @brief : read MISI Data.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_ReadMISIData(uint8_t pdrv)
{
    uint16_t count = 0;
    if (CS101_Slave_App[pdrv].RxdHead < CS101_Slave_App[pdrv].RxdTail) // 接收缓冲区中有尚未处理的数据
    {
        if (CS101_Slave_App[pdrv].RxdHead != 0)
        {
            memcpy(CS101_Slave_App[pdrv].RxdBuf, CS101_Slave_App[pdrv].RxdBuf+CS101_Slave_App[pdrv].RxdHead, CS101_Slave_App[pdrv].RxdTail-CS101_Slave_App[pdrv].RxdHead);//将未处理的数据移到缓冲区头
            CS101_Slave_App[pdrv].RxdTail -= CS101_Slave_App[pdrv].RxdHead;
            CS101_Slave_App[pdrv].RxdHead = 0;
        }
    }
    else
    {
        CS101_Slave_App[pdrv].RxdHead = 0;
		CS101_Slave_App[pdrv].RxdTail = 0;
    }
    count = cs101SlaveReadData(pdrv,(uint8_t *)&CS101_Slave_App[pdrv].RxdBuf[CS101_Slave_App[pdrv].RxdTail], CS101_SLAVE_LPDUSIZE-CS101_Slave_App[pdrv].RxdTail);
    if (count > 0)
    {
        CS101_Slave_App[pdrv].RxdTail += count;
        printf("count = %d\r\n", count);
        cs101Slave_SearchValidFrame(pdrv);

        CS101_Slave_App[pdrv].TimeOutLink = CS101_Slave_Pad[pdrv].TimeOutLink;
    }
}

/**
  * @brief : Slave Frame 10 Packet.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_SlaveFrame10Packet(uint8_t pdrv, uint8_t Function)			//回复封装包
{
    uint8_t *pBuf;
    uint8_t i = 0;

    pBuf = CS101_Slave_App[pdrv].TxdBuf + CS101_Slave_App[pdrv].TxdTail;
    pBuf[0] = CS101_SLAVE_STARTCODE10;
    pBuf[1] = Function&0x0f;
    if(CS101_Slave_Pad[pdrv].BalanMode)
    {
        pBuf[1] |= CS101_Slave_Pad[pdrv].IEC_DIR; // 子站向主站传输，平衡模式为1，非平衡模式为0
    }
    else
    {
        if(CS101_Slave_App[pdrv].Data1Flag)
        {pBuf[1] |= CS101_SLAVE_ACD;}
    }

    for (i=0; i<CS101_Slave_Pad[pdrv].LinkAddrSize; i++)
    {
        pBuf[2+i] = (CS101_Slave_Pad[pdrv].SourceAddr>>(8*i)) & 0xff;
    }

    pBuf[2+CS101_Slave_Pad[pdrv].LinkAddrSize] = CS101_Slave_CKS(pdrv, pBuf);
    pBuf[3+CS101_Slave_Pad[pdrv].LinkAddrSize] = CS101_SLAVE_ENDCODE;
    CS101_Slave_App[pdrv].TxdTail += CS101_Slave_Pad[pdrv].FixFrmLength;//移动发送尾指针

    if(!CS101_Slave_Pad[pdrv].BalanMode)
    {
        memcpy(CS101_Slave_App[pdrv].TxdBuf_Pri, pBuf, CS101_Slave_Pad[pdrv].FixFrmLength); // 将发送数据保存到启动站重发数据区
        CS101_Slave_App[pdrv].TxdTail_Pri = CS101_Slave_Pad[pdrv].FixFrmLength;
    }
}

/**
  * @brief : Master Frame 10 Packet.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_MasterFrame10Packet(uint8_t pdrv, uint8_t Function)
{
    uint8_t *pBuf;
    uint8_t i = 0;

    pBuf = CS101_Slave_App[pdrv].TxdBuf + CS101_Slave_App[pdrv].TxdTail;
    pBuf[0] = CS101_SLAVE_STARTCODE10;
    pBuf[1] = Function&0x0f;
    pBuf[1] |= CS101_SLAVE_PRM;
    pBuf[1] |= CS101_Slave_Pad[pdrv].IEC_DIR; // 子站向主站传输，平衡模式为1，非平衡模式为0

    for (i=0; i<CS101_Slave_Pad[pdrv].LinkAddrSize; i++)
    {
        pBuf[2+i] = (CS101_Slave_Pad[pdrv].SourceAddr>>(8*i)) & 0xff;
    }

    pBuf[2+CS101_Slave_Pad[pdrv].LinkAddrSize] = CS101_Slave_CKS(pdrv, pBuf);
    pBuf[3+CS101_Slave_Pad[pdrv].LinkAddrSize] = CS101_SLAVE_ENDCODE;
    CS101_Slave_App[pdrv].TxdTail += CS101_Slave_Pad[pdrv].FixFrmLength;//移动发送尾指针

    memcpy(CS101_Slave_App[pdrv].TxdBuf_Pri, pBuf, CS101_Slave_Pad[pdrv].FixFrmLength); // 将发送数据保存到启动站重发数据区
    CS101_Slave_App[pdrv].TxdTail_Pri = CS101_Slave_Pad[pdrv].FixFrmLength;
}

/**
  * @brief : Link 10 Process.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_Link10Process(uint8_t pdrv)						//回复
{
    uint64_t timeMs = CS101_Slave_getAlarmOfMs(pdrv);
	if(CS101_Slave_App[pdrv].LinkFlag&CS101_SLAVE_REQSTATUS)			//收到10码 FC = 9
	{
		CS101_Slave_SlaveFrame10Packet(pdrv, CS101_SLAVE_S_FUN11);		//返回10码FC = 11
		CS101_Slave_WriteDataToMISI(pdrv);
		CS101_Slave_App[pdrv].LinkFlag &= (~CS101_SLAVE_REQSTATUS);
		return;
	}

	if(CS101_Slave_App[pdrv].LinkFlag&CS101_SLAVE_RECONFIRM)			//收到10码FC = 0
	{
		CS101_Slave_SlaveFrame10Packet(pdrv, CS101_SLAVE_S_FUN0);		//返回10码FC = 0
		CS101_Slave_WriteDataToMISI(pdrv);
		CS101_Slave_App[pdrv].LinkFlag &= (~CS101_SLAVE_RECONFIRM);
		return;
	}

	if(CS101_Slave_App[pdrv].LinkFlag&CS101_SLAVE_ASKSTATUS)
	{
		CS101_Slave_MasterFrame10Packet(pdrv, CS101_SLAVE_M_FUN9);
		CS101_Slave_WriteDataToMISI(pdrv);
		CS101_Slave_App[pdrv].LinkFlag &= (~CS101_SLAVE_ASKSTATUS);
		CS101_Slave_App[pdrv].TimeOutTick_AskSta = CS101_Slave_Pad[pdrv].AskStaOutValue;	/*重新及时*/
		CS101_Slave_App[pdrv].timeAskBegin = CS101_Slave_App[pdrv].timeAskEnd = timeMs;
		return;
	}
	if(CS101_Slave_App[pdrv].LinkFlag&CS101_SLAVE_ASKRELINK)
	{
		CS101_Slave_MasterFrame10Packet(pdrv, CS101_SLAVE_M_FUN0);
		CS101_Slave_WriteDataToMISI(pdrv);
		CS101_Slave_App[pdrv].LinkFlag &= (~CS101_SLAVE_ASKRELINK);
		return;
	}
}

/**
  * @brief : Rest Reply.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_RestReply(uint8_t pdrv)
{
    CS101_Slave_App[pdrv].TX_Temp.status.Length = sizeof(CS101_Slave_App[pdrv].TX_Temp.status) + sizeof(CS101_Slave_App[pdrv].TX_Temp.Head) + sizeof(CS101_Slave_App[pdrv].TX_Temp.Data.C_105);
    CS101_Slave_App[pdrv].TX_Temp.Head.TypeID = CS101_SLAVE_C_RP_NA_1;
    CS101_Slave_App[pdrv].TX_Temp.Head.VSQ = 1;
    CS101_Slave_App[pdrv].TX_Temp.Head.COT_L = CS101_SLAVE_COT_ACTCON;
    CS101_Slave_App[pdrv].TX_Temp.Head.PubAddr_H = (uint8_t)((CS101_Slave_Pad[pdrv].ASDUAddr>>8)&0xff);
    CS101_Slave_App[pdrv].TX_Temp.Head.PubAddr_L = (uint8_t)(CS101_Slave_Pad[pdrv].ASDUAddr&0xff);
    CS101_Slave_App[pdrv].TX_Temp.Data.C_105.QRP = 1;
}

/**
  * @brief : Init End Reply.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_InitEndReply(uint8_t pdrv)
{
    CS101_Slave_App[pdrv].TX_Temp.status.Length = sizeof(CS101_Slave_App[pdrv].TX_Temp.status) + sizeof(CS101_Slave_App[pdrv].TX_Temp.Head) + sizeof(CS101_Slave_App[pdrv].TX_Temp.Data.C_70);
    CS101_Slave_App[pdrv].TX_Temp.status.symbol.priority = CS101_SLAVE_M_EI_NA_P;
    CS101_Slave_App[pdrv].TX_Temp.Head.TypeID = CS101_SLAVE_M_EI_NA_1;
    CS101_Slave_App[pdrv].TX_Temp.Head.VSQ = 1;
    CS101_Slave_App[pdrv].TX_Temp.Head.COT_L = CS101_SLAVE_COT_INIT;
    CS101_Slave_App[pdrv].TX_Temp.Head.PubAddr_H = (uint8_t)((CS101_Slave_Pad[pdrv].ASDUAddr>>8)&0xff);
    CS101_Slave_App[pdrv].TX_Temp.Head.PubAddr_L = (uint8_t)(CS101_Slave_Pad[pdrv].ASDUAddr&0xff);
    CS101_Slave_App[pdrv].TX_Temp.Data.C_70.COI = 2;
}

/**
  * @brief : All Data Con Reply.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_AllDataConReply(uint8_t pdrv)			//总召唤回复
{
    CS101_Slave_App[pdrv].TX_Temp.status.Length = sizeof(CS101_Slave_App[pdrv].TX_Temp.status) + sizeof(CS101_Slave_App[pdrv].TX_Temp.Head) + sizeof(CS101_Slave_App[pdrv].TX_Temp.Data.C_100);
    CS101_Slave_App[pdrv].TX_Temp.status.symbol.priority = CS101_SLAVE_C_IC_NA_PF;
    CS101_Slave_App[pdrv].TX_Temp.Head.TypeID = CS101_SLAVE_C_IC_NA_1;
    CS101_Slave_App[pdrv].TX_Temp.Head.VSQ = 1;
    CS101_Slave_App[pdrv].TX_Temp.Head.COT_L = CS101_SLAVE_COT_ACTCON;
    CS101_Slave_App[pdrv].TX_Temp.Head.PubAddr_H = (uint8_t)((CS101_Slave_Pad[pdrv].ASDUAddr>>8)&0xff);
    CS101_Slave_App[pdrv].TX_Temp.Head.PubAddr_L = (uint8_t)(CS101_Slave_Pad[pdrv].ASDUAddr&0xff);
    CS101_Slave_App[pdrv].TX_Temp.Data.C_70.COI = 20;
}

/**
  * @brief : All Data End Reply.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_AllDataEndReply(uint8_t pdrv)			//总召唤结束回复
{
    CS101_Slave_App[pdrv].TX_Temp.status.Length = sizeof(CS101_Slave_App[pdrv].TX_Temp.status) + sizeof(CS101_Slave_App[pdrv].TX_Temp.Head) + sizeof(CS101_Slave_App[pdrv].TX_Temp.Data.C_100);
    CS101_Slave_App[pdrv].TX_Temp.status.symbol.priority = CS101_SLAVE_C_IC_NA_PF;
    CS101_Slave_App[pdrv].TX_Temp.Head.TypeID = CS101_SLAVE_C_IC_NA_1;
    CS101_Slave_App[pdrv].TX_Temp.Head.VSQ = 1;
    CS101_Slave_App[pdrv].TX_Temp.Head.COT_L = CS101_SLAVE_COT_ACCTTERM;
    CS101_Slave_App[pdrv].TX_Temp.Head.PubAddr_H = (uint8_t)((CS101_Slave_Pad[pdrv].ASDUAddr>>8)&0xff);
    CS101_Slave_App[pdrv].TX_Temp.Head.PubAddr_L = (uint8_t)(CS101_Slave_Pad[pdrv].ASDUAddr&0xff);
    CS101_Slave_App[pdrv].TX_Temp.Data.C_70.COI = 20;
}

/**
  * @brief : Read All Data Process.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_ReadAllDataProcess(uint8_t pdrv)//总召包组包
{
	switch(CS101_Slave_App[pdrv].AllDataFlag)			//激活确认
	{
		case 0:
            CS101_Slave_AllDataConReply(pdrv);
            CS101_Slave_App[pdrv].AllData_yx_sendaddr = CS101_Slave_Pad[pdrv].YX_FirstAddr;			//遥信赋发送地址
            CS101_Slave_App[pdrv].AllData_yc_sendaddr = CS101_Slave_Pad[pdrv].YC_FirstAddr;			//遥测赋发送地址
            CS101_Slave_App[pdrv].AllDataFlag = 1;
			break;
		case 1:
            if(CS101_Slave_Pad[pdrv].YX_AllNum != 0)	//遥信组包
            {//总召唤遥信
                CS101_Slave_App[pdrv].AllData_yx_sendaddr = CS101_Slave_R_YXDATA(pdrv ,CS101_Slave_App[pdrv].AllData_yx_sendaddr ,CS101_Slave_Pad[pdrv].YX_FrameNum ,(uint8_t *)&CS101_Slave_App[pdrv].TX_Temp);
                if(CS101_Slave_App[pdrv].AllData_yx_sendaddr >= CS101_Slave_Pad[pdrv].YX_FirstAddr + CS101_Slave_Pad[pdrv].YX_AllNum)
                {
                    CS101_Slave_App[pdrv].AllDataFlag = 2;
                }
                break;
            }
		case 2:
            if(CS101_Slave_Pad[pdrv].YC_AllNum != 0)	//遥测组包
            {//总召唤遥测
                CS101_Slave_App[pdrv].AllData_yc_sendaddr = CS101_Slave_R_YCDATA(pdrv ,CS101_Slave_App[pdrv].AllData_yc_sendaddr ,CS101_Slave_Pad[pdrv].YC_FrameNum ,(uint8_t *)&CS101_Slave_App[pdrv].TX_Temp);
                if(CS101_Slave_App[pdrv].AllData_yc_sendaddr >= CS101_Slave_Pad[pdrv].YC_FirstAddr + CS101_Slave_Pad[pdrv].YC_AllNum)
                {
                    CS101_Slave_App[pdrv].AllDataFlag = 3;
                }
                break;
            }
		case 3:
            CS101_Slave_App[pdrv].AllDataFlag = 0;		//总召结束
            CS101_Slave_AllDataEndReply(pdrv);
            CS101_Slave_App[pdrv].Data1Flag &= (~CS101_SLAVE_CALLALLDATA);
			break;
		default:
			break;

	}
}

/**
  * @brief : Data1 Process.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_Data1Process(uint8_t pdrv)
{
    uint64_t timeMs = CS101_Slave_getAlarmOfMs(pdrv);

    if (CS101_Slave_App[pdrv].Data1Flag & CS101_SLAVE_HAVERESET) // 复位进程
    {
//    	log_i("CS101_SLAVE_HAVERESET");
        CS101_Slave_RestReply(pdrv);
        CS101_Slave_App[pdrv].Data1Flag &= (~CS101_SLAVE_HAVERESET);
//        CS101_Slave_App[pdrv].LinkFlag = 0;
        return;
    }

    if ((CS101_Slave_App[pdrv].Data1Flag & CS101_SLAVE_HAVEINITEND)&&
			((CS101_Slave_App[pdrv].StoreDATA1IN->status.symbol.priority > CS101_SLAVE_M_EI_NA_P)||
			(CS101_Slave_App[pdrv].StoreDATA1IN->status.symbol.priority == 0))) // 初始化结束回复过程
    {
        CS101_Slave_InitEndReply(pdrv);											// 初始化结束回复TI = 70
        CS101_Slave_App[pdrv].Data1Flag &= (~CS101_SLAVE_HAVEINITEND);
        return;
    }

    if(CS101_Slave_App[pdrv].TxdTail_Special)//特殊数据重发
    {
//    	log_i("CS101_Slave_App[pdrv].TxdTail_Special");
        memcpy(CS101_Slave_App[pdrv].TxdBuf + CS101_Slave_App[pdrv].TxdTail, CS101_Slave_App[pdrv].TxdBuf_Special, CS101_Slave_App[pdrv].TxdTail_Special);
        CS101_Slave_App[pdrv].TxdTail += CS101_Slave_App[pdrv].TxdTail_Special;
        CS101_Slave_App[pdrv].TxdTail_Special = 0;
        memcpy(CS101_Slave_App[pdrv].TxdBuf_Pri, CS101_Slave_App[pdrv].TxdBuf_Special, CS101_Slave_App[pdrv].TxdTail_Special);
        CS101_Slave_App[pdrv].TxdTail_Pri = CS101_Slave_App[pdrv].TxdTail_Special;
        CS101_Slave_WriteDataToMISI(pdrv);
        CS101_Slave_App[pdrv].TimeOutTick_Pri = CS101_Slave_Pad[pdrv].TimeOutValue; /*重发重新计时*/
        CS101_Slave_App[pdrv].timePriBegin = CS101_Slave_App[pdrv].timePriEnd = timeMs;
        CS101_Slave_App[pdrv].RetryCount  = CS101_SLAVE_NUMOF_MAXRETRY;
        CS101_Slave_App[pdrv].LinkFlag &= (~CS101_SLAVE_SENDABLE);
        return;
    }

    if ((CS101_Slave_H_COS(pdrv))&&
        ((CS101_Slave_App[pdrv].StoreDATA1OUT->status.symbol.priority > CS101_SLAVE_M_SP_TB_P)||
        (CS101_Slave_App[pdrv].StoreDATA1OUT->status.symbol.priority == 0)))
    {
//    	log_i("CS101_Slave_H_COS");
        CS101_Slave_R_COS(pdrv,(uint8_t *)&CS101_Slave_App[pdrv].TX_Temp);
        CS101_Slave_App[pdrv].Data1Flag &= (~CS101_SLAVE_HAVECOS);
        return;
    }

    if ((CS101_Slave_H_SOE(pdrv))&&
        ((CS101_Slave_App[pdrv].StoreDATA1OUT->status.symbol.priority > CS101_SLAVE_M_SP_TB_P)||
        (CS101_Slave_App[pdrv].StoreDATA1OUT->status.symbol.priority == 0)))
    {
//    	log_i("CS101_Slave_H_SOE");
        CS101_Slave_R_SOE(pdrv,(uint8_t *)&CS101_Slave_App[pdrv].TX_Temp);
        CS101_Slave_App[pdrv].Data1Flag &= (~CS101_SLAVE_HAVESOE);
        return;
    }

    if ((CS101_Slave_App[pdrv].Data1Flag & CS101_SLAVE_CALLALLDATA)&&
        ((CS101_Slave_App[pdrv].StoreDATA1OUT->status.symbol.priority > CS101_SLAVE_M_SP_TB_P)||
        (CS101_Slave_App[pdrv].StoreDATA1OUT->status.symbol.priority == 0)))  // 召唤全数据
    {
    	//log_i("CS101_SLAVE_CALLALLDATA");
        CS101_Slave_ReadAllDataProcess(pdrv);
        return;
    }

    if(CS101_Slave_App[pdrv].StoreDATA1OUT != CS101_Slave_App[pdrv].StoreDATA1IN)
    {
    	// log_w("CS101_Slave_App[pdrv].StoreDATA1OUT != CS101_Slave_App[pdrv].StoreDATA1IN");
        memcpy((uint8_t *)&CS101_Slave_App[pdrv].TX_Temp, (uint8_t *)CS101_Slave_App[pdrv].StoreDATA1OUT, CS101_Slave_App[pdrv].StoreDATA1OUT->status.Length);
        memset((uint8_t *)CS101_Slave_App[pdrv].StoreDATA1OUT,0,sizeof(DLT634_5101SLAVE_PASDU));
        if(++CS101_Slave_App[pdrv].StoreDATA1OUT >= CS101_Slave_App[pdrv].StoreDATA1Buf + CS101_SLAVE_STOREDATA1NUM)
        {
            CS101_Slave_App[pdrv].StoreDATA1OUT = CS101_Slave_App[pdrv].StoreDATA1Buf;
        }
        return;
    }
}

/**
  * @brief : Data2 Process.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_Data2Process(uint8_t pdrv)
{
    uint64_t timeMs = CS101_Slave_getAlarmOfMs(pdrv);

    if(CS101_Slave_App[pdrv].TxdTail_Special)//特殊数据重发
    {
        memcpy(CS101_Slave_App[pdrv].TxdBuf + CS101_Slave_App[pdrv].TxdTail, CS101_Slave_App[pdrv].TxdBuf_Special, CS101_Slave_App[pdrv].TxdTail_Special);
        CS101_Slave_App[pdrv].TxdTail += CS101_Slave_App[pdrv].TxdTail_Special;
        memcpy(CS101_Slave_App[pdrv].TxdBuf_Pri, CS101_Slave_App[pdrv].TxdBuf_Special, CS101_Slave_App[pdrv].TxdTail_Special);
        CS101_Slave_App[pdrv].TxdTail_Pri = CS101_Slave_App[pdrv].TxdTail_Special;
        CS101_Slave_App[pdrv].TxdTail_Special = 0;
        CS101_Slave_WriteDataToMISI(pdrv);
        CS101_Slave_App[pdrv].TimeOutTick_Pri = CS101_Slave_Pad[pdrv].TimeOutValue; /*重发重新计时*/
        CS101_Slave_App[pdrv].timePriBegin = CS101_Slave_App[pdrv].timePriEnd = timeMs;
        CS101_Slave_App[pdrv].RetryCount  = CS101_SLAVE_NUMOF_MAXRETRY;
        CS101_Slave_App[pdrv].LinkFlag &= (~CS101_SLAVE_SENDABLE);
        return;
    }

    if ((CS101_Slave_H_NVA(pdrv))&&
        ((CS101_Slave_App[pdrv].StoreDATA2OUT->status.symbol.priority > CS101_SLAVE_M_ME_NC_P)||
        (CS101_Slave_App[pdrv].StoreDATA2OUT->status.symbol.priority == 0)))
    {
        CS101_Slave_R_NVA(pdrv,(uint8_t *)&CS101_Slave_App[pdrv].TX_Temp);
        return;
    }

    if ((CS101_Slave_H_FEvent(pdrv))&&
        ((CS101_Slave_App[pdrv].StoreDATA2OUT->status.symbol.priority > CS101_SLAVE_M_FT_NA_P)||
        (CS101_Slave_App[pdrv].StoreDATA2OUT->status.symbol.priority == 0)))
    {
        CS101_Slave_R_FEvent(pdrv,(uint8_t *)&CS101_Slave_App[pdrv].TX_Temp);
        return;
    }

    if(CS101_Slave_App[pdrv].StoreDATA2OUT != CS101_Slave_App[pdrv].StoreDATA2IN)
    {
        memcpy((uint8_t *)&CS101_Slave_App[pdrv].TX_Temp, (uint8_t *)CS101_Slave_App[pdrv].StoreDATA2OUT, CS101_Slave_App[pdrv].StoreDATA2OUT->status.Length);
        memset((uint8_t *)CS101_Slave_App[pdrv].StoreDATA2OUT,0,sizeof(DLT634_5101SLAVE_PASDU));
        if(++CS101_Slave_App[pdrv].StoreDATA2OUT >= CS101_Slave_App[pdrv].StoreDATA2Buf + CS101_SLAVE_STOREDATA2NUM)
        {
            CS101_Slave_App[pdrv].StoreDATA2OUT = CS101_Slave_App[pdrv].StoreDATA2Buf;
        }
        return;
    }

    CS101_Slave_R_IDLE(pdrv,(uint8_t *)&CS101_Slave_App[pdrv].TX_Temp);
}

/**
  * @brief : Master Frame 68 Packet.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_MasterFrame68Packet(uint8_t pdrv, uint8_t Function)
{
    uint16_t length = 0;
    uint8_t *pBuf;
    uint8_t num;

    pBuf = CS101_Slave_App[pdrv].TxdBuf + CS101_Slave_App[pdrv].TxdTail;

    pBuf[0] = pBuf[3] = CS101_SLAVE_STARTCODE68;
    pBuf[4] = Function&0x0f;
    pBuf[4] |= CS101_SLAVE_DIR;
    pBuf[4] |= CS101_SLAVE_PRM;
    CS101_Slave_App[pdrv].SendConCode = (CS101_SLAVE_FCB^CS101_Slave_App[pdrv].SendConCode);
    pBuf[4] |= CS101_Slave_App[pdrv].SendConCode;
    pBuf[4] |= CS101_SLAVE_FCV;

    num = 5;

    if(CS101_Slave_Pad[pdrv].LinkAddrSize == 1)
    {
        pBuf[num++] = CS101_Slave_Pad[pdrv].SourceAddr&0xff;
    }
    else
    {
        pBuf[num++] = CS101_Slave_Pad[pdrv].SourceAddr&0xff;
        pBuf[num++] = (CS101_Slave_Pad[pdrv].SourceAddr>>8)&0xff;
    }

    pBuf[num++] = CS101_Slave_App[pdrv].TX_Temp.Head.TypeID;
    pBuf[num++] = CS101_Slave_App[pdrv].TX_Temp.Head.VSQ;

    if(CS101_Slave_Pad[pdrv].ASDUCotSize == 1)
    {
        pBuf[num++] = CS101_Slave_App[pdrv].TX_Temp.Head.COT_L;
    }
    else
    {
        pBuf[num++] = CS101_Slave_App[pdrv].TX_Temp.Head.COT_L;
        pBuf[num++] = CS101_Slave_App[pdrv].TX_Temp.Head.COT_H;
    }

    if(CS101_Slave_Pad[pdrv].ASDUAddrSize == 1)
    {
        pBuf[num++] = CS101_Slave_Pad[pdrv].ASDUAddr&0xff;
    }
    else
    {
        pBuf[num++] = CS101_Slave_Pad[pdrv].ASDUAddr&0xff;
        pBuf[num++] = (CS101_Slave_Pad[pdrv].ASDUAddr>>8)&0xff;
    }

    memcpy(&pBuf[num], &CS101_Slave_App[pdrv].TX_Temp.Data, CS101_Slave_App[pdrv].TX_Temp.status.Length - sizeof(CS101_Slave_App[pdrv].TX_Temp.status) - sizeof(CS101_Slave_App[pdrv].TX_Temp.Head));
    num += CS101_Slave_App[pdrv].TX_Temp.status.Length - sizeof(CS101_Slave_App[pdrv].TX_Temp.status) - sizeof(CS101_Slave_App[pdrv].TX_Temp.Head);

    length = num + 2; // 全部报文长度
    pBuf[1] = pBuf[2] = length - 6;

    pBuf[length-2] = CS101_Slave_CKS(pdrv, pBuf)&0xFF;
    pBuf[length-1] = CS101_SLAVE_ENDCODE;
    CS101_Slave_App[pdrv].TxdTail += length;

    memcpy(CS101_Slave_App[pdrv].TxdBuf_Pri, pBuf, length);
    CS101_Slave_App[pdrv].TxdTail_Pri = length;
}

/**
  * @brief : Slave Frame 68 Packet.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_SlaveFrame68Packet(uint8_t pdrv, uint8_t Function)
{
    uint16_t length = 0;
    uint8_t *pBuf = 0;
    uint8_t num;

    pBuf = CS101_Slave_App[pdrv].TxdBuf + CS101_Slave_App[pdrv].TxdTail;

    pBuf[0] = pBuf[3] = CS101_SLAVE_STARTCODE68;
    pBuf[4] = Function&0x0f;
    pBuf[4] |= CS101_SLAVE_DIR;
    pBuf[4] |= CS101_SLAVE_PRM;

    num = 5;

    if(CS101_Slave_Pad[pdrv].LinkAddrSize == 1)
    {
        pBuf[num++] = CS101_Slave_Pad[pdrv].SourceAddr&0xff;
    }
    else
    {
        pBuf[num++] = CS101_Slave_Pad[pdrv].SourceAddr&0xff;
        pBuf[num++] = (CS101_Slave_Pad[pdrv].SourceAddr>>8)&0xff;
    }

    pBuf[num++] = CS101_Slave_App[pdrv].TX_Temp.Head.TypeID;
    pBuf[num++] = CS101_Slave_App[pdrv].TX_Temp.Head.VSQ;

    if(CS101_Slave_Pad[pdrv].ASDUCotSize == 1)
    {
        pBuf[num++] = CS101_Slave_App[pdrv].TX_Temp.Head.COT_L;
    }
    else
    {
        pBuf[num++] = CS101_Slave_App[pdrv].TX_Temp.Head.COT_L;
        pBuf[num++] = CS101_Slave_App[pdrv].TX_Temp.Head.COT_H;
    }

    if(CS101_Slave_Pad[pdrv].ASDUAddrSize == 1)
    {
        pBuf[num++] = CS101_Slave_Pad[pdrv].ASDUAddr&0xff;
    }
    else
    {
        pBuf[num++] = CS101_Slave_Pad[pdrv].ASDUAddr&0xff;
        pBuf[num++] = (CS101_Slave_Pad[pdrv].ASDUAddr>>8)&0xff;
    }

    memcpy(&pBuf[num], &CS101_Slave_App[pdrv].TX_Temp.Data, CS101_Slave_App[pdrv].TX_Temp.status.Length - sizeof(CS101_Slave_App[pdrv].TX_Temp.status) - sizeof(CS101_Slave_App[pdrv].TX_Temp.Head));
    num += CS101_Slave_App[pdrv].TX_Temp.status.Length - sizeof(CS101_Slave_App[pdrv].TX_Temp.status) - sizeof(CS101_Slave_App[pdrv].TX_Temp.Head);

    length = num + 2; // 全部报文长度
    pBuf[1] = pBuf[2] = length - 6;

    pBuf[length-2] = CS101_Slave_CKS(pdrv, pBuf)&0xFF;
    pBuf[length-1] = CS101_SLAVE_ENDCODE;
    CS101_Slave_App[pdrv].TxdTail += length;

    memcpy(CS101_Slave_App[pdrv].TxdBuf_Pri, pBuf, length);
    CS101_Slave_App[pdrv].TxdTail_Pri = length;
}

/**
  * @brief : Set Flag Process.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_SetFlagProcess(uint8_t pdrv)
{
    if(CS101_Slave_H_COS(pdrv))//soe
    {CS101_Slave_App[pdrv].Data1Flag |= CS101_SLAVE_HAVECOS;}
    if(CS101_Slave_H_SOE(pdrv))//soe
    {CS101_Slave_App[pdrv].Data1Flag |= CS101_SLAVE_HAVESOE;}
    if(CS101_Slave_App[pdrv].StoreDATA1OUT->status.Length)
    {CS101_Slave_App[pdrv].Data1Flag |= CS101_SLAVE_HAVESTDATA1;}
}

/**
  * @brief : Link 68 Process.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_Link68Process(uint8_t pdrv)
{
    uint64_t timeMs = CS101_Slave_getAlarmOfMs(pdrv);
    CS101_Slave_SetFlagProcess(pdrv);

    if(CS101_Slave_App[pdrv].TX_Temp.status.Length)			//ASDU长度
    {
        if(CS101_Slave_Pad[pdrv].BalanMode)
        {
            CS101_Slave_MasterFrame68Packet(pdrv, CS101_SLAVE_M_FUN3);
            CS101_Slave_WriteDataToMISI(pdrv);
            CS101_Slave_App[pdrv].TimeOutTick_Pri = CS101_Slave_Pad[pdrv].TimeOutValue; /*重发重新计时*/
            CS101_Slave_App[pdrv].timePriBegin = CS101_Slave_App[pdrv].timePriEnd = timeMs;
            CS101_Slave_App[pdrv].RetryCount  = CS101_SLAVE_NUMOF_MAXRETRY;
            CS101_Slave_App[pdrv].LinkFlag &= (~CS101_SLAVE_SENDABLE);
        }
        else
        {
            CS101_Slave_SlaveFrame68Packet(pdrv, CS101_SLAVE_M_FUN8);
            CS101_Slave_WriteDataToMISI(pdrv);
        }
        memset((uint8_t *)&CS101_Slave_App[pdrv].TX_Temp,0,sizeof(DLT634_5101SLAVE_PASDU));
    }
    else
    {
        if (!CS101_Slave_Pad[pdrv].BalanMode)
        {
            if(CS101_SLAVE_E5)
            {
                *(CS101_Slave_App[pdrv].TxdBuf + CS101_Slave_App[pdrv].TxdTail) = 0xE5;
                CS101_Slave_App[pdrv].TxdTail += 1;
            }
            else
            {
                CS101_Slave_SlaveFrame10Packet(pdrv, CS101_SLAVE_S_FUN9);		//呼吸报文
            }
            CS101_Slave_WriteDataToMISI(pdrv);
        }
    }
}

/**
  * @brief : Send Process.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_SendProcess(uint8_t pdrv)
{
	if(CS101_Slave_Pad[pdrv].Encrypt)
	{
		if(CS101_Slave_H_Encrypt(pdrv))					//加密模块
		{return;}
	}

    if(CS101_Slave_App[pdrv].LinkFlag&0x000000ff)
    {
        CS101_Slave_Link10Process(pdrv);				//10包数据
        return;
    }

    if(CS101_Slave_Pad[pdrv].BalanMode)									//平衡
    {
        if(CS101_Slave_App[pdrv].LinkFlag&CS101_SLAVE_SENDABLE)
        {
            CS101_Slave_Data1Process(pdrv);
            if(CS101_Slave_App[pdrv].TX_Temp.status.Length == 0)
            {CS101_Slave_Data2Process(pdrv);}
            CS101_Slave_Link68Process(pdrv);
            return;
        }
    }
    else																//非平衡
    {
        if(CS101_Slave_App[pdrv].LinkFlag&CS101_SLAVE_REDATA1)			//1级数据
        {
            CS101_Slave_Data1Process(pdrv);
            CS101_Slave_Link68Process(pdrv);							//68包数据
        }
        else if(CS101_Slave_App[pdrv].LinkFlag&CS101_SLAVE_REDATA2)		//2级数据
        {
            CS101_Slave_Data2Process(pdrv);
            if(CS101_Slave_App[pdrv].TX_Temp.status.Length == 0)
            {CS101_Slave_Data1Process(pdrv);}
            CS101_Slave_Link68Process(pdrv);
        }
        CS101_Slave_App[pdrv].LinkFlag &= (~(CS101_SLAVE_REDATA1|CS101_SLAVE_REDATA2));
        return;
    }
}

/**
  * @brief : Link On Timer.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS101_Slave_LinkOnTimer(uint8_t pdrv)
{
    static uint8_t resetStatus = 1;
	uint64_t timeOut;
    uint64_t timeMs = CS101_Slave_getAlarmOfMs(pdrv);

    if (CS101_Slave_App[pdrv].TimeOutTick_Pri) // 单位是秒
    {
    	CS101_Slave_App[pdrv].timePriEnd = timeMs;
    	timeOut = CS101_Slave_App[pdrv].timePriEnd - CS101_Slave_App[pdrv].timePriBegin;
        //CS101_Slave_App[pdrv].TimeOutTick_Pri--;
        //if (!CS101_Slave_App[pdrv].TimeOutTick_Pri)
    	if(timeOut >= CS101_Slave_Pad[pdrv].AskStaOutValue)
        {
            if (!CS101_Slave_App[pdrv].RetryCount)
            {
                return;
            }

            CS101_Slave_App[pdrv].RetryCount--;
            if (CS101_Slave_App[pdrv].RetryCount)
            {
                memcpy(CS101_Slave_App[pdrv].TxdBuf + CS101_Slave_App[pdrv].TxdTail, CS101_Slave_App[pdrv].TxdBuf_Pri, CS101_Slave_App[pdrv].TxdTail_Pri);
                CS101_Slave_App[pdrv].TxdTail += CS101_Slave_App[pdrv].TxdTail_Pri;

                CS101_Slave_WriteDataToMISI(pdrv);
                CS101_Slave_App[pdrv].TimeOutTick_Pri = CS101_Slave_Pad[pdrv].TimeOutValue; /*重发重新计时*/
                CS101_Slave_App[pdrv].timePriBegin = CS101_Slave_App[pdrv].timePriEnd = timeMs;
                return;
            }
            else // 超过重发次数后，重新建立链路，并通知应用层。
            {
                CS101_Slave_App[pdrv].LinkFlag = 0;
                CS101_Slave_App[pdrv].TimeOutTick_AskCount = CS101_SLAVE_NUMOF_MAXRETRY;
                CS101_Slave_App[pdrv].TimeOutTick_AskSta = CS101_Slave_Pad[pdrv].AskStaOutValue;	/*重新计时*/
                CS101_Slave_App[pdrv].timeAskBegin = CS101_Slave_App[pdrv].timeAskEnd = timeMs;

                CS101_Slave_App[pdrv].LinkFlag |= CS101_SLAVE_ASKSTATUS;
                if (CS101_Slave_App[pdrv].TimeOutTick_AskCount)
                {
                    CS101_Slave_App[pdrv].TimeOutTick_AskCount--;
                }
                if(CS101_Slave_App[pdrv].TxdBuf_Pri[5+CS101_Slave_Pad[pdrv].LinkAddrSize]& (CS101_SLAVE_M_SP_TB_1|CS101_SLAVE_M_DP_TB_1))
                {
                    CS101_Slave_App[pdrv].TxdTail_Special= CS101_Slave_App[pdrv].TxdTail_Pri;
                    memcpy(CS101_Slave_App[pdrv].TxdBuf_Special, CS101_Slave_App[pdrv].TxdBuf_Pri, CS101_Slave_App[pdrv].TxdTail_Pri);
                }
            }
        }
    }

    if(CS101_Slave_App[pdrv].TimeOutTick_AskSta)
    {
    	CS101_Slave_App[pdrv].timeAskEnd = timeMs;
    	timeOut = CS101_Slave_App[pdrv].timeAskEnd - CS101_Slave_App[pdrv].timeAskBegin;

    	//CS101_Slave_App[pdrv].TimeOutTick_AskSta--;
        //if (!CS101_Slave_App[pdrv].TimeOutTick_AskSta)
    	if(timeOut >= CS101_Slave_Pad[pdrv].AskStaOutValue)
        {
        	CS101_Slave_App[pdrv].TimeOutTick_AskSta = 0;	/*停止计时*/
            if (!CS101_Slave_App[pdrv].TimeOutTick_AskCount)
            {
                return;
            }
            if((!(CS101_Slave_App[pdrv].LinkFlag&CS101_SLAVE_INITEND))&&(CS101_Slave_Pad[pdrv].BalanMode))
            {
                CS101_Slave_App[pdrv].LinkFlag |= CS101_SLAVE_ASKSTATUS;
                CS101_Slave_App[pdrv].TimeOutTick_AskCount--;
            }
        }
    }

    if(CS101_Slave_App[pdrv].TimeOutLink)
    {
        CS101_Slave_App[pdrv].TimeOutLink--;
    }

    if(getDzPreTimeOut101(pdrv) == TRUE)/*定值预制超时*/
    {
        CS101_Slave_DZ_RevokeOperation(pdrv);
    }

    if(CS101_Slave_Pad[pdrv].resetFlag && resetStatus)//复位计时
    {
        CS101_Slave_App[pdrv].timeResetBegin = timeMs;
        CS101_Slave_App[pdrv].timeResetEnd = timeMs;
        resetStatus = 0;
    }
    if(CS101_Slave_Pad[pdrv].resetFlag || CS101_Slave_Pad[pdrv].updateFlag)
    {
        CS101_Slave_App[pdrv].timeResetEnd = timeMs;
        timeOut = CS101_Slave_App[pdrv].timeResetEnd - CS101_Slave_App[pdrv].timeResetBegin;
        if(CS101_Slave_Pad[pdrv].updateFlag && CS101_Slave_Pad[pdrv].Encrypt)//加密升级结束
        {
            if(timeOut >= CS101_Slave_Pad[pdrv].TimeOutReset/2)
            {
//                Encrypt_ReturnMd5Fault_IN();//上送错误校验
//                if(!Encrypt_UPvalue_Read(pdrv))
//                {
//                    CS101_Slave_Pad[pdrv].resetFlag = 0;
//                    resetStatus = 1;
//                }
                CS101_Slave_Pad[pdrv].updateFlag = 0;
            }
        }
    	if(timeOut >= CS101_Slave_Pad[pdrv].TimeOutReset)//超时 停止进程
        {
            CS101_Slave_Pad[pdrv].resetFlag = 0;
            CS101_Slave_Pad[pdrv].updateFlag = 0;
            CS101_Slave_exit(pdrv);
        }
    }
}

/* PUBLIC FUNCTION PROTOTYPES -----------------------------------------------*/
/**
  * @brief : SLAVE_REPLY.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS101_Slave_Reply(uint8_t pdrv, uint8_t *pbuf)				//回复
{
	uint8_t res = FALSE;

	memcpy(&CS101_Slave_App[pdrv].ST_Temp, pbuf,CS101_SLAVE_LPDUSIZE);
	switch (CS101_Slave_App[pdrv].ST_Temp.Head.TypeID)
	{
        //控制方向过程信息
        case CS101_SLAVE_C_SC_NA_1: // 单点遥控命令
        case CS101_SLAVE_C_SC_NB_1: // 双点遥控命令
            CS101_Slave_App[pdrv].ST_Temp.status.symbol.priority = CS101_SLAVE_C_SC_NA_P;
            res = CS101_Slave_StoreIN(pdrv,&CS101_Slave_App[pdrv].ST_Temp);
                break;

        case CS101_SLAVE_C_CS_NA_1: // 对时命令
            CS101_Slave_App[pdrv].ST_Temp.status.symbol.priority = CS101_SLAVE_C_CS_NA_P;
            res = CS101_Slave_StoreIN(pdrv,&CS101_Slave_App[pdrv].ST_Temp);
                break;

        case CS101_SLAVE_C_RP_NA_1: // 复位进程命令
                break;

        case CS101_SLAVE_C_RR_NA_1: // 读定值区号
        case CS101_SLAVE_C_RS_NA_1: // 读参数和定值命令
            CS101_Slave_App[pdrv].ST_Temp.status.symbol.priority = CS101_SLAVE_C_IC_NA_P;
//            log_w(" DZ CS101_Slave_StoreIN");
            res = CS101_Slave_StoreIN(pdrv,&CS101_Slave_App[pdrv].ST_Temp);
//            log_w(" DZ res:%d",res);
                break;

        case CS101_SLAVE_C_SR_NA_1: // 切换定值区
        case CS101_SLAVE_C_WS_NA_1: // 写参数和定值命令
            CS101_Slave_App[pdrv].ST_Temp.status.symbol.priority = CS101_SLAVE_C_SC_NA_P;
            res = CS101_Slave_StoreIN(pdrv,&CS101_Slave_App[pdrv].ST_Temp);
                break;

        case CS101_SLAVE_F_FR_NA_1: // 文件传输
            CS101_Slave_App[pdrv].ST_Temp.status.symbol.priority = CS101_SLAVE_F_FW_NA_P;
            res = CS101_Slave_StoreIN(pdrv,&CS101_Slave_App[pdrv].ST_Temp);
                break;

        case CS101_SLAVE_F_SR_NA_1: // 软件升级
            CS101_Slave_App[pdrv].ST_Temp.status.symbol.priority = CS101_SLAVE_F_FW_NA_P;
            res = CS101_Slave_StoreIN(pdrv,&CS101_Slave_App[pdrv].ST_Temp);
                break;

        default: // 类型标识有错误或不支持
                break;
	}
	return(res);
}

/**
  * @brief : app初始化.
  * @param : [void]
  * @return: void
  * @updata: [2018-11-09][Chen][newly increased]
  */
uint8_t CS101_Slave_Appinit(void)
{
	if (CS101_Slave_App == NULL)
	{
		CS101_Slave_App = malloc(sizeof(sCS101_Slave_AppInfo) * CS101_SLAVE_DISK_VOLUMES);
        memset (CS101_Slave_App, 0, sizeof (sCS101_Slave_AppInfo) * CS101_SLAVE_DISK_VOLUMES);
	}

    return(TRUE);
}

/**
  * @brief : 101数据交互函数.
  * @param : [void]
  * @return: void
  * @updata: [2018-11-09][Chen][newly increased]
  */
uint8_t CS101_Slave_Clock(uint8_t pdrv)
{
	/* 查询MISI接收缓冲区*/
	CS101_Slave_ReadMISIData(pdrv);
	/* 发送处理*/
	CS101_Slave_SendProcess(pdrv);
	/* 链路层定时器*/
	CS101_Slave_LinkOnTimer(pdrv);
    
    /* 返回链路状态*/
    return 0;
}


/* END OF FILE ----------------------------------------------------------------*/
