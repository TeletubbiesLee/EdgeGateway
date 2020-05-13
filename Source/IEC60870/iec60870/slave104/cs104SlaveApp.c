/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      cs104SlaveApp.c
  * @brief:		104app交互处理
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2018-11-09
  * @update:    [2018-11-09][Chen][newly increased]
  */
#define LOG_TAG    "CS104_Slave_App "
/* INCLUDE FILES -------------------------------------------------------------*/
#include "cs104SlaveApp.h"
#include "cs104SlaveDisk.h"
#include "../common/common.h"
//#include "encrypt_disk.h"
#include "string.h"
#include <time.h>
#include <stdlib.h>
//#include "elog.h"
//#include "channel_monitor.h"
/* PRIVATE VARIABLES ----------------------------------------------------------*/

/* PUBLIC VARIABLES ----------------------------------------------------------*/
struct sCS104_Slave_Pad CS104_Slave_Pad[CS104_SLAVE_VOLUMES]; // DL/T634_5101规约参数
struct CS104_SLAVE_LINKINFO *CS104SlaveLink = NULL;//[CS104_SLAVE_VOLUMES]; // 链路层变量
struct CS104_SLAVE_APPINFO *CS104SlaveApp = NULL;//[CS104_SLAVE_VOLUMES] __attribute__((at(0x00002000 + SDRAM_ADDR_104)));

/* PRIVATE FUNCTION PROTOTYPES -----------------------------------------------*/
/**
  * @brief : Set Timer.
  * @param : [pdrv]
  * @param : [No]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_Slave_SetTimer(uint8_t pdrv, uint8_t No)
{
    uint8_t	i = 0;
    uint64_t timeMs = CS104_SLAVE_getAlarmOfMs(pdrv);
    switch (No)
    {
        case 0:
        	CS104SlaveLink[pdrv].Tick.timeBeginT0  = timeMs;
            CS104SlaveLink[pdrv].Tick.Flag |= CS104_SLAVE_T0FLAG;		//T0时间标志置位
            break;
            
        case 1:
            for (i = 0; i < CS104_SLAVE_K; i++)
            {
                CS104SlaveLink[pdrv].Tick.timeBeginT1[i] = timeMs;
            }
            CS104SlaveLink[pdrv].Tick.Flag |= CS104_SLAVE_T1FLAG;		//T1时间标志置位
            break;
            
        case 2:
            CS104SlaveLink[pdrv].Tick.timeBeginT2 = timeMs;
            CS104SlaveLink[pdrv].Tick.Flag |= CS104_SLAVE_T2FLAG;		//T2时间标志置位
            break;
            
        case 3:
            CS104SlaveLink[pdrv].Tick.timeBeginT3 = timeMs;
            CS104SlaveLink[pdrv].Tick.Flag |= CS104_SLAVE_T3FLAG;		//T3时间标志置位
            break;
        
        default:
            break;
    }
}

/**
  * @brief : Kill Timer.
  * @param : [pdrv]
  * @param : [No]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_Slave_KillTimer(uint8_t pdrv, uint8_t No)
{
    switch (No)
    {
        case 0:
            CS104SlaveLink[pdrv].Tick.Flag &= (~CS104_SLAVE_T0FLAG);
            break;
            
        case 1:
            CS104SlaveLink[pdrv].Tick.Flag &= (~CS104_SLAVE_T1FLAG);
            break;
            
        case 2:
            CS104SlaveLink[pdrv].Tick.Flag &= (~CS104_SLAVE_T2FLAG);
            break;
            
        case 3:
            CS104SlaveLink[pdrv].Tick.Flag &= (~CS104_SLAVE_T3FLAG);
            break;
            
        default:
            break;
    }
}

/**
  * @brief : Kill Timer.
  * @param : [pdrv]
  * @param : [LastNoAckNum]
  * @param : [NowNoAckNum]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS104_SLAVE_BackupProcess(uint8_t pdrv, uint16_t LastNoAckNum, uint16_t NowNoAckNum)
{
    uint16_t num;
    struct CS104_SLAVE_APPINFO *pMsg;

    pMsg = &CS104SlaveApp[pdrv];
    num = LastNoAckNum - NowNoAckNum; // 本次被确认的数量
    memcpy(pMsg->SData, &pMsg->SData[num], sizeof(struct CS104_SLAVE_SendData) * LastNoAckNum);
    pMsg->CurSData = (pMsg->CurSData - num) >= 0 ? pMsg->CurSData - num : 0;
	pMsg->SDataIn  = (pMsg->SDataIn - num ) >= 0 ? pMsg->SDataIn  - num : 0;
}
/**
  * @brief : Peer No Ack Num Process.
  * @param : [pdrv]
  * @param : [PeerNR]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static uint8_t CS104_SLAVE_PeerNoAckNumProcess(uint8_t pdrv, uint16_t PeerNR)
{
    uint16_t i = 0;
    uint16_t num = 0;
    uint64_t timeMs = CS104_SLAVE_getAlarmOfMs(pdrv);
    // static uint8_t flag = 1;
    // if(flag)
    // {
    //     for (i = 0; i < CS104_SLAVE_K*2; i++)
    //     {
    //         CS104SlaveLink[pdrv].Tick.timeBeginT1[i] = timeMs;
    //     }
    //     flag = 0;
    // }

    if (PeerNR <= CS104SlaveLink[pdrv].NS)
    {
        num = CS104SlaveLink[pdrv].NS - PeerNR;
    }
    else
    {
        num = 32767 + CS104SlaveLink[pdrv].NS - PeerNR;
    }
    
//    log_w("NS:%d(%x) NR:%d(%x)",CS104SlaveLink[pdrv].NS,CS104SlaveLink[pdrv].NS*2,PeerNR,PeerNR*2);

    if (num > CS104SlaveLink[pdrv].PeerNoAckNum)
    {
//        log_e("num(%d) > CS104SlaveLink[pdrv].PeerNoAckNum(%d)",num,CS104SlaveLink[pdrv].PeerNoAckNum);
        CS104SlaveLink[pdrv].NR = 0;
        CS104SlaveLink[pdrv].NS = 0;
        CS104SlaveLink[pdrv].NRACK = 0;
        CS104SlaveLink[pdrv].PeerNoAckNum = 0;
        num = 0;
//      return (FALSE);
    }
    CS104SlaveLink[pdrv].PeerNoAckNum = num;
    
    if(num >= CS104_SLAVE_K)
    {
//        log_e("XXXXXXXXXXXXXX num:%d XXXXXXXXXXXX",num);
//        log_e("NS:%d(%x) NR:%d(%x)",CS104SlaveLink[pdrv].NS,CS104SlaveLink[pdrv].NS*2,PeerNR,PeerNR*2);
        return (FALSE);
    }
    else
    {
        for (i = 0; i < num; i++)
        {
            CS104SlaveLink[pdrv].Tick.timeBeginT1[i] = CS104SlaveLink[pdrv].Tick.timeBeginT1[num+i];//未被确定的,不需重新计时
        }
        for (i = num; i < 24; i++)
        {
            CS104SlaveLink[pdrv].Tick.timeBeginT1[i] = timeMs;//已经被确认的重新计时
        }
    }

    return (TRUE);
}

/**
  * @brief : Store IN.
  * @param : [pdrv]
  * @param : [buf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static uint8_t CS104_SLAVE_StoreIN(uint8_t pdrv, CS104_SLAVE_PASDU *buf)
{		
	CS104_SLAVE_PASDU *temp;
	CS104_SLAVE_PASDU tempbuf;
	
	if(buf->status.symbol.priority<CS104_SLAVE_M_FT_NA_P)
	{
		temp = CS104SlaveApp[pdrv].StoreDATA1IN;
		if(++temp >= CS104SlaveApp[pdrv].StoreDATA1Buf + CS104_SLAVE_STOREDATA1NUM)
		{
			temp = CS104SlaveApp[pdrv].StoreDATA1Buf;
		}
		
		if(temp == CS104SlaveApp[pdrv].StoreDATA1OUT)
		{
			return(FALSE);
		}
	
		temp = CS104SlaveApp[pdrv].StoreDATA1OUT;
		while(temp != CS104SlaveApp[pdrv].StoreDATA1IN)
		{
			if(((temp->status.symbol.priority > buf->status.symbol.priority)||(temp->status.symbol.priority == 0))&&(temp != CS104SlaveApp[pdrv].StoreDATA1OUT))
			{
				memcpy(&tempbuf,temp,sizeof(CS104_SLAVE_PASDU));
				memcpy(temp,buf,sizeof(CS104_SLAVE_PASDU));
				memcpy(buf,&tempbuf,sizeof(CS104_SLAVE_PASDU));
			}
			if(++temp >= CS104SlaveApp[pdrv].StoreDATA1Buf + CS104_SLAVE_STOREDATA1NUM)
			{
				temp = CS104SlaveApp[pdrv].StoreDATA1Buf;
			}
		}
		
		memcpy(temp,buf,sizeof(CS104_SLAVE_PASDU));
		if(++CS104SlaveApp[pdrv].StoreDATA1IN >= CS104SlaveApp[pdrv].StoreDATA1Buf + CS104_SLAVE_STOREDATA1NUM)
		{
			CS104SlaveApp[pdrv].StoreDATA1IN = CS104SlaveApp[pdrv].StoreDATA1Buf;
		}
	}
	else
	{
		temp = CS104SlaveApp[pdrv].StoreDATA2IN;
		if(++temp >= CS104SlaveApp[pdrv].StoreDATA2Buf + CS104_SLAVE_STOREDATA2NUM)
		{
			temp = CS104SlaveApp[pdrv].StoreDATA2Buf;
		}
		
		if(temp == CS104SlaveApp[pdrv].StoreDATA2OUT)
		{
			return(FALSE);
		}
		
		temp = CS104SlaveApp[pdrv].StoreDATA2OUT;
		while(temp != CS104SlaveApp[pdrv].StoreDATA2IN)
		{
			if(((temp->status.symbol.priority > buf->status.symbol.priority)||(temp->status.symbol.priority == 0))&&(temp != CS104SlaveApp[pdrv].StoreDATA2OUT))
			{
				memcpy(&tempbuf,temp,sizeof(CS104_SLAVE_PASDU));
				memcpy(temp,buf,sizeof(CS104_SLAVE_PASDU));
				memcpy(buf,&tempbuf,sizeof(CS104_SLAVE_PASDU));
			}
			if(++temp >= CS104SlaveApp[pdrv].StoreDATA2Buf + CS104_SLAVE_STOREDATA2NUM)
			{
				temp = CS104SlaveApp[pdrv].StoreDATA2Buf;
			}
		}
		
		memcpy(temp,buf,sizeof(CS104_SLAVE_PASDU));
		if(++CS104SlaveApp[pdrv].StoreDATA2IN >= CS104SlaveApp[pdrv].StoreDATA2Buf + CS104_SLAVE_STOREDATA2NUM)
		{
			CS104SlaveApp[pdrv].StoreDATA2IN = CS104SlaveApp[pdrv].StoreDATA2Buf;
		}
	}
	return(TRUE);
}

/**
  * @brief : App Init Reset.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS104_AppInitReset(uint8_t pdrv)
{
    CS104SlaveApp[pdrv].AllDataFlag = FALSE;
    CS104SlaveApp[pdrv].AppNextFlag = FALSE;
	CS104SlaveApp[pdrv].Data1Flag = 0;
    CS104SlaveApp[pdrv].SDataEn = 1;
    CS104SlaveApp[pdrv].SDataOut = 0;
	
    // Link
    CS104SlaveLink[pdrv].Connect = FALSE;
    CS104SlaveLink[pdrv].NR = 0;
    CS104SlaveLink[pdrv].NS = 0;
    CS104SlaveLink[pdrv].NRACK = 0;
    CS104SlaveLink[pdrv].PeerNoAckNum = 0;
    CS104SlaveLink[pdrv].FrameHead = 0;
    CS104SlaveLink[pdrv].TxdHead = 0;
    CS104SlaveLink[pdrv].TxdTail = 0;
	CS104SlaveLink[pdrv].StopSendFlag = FALSE; // 清停止发送标志
	
    memset(CS104SlaveLink[pdrv].TxdBuf, 0, sizeof(CS104SlaveLink[pdrv].RxdBuf)); // 清空接收缓冲区
    memset(CS104SlaveLink[pdrv].RxdBuf, 0, sizeof(CS104SlaveLink[pdrv].TxdBuf)); // 清空发送缓冲区

    CS104SlaveApp[pdrv].StoreDATA1IN = CS104SlaveApp[pdrv].StoreDATA1Buf;
    CS104SlaveApp[pdrv].StoreDATA1OUT = CS104SlaveApp[pdrv].StoreDATA1Buf;
    memset(CS104SlaveApp[pdrv].StoreDATA1Buf,0,sizeof(CS104SlaveApp[pdrv].StoreDATA1Buf));
    CS104SlaveApp[pdrv].StoreDATA2IN = CS104SlaveApp[pdrv].StoreDATA2Buf;
    CS104SlaveApp[pdrv].StoreDATA2OUT = CS104SlaveApp[pdrv].StoreDATA2Buf;
    memset(CS104SlaveApp[pdrv].StoreDATA2Buf,0,sizeof(CS104SlaveApp[pdrv].StoreDATA2Buf));

    return (TRUE);
}

/**
  * @brief : App Process.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS104_SLAVE_AppProcess(uint8_t pdrv, struct CS104_SLAVE_AppMsg* Msg)
{	
    CS104SlaveApp[pdrv].TxLen = 0;
    
//    CS104SlaveApp[pdrv].RxMsg = (struct CS104_SLAVE_PASDU_104*)Msg->pData;
//    memcpy((uint8_t *)&CS104SlaveApp[pdrv].RxMsg.Head,Msg->pData, Msg->Len);
//  这里将104报文转为02版104报文	
	memset((uint8_t *)&CS104SlaveApp[pdrv].RX_Temp,0,sizeof(CS104_SLAVE_PASDU));
	memcpy((uint8_t *)&CS104SlaveApp[pdrv].RX_Temp.Head,Msg->pData, Msg->Len);
    CS104SlaveApp[pdrv].RX_Temp.status.Length = Msg->Len + sizeof(CS104SlaveApp[pdrv].RX_Temp.status);

	switch (Msg->Cmd)
    {
        case CS104_SLAVE_LINK_CALLDATA:
            if((CS104SlaveApp[pdrv].RX_Temp.Head.PubAddr_L|(CS104SlaveApp[pdrv].RX_Temp.Head.PubAddr_H<<8)) == CS104_Slave_Pad[pdrv].PubAddress)
            {
                switch (CS104SlaveApp[pdrv].RX_Temp.Head.TypeID) // 具体类型标识处理
                {
                    case CS104_SLAVE_C_SC_NA_1:	// <45>单点遥控命令
                    case CS104_SLAVE_C_SC_NB_1:	// <46>双点遥控命令
                        CS104_SLAVE_HandleCtrlProcess(pdrv, (uint8_t *)&CS104SlaveApp[pdrv].RX_Temp);
                        break;                        
                            
                    case CS104_SLAVE_C_IC_NA_1:	// <100>总召唤或分组召唤
                        if((CS104SlaveApp[pdrv].RX_Temp.Data.C_100.InfoAddr_H << 16 |
                            CS104SlaveApp[pdrv].RX_Temp.Data.C_100.InfoAddr_M <<8 |
                            CS104SlaveApp[pdrv].RX_Temp.Data.C_100.InfoAddr_L) != 0)		//判断信息体地址是否为0,0代表总召
                        {
                            CS104SlaveApp[pdrv].RX_Temp.status.symbol.priority = CS104_SLAVE_C_IC_NA_PF;
                            CS104SlaveApp[pdrv].RX_Temp.Head.COT_L = CS104_SLAVE_COT_ADDRERR;
                            CS104_SLAVE_StoreIN(pdrv,&CS104SlaveApp[pdrv].RX_Temp);
                        }
                        else if(!((CS104SlaveApp[pdrv].RX_Temp.Head.COT_L|(CS104SlaveApp[pdrv].RX_Temp.Head.COT_H<<8)) == CS104_SLAVE_COT_ACT ||
                            (CS104SlaveApp[pdrv].RX_Temp.Head.COT_L|(CS104SlaveApp[pdrv].RX_Temp.Head.COT_H<<8)) == CS104_SLAVE_COT_DEACT))	//传输原因不为 6或8
                        {
                            CS104SlaveApp[pdrv].RX_Temp.status.symbol.priority = CS104_SLAVE_C_IC_NA_PF;
                            CS104SlaveApp[pdrv].RX_Temp.Head.COT_L = CS104_SLAVE_COT_COTERR;
                            CS104_SLAVE_StoreIN(pdrv,&CS104SlaveApp[pdrv].RX_Temp);
                        }
                        else		//总召进入
                        {
                            if((CS104SlaveApp[pdrv].RX_Temp.Head.COT_L|(CS104SlaveApp[pdrv].RX_Temp.Head.COT_H<<8)) == CS104_SLAVE_COT_ACT)	//总召激活
                            {
                                CS104SlaveApp[pdrv].Data1Flag |= CS104_SLAVE_CALLALLDATA;
                                CS104SlaveApp[pdrv].AllDataFlag = 0;
                                CS104_SLAVE_CallAllDataProcess(pdrv,(uint8_t *)&CS104SlaveApp[pdrv].RX_Temp);
                            }
                            else if((CS104SlaveApp[pdrv].RX_Temp.Head.COT_L|(CS104SlaveApp[pdrv].RX_Temp.Head.COT_H<<8)) == CS104_SLAVE_COT_DEACT)//总召停止激活
                            {
                                CS104SlaveApp[pdrv].Data1Flag &= (~CS104_SLAVE_CALLALLDATA);
                                CS104SlaveApp[pdrv].AllDataFlag = 0;
                                CS104SlaveApp[pdrv].RX_Temp.status.symbol.priority = CS104_SLAVE_M_EI_NA_P;
                                CS104SlaveApp[pdrv].RX_Temp.Head.COT_L = CS104_SLAVE_COT_DEACTCON;
                                CS104_SLAVE_StoreIN(pdrv,&CS104SlaveApp[pdrv].RX_Temp);
                            }
                        }
                        break;
                            
                    case CS104_SLAVE_C_CI_NA_1:	// <101>电度总召唤或分组召唤
                        break;
                            
                    case CS104_SLAVE_C_CS_NA_1:	// <103>对钟命令
                        if((CS104SlaveApp[pdrv].RX_Temp.Data.C_100.InfoAddr_H << 16 |
                            CS104SlaveApp[pdrv].RX_Temp.Data.C_100.InfoAddr_M <<8 |
                            CS104SlaveApp[pdrv].RX_Temp.Data.C_100.InfoAddr_L) != 0)
                        {
                            CS104SlaveApp[pdrv].RX_Temp.status.symbol.priority = CS104_SLAVE_C_CS_NA_P;
                            CS104SlaveApp[pdrv].RX_Temp.Head.COT_L = CS104_SLAVE_COT_ADDRERR;
                            CS104_SLAVE_StoreIN(pdrv,&CS104SlaveApp[pdrv].RX_Temp);
                        }
                        else
                        {CS104_SLAVE_SYNProcess(pdrv, (uint8_t *)&CS104SlaveApp[pdrv].RX_Temp);}
                        break;
                            
                    case CS104_SLAVE_C_RP_NA_1:	// <105>复位进程命令
                        if((CS104SlaveApp[pdrv].RX_Temp.Data.C_100.InfoAddr_H << 16 |
                            CS104SlaveApp[pdrv].RX_Temp.Data.C_100.InfoAddr_M <<8 |
                            CS104SlaveApp[pdrv].RX_Temp.Data.C_100.InfoAddr_L) != 0)
                        {
                            CS104SlaveApp[pdrv].RX_Temp.status.symbol.priority = CS104_SLAVE_C_RP_NA_P;
                            CS104SlaveApp[pdrv].RX_Temp.Head.COT_L = CS104_SLAVE_COT_ADDRERR;
                            CS104_SLAVE_StoreIN(pdrv,&CS104SlaveApp[pdrv].RX_Temp);
                        }
                        else if(!((CS104SlaveApp[pdrv].RX_Temp.Head.COT_L|(CS104SlaveApp[pdrv].RX_Temp.Head.COT_H<<8)) == CS104_SLAVE_COT_ACT))
                        {
                            CS104SlaveApp[pdrv].RX_Temp.status.symbol.priority = CS104_SLAVE_C_RP_NA_P;
                            CS104SlaveApp[pdrv].RX_Temp.Head.COT_L = CS104_SLAVE_COT_COTERR;
                            CS104_SLAVE_StoreIN(pdrv,&CS104SlaveApp[pdrv].RX_Temp);
                        }
                        else
                        {
                            CS104SlaveApp[pdrv].Data1Flag |= CS104_SLAVE_CallReset;
                            CS104_SLAVE_ResetProcess(pdrv, (uint8_t *)&CS104SlaveApp[pdrv].RX_Temp);
                        }
                        break;                        
                            
                    case CS104_SLAVE_C_TS_NA_1:	// <104>测试命令
                        CS104SlaveApp[pdrv].RX_Temp.status.symbol.priority = CS104_SLAVE_C_TS_NA_P;
                        CS104SlaveApp[pdrv].RX_Temp.Head.COT_L = CS104_SLAVE_COT_ACTCON;
                        CS104_SLAVE_StoreIN(pdrv,&CS104SlaveApp[pdrv].RX_Temp);
                        break;   
                    
                    case CS104_SLAVE_C_SR_NA_1: // <200>切换定值区
                    case CS104_SLAVE_C_RR_NA_1: // <201>读定值区号
                    case CS104_SLAVE_C_RS_NA_1: // <202>读参数和定值
                    case CS104_SLAVE_C_WS_NA_1: // <203>写参数和定值
                        CS104_SLAVE_FixedParaProcess(pdrv,(uint8_t *)&CS104SlaveApp[pdrv].RX_Temp);
                        break;
                    
                    case CS104_SLAVE_F_FR_NA_1: // <210>文件传输
                        CS104_SLAVE_FileHandleProcess(pdrv,(uint8_t *)&CS104SlaveApp[pdrv].RX_Temp);
                        break;
                                    
                    case CS104_SLAVE_F_SR_NA_1: // <211>软件升级
                        CS104_SLAVE_SoftwareUpdate(pdrv,(uint8_t *)&CS104SlaveApp[pdrv].RX_Temp);
                        break;
                                    
                    default:	// 类型标识有错误或不支持
                        CS104SlaveApp[pdrv].RX_Temp.status.symbol.priority = CS104_SLAVE_M_EI_NA_P;
                        CS104SlaveApp[pdrv].RX_Temp.Head.COT_L = CS104_SLAVE_COT_TIERR;
                        CS104_SLAVE_StoreIN(pdrv,&CS104SlaveApp[pdrv].RX_Temp);
                        break;
                }
            }
            else
            {
                CS104SlaveApp[pdrv].RX_Temp.status.symbol.priority = CS104_SLAVE_M_EI_NA_P;
                CS104SlaveApp[pdrv].RX_Temp.Head.COT_L = CS104_SLAVE_COT_ASUDERR;
                CS104_SLAVE_StoreIN(pdrv,&CS104SlaveApp[pdrv].RX_Temp);
            }
            break;
                
        case CS104_SLAVE_LINK_APPCON:
            break;	

        case CS104_SLAVE_LINK_WORK:
            CS104SlaveApp[pdrv].Connect = TRUE; // 收到STSRTDT后
            break;
                
        case CS104_SLAVE_LINK_NOWORK:
            CS104_AppInitReset(pdrv);
            CS104SlaveApp[pdrv].Connect = FALSE;
            break;
        
        default:
            break; 
    }				  
}

/**
  * @brief : Stop DT Oper.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_SLAVE_StopDTOper(uint8_t pdrv)
{
    struct CS104_SLAVE_AppMsg	msg;
	
    msg.Cmd = CS104_SLAVE_LINK_NOWORK;
    msg.ConNum = 0;
    msg.pData = NULL;
    msg.Len = 0;
    CS104_SLAVE_AppProcess(pdrv, &msg);
    CS104_SLAVE_StopLink(pdrv);
    CS104SlaveLink[pdrv].Connect = FALSE;	// 清链路连接标志
}

/**
  * @brief : Write Data To MISI.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS104_SLAVE_WriteDataToMISI(uint8_t pdrv)
{
    uint16_t count = 0, num = 0;
    
    if (CS104SlaveLink[pdrv].TxdHead >= CS104SlaveLink[pdrv].TxdTail)
    {
        return ;
    }
    
    num = CS104SlaveLink[pdrv].TxdTail - CS104SlaveLink[pdrv].TxdHead;
    
    // 写数据库函数
    count = CS104_Slave_WriteData(pdrv,(uint8_t *)&CS104SlaveLink[pdrv].TxdBuf[CS104SlaveLink[pdrv].TxdHead], num);
    CS104SlaveLink[pdrv].TxdHead += count;
    
    if (CS104SlaveLink[pdrv].TxdHead >= CS104SlaveLink[pdrv].TxdTail)
    {
        CS104SlaveLink[pdrv].TxdTail = 0;
        CS104SlaveLink[pdrv].TxdHead = 0;
    }
}

/**
  * @brief : Send Ctrl Frame.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_SLAVE_S104SendCtrlFrame(uint8_t pdrv, uint8_t ctrl)
{
    struct CS104_SLAVE_PAPCI *pFrame;
    
    pFrame = (struct CS104_SLAVE_PAPCI*)&CS104SlaveLink[pdrv].TxdBuf[CS104SlaveLink[pdrv].TxdHead];
    
    pFrame->StartCode = CS104_SLAVE_STARTCODE68;
    pFrame->Length = 0x04;				//报文长度(除前两字节总长)
    pFrame->NS = ctrl;					//发送控制域
    if (ctrl == CS104_SLAVE_S_FRAME)	//判断S码
    {
        CS104SlaveLink[pdrv].NRACK = CS104SlaveLink[pdrv].NR;
        pFrame->NR = CS104SlaveLink[pdrv].NR << 1;

        CS104_Slave_KillTimer(pdrv, 2);
    }
    else
    {
        pFrame->NR = 0x00;
			
        CS104_Slave_SetTimer(pdrv, 3);
			
        if (ctrl == CS104_SLAVE_U_TESTFRACT)
        {
            CS104_Slave_SetTimer(pdrv, 1); // U测试针应在t1时间内得到确认
            CS104SlaveLink[pdrv].PeerNoAckNum++;
        }
    }
    
    CS104SlaveLink[pdrv].TxdTail += 0x06;
    
    // 发送										
    CS104_SLAVE_WriteDataToMISI(pdrv);
    
    CS104SlaveApp[pdrv].Data1Flag |= CS104_SLAVE_SENDDATA;		//发送数据标志置位
}

/**
  * @brief : Decode S Frame.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_SLAVE_DecodeSFrame(uint8_t pdrv)
{
    uint16_t tmp = 0;
    struct CS104_SLAVE_PAPCI *pAPCI;
    struct CS104_SLAVE_AppMsg	msg;
    
    pAPCI = (struct CS104_SLAVE_PAPCI*)CS104SlaveLink[pdrv].RxdBuf;
    
    if (!CS104SlaveLink[pdrv].Connect)
    {
        return;
    }
    
    tmp = CS104SlaveLink[pdrv].PeerNoAckNum;
    if (!CS104_SLAVE_PeerNoAckNumProcess(pdrv, pAPCI->NR >> 1))
    {
        return;
    }
    CS104SlaveLink[pdrv].StopSendFlag = FALSE;
		
    msg.Cmd = CS104_SLAVE_LINK_APPCON;
    msg.ConNum = tmp - CS104SlaveLink[pdrv].PeerNoAckNum;
    msg.pData = NULL;
    msg.Len = 0;
    CS104_SLAVE_AppProcess(pdrv, &msg);
}

/**
  * @brief : Decode U Frame.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_SLAVE_DecodeUFrame(uint8_t pdrv)
{
    struct CS104_SLAVE_PAPCI *pAPCI;
    struct CS104_SLAVE_AppMsg	msg;
    static uint8_t initflag[CS104_SLAVE_VOLUMES] = {0};
            
    pAPCI = (struct CS104_SLAVE_PAPCI*)CS104SlaveLink[pdrv].RxdBuf;
    
    switch (pAPCI->NS & 0xFF)
    {
        case CS104_SLAVE_U_STARTDTACT:		//初始化
            // 是否初始化
            CS104SlaveLink[pdrv].Tick.Flag = CS104SlaveLink[pdrv].Tick.FlagBak;
            if(initflag[pdrv] == 0)
            {
                initflag[pdrv] = 1;
                CS104SlaveApp[pdrv].Data1Flag |= CS104_SLAVE_HAVEINITEND;
            }
            CS104_SLAVE_S104SendCtrlFrame(pdrv, CS104_SLAVE_U_STARTDTCON);//发送确认命令
            CS104SlaveLink[pdrv].Connect = TRUE;		//链路开启
        
            msg.Cmd = CS104_SLAVE_LINK_WORK;
            msg.ConNum = 0;
            msg.pData = NULL;
            msg.Len = 0;
            CS104_SLAVE_AppProcess(pdrv, &msg);
            break;
            
        case CS104_SLAVE_U_STARTDTCON: // 终端 应该不会收到该命令
            break;
    
        case CS104_SLAVE_U_STOPDTACT:
            CS104SlaveLink[pdrv].Tick.FlagBak = CS104SlaveLink[pdrv].Tick.Flag;
            CS104SlaveLink[pdrv].Tick.Flag = 0;
				    /*回复停止确认之前需对控制站未确认报文进行确认，若被控站存在未确认报文 需等待确认*/
            if (CS104SlaveLink[pdrv].NR > CS104SlaveLink[pdrv].NRACK)
            {
                CS104_SLAVE_S104SendCtrlFrame(pdrv, CS104_SLAVE_S_FRAME); // 收到主站U_STOPDTACT命令
            }
            else
            {
                CS104_SLAVE_S104SendCtrlFrame(pdrv, CS104_SLAVE_U_STOPDTCON);
    
                msg.Cmd = CS104_SLAVE_LINK_NOWORK;
                msg.ConNum = 0;
                msg.pData = NULL;
                msg.Len = 0;
                CS104_SLAVE_AppProcess(pdrv, &msg);
            }
            break;
    
        case CS104_SLAVE_U_STOPDTCON: // 终端 应该不会收到该命令
            break;
    
        case CS104_SLAVE_U_TESTFRACT:
            CS104_SLAVE_S104SendCtrlFrame(pdrv, CS104_SLAVE_U_TESTFRCON);
            break;
    
        case CS104_SLAVE_U_TESTFRCON:
				    /*收到测试确认后 应清t1, 此时t1应该只有测试计时*/	   
            break;
            
        default:
            break;
    }
}

/**
  * @brief : Decode I Frame.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_SLAVE_DecodeIFrame(uint8_t pdrv)
{
    uint16_t tmp = 0;
    struct CS104_SLAVE_AppMsg msg;
    struct CS104_SLAVE_PAPCI *pAPCI;
    
    pAPCI = (struct CS104_SLAVE_PAPCI*)CS104SlaveLink[pdrv].RxdBuf;
    
    if (!CS104SlaveLink[pdrv].Connect)
    {
        return;
    }
    
    tmp = CS104SlaveLink[pdrv].PeerNoAckNum;
    if (!CS104_SLAVE_PeerNoAckNumProcess(pdrv, pAPCI->NR >> 1))
    {
        CS104_SLAVE_StopDTOper(pdrv);
        return;
    }

    if (CS104SlaveLink[pdrv].NR == 32768)
    //if (CS104SlaveLink[pdrv].NR == 63)
    {
        CS104SlaveLink[pdrv].NR = 0;
    }
    else
    {
        CS104SlaveLink[pdrv].NR++;
    }

    CS104SlaveLink[pdrv].StopSendFlag = FALSE; // 清停止发送标志
		
    CS104_Slave_KillTimer(pdrv, 1);
    CS104_Slave_SetTimer(pdrv, 2);
    CS104_Slave_SetTimer(pdrv, 3);
    
    msg.Cmd = CS104_SLAVE_LINK_CALLDATA;
    msg.ConNum = tmp - CS104SlaveLink[pdrv].PeerNoAckNum;
    msg.pData = &CS104SlaveLink[pdrv].RxdBuf[6];
    msg.Len = pAPCI->Length - 4;
    
    CS104_SLAVE_AppProcess(pdrv, &msg);
}

/**
  * @brief : Search Valid Frame.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_SLAVE_SearchValidFrame(uint8_t pdrv)
{
    uint8_t stop = FALSE;
    struct CS104_SLAVE_PAPCI *pAPCI = NULL;				//规约APCI部分
	
    while ((CS104SlaveLink[pdrv].RxdHead < CS104SlaveLink[pdrv].RxdTail) && (!stop))	//判断队列是否有数据
    {
        if (CS104SlaveLink[pdrv].RxdStatus == 0)/////////////////////////RXDSTART
        {
            if (CS104SlaveLink[pdrv].RxdBuf[CS104SlaveLink[pdrv].RxdHead] != CS104_SLAVE_STARTCODE68)	//查找字节是否0x68
            {
                CS104SlaveLink[pdrv].RxdHead++;
				if (CS104SlaveLink[pdrv].RxdHead >= CS104SlaveLink[pdrv].RxdTail)
				{
					CS104SlaveLink[pdrv].RxdLength = CS104SlaveLink[pdrv].RxdTail - CS104SlaveLink[pdrv].RxdHead;
					memcpy (CS104SlaveLink[pdrv].RxdBuf, &CS104SlaveLink[pdrv].RxdBuf[CS104SlaveLink[pdrv].RxdHead], CS104SlaveLink[pdrv].RxdLength);
					CS104SlaveLink[pdrv].RxdHead = 0;
					CS104SlaveLink[pdrv].RxdTail = CS104SlaveLink[pdrv].RxdLength;
				}
                continue;
            }
            
            // 找到后，清除无用的缓冲区报文
            if (CS104SlaveLink[pdrv].RxdTail > CS104SlaveLink[pdrv].RxdHead)
            {
                CS104SlaveLink[pdrv].RxdLength = CS104SlaveLink[pdrv].RxdTail - CS104SlaveLink[pdrv].RxdHead;
                memcpy (CS104SlaveLink[pdrv].RxdBuf, &CS104SlaveLink[pdrv].RxdBuf[CS104SlaveLink[pdrv].RxdHead], CS104SlaveLink[pdrv].RxdLength);
                CS104SlaveLink[pdrv].RxdHead = 0;
                CS104SlaveLink[pdrv].RxdTail = CS104SlaveLink[pdrv].RxdLength;
            }
            
            CS104SlaveLink[pdrv].RxdStatus = 1;//////////////////////////RXDHEAD
        }
       
        if (CS104SlaveLink[pdrv].RxdStatus == 1)////////////////////////RXDHEAD
        {   

            pAPCI = (struct CS104_SLAVE_PAPCI*)CS104SlaveLink[pdrv].RxdBuf;	//规约APCI部分
            if (CS104SlaveLink[pdrv].RxdLength >= sizeof (struct CS104_SLAVE_PAPCI))	//判断报文长度是否大于等于APCI部分 ,因为I格式、S格式和U格式 报文都大于等于APCI部分
            {
                switch (pAPCI->NS & CS104_SLAVE_U_FRAME)
                {
                    case CS104_SLAVE_S_FRAME:				//S格式
                        if (pAPCI->Length == 4)
                        {
                            CS104_Slave_KillTimer(pdrv, 1);
                            CS104_Slave_SetTimer(pdrv, 3);
                            CS104_SLAVE_DecodeSFrame(pdrv);
                        }
                        break;
                
                    case CS104_SLAVE_U_FRAME:				//U格式
                        if (pAPCI->Length == 4)
                        {
                            CS104_Slave_KillTimer(pdrv, 1);	//清除定时标志
                            CS104_Slave_SetTimer(pdrv, 3);	//设置T3时间  20S
                            CS104_SLAVE_DecodeUFrame(pdrv);	//U码解析
                        }
                        break;
                        
                    default:
                        if ((CS104SlaveLink[pdrv].RxdLength >= (pAPCI->Length + 2)))
                        {
                            CS104_SLAVE_DecodeIFrame(pdrv);	//I格式
                        }
                        else
                        {
                            stop = TRUE;
                        }
                        break;
                }
            }
            else	//报文长度不够
            {
                stop = TRUE;
            }
            
            if (!stop)
            {
                CS104SlaveLink[pdrv].RxdStatus = 2;////////////////////////RXDCONTINUE
            }
        }
            
        if (CS104SlaveLink[pdrv].RxdStatus == 2)/////////////////////////RXDCONTINUE
        {
            CS104SlaveLink[pdrv].RxdLength = CS104SlaveLink[pdrv].RxdTail - (pAPCI->Length + 2);		//读取长度清0
            memcpy (CS104SlaveLink[pdrv].RxdBuf, &CS104SlaveLink[pdrv].RxdBuf[pAPCI->Length + 2], CS104SlaveLink[pdrv].RxdLength);	//清空缓冲区
            CS104SlaveLink[pdrv].RxdHead = 0;		//报文头清零
            CS104SlaveLink[pdrv].RxdTail = CS104SlaveLink[pdrv].RxdLength;	//报文尾清零
            CS104SlaveLink[pdrv].RxdStatus = 0;/////////////////////////////RXDSTART
        
            if (CS104SlaveLink[pdrv].RxdTail > CS104SlaveLink[pdrv].RxdHead)
            {
                stop = TRUE;
            }
        }	
    }
}

/**
  * @brief : All Data Conf Reply.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_SLAVE_AllDataConfReply(uint8_t pdrv)
{
    CS104SlaveApp[pdrv].TX_Temp.status.Length = sizeof(CS104SlaveApp[pdrv].TX_Temp.status) + sizeof(CS104SlaveApp[pdrv].TX_Temp.Head) + sizeof(CS104SlaveApp[pdrv].TX_Temp.Data.C_100);
    CS104SlaveApp[pdrv].TX_Temp.status.symbol.priority = CS104_SLAVE_C_IC_NA_PF;
    CS104SlaveApp[pdrv].TX_Temp.Head.TypeID = CS104_SLAVE_C_IC_NA_1;
    CS104SlaveApp[pdrv].TX_Temp.Head.VSQ = 1;
    CS104SlaveApp[pdrv].TX_Temp.Head.COT_L = CS104_SLAVE_COT_ACTCON;
    CS104SlaveApp[pdrv].TX_Temp.Head.PubAddr_H = (uint8_t)((CS104_Slave_Pad[pdrv].PubAddress>>8)&0xff);
    CS104SlaveApp[pdrv].TX_Temp.Head.PubAddr_L = (uint8_t)(CS104_Slave_Pad[pdrv].PubAddress&0xff);
    CS104SlaveApp[pdrv].TX_Temp.Data.C_100.InfoAddr_L = 0;
    CS104SlaveApp[pdrv].TX_Temp.Data.C_100.QOI = 20;
}


/**
  * @brief : Init End Reply.
  * @param : [pdrv]
  * @return: res
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_SLAVE_InitEndReply(uint8_t pdrv)
{
    CS104SlaveApp[pdrv].TX_Temp.status.Length = sizeof(CS104SlaveApp[pdrv].TX_Temp.status) + sizeof(CS104SlaveApp[pdrv].TX_Temp.Head) + sizeof(CS104SlaveApp[pdrv].TX_Temp.Data.C_70);
    CS104SlaveApp[pdrv].TX_Temp.status.symbol.priority = CS104_SLAVE_M_EI_NA_P;
    CS104SlaveApp[pdrv].TX_Temp.Head.TypeID = CS104_SLAVE_M_EI_NA_1;
    CS104SlaveApp[pdrv].TX_Temp.Head.VSQ = 1;
    CS104SlaveApp[pdrv].TX_Temp.Head.COT_L = CS104_SLAVE_COT_INIT;
    CS104SlaveApp[pdrv].TX_Temp.Head.PubAddr_H = (uint8_t)((CS104_Slave_Pad[pdrv].PubAddress>>8)&0xff);
    CS104SlaveApp[pdrv].TX_Temp.Head.PubAddr_L = (uint8_t)(CS104_Slave_Pad[pdrv].PubAddress&0xff);
    CS104SlaveApp[pdrv].TX_Temp.Data.C_70.COI = 2;
}

/**
  * @brief : All Data End Reply.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_SLAVE_AllDataEndReply(uint8_t pdrv)
{
    CS104SlaveApp[pdrv].TX_Temp.status.Length = sizeof(CS104SlaveApp[pdrv].TX_Temp.status) + sizeof(CS104SlaveApp[pdrv].TX_Temp.Head) + sizeof(CS104SlaveApp[pdrv].TX_Temp.Data.C_100);
    CS104SlaveApp[pdrv].TX_Temp.status.symbol.priority = CS104_SLAVE_C_IC_NA_PF;
    CS104SlaveApp[pdrv].TX_Temp.Head.TypeID = CS104_SLAVE_C_IC_NA_1;
    CS104SlaveApp[pdrv].TX_Temp.Head.VSQ = 1;
    CS104SlaveApp[pdrv].TX_Temp.Head.COT_L = CS104_SLAVE_COT_ACCTTERM;
    CS104SlaveApp[pdrv].TX_Temp.Head.PubAddr_H = (uint8_t)((CS104_Slave_Pad[pdrv].PubAddress>>8)&0xff);
    CS104SlaveApp[pdrv].TX_Temp.Head.PubAddr_L = (uint8_t)(CS104_Slave_Pad[pdrv].PubAddress&0xff);
    CS104SlaveApp[pdrv].TX_Temp.Data.C_100.QOI = 20;
}

/**
  * @brief : Reset Processe Reply.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_SLAVE_ResetProcesseReply(uint8_t pdrv)
{
    CS104SlaveApp[pdrv].TX_Temp.status.Length = sizeof(CS104SlaveApp[pdrv].TX_Temp.status) + sizeof(CS104SlaveApp[pdrv].TX_Temp.Head) + sizeof(CS104SlaveApp[pdrv].TX_Temp.Data.C_105);
    CS104SlaveApp[pdrv].TX_Temp.status.symbol.priority = CS104_SLAVE_C_RP_NA_P;
    CS104SlaveApp[pdrv].TX_Temp.Head.TypeID = CS104_SLAVE_C_RP_NA_1;
    CS104SlaveApp[pdrv].TX_Temp.Head.VSQ = 1;
    CS104SlaveApp[pdrv].TX_Temp.Head.COT_L = CS104_SLAVE_COT_ACTCON;
    CS104SlaveApp[pdrv].TX_Temp.Head.PubAddr_H = (uint8_t)((CS104_Slave_Pad[pdrv].PubAddress>>8)&0xff);
    CS104SlaveApp[pdrv].TX_Temp.Head.PubAddr_L = (uint8_t)(CS104_Slave_Pad[pdrv].PubAddress&0xff);
    CS104SlaveApp[pdrv].TX_Temp.Data.C_105.QRP = 1;
}

/**
  * @brief : Link Send Process.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_SLAVE_LinkSendProcess(uint8_t pdrv, uint8_t* pData, uint8_t Len)
{
    struct CS104_SLAVE_PAPCI *pFrame;
    
    pFrame = (struct CS104_SLAVE_PAPCI*)&CS104SlaveLink[pdrv].TxdBuf[CS104SlaveLink[pdrv].TxdHead];
    
    if (Len == 0)
    {
        return;
    }
    CS104SlaveLink[pdrv].NRACK = CS104SlaveLink[pdrv].NR;
		
    pFrame->StartCode = CS104_SLAVE_STARTCODE68;
    pFrame->Length = Len + 0x04;
    pFrame->NS = CS104SlaveLink[pdrv].NS << 1;
    pFrame->NR = CS104SlaveLink[pdrv].NR << 1;
    memcpy((uint8_t*)(pFrame+1), pData, Len);
    CS104SlaveLink[pdrv].TxdTail += Len + 0x06;
//    log_e("pFrame->NS:%x",pFrame->NS);
    // CS104SlaveLink[pdrv].DataType[CS104SlaveLink[pdrv].PeerNoAckNum] = TRUE;
    CS104SlaveLink[pdrv].PeerNoAckNum++;
    CS104SlaveLink[pdrv].NS++;
    if (CS104SlaveLink[pdrv].NS > 32767)
    {
        CS104SlaveLink[pdrv].NS = 0;
    }
		
    if (CS104SlaveLink[pdrv].PeerNoAckNum >= CS104_SLAVE_K) // 未被确认的I帧数量达到K个时  停止发送
    {
        CS104SlaveLink[pdrv].StopSendFlag = TRUE;
    }
    // 发送
    CS104_Slave_KillTimer(pdrv, 2);
    CS104_Slave_SetTimer(pdrv, 3);
    CS104_SLAVE_WriteDataToMISI(pdrv);
    CS104_Slave_SetTimer(pdrv, 1); //
		
}

/**
  * @brief : Save BackUp Data.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void SaveBackUpData(uint8_t pdrv)
{
	if (!CS104SlaveApp[pdrv].SDataEn && CS104SlaveApp[pdrv].SDataIn <= CS104_SLAVE_K)
	{
		CS104SlaveApp[pdrv].TxLen = CS104SlaveApp[pdrv].TX_Temp.status.Length - sizeof(CS104SlaveApp[pdrv].TX_Temp.status);
		CS104SlaveApp[pdrv].SData[CS104SlaveApp[pdrv].SDataIn].TxLen = CS104SlaveApp[pdrv].TxLen;
		memcpy(&CS104SlaveApp[pdrv].SData[CS104SlaveApp[pdrv].SDataIn].TxData, (uint8_t*)&CS104SlaveApp[pdrv].TX_Temp.Head, CS104SlaveApp[pdrv].TxLen);
		CS104SlaveApp[pdrv].SDataIn++;
		CS104SlaveApp[pdrv].CurSData++;
	}	
}
/**
  * @brief : Read All Data Process.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
/**
  * @brief : Read All Data Process.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void ReadAllDataProcess(uint8_t pdrv)
{
	switch(CS104SlaveApp[pdrv].AllDataFlag)
	{
		case 0:
//			log_i("CS104_SLAVE_AllDataConfReply");
            CS104_SLAVE_AllDataConfReply(pdrv);
            CS104SlaveApp[pdrv].AllData_yx_sendaddr = CS104_Slave_Pad[pdrv].YX_FirstAddr;
            CS104SlaveApp[pdrv].AllData_yc_sendaddr = CS104_Slave_Pad[pdrv].YC_FirstAddr;
            CS104SlaveApp[pdrv].AllDataFlag = 1;
			break;
		case 1:
//			log_i("CS104_SLAVE_ReadYxData");
            if(CS104_Slave_Pad[pdrv].YX_AllNum != 0)
            {
                CS104SlaveApp[pdrv].AllData_yx_sendaddr = CS104_SLAVE_ReadYxData(pdrv ,CS104SlaveApp[pdrv].AllData_yx_sendaddr ,CS104_Slave_Pad[pdrv].YX_FrameNum ,(uint8_t *)&CS104SlaveApp[pdrv].TX_Temp);
                if(CS104SlaveApp[pdrv].AllData_yx_sendaddr >= CS104_Slave_Pad[pdrv].YX_FirstAddr + CS104_Slave_Pad[pdrv].YX_AllNum)
                {
                    CS104SlaveApp[pdrv].AllDataFlag = 2;
                }
                break;
            }
		case 2:
//			log_i("CS104_SLAVE_ReadYcData");
            if(CS104_Slave_Pad[pdrv].YC_AllNum != 0)
            {
                CS104SlaveApp[pdrv].AllData_yc_sendaddr = CS104_SLAVE_ReadYcData(pdrv ,CS104SlaveApp[pdrv].AllData_yc_sendaddr ,CS104_Slave_Pad[pdrv].YC_FrameNum ,(uint8_t *)&CS104SlaveApp[pdrv].TX_Temp);
                if(CS104SlaveApp[pdrv].AllData_yc_sendaddr >= CS104_Slave_Pad[pdrv].YC_FirstAddr + CS104_Slave_Pad[pdrv].YC_AllNum)
                {
                    CS104SlaveApp[pdrv].AllDataFlag = 3;
                }
                break;
            }
		case 3:
//			log_i("CS104_SLAVE_AllDataEndReply");
            CS104_SLAVE_AllDataEndReply(pdrv);
            CS104SlaveApp[pdrv].Data1Flag &= (~CS104_SLAVE_CALLALLDATA);
			break;
		default:
			break;
		
	}  
}

/**
  * @brief : Read Data1 Process.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void ReadData1Process(uint8_t pdrv)
{
    if ((CS104SlaveApp[pdrv].Data1Flag & CS104_SLAVE_HAVEINITEND)&&
        ((CS104SlaveApp[pdrv].StoreDATA1OUT->status.symbol.priority > CS104_SLAVE_M_EI_NA_P)||
        (CS104SlaveApp[pdrv].StoreDATA1OUT->status.symbol.priority == 0)))
    {
        CS104_SLAVE_InitEndReply(pdrv);
        CS104SlaveApp[pdrv].Data1Flag &= (~CS104_SLAVE_HAVEINITEND);
        return;
    }

	if (CS104SlaveApp[pdrv].SDataEn)
	{
		if (CS104SlaveApp[pdrv].CurSData && CS104SlaveApp[pdrv].SData[CS104SlaveApp[pdrv].SDataOut].TxLen)
		{
			if (CS104SlaveApp[pdrv].SDataOut != CS104SlaveApp[pdrv].CurSData)
			{
				CS104_SLAVE_LinkSendProcess(pdrv, (uint8_t*)&CS104SlaveApp[pdrv].SData[CS104SlaveApp[pdrv].SDataOut].TxData, CS104SlaveApp[pdrv].SData[CS104SlaveApp[pdrv].SDataOut].TxLen);
				CS104SlaveApp[pdrv].SDataOut++;
			}
			else
			{
				CS104SlaveApp[pdrv].SDataEn = 0;
			}
       		return;			
		}
		else
		{
		    CS104SlaveApp[pdrv].SDataEn = 0;
			return;
		}
	}
    
    if ((CS104_SLAVE_H_COS(pdrv))&&
        ((CS104SlaveApp[pdrv].StoreDATA1OUT->status.symbol.priority > CS104_SLAVE_M_SP_TB_P)||
        (CS104SlaveApp[pdrv].StoreDATA1OUT->status.symbol.priority == 0)))
    {
        CS104_SLAVE_R_COS(pdrv,(uint8_t *)&CS104SlaveApp[pdrv].TX_Temp);
		//SaveBackUpData(pdrv);
        CS104SlaveApp[pdrv].Data1Flag &= (~CS104_SLAVE_HAVECOS);
        return;
    }
    
    if ((CS104_SLAVE_H_SOE(pdrv))&&
        ((CS104SlaveApp[pdrv].StoreDATA1OUT->status.symbol.priority > CS104_SLAVE_M_SP_TB_P)||
        (CS104SlaveApp[pdrv].StoreDATA1OUT->status.symbol.priority == 0)))
    {
        CS104_SLAVE_R_SOE(pdrv,(uint8_t *)&CS104SlaveApp[pdrv].TX_Temp);
        SaveBackUpData(pdrv);					
        CS104SlaveApp[pdrv].Data1Flag &= (~CS104_SLAVE_HAVESOE);
        return;
    }
		
    if ((CS104SlaveApp[pdrv].Data1Flag & CS104_SLAVE_CALLALLDATA)&&
        ((CS104SlaveApp[pdrv].StoreDATA1OUT->status.symbol.priority > CS104_SLAVE_C_IC_NA_P)||
        (CS104SlaveApp[pdrv].StoreDATA1OUT->status.symbol.priority == 0)))
    {
        ReadAllDataProcess(pdrv);
        return;
    }
    if (CS104SlaveApp[pdrv].Data1Flag & CS104_SLAVE_CallReset) // 复位进程
    {
        CS104SlaveApp[pdrv].Data1Flag &= ~CS104_SLAVE_CallReset;
        CS104_SLAVE_ResetProcesseReply(pdrv);
//        CS104_AppInitReset(pdrv);
//        CS104SlaveApp[pdrv].Connect = FALSE;
        return;
    }
    
    if (CS104SlaveApp[pdrv].StoreDATA1OUT != CS104SlaveApp[pdrv].StoreDATA1IN)
    {
        memcpy((uint8_t *)&CS104SlaveApp[pdrv].TX_Temp, (uint8_t *)CS104SlaveApp[pdrv].StoreDATA1OUT, CS104SlaveApp[pdrv].StoreDATA1OUT->status.Length);
        memset((uint8_t *)CS104SlaveApp[pdrv].StoreDATA1OUT,0,sizeof(CS104_SLAVE_PASDU));
        if (++CS104SlaveApp[pdrv].StoreDATA1OUT >= CS104SlaveApp[pdrv].StoreDATA1Buf + CS104_SLAVE_STOREDATA1NUM)
        {
            CS104SlaveApp[pdrv].StoreDATA1OUT = CS104SlaveApp[pdrv].StoreDATA1Buf;
        }
        return;
    }		
}

/**
  * @brief : Read Data2 Process.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void ReadData2Process(uint8_t pdrv)
{
    if ((CS104_SLAVE_H_NVA(pdrv))&&
        ((CS104SlaveApp[pdrv].StoreDATA2IN->status.symbol.priority > CS104_SLAVE_M_ME_NC_P)||
        (CS104SlaveApp[pdrv].StoreDATA2IN->status.symbol.priority == 0)))
    {
        CS104_SLAVE_R_NVA(pdrv,(uint8_t *)&CS104SlaveApp[pdrv].TX_Temp);
        return;
    }
    
    if ((CS104_SLAVE_H_FEvent(pdrv))&&
        ((CS104SlaveApp[pdrv].StoreDATA2IN->status.symbol.priority > CS104_SLAVE_M_FT_NA_P)||
        (CS104SlaveApp[pdrv].StoreDATA2IN->status.symbol.priority == 0)))
    {
        CS104_SLAVE_R_FEvent(pdrv,(uint8_t *)&CS104SlaveApp[pdrv].TX_Temp);
        return;
    }

    if (CS104SlaveApp[pdrv].StoreDATA2OUT != CS104SlaveApp[pdrv].StoreDATA2IN)
    {
        memcpy((uint8_t *)&CS104SlaveApp[pdrv].TX_Temp, (uint8_t *)CS104SlaveApp[pdrv].StoreDATA2OUT, CS104SlaveApp[pdrv].StoreDATA2OUT->status.Length);
        memset((uint8_t *)CS104SlaveApp[pdrv].StoreDATA2OUT,0,sizeof(CS104_SLAVE_PASDU));
        if (++CS104SlaveApp[pdrv].StoreDATA2OUT >= CS104SlaveApp[pdrv].StoreDATA2Buf + CS104_SLAVE_STOREDATA2NUM)
        {
            CS104SlaveApp[pdrv].StoreDATA2OUT = CS104SlaveApp[pdrv].StoreDATA2Buf;
        }
        return;
    }
    
    CS104_SLAVE_IdleProcess(pdrv,(uint8_t *)&CS104SlaveApp[pdrv].TX_Temp);
}

/**
  * @brief : Net Tes.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_SLAVE_NetTest(uint8_t pdrv)
{   
    if (!CS104_Slave_CheckLink(pdrv))//是否链接
    {            
        CS104_AppInitReset(pdrv);
        CS104SlaveApp[pdrv].Connect = 0;
    }
    else if (!CS104SlaveLink[pdrv].Connect)
    {				
        CS104SlaveLink[pdrv].Connect = 1;
    }
}

/**
  * @brief : Send Process.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_SLAVE_SendProcess(uint8_t pdrv)
{	
    if (CS104SlaveLink[pdrv].StopSendFlag)
    {
        return;
    }
    if (CS104SlaveApp[pdrv].Data1Flag & CS104_SLAVE_SENDDATA)
    {
        CS104SlaveApp[pdrv].Data1Flag &= (~CS104_SLAVE_SENDDATA);
        return;
    }

    ReadData1Process(pdrv);    

    if (CS104SlaveApp[pdrv].TX_Temp.status.Length)
    {
        CS104SlaveApp[pdrv].TxLen = CS104SlaveApp[pdrv].TX_Temp.status.Length - sizeof(CS104SlaveApp[pdrv].TX_Temp.status);
        memcpy(&CS104SlaveApp[pdrv].TxMsg, &CS104SlaveApp[pdrv].TX_Temp.Head, CS104SlaveApp[pdrv].TxLen);
        CS104_SLAVE_LinkSendProcess(pdrv, (uint8_t*)&CS104SlaveApp[pdrv].TxMsg, CS104SlaveApp[pdrv].TxLen);
        memset((uint8_t *)&CS104SlaveApp[pdrv].TX_Temp, 0, sizeof(CS104_SLAVE_PASDU));
    }	
    else
    {
        ReadData2Process(pdrv);
        if (CS104SlaveApp[pdrv].TX_Temp.status.Length)
        {
            CS104SlaveApp[pdrv].TxLen = CS104SlaveApp[pdrv].TX_Temp.status.Length - sizeof(CS104SlaveApp[pdrv].TX_Temp.status);
            memcpy(&CS104SlaveApp[pdrv].TxMsg, &CS104SlaveApp[pdrv].TX_Temp.Head, CS104SlaveApp[pdrv].TxLen);
            CS104_SLAVE_LinkSendProcess(pdrv, (uint8_t*)&CS104SlaveApp[pdrv].TxMsg, CS104SlaveApp[pdrv].TxLen);
            memset((uint8_t *)&CS104SlaveApp[pdrv].TX_Temp, 0, sizeof(CS104_SLAVE_PASDU));
        }				
    }
}

/**
  * @brief : Link On Timer.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_SLAVE_LinkOnTimer(uint8_t pdrv)
{
    uint8_t	i = 0;
    uint16_t num = 0;     
    uint64_t timeOut;
    static uint8_t resetStatus = 1;
    uint64_t timeMs = CS104_SLAVE_getAlarmOfMs(pdrv);

    if (CS104SlaveLink[pdrv].Tick.Flag & CS104_SLAVE_T0FLAG)
    {
    	CS104SlaveLink[pdrv].Tick.timeEndT0 = timeMs;
        timeOut = CS104SlaveLink[pdrv].Tick.timeEndT0 - CS104SlaveLink[pdrv].Tick.timeBeginT0;

        if (timeOut > CS104_Slave_Pad[pdrv].TickValue[0])
        {
//        	log_e("T0 reset");
            CS104_Slave_KillTimer(pdrv, 0);
            CS104_SLAVE_StopDTOper(pdrv);
        }
    }

    if (CS104SlaveLink[pdrv].Tick.Flag & CS104_SLAVE_T1FLAG)
    {
        for (i=0; i<CS104SlaveLink[pdrv].PeerNoAckNum; i++)
        {
        	CS104SlaveLink[pdrv].Tick.timeEndT1[i] = timeMs;
            timeOut = CS104SlaveLink[pdrv].Tick.timeEndT1[i] - CS104SlaveLink[pdrv].Tick.timeBeginT1[i];
            if (timeOut > CS104_Slave_Pad[pdrv].TickValue[1])
            {
//            	log_e("T1 reset %d:PeerNoAckNum:%d",i,CS104SlaveLink[pdrv].PeerNoAckNum);
                CS104_Slave_KillTimer(pdrv, 1);
                CS104_SLAVE_StopDTOper(pdrv);
            }
        }
    }

    if (CS104SlaveLink[pdrv].NRACK <= CS104SlaveLink[pdrv].NR)
    {
        num = CS104SlaveLink[pdrv].NR - CS104SlaveLink[pdrv].NRACK;
    }
    else
    {
        num = 32767 + CS104SlaveLink[pdrv].NR - CS104SlaveLink[pdrv].NRACK;
    }
    
    if (num >= CS104_SLAVE_W)
    {
//    	log_i("CS104_Slave_KillTimer");
        CS104SlaveLink[pdrv].NRACK = CS104SlaveLink[pdrv].NR;
        CS104_Slave_KillTimer(pdrv, 2);
        CS104_SLAVE_S104SendCtrlFrame(pdrv, CS104_SLAVE_S_FRAME);
    }
    
    if (CS104SlaveLink[pdrv].Tick.Flag & CS104_SLAVE_T2FLAG)
    {
    	CS104SlaveLink[pdrv].Tick.timeEndT2 = timeMs;
        timeOut = CS104SlaveLink[pdrv].Tick.timeEndT2 - CS104SlaveLink[pdrv].Tick.timeBeginT2;
        if ( timeOut > CS104_Slave_Pad[pdrv].TickValue[2] )
        {
//        	log_e("T2 reset");
            CS104_Slave_KillTimer(pdrv, 2);
            CS104_SLAVE_S104SendCtrlFrame(pdrv, CS104_SLAVE_S_FRAME);
        }
    }
    
    if (CS104SlaveLink[pdrv].Tick.Flag & CS104_SLAVE_T3FLAG)			//T3标志判断
    {
    	CS104SlaveLink[pdrv].Tick.timeEndT3 = timeMs;
        timeOut = CS104SlaveLink[pdrv].Tick.timeEndT3 - CS104SlaveLink[pdrv].Tick.timeBeginT3;
        if (timeOut > CS104_Slave_Pad[pdrv].TickValue[3])				//超时判断
        {
//        	log_e("T3 reset");
            CS104_Slave_KillTimer(pdrv, 3);
            
            if (CS104SlaveLink[pdrv].PeerNoAckNum == 0)
            {
                CS104_SLAVE_S104SendCtrlFrame(pdrv, CS104_SLAVE_U_TESTFRACT);
            }
            else if (!(CS104SlaveLink[pdrv].Tick.Flag & CS104_SLAVE_T1FLAG))
            {
                CS104_Slave_SetTimer(pdrv, 1);
            }
        }
    }
    
    if(getDzPreTimeOut104(pdrv) == TRUE)/*定值预制超时*/
    {
        CS104_Slave_DZ_RevokeOperation(pdrv);
    }

    if(CS104_Slave_Pad[pdrv].resetFlag && resetStatus)//复位计时
    {
        CS104SlaveLink[pdrv].Tick.timeResetBegin = timeMs;
        CS104SlaveLink[pdrv].Tick.timeResetEnd = timeMs;
        resetStatus = 0;
    }
    if(CS104_Slave_Pad[pdrv].resetFlag || CS104_Slave_Pad[pdrv].updateFlag)
    {
        CS104SlaveLink[pdrv].Tick.timeResetEnd = timeMs;
        timeOut = CS104SlaveLink[pdrv].Tick.timeResetEnd - CS104SlaveLink[pdrv].Tick.timeResetBegin;
        if(CS104_Slave_Pad[pdrv].updateFlag && CS104_Slave_Pad[pdrv].Encrypt)//加密升级结束
        {
            if(timeOut >= CS104_Slave_Pad[pdrv].TimeOutReset/2)
            {
//                Encrypt_ReturnMd5Fault_IN();//上送错误校验						TXL注释
//                if(!Encrypt_UPvalue_Read(pdrv))
//                {
//                    CS104_Slave_Pad[pdrv].resetFlag = 0;
//                    resetStatus = 1;
//                }
                CS104_Slave_Pad[pdrv].updateFlag = 0;
            }
        }
    	if(timeOut >= CS104_Slave_Pad[pdrv].TimeOutReset)//超时 停止进程
        {
            CS104_Slave_Pad[pdrv].resetFlag = 0;
            CS104_Slave_Pad[pdrv].updateFlag = 0;
            CS104_Slave_exit(pdrv);
        }
    }
}

/**
  * @brief : Read MISI Data.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_SLAVE_ReadMISIData(uint8_t pdrv, uint8_t Flag)
{
    uint16_t count = 0;
    
	  /*读取接收缓冲区*/
	if (CS104SlaveLink[pdrv].RxdTail < sizeof(CS104SlaveLink[pdrv].RxdBuf) - CS104_SLAVE_FRAMEBUFSIZE)//尾部不大于256都可以接收
	{
        count = CS104_Slave_ReadData(pdrv, (uint8_t *)&CS104SlaveLink[pdrv].RxdBuf[CS104SlaveLink[pdrv].RxdTail], CS104_SLAVE_FRAMEBUFSIZE);
	}
    
	if (Flag)
    {
        CS104SlaveLink[pdrv].RxdTail += count;
        CS104SlaveLink[pdrv].RxdLength += count;

    }
//	DLT634_5104
//    if(!DLT634_ChannelToMonitor_SLAVE_SearchMonitorFrame(pdrv, DLT634_5104, CS104SlaveLink[pdrv].RxdBuf, 256))
    {
        CS104_SLAVE_SearchValidFrame(pdrv);
    }
}

/* PUBLIC FUNCTION PROTOTYPES ------------------------------------------------*/
/**
  * @brief : Reply.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS104_Slave_Reply(uint8_t pdrv, uint8_t *pbuf) //回复
{ 
    uint8_t res = 0;
    memcpy(&CS104SlaveApp[pdrv].ST_Temp, pbuf, CS104_SLAVE_LPDUSIZE);
    switch (CS104SlaveApp[pdrv].ST_Temp.Head.TypeID)
    {
        //控制方向过程信息
        case CS104_SLAVE_C_SC_NA_1: // 单点遥控命令
        case CS104_SLAVE_C_SC_NB_1: // 双点遥控命令
            CS104SlaveApp[pdrv].ST_Temp.status.symbol.priority = CS104_SLAVE_C_SC_NA_P;
            res = CS104_SLAVE_StoreIN(pdrv,&CS104SlaveApp[pdrv].ST_Temp);
            break;

        case CS104_SLAVE_C_CS_NA_1: // 对时命令
            CS104SlaveApp[pdrv].ST_Temp.status.symbol.priority = CS104_SLAVE_C_CS_NA_P;
            res = CS104_SLAVE_StoreIN(pdrv,&CS104SlaveApp[pdrv].ST_Temp);
            break;

        case CS104_SLAVE_C_RP_NA_1: // 复位进程命令
            break;

        case CS104_SLAVE_C_RR_NA_1: // 读定值区号
        case CS104_SLAVE_C_RS_NA_1: // 读参数和定值命令
            CS104SlaveApp[pdrv].ST_Temp.status.symbol.priority = CS104_SLAVE_C_IC_NA_P;
            res = CS104_SLAVE_StoreIN(pdrv,&CS104SlaveApp[pdrv].ST_Temp);
            break;

        case CS104_SLAVE_C_SR_NA_1: // 切换定值区
        case CS104_SLAVE_C_WS_NA_1: // 写参数和定值命令
            CS104SlaveApp[pdrv].ST_Temp.status.symbol.priority = CS104_SLAVE_C_SC_NA_P;
            res = CS104_SLAVE_StoreIN(pdrv,&CS104SlaveApp[pdrv].ST_Temp);
            break;

        case CS104_SLAVE_F_FR_NA_1: // 文件传输
            CS104SlaveApp[pdrv].ST_Temp.status.symbol.priority = CS104_SLAVE_F_FW_NA_P;
            res = CS104_SLAVE_StoreIN(pdrv,&CS104SlaveApp[pdrv].ST_Temp);
            break;

        case CS104_SLAVE_F_SR_NA_1: // 软件升级
            CS104SlaveApp[pdrv].ST_Temp.status.symbol.priority = CS104_SLAVE_F_FW_NA_P;
            res = CS104_SLAVE_StoreIN(pdrv,&CS104SlaveApp[pdrv].ST_Temp);
            break;			

        default: // 类型标识有错误或不支持
            break;
    }
    return(res);
}

/**
  * @brief : App Init.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS104_Slave_Appinit(uint8_t pdrv)
{
	if (CS104SlaveLink == NULL)
	{
	    CS104SlaveLink = malloc(sizeof(struct CS104_SLAVE_LINKINFO) * CS104_SLAVE_VOLUMES);
        memset (CS104SlaveLink, 0, sizeof (struct CS104_SLAVE_LINKINFO) * CS104_SLAVE_VOLUMES);
	}
	if (CS104SlaveApp == NULL)
	{
	    CS104SlaveApp = malloc(sizeof(struct CS104_SLAVE_APPINFO) * CS104_SLAVE_VOLUMES);
        memset (CS104SlaveApp, 0, sizeof (struct CS104_SLAVE_APPINFO) * CS104_SLAVE_VOLUMES);
	}	
//    log_w("sizeof(struct CS104_SLAVE_LINKINFO):%d",sizeof(struct CS104_SLAVE_LINKINFO));
//    log_w("sizeof(struct CS104_SLAVE_APPINFO):%d",sizeof(struct CS104_SLAVE_APPINFO));
    CS104SlaveApp[pdrv].YXTypeID = CS104_SLAVE_M_SP_NA_1;
    CS104SlaveApp[pdrv].YCTypeID = CS104_SLAVE_M_ME_NC_1;
    CS104_AppInitReset(pdrv);
	  
    return(TRUE);
}

/**
  * @brief : Clock.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS104_Slave_Clock(uint8_t pdrv)
{
	if(!CS104SlaveApp[pdrv].Connect) // 状态连接
	{
		CS104_SLAVE_NetTest(pdrv);				//网络检查
		CS104_SLAVE_ReadMISIData(pdrv, TRUE);
//		if(CS104_Slave_Pad[pdrv].Encrypt)
//		{
//			if(CS104_SLAVE_H_Encrypt(pdrv))
//			{
//				return(CS104SlaveApp[pdrv].Connect);
//			}
//		}
	}
	else
	{
		CS104_SLAVE_WriteDataToMISI(pdrv);

//		if(CS104_Slave_Pad[pdrv].Encrypt)
//		{
//			if(CS104_SLAVE_H_Encrypt(pdrv))
//			{
//				return(CS104SlaveApp[pdrv].Connect);
//			}
//		}

		if(CS104SlaveLink[pdrv].TxdHead != CS104SlaveLink[pdrv].TxdTail)
		{
			return(CS104SlaveApp[pdrv].Connect);
		}

		CS104_SLAVE_ReadMISIData(pdrv, TRUE); // 链路层数据查询

		CS104_SLAVE_SendProcess(pdrv);	// 发送数据

		CS104_SLAVE_LinkOnTimer(pdrv); // 链路层定时器

		CS104_SLAVE_NetTest(pdrv); // 网络判断
	}
	return(CS104SlaveApp[pdrv].Connect);
}

