/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      cs104MasterApp.c
  * @brief:		104app交互处理
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2019-03-18
  * @update:    [2019-03-18][Chen][newly increased]
  */
#define LOG_TAG "CS104_Master_App "
/* INCLUDE FILES -------------------------------------------------------------*/
#include "cs104MasterApp.h"
#include "cs104MasterDisk.h"
#include "../common/common.h"
#include "string.h"
#include <time.h>
//#include "elog.h"
#include "../port/iec_interface.h"
//#include "channel_monitor.h"
/* PRIVATE VARIABLES ----------------------------------------------------------*/

/* PUBLIC VARIABLES ----------------------------------------------------------*/
struct sCS104_Master_Pad CS104_Master_Pad[CS104_MASTER_VOLUMES]; // DL/T634_5101规约参数
struct sMasterParam *CS104_Master_Param = NULL;
struct CS104_MASTER_LINKINFO *CS104MasterLink = NULL;            //[CS104_MASTER_VOLUMES]; // 链路层变量
struct CS104_MASTER_APPINFO *CS104MasterApp = NULL;              //[CS104_MASTER_VOLUMES] __attribute__((at(0x00002000 + SDRAM_ADDR_104)));

/* PRIVATE FUNCTION PROTOTYPES -----------------------------------------------*/
/**
  * @brief : Set Timer.
  * @param : [pdrv]
  * @param : [No]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_Master_SetTimer(uint8_t pdrv, uint8_t No)
{
    uint8_t i = 0;
    uint64_t timeMs = CS104_Master_getAlarmOfMs(pdrv);
//    log_w("设置时间CS104_Master_SetTimer(%d):%llu",pdrv,timeMs);
    switch (No)
    {
    case 0:
        CS104MasterLink[pdrv].Tick.timeBeginT0 = timeMs;
        CS104MasterLink[pdrv].Tick.Flag |= CS104_MASTER_T0FLAG;
        break;

    case 1:
        for (i = 0; i < CS104_MASTER_K; i++)
        {
            CS104MasterLink[pdrv].Tick.timeBeginT1[i] = timeMs;
        }
        CS104MasterLink[pdrv].Tick.Flag |= CS104_MASTER_T1FLAG;
        break;

    case 2:
        CS104MasterLink[pdrv].Tick.timeBeginT2 = timeMs;
        CS104MasterLink[pdrv].Tick.Flag |= CS104_MASTER_T2FLAG;
        break;

    case 3:
        CS104MasterLink[pdrv].Tick.timeBeginT3 = timeMs;
        CS104MasterLink[pdrv].Tick.Flag |= CS104_MASTER_T3FLAG;
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
static void CS104_Master_KillTimer(uint8_t pdrv, uint8_t No)
{
    switch (No)
    {
    case 0:
        CS104MasterLink[pdrv].Tick.Flag &= (~CS104_MASTER_T0FLAG);
        break;

    case 1:
        CS104MasterLink[pdrv].Tick.Flag &= (~CS104_MASTER_T1FLAG);
        break;

    case 2:
        CS104MasterLink[pdrv].Tick.Flag &= (~CS104_MASTER_T2FLAG);
        break;

    case 3:
        CS104MasterLink[pdrv].Tick.Flag &= (~CS104_MASTER_T3FLAG);
        break;

    default:
        break;
    }
}

/**
  * @brief : 设置发送类型标志.
  * @param : [pdrv]
  * @param : [No]
  * @return: none
  * @updata: [2019-04-18][Chen][New]
  */
static void CS104_Master_sendDateType(uint8_t pdrv, uint8_t No)
{
    switch (No)
    {
    case CLOCKSYN:
        CS104MasterApp[pdrv].sendDateType |= CS104_MASTER_CLOCKSYN;
        break;

    case ALLDATA:
//    log_w("总召下发(%d)",pdrv);
        CS104MasterApp[pdrv].sendDateType |= CS104_MASTER_ALLDATA;
        break;
    
    case ELEDATA:
        CS104MasterApp[pdrv].sendDateType |= CS104_MASTER_ELEDATA;
        break;
   
    case RESETPRO:
        CS104MasterApp[pdrv].sendDateType |= CS104_MASTER_RESETPRO;
        break;
    
    case TEST:
        CS104MasterApp[pdrv].sendDateType |= CS104_MASTER_TEST;
        break;
    
    case SWICHAREA:
        CS104MasterApp[pdrv].sendDateType |= CS104_MASTER_SWICHAREA;
        break;
    
    case RDAREA:
        CS104MasterApp[pdrv].sendDateType |= CS104_MASTER_RDAREA;
        break;
   
    case RDSETIG:
        CS104MasterApp[pdrv].sendDateType |= CS104_MASTER_RDSETIG;
        break;
    
    case WRSETIG:
        CS104MasterApp[pdrv].sendDateType |= CS104_MASTER_WRSETIG;
        break;

    case REMOTE:
        CS104MasterApp[pdrv].sendDateType |= CS104_MASTER_REMOTECTL;
        break;
    default:
        break;
    }
}

/**
  * @brief : 清除发送类型标志.
  * @param : [pdrv]
  * @param : [No]
  * @return: none
  * @updata: [2019-04-18][Chen][New]
  */
static void CS104_Master_clearSendDateType(uint8_t pdrv, uint64_t Type)
{
    CS104MasterApp[pdrv].sendDateType &= (~Type);
}

/**
  * @brief : Peer No Ack Num Process.
  * @param : [pdrv]
  * @param : [PeerNR]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static uint8_t CS104_MASTER_PeerNoAckNumProcess(uint8_t pdrv, uint16_t PeerNR)
{
    uint16_t i = 0;
    uint16_t num = 0;
    uint64_t timeMs = CS104_Master_getAlarmOfMs(pdrv);

    if (PeerNR <= CS104MasterLink[pdrv].NS)
    {
        num = CS104MasterLink[pdrv].NS - PeerNR;
    }
    else
    {
        num = 32767 + CS104MasterLink[pdrv].NS - PeerNR;
    }
    
//    log_w("(%d) NS:%d(%x) NR:%d(%x)",pdrv,CS104MasterLink[pdrv].NS,CS104MasterLink[pdrv].NS*2,PeerNR,PeerNR*2);

    if (num > CS104MasterLink[pdrv].PeerNoAckNum)
    {
//        log_e("num(%d) > CS104SlaveLink[pdrv].PeerNoAckNum(%d)",num,CS104MasterLink[pdrv].PeerNoAckNum);

        CS104MasterLink[pdrv].NR = 0;
        CS104MasterLink[pdrv].NS = 0;
        CS104MasterLink[pdrv].NRACK = 0;
        CS104MasterLink[pdrv].PeerNoAckNum = 0;
        num = 0;

        exit(1);//////////////////////////////////////////////////////////////////删掉
//      return (FALSE);
    }
    CS104MasterLink[pdrv].PeerNoAckNum = num;
    
    if(num >= CS104_MASTER_K)
    {
//        log_e("XXXXXXXXXXXXXX num:%d XXXXXXXXXXXX",num);
//        log_e("NS:%d(%x) NR:%d(%x)",CS104MasterLink[pdrv].NS,CS104MasterLink[pdrv].NS*2,PeerNR,PeerNR*2);
        return (FALSE);
    }
    else
    {
        for (i = 0; i < num; i++)
        {
            CS104MasterLink[pdrv].Tick.timeBeginT1[i] = CS104MasterLink[pdrv].Tick.timeBeginT1[num+i];//未被确定的,不需重新计时
        }
        for (i = num; i < 24; i++)
        {
            CS104MasterLink[pdrv].Tick.timeBeginT1[i] = timeMs;//已经被确认的重新计时
        }
    }

    return (TRUE);
}

/**
  * @brief : App Init Reset.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS104_Master_AppInitReset(uint8_t pdrv)
{
    /*Link*/
    CS104MasterLink[pdrv].Connect = FALSE;
    CS104MasterLink[pdrv].NR = 0;
    CS104MasterLink[pdrv].NS = 0;
    CS104MasterLink[pdrv].NRACK = 0;
    CS104MasterLink[pdrv].PeerNoAckNum = 0;
    CS104MasterLink[pdrv].FrameHead = 0;
    CS104MasterLink[pdrv].TxdHead = 0;
    CS104MasterLink[pdrv].TxdTail = 0;
    CS104MasterLink[pdrv].StopSendFlag = FALSE; // 清停止发送标志

    memset(CS104MasterLink[pdrv].TxdBuf, 0, sizeof(CS104MasterLink[pdrv].RxdBuf)); // 清空接收缓冲区
    memset(CS104MasterLink[pdrv].RxdBuf, 0, sizeof(CS104MasterLink[pdrv].TxdBuf)); // 清空发送缓冲区
    /*App*/
    CS104_Master_KillTimer(pdrv,0);
    CS104_Master_KillTimer(pdrv,1);
    CS104_Master_KillTimer(pdrv,2);
    CS104_Master_KillTimer(pdrv,3);
    CS104MasterApp[pdrv].Connect = FALSE;
    CS104MasterApp[pdrv].FirstInit = 0;
    CS104MasterApp[pdrv].firstAllData = FALSE;
    CS104MasterApp[pdrv].sendDateType = 0;
    return (TRUE);
}

/**
  * @brief : App Process.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS104_MASTER_AppProcess(uint8_t pdrv, struct CS104_MASTER_AppMsg *Msg)
{
    CS104MasterApp[pdrv].TxLen = 0;
    memset((uint8_t *)&CS104MasterApp[pdrv].RX_Temp, 0, sizeof(CS104_MASTER_PASDU));
    memcpy((uint8_t *)&CS104MasterApp[pdrv].RX_Temp.Head, Msg->pData, Msg->Len);
    CS104MasterApp[pdrv].RX_Temp.status.Length = Msg->Len + sizeof(CS104MasterApp[pdrv].RX_Temp.status);

    switch (Msg->Cmd)
    {
    case CS104_MASTER_LINK_APPCON:/*S确认帧*/
        break;

    case CS104_MASTER_LINK_WORK:/*启动链路*/
        CS104MasterApp[pdrv].Connect = TRUE;
        break;

    case CS104_MASTER_LINK_NOWORK:/*断开链路*/
        CS104_Master_AppInitReset(pdrv);
        break;

    case CS104_MASTER_LINK_CALLDATA:/*I帧*/
        if((CS104MasterApp[pdrv].RX_Temp.Head.PubAddr_L|(CS104MasterApp[pdrv].RX_Temp.Head.PubAddr_H<<8)) == CS104_Master_Pad[pdrv].PubAddress)
        {
            switch (CS104MasterApp[pdrv].RX_Temp.Head.TypeID)/*类型标识*/
            {
                case CS104_MASTER_M_SP_NA_1: // <1>单点信息
                case CS104_MASTER_M_DP_NA_1: // <3>双点信息
                     if(!CS104MasterApp[pdrv].firstAllData)
                     {
                        CS104_Master_ReadYxProcess(pdrv, (uint8_t *)&CS104MasterApp[pdrv].RX_Temp);
                     }
                    break;
                case CS104_MASTER_M_ME_NA_1: // <9>归一化值
                case CS104_MASTER_M_ME_NB_1: // <11>标度化值
                case CS104_MASTER_M_ME_NC_1: // <13>短浮点数
                    CS104_Master_ReadYcProcess(pdrv, (uint8_t *)&CS104MasterApp[pdrv].RX_Temp);
                    break;
                case CS104_MASTER_M_SP_TB_1: // <30>带时标的单点信息
                case CS104_MASTER_M_DP_TB_1: // <31>带时标的双点信息
                    CS104_Master_ReadSoeProcess(pdrv, (uint8_t *)&CS104MasterApp[pdrv].RX_Temp);
                    break;
                case CS104_MASTER_M_IT_NB_1: // <206>累计量 短浮点数
                    break;
                case CS104_MASTER_M_IT_TC_1: // <207>带时标累计量 短浮点数
                    break;

                case CS104_MASTER_C_SC_NA_1: // <45>单点遥控命令
                case CS104_MASTER_C_SC_NB_1: // <46>双点遥控命令
                     CS104_Master_ReadRemoteCtlProcess(pdrv,(uint8_t *)&CS104MasterApp[pdrv].RX_Temp);
                     break;

                case CS104_MASTER_M_EI_NA_1: // <70>初始化结束帧
                case CS104_MASTER_C_IC_NA_1: // <100>站总召唤命令
                     if(CS104_Master_ReadAllDataProcess(pdrv, (uint8_t *)&CS104MasterApp[pdrv].RX_Temp))
                     {
                        CS104MasterApp[pdrv].firstAllData = TRUE;
                     }
                     break;
                case CS104_MASTER_C_CI_NA_1: // <101>电能量召唤命令
                case CS104_MASTER_C_CS_NA_1: // <103>时钟同步命令
                case CS104_MASTER_C_TS_NA_1: // <104>测试命令
                    break;
                case CS104_MASTER_C_RP_NA_1: // <105>复位进程命令
                    CS104_Master_AppInitReset(pdrv);
                    CS104_Master_StopLink(pdrv);
                    break;

                case CS104_MASTER_C_SR_NA_1: // <200>切换定值区
                case CS104_MASTER_C_RR_NA_1: // <201>读定值区号
                case CS104_MASTER_C_RS_NA_1: // <202>读参数和定值
                case CS104_MASTER_C_WS_NA_1: // <203>写参数和定值
                    /////////////////   切换定值区：返回定值区号 保存区号
                    /////////////////   读定值区号：返回当前定值区号 保存区号
                    /////////////////   读参数定值：返回值，保存
                    /////////////////   写参数定值：返回预制 固化 取消的确认帧 标志设置
                    break;
                
                case CS104_MASTER_F_FR_NA_1: // <210>文件传输
                    ////////////////    读目录返回目录数据；读文件返回文件数据；写文件确认
                    break;
                                
                case CS104_MASTER_F_SR_NA_1: // <211>软件升级
                    /////////////// 启动确认 升级结束确认 终止确认  设置标志
                    break;
                                
                default:	// 类型标识有错误或不支持
                    break;
            }
        }
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
static void CS104_MASTER_StopDTOper(uint8_t pdrv)
{
    struct CS104_MASTER_AppMsg msg;

    msg.Cmd = CS104_MASTER_LINK_NOWORK;
    msg.ConNum = 0;
    msg.pData = NULL;
    msg.Len = 0;
    CS104_MASTER_AppProcess(pdrv, &msg);

    CS104_Master_StopLink(pdrv);
    CS104MasterLink[pdrv].Connect = FALSE; // 清链路连接标志
}

/**
  * @brief : Write Data To MISI.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS104_MASTER_WriteDataToMISI(uint8_t pdrv)
{
    uint16_t count = 0, num = 0;

    if (CS104MasterLink[pdrv].TxdHead >= CS104MasterLink[pdrv].TxdTail)
    {
        return;
    }

    num = CS104MasterLink[pdrv].TxdTail - CS104MasterLink[pdrv].TxdHead;

    // 写数据库函数
    count = CS104_Master_WriteData(pdrv, (uint8_t *)&CS104MasterLink[pdrv].TxdBuf[CS104MasterLink[pdrv].TxdHead], num);
    CS104MasterLink[pdrv].TxdHead += count;

    if (CS104MasterLink[pdrv].TxdHead >= CS104MasterLink[pdrv].TxdTail)
    {
        CS104MasterLink[pdrv].TxdTail = 0;
        CS104MasterLink[pdrv].TxdHead = 0;
    }
}

/**
  * @brief : Send Ctrl Frame.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_MASTER_S104SendCtrlFrame(uint8_t pdrv, uint8_t ctrl)
{
    struct CS104_MASTER_PAPCI *pFrame;

    pFrame = (struct CS104_MASTER_PAPCI *)&CS104MasterLink[pdrv].TxdBuf[CS104MasterLink[pdrv].TxdHead];

    pFrame->StartCode = CS104_MASTER_STARTCODE68;
    pFrame->Length = 0x04;
    pFrame->NS = ctrl;
    if (ctrl == CS104_MASTER_S_FRAME)
    {
        CS104MasterLink[pdrv].NRACK = CS104MasterLink[pdrv].NR;
        pFrame->NR = CS104MasterLink[pdrv].NR << 1;

        CS104_Master_KillTimer(pdrv, 2);
    }
    else
    {
        pFrame->NR = 0x00;

        CS104_Master_SetTimer(pdrv, 3);

        if (ctrl == CS104_MASTER_U_TESTFRACT)/*主动发送的 测试帧*/
        {
            CS104_Master_SetTimer(pdrv, 1); // U测试针应在t1时间内得到确认
            CS104MasterLink[pdrv].PeerNoAckNum++;
        }
    }

    CS104MasterLink[pdrv].TxdTail += 0x06;

    // 发送
    CS104_MASTER_WriteDataToMISI(pdrv);
}

/**
  * @brief : Decode S Frame.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_MASTER_DecodeSFrame(uint8_t pdrv)
{
    uint16_t tmp = 0;
    struct CS104_MASTER_PAPCI *pAPCI;
    struct CS104_MASTER_AppMsg msg;

    pAPCI = (struct CS104_MASTER_PAPCI *)CS104MasterLink[pdrv].RxdBuf;

    if (!CS104MasterLink[pdrv].Connect)
    {
        return;
    }

    tmp = CS104MasterLink[pdrv].PeerNoAckNum;
    if (!CS104_MASTER_PeerNoAckNumProcess(pdrv, pAPCI->NR >> 1))
    {
        return;
    }
    CS104MasterLink[pdrv].StopSendFlag = FALSE;

    msg.Cmd = CS104_MASTER_LINK_APPCON;
    msg.ConNum = tmp - CS104MasterLink[pdrv].PeerNoAckNum;
    msg.pData = NULL;
    msg.Len = 0;
    CS104_MASTER_AppProcess(pdrv, &msg);
}

/**
  * @brief : Decode U Frame.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_MASTER_DecodeUFrame(uint8_t pdrv)
{
    struct CS104_MASTER_PAPCI *pAPCI;

    pAPCI = (struct CS104_MASTER_PAPCI *)CS104MasterLink[pdrv].RxdBuf;

    switch (pAPCI->NS & 0xFF)
    {
    case CS104_MASTER_U_STARTDTCON: /*STARTDT启动确认*/
         CS104MasterApp[pdrv].Connect = TRUE;
        break;

    case CS104_MASTER_U_STOPDTCON: /*STOPDT停止确认*/
        CS104_Master_AppInitReset(pdrv);
        break;

    case CS104_MASTER_U_TESTFRACT:/*TESTFR测试帧*/
        CS104_MASTER_S104SendCtrlFrame(pdrv, CS104_MASTER_U_TESTFRCON);
        break;

    case CS104_MASTER_U_TESTFRCON:
        /*收到测试确认后 应清t1, 此时t1应该只有测试计时*/
        if(CS104MasterLink[pdrv].PeerNoAckNum)
        {
            CS104MasterLink[pdrv].PeerNoAckNum -= 1;//收到测试帧应答帧，清
        }
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
static void CS104_MASTER_DecodeIFrame(uint8_t pdrv)
{
    uint16_t tmp = 0;
    struct CS104_MASTER_AppMsg msg;
    struct CS104_MASTER_PAPCI *pAPCI;

    pAPCI = (struct CS104_MASTER_PAPCI *)CS104MasterLink[pdrv].RxdBuf;

    if (!CS104MasterLink[pdrv].Connect)
    {
        return;
    }

    tmp = CS104MasterLink[pdrv].PeerNoAckNum;
    if (!CS104_MASTER_PeerNoAckNumProcess(pdrv, pAPCI->NR >> 1))
    {
        CS104_MASTER_StopDTOper(pdrv);
        return;
    }

    if (CS104MasterLink[pdrv].NR == 32768)
    //if (CS104MasterLink[pdrv].NR == 63)
    {
        CS104MasterLink[pdrv].NR = 0;
    }
    else
    {
        CS104MasterLink[pdrv].NR++;
    }

    CS104MasterLink[pdrv].StopSendFlag = FALSE; // 清停止发送标志

    CS104_Master_KillTimer(pdrv, 1);
    CS104_Master_SetTimer(pdrv, 2);
    CS104_Master_SetTimer(pdrv, 3);

    msg.Cmd = CS104_MASTER_LINK_CALLDATA;
    msg.ConNum = tmp - CS104MasterLink[pdrv].PeerNoAckNum;
    msg.pData = &CS104MasterLink[pdrv].RxdBuf[6];
    msg.Len = pAPCI->Length - 4;

    CS104_MASTER_AppProcess(pdrv, &msg);
}

/**
  * @brief : Search Valid Frame.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-04-09][Chen][BRIEF]
  */
static void CS104_MASTER_SearchValidFrame(uint8_t pdrv)
{
    uint8_t stop = FALSE;
    struct CS104_MASTER_PAPCI *pAPCI = NULL;

    while ((CS104MasterLink[pdrv].RxdHead < CS104MasterLink[pdrv].RxdTail) && (!stop))
    {
        if (CS104MasterLink[pdrv].RxdStatus == 0)
        {
            if (CS104MasterLink[pdrv].RxdBuf[CS104MasterLink[pdrv].RxdHead] != CS104_MASTER_STARTCODE68)
            {
                CS104MasterLink[pdrv].RxdHead++;
                if (CS104MasterLink[pdrv].RxdHead >= CS104MasterLink[pdrv].RxdTail)
                {
//                    log_e("CS104MasterLink[%d].RxdHead:%d CS104MasterLink[%d].RxdTail:%d",pdrv,CS104MasterLink[pdrv].RxdHead,pdrv,CS104MasterLink[pdrv].RxdTail);
                    CS104MasterLink[pdrv].RxdLength = CS104MasterLink[pdrv].RxdTail - CS104MasterLink[pdrv].RxdHead;
                    memcpy(CS104MasterLink[pdrv].RxdBuf, &CS104MasterLink[pdrv].RxdBuf[CS104MasterLink[pdrv].RxdHead], CS104MasterLink[pdrv].RxdLength);
                    CS104MasterLink[pdrv].RxdHead = 0;
                    CS104MasterLink[pdrv].RxdTail = CS104MasterLink[pdrv].RxdLength;
                }
                continue;
            }

            // 找到后，清除无用的缓冲区报文
            if (CS104MasterLink[pdrv].RxdTail > CS104MasterLink[pdrv].RxdHead)
            {
                CS104MasterLink[pdrv].RxdLength = CS104MasterLink[pdrv].RxdTail - CS104MasterLink[pdrv].RxdHead;
                memcpy(CS104MasterLink[pdrv].RxdBuf, &CS104MasterLink[pdrv].RxdBuf[CS104MasterLink[pdrv].RxdHead], CS104MasterLink[pdrv].RxdLength);
                CS104MasterLink[pdrv].RxdHead = 0;
                CS104MasterLink[pdrv].RxdTail = CS104MasterLink[pdrv].RxdLength;
            }
            
            CS104MasterLink[pdrv].RxdStatus = 1;
        }

        if (CS104MasterLink[pdrv].RxdStatus == 1)
        {
            pAPCI = (struct CS104_MASTER_PAPCI *)CS104MasterLink[pdrv].RxdBuf;
            if (CS104MasterLink[pdrv].RxdLength >= sizeof(struct CS104_MASTER_PAPCI))
            {
                switch (pAPCI->NS & CS104_MASTER_U_FRAME)
                {
                case CS104_MASTER_S_FRAME:/*S帧*/
                    if (pAPCI->Length == 4)
                    {
                        CS104_Master_KillTimer(pdrv, 1);
                        CS104_Master_SetTimer(pdrv, 3);
                        CS104_MASTER_DecodeSFrame(pdrv);
                    }
                    break;

                case CS104_MASTER_U_FRAME:/*U帧*/
                    if (pAPCI->Length == 4)
                    {
                        CS104_Master_KillTimer(pdrv, 1);
                        CS104_Master_SetTimer(pdrv, 3);
                        CS104_MASTER_DecodeUFrame(pdrv);
                    }
                    break;

                default:
                    if ((CS104MasterLink[pdrv].RxdLength >= (pAPCI->Length + 2)))/*I帧*/
                    {
                        CS104_MASTER_DecodeIFrame(pdrv);
                    }
                    else
                    {
                        stop = TRUE;
                    }
                    break;
                }
            }
            else
            {
                stop = TRUE;
            }

            if (!stop)
            {
                CS104MasterLink[pdrv].RxdStatus = 2;
            }
        }

        if (CS104MasterLink[pdrv].RxdStatus == 2)
        {
            CS104MasterLink[pdrv].RxdLength = CS104MasterLink[pdrv].RxdTail - (pAPCI->Length + 2);
            memcpy(CS104MasterLink[pdrv].RxdBuf, &CS104MasterLink[pdrv].RxdBuf[pAPCI->Length + 2], CS104MasterLink[pdrv].RxdLength);
            CS104MasterLink[pdrv].RxdHead = 0;
            CS104MasterLink[pdrv].RxdTail = CS104MasterLink[pdrv].RxdLength;
            CS104MasterLink[pdrv].RxdStatus = 0;
            
            if (CS104MasterLink[pdrv].RxdTail > CS104MasterLink[pdrv].RxdHead)
            {
                stop = TRUE;
            }
        }
    }
}

/**
  * @brief : 远方遥控.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-06-20][Chen][new]
  */
static void CS104_MASTER_RemoteCtlSendPack(uint8_t pdrv)
{
    CS104MasterApp[pdrv].TX_Temp.status.Length = sizeof(CS104MasterApp[pdrv].TX_Temp.status) + sizeof(CS104MasterApp[pdrv].TX_Temp.Head) + sizeof(CS104MasterApp[pdrv].TX_Temp.Data.C_45);
    // CS104MasterApp[pdrv].TX_Temp.Head.TypeID = ;
    CS104MasterApp[pdrv].TX_Temp.Head.VSQ = 0;
    // CS104MasterApp[pdrv].TX_Temp.Head.COT_L = ;
    CS104MasterApp[pdrv].TX_Temp.Head.PubAddr_H = (uint8_t)((CS104_Master_Pad[pdrv].PubAddress >> 8) & 0xff);
    CS104MasterApp[pdrv].TX_Temp.Head.PubAddr_L = (uint8_t)(CS104_Master_Pad[pdrv].PubAddress & 0xff);
    // CS104MasterApp[pdrv].TX_Temp.Data.C_45.InfoAddr_L = ;
    // CS104MasterApp[pdrv].TX_Temp.Data.C_45.SCO = ;
    CS104_Master_getRemoteCtlInfo(pdrv,(uint8_t *)&CS104MasterApp[pdrv].TX_Temp);
}

/**
  * @brief : 站总召唤命令.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-04-17][Chen][new]
  */
static void CS104_MASTER_AllDataSendPack(uint8_t pdrv)
{
    CS104MasterApp[pdrv].TX_Temp.status.Length = sizeof(CS104MasterApp[pdrv].TX_Temp.status) + sizeof(CS104MasterApp[pdrv].TX_Temp.Head) + sizeof(CS104MasterApp[pdrv].TX_Temp.Data.C_100);
    CS104MasterApp[pdrv].TX_Temp.Head.TypeID = CS104_MASTER_C_IC_NA_1;
    CS104MasterApp[pdrv].TX_Temp.Head.VSQ = 0x01;
    CS104MasterApp[pdrv].TX_Temp.Head.COT_L = CS104_MASTER_COT_ACT;
    CS104MasterApp[pdrv].TX_Temp.Head.PubAddr_H = (uint8_t)((CS104_Master_Pad[pdrv].PubAddress >> 8) & 0xff);
    CS104MasterApp[pdrv].TX_Temp.Head.PubAddr_L = (uint8_t)(CS104_Master_Pad[pdrv].PubAddress & 0xff);
    CS104MasterApp[pdrv].TX_Temp.Data.C_100.InfoAddr_L = 0;
    CS104MasterApp[pdrv].TX_Temp.Data.C_100.QOI = 0x14;
}

/**
  * @brief : 电能量召唤命令.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-04-17][Chen][new]
  */
static void CS104_MASTER_ElectricDataSendPack(uint8_t pdrv)
{
    CS104MasterApp[pdrv].TX_Temp.status.Length = sizeof(CS104MasterApp[pdrv].TX_Temp.status) + sizeof(CS104MasterApp[pdrv].TX_Temp.Head) + sizeof(CS104MasterApp[pdrv].TX_Temp.Data.C_101);
    CS104MasterApp[pdrv].TX_Temp.Head.TypeID = CS104_MASTER_C_CI_NA_1;
    CS104MasterApp[pdrv].TX_Temp.Head.VSQ = 0x01;
    CS104MasterApp[pdrv].TX_Temp.Head.COT_L = CS104_MASTER_COT_ACT;
    CS104MasterApp[pdrv].TX_Temp.Head.PubAddr_H = (uint8_t)((CS104_Master_Pad[pdrv].PubAddress >> 8) & 0xff);
    CS104MasterApp[pdrv].TX_Temp.Head.PubAddr_L = (uint8_t)(CS104_Master_Pad[pdrv].PubAddress & 0xff);
    CS104MasterApp[pdrv].TX_Temp.Data.C_101.InfoAddr_L = 0;
    CS104MasterApp[pdrv].TX_Temp.Data.C_101.QOI = 0;
}

/**
  * @brief : 复位进程命令.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-04-17][Chen][new]
  */
static void CS104_MASTER_ResetProcessSendPack(uint8_t pdrv)
{
    CS104MasterApp[pdrv].TX_Temp.status.Length = sizeof(CS104MasterApp[pdrv].TX_Temp.status) + sizeof(CS104MasterApp[pdrv].TX_Temp.Head) + sizeof(CS104MasterApp[pdrv].TX_Temp.Data.C_105);
    CS104MasterApp[pdrv].TX_Temp.Head.TypeID = CS104_MASTER_C_RP_NA_1;
    CS104MasterApp[pdrv].TX_Temp.Head.VSQ = 1;
    CS104MasterApp[pdrv].TX_Temp.Head.COT_L = CS104_MASTER_COT_ACT;
    CS104MasterApp[pdrv].TX_Temp.Head.PubAddr_H = (uint8_t)((CS104_Master_Pad[pdrv].PubAddress >> 8) & 0xff);
    CS104MasterApp[pdrv].TX_Temp.Head.PubAddr_L = (uint8_t)(CS104_Master_Pad[pdrv].PubAddress & 0xff);
    CS104MasterApp[pdrv].TX_Temp.Data.C_105.QRP = 1;
}

/**
  * @brief : 时钟同步命令.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-04-17][Chen][new]
  */
static void CS104_MASTER_ClockSynSendPack(uint8_t pdrv)
{
    struct ds_privateTime getTime;
    CS104MasterApp[pdrv].TX_Temp.status.Length = sizeof(CS104MasterApp[pdrv].TX_Temp.status) + sizeof(CS104MasterApp[pdrv].TX_Temp.Head) + sizeof(CS104MasterApp[pdrv].TX_Temp.Data.C_103);
    CS104MasterApp[pdrv].TX_Temp.Head.TypeID = CS104_MASTER_C_CS_NA_1;
    CS104MasterApp[pdrv].TX_Temp.Head.VSQ = 1;
    CS104MasterApp[pdrv].TX_Temp.Head.COT_L = CS104_MASTER_COT_ACT;
    CS104MasterApp[pdrv].TX_Temp.Head.PubAddr_H = (uint8_t)((CS104_Master_Pad[pdrv].PubAddress >> 8) & 0xff);
    CS104MasterApp[pdrv].TX_Temp.Head.PubAddr_L = (uint8_t)(CS104_Master_Pad[pdrv].PubAddress & 0xff);
    CS104MasterApp[pdrv].TX_Temp.Data.C_103.InfoAddr_L = 0;
    CS104_Master_SYNProcess(pdrv,&getTime);
    memcpy(&CS104MasterApp[pdrv].TX_Temp.Data.C_103.CP56Time2a,&getTime.time_cp56,sizeof(struct ds_cp56Time2a));
}

/**
  * @brief : 测试命令.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-04-17][Chen][new]
  */
static void CS104_MASTER_TestSendPack(uint8_t pdrv)
{
    CS104MasterApp[pdrv].TX_Temp.status.Length = sizeof(CS104MasterApp[pdrv].TX_Temp.status) + sizeof(CS104MasterApp[pdrv].TX_Temp.Head) + sizeof(CS104MasterApp[pdrv].TX_Temp.Data.C_104);
    CS104MasterApp[pdrv].TX_Temp.Head.TypeID = CS104_MASTER_C_TS_NA_1;
    CS104MasterApp[pdrv].TX_Temp.Head.VSQ = 1;
    CS104MasterApp[pdrv].TX_Temp.Head.COT_L = CS104_MASTER_COT_ACT;
    CS104MasterApp[pdrv].TX_Temp.Head.PubAddr_H = (uint8_t)((CS104_Master_Pad[pdrv].PubAddress >> 8) & 0xff);
    CS104MasterApp[pdrv].TX_Temp.Head.PubAddr_L = (uint8_t)(CS104_Master_Pad[pdrv].PubAddress & 0xff);
    CS104MasterApp[pdrv].TX_Temp.Data.C_104.InfoAddr_L = 0;
    CS104MasterApp[pdrv].TX_Temp.Data.C_104.FBP_L = 0xAA;
    CS104MasterApp[pdrv].TX_Temp.Data.C_104.FBP_H = 0x55;
}

/**
  * @brief : 定值操作命令.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-04-17][Chen][new]
  */
static void CS104_MASTER_SettingOperSendPack(uint8_t pdrv, uint8_t opertype)
{
//  CS104_MASTER_I_SWITCHAREA   // 切换定值区命令
//  CS104_MASTER_I_READAREA     // 读定值区号命令
//  CS104_MASTER_I_READSETTING  // 读定值参数命令
//  CS104_MASTER_I_WRITESETTING // 写定值参数命令
}

/**
  * @brief : 文件传输.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-04-17][Chen][new]
  */
static void CS104_MASTER_FileTransferSendPack(uint8_t pdrv)
{
    
}

/**
  * @brief : 软件升级.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-04-17][Chen][new]
  */
static void CS104_MASTER_SoftUpSendPack(uint8_t pdrv)
{

}

/**
  * @brief : Link Send Process.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_MASTER_LinkSendProcess(uint8_t pdrv, uint8_t *pData, uint8_t Len)
{
    struct CS104_MASTER_PAPCI *pFrame;

    pFrame = (struct CS104_MASTER_PAPCI *)&CS104MasterLink[pdrv].TxdBuf[CS104MasterLink[pdrv].TxdHead];

    if (Len == 0)
    {
        return;
    }
    CS104MasterLink[pdrv].NRACK = CS104MasterLink[pdrv].NR;

    pFrame->StartCode = CS104_MASTER_STARTCODE68;
    pFrame->Length = Len + 0x04;
    pFrame->NS = CS104MasterLink[pdrv].NS << 1;
    pFrame->NR = CS104MasterLink[pdrv].NR << 1;
    memcpy((uint8_t *)(pFrame + 1), pData, Len);
    CS104MasterLink[pdrv].TxdTail += Len + 0x06;

    CS104MasterLink[pdrv].PeerNoAckNum++;
    CS104MasterLink[pdrv].NS++;
    if (CS104MasterLink[pdrv].NS > 32767)
    {
        CS104MasterLink[pdrv].NS = 0;
    }

    if (CS104MasterLink[pdrv].PeerNoAckNum >= CS104_MASTER_K) // 未被确认的I帧数量达到K个时  停止发送
    {
        CS104MasterLink[pdrv].StopSendFlag = TRUE;
    }
    // 发送
    CS104_Master_KillTimer(pdrv, 2);
    CS104_Master_SetTimer(pdrv, 3);
    CS104_MASTER_WriteDataToMISI(pdrv);
    CS104_Master_SetTimer(pdrv, 1); //
}

/**
  * @brief : 根据时间轮询发送的数据.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-06-12][Chen][New]
  */
static void CS104_Master_packDataOfTime(uint8_t pdrv)
{
    if(CS104MasterApp[pdrv].sendDateType & CS104_MASTER_REMOTECTL)/*远方遥控*/
    {
        CS104_MASTER_RemoteCtlSendPack(pdrv);
        CS104_Master_clearSendDateType(pdrv,CS104_MASTER_REMOTECTL);
    }
    else if(CS104MasterApp[pdrv].sendDateType & CS104_MASTER_CLOCKSYN)/*时钟同步命令*/
    {
        CS104_MASTER_ClockSynSendPack(pdrv);/*时钟同步命令*/
        CS104_Master_clearSendDateType(pdrv,CS104_MASTER_CLOCKSYN);/*清除标志*/
    }
    else if(CS104MasterApp[pdrv].sendDateType & CS104_MASTER_ALLDATA)/*总召命令*/
    {
        CS104_MASTER_AllDataSendPack(pdrv);
        CS104_Master_clearSendDateType(pdrv,CS104_MASTER_ALLDATA);
    }
    else if(CS104MasterApp[pdrv].sendDateType & CS104_MASTER_ELEDATA)/*电度总召命令*/
    {
        CS104_MASTER_ElectricDataSendPack(pdrv);
        CS104_Master_clearSendDateType(pdrv,CS104_MASTER_ELEDATA);
    }
    else if(CS104MasterApp[pdrv].sendDateType & CS104_MASTER_RESETPRO)/*复位进程命令*/
    {
        CS104_MASTER_ResetProcessSendPack(pdrv);
        CS104_Master_clearSendDateType(pdrv,CS104_MASTER_RESETPRO);
    }
    else if(CS104MasterApp[pdrv].sendDateType & CS104_MASTER_TEST)/*测试命令*/
    {
        CS104_MASTER_TestSendPack(pdrv);
        CS104_Master_clearSendDateType(pdrv,CS104_MASTER_TEST);
    }
    else if(CS104MasterApp[pdrv].sendDateType & CS104_MASTER_SWICHAREA)/*切换定值区命令*/
    {
        CS104_MASTER_SettingOperSendPack(pdrv,CS104_MASTER_I_SWITCHAREA);
        CS104_Master_clearSendDateType(pdrv,CS104_MASTER_SWICHAREA);
    }
    else if(CS104MasterApp[pdrv].sendDateType & CS104_MASTER_RDAREA)/*读定值区号命令*/
    {
        CS104_MASTER_SettingOperSendPack(pdrv,CS104_MASTER_I_READAREA);
        CS104_Master_clearSendDateType(pdrv,CS104_MASTER_RDAREA);
    }
    else if(CS104MasterApp[pdrv].sendDateType & CS104_MASTER_RDSETIG)/*读定值参数命令*/
    {
        CS104_MASTER_SettingOperSendPack(pdrv,CS104_MASTER_I_READSETTING);
        CS104_Master_clearSendDateType(pdrv,CS104_MASTER_RDSETIG);
    }
    else if(CS104MasterApp[pdrv].sendDateType & CS104_MASTER_WRSETIG)/*写定值参数命令*/
    {
        CS104_MASTER_SettingOperSendPack(pdrv,CS104_MASTER_I_WRITESETTING);
        CS104_Master_clearSendDateType(pdrv,CS104_MASTER_WRSETIG);
    }
    else if(CS104MasterApp[pdrv].sendDateType & CS104_MASTER_FILETRANS)/*文件传输命令*/
    {
        CS104_MASTER_FileTransferSendPack(pdrv);
        CS104_Master_clearSendDateType(pdrv,CS104_MASTER_FILETRANS);
    }
    else if(CS104MasterApp[pdrv].sendDateType & CS104_MASTER_SOFTUP)/*软件升级命令*/
    {
        CS104_MASTER_SoftUpSendPack(pdrv);
        CS104_Master_clearSendDateType(pdrv,CS104_MASTER_SOFTUP);
    }
}

/**
  * @brief : Net Tes.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_MASTER_NetTest(uint8_t pdrv)
{
    if(CS104_Master_CheckLink(pdrv))
    {
        CS104MasterLink[pdrv].Connect = TRUE;
    }
    else
    {
        CS104_Master_AppInitReset(pdrv);
    }
}

/**
  * @brief : Send Process.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-04-09][Chen][BRIEF]
  */
static void CS104_MASTER_SendProcess(uint8_t pdrv)
{
    //数据打包
    CS104_Master_packDataOfTime(pdrv);

    /*发送*/
    if (CS104MasterApp[pdrv].TX_Temp.status.Length)
    {
        CS104MasterApp[pdrv].TxLen = CS104MasterApp[pdrv].TX_Temp.status.Length - sizeof(CS104MasterApp[pdrv].TX_Temp.status);
        memcpy(&CS104MasterApp[pdrv].TxMsg, &CS104MasterApp[pdrv].TX_Temp.Head, CS104MasterApp[pdrv].TxLen);
        CS104_MASTER_LinkSendProcess(pdrv, (uint8_t*)&CS104MasterApp[pdrv].TxMsg, CS104MasterApp[pdrv].TxLen);
        memset((uint8_t *)&CS104MasterApp[pdrv].TX_Temp, 0, sizeof(CS104_MASTER_PASDU));
    }
}

/**
  * @brief : Link On Timer.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static void CS104_MASTER_LinkOnTimer(uint8_t pdrv)
{
    uint8_t i = 0;
    uint16_t num = 0;
    uint64_t timeOut;
    uint64_t timeMs = CS104_Master_getAlarmOfMs(pdrv);
    static uint8_t sendI_InitFlg = FALSE;

    if(CS104MasterApp[pdrv].Connect == FALSE)/*启动帧*/
    {
        CS104MasterLink[pdrv].Tick.timeEndSendUSTART = timeMs;	//
        timeOut = CS104MasterLink[pdrv].Tick.timeEndSendUSTART - CS104MasterLink[pdrv].Tick.timeBeginSendUSTART;
        if(timeOut >= CS104_Master_Pad[pdrv].sendUSTARTClock)	//系统启动大于1S
        {
            CS104_MASTER_S104SendCtrlFrame(pdrv,CS104_MASTER_U_STARTDTACT);		//启动针发送U针
            CS104MasterLink[pdrv].Tick.timeBeginSendUSTART = timeMs;/*重新计时*/
        }
        sendI_InitFlg = FALSE;
        return;
    }
    
    if (CS104MasterLink[pdrv].Tick.Flag & CS104_MASTER_T0FLAG)
    {
        CS104MasterLink[pdrv].Tick.timeEndT0 = timeMs;
        timeOut = CS104MasterLink[pdrv].Tick.timeEndT0 - CS104MasterLink[pdrv].Tick.timeBeginT0;

        if (timeOut > CS104_Master_Pad[pdrv].TickValue[0])
        {
//            log_e("T0 reset(%d)",pdrv);
            CS104_Master_KillTimer(pdrv, 0);
            CS104_MASTER_StopDTOper(pdrv);
        }
    }

    if (CS104MasterLink[pdrv].Tick.Flag & CS104_MASTER_T1FLAG)
    {
        for (i = 0; i < CS104MasterLink[pdrv].PeerNoAckNum; i++)
        {
            CS104MasterLink[pdrv].Tick.timeEndT1[i] = timeMs;
            timeOut = CS104MasterLink[pdrv].Tick.timeEndT1[i] - CS104MasterLink[pdrv].Tick.timeBeginT1[i];
            if (timeOut > CS104_Master_Pad[pdrv].TickValue[1])
            {
//                log_e("T1 reset(%d)",pdrv);
//                log_w("T1超时时间(%d):%llu end:%llu begin:%llu ",pdrv,timeOut,CS104MasterLink[pdrv].Tick.timeEndT1[i],CS104MasterLink[pdrv].Tick.timeBeginT1[i]);
                CS104_Master_KillTimer(pdrv, 1);
                CS104_MASTER_StopDTOper(pdrv);
            }
        }
    }

    if (CS104MasterLink[pdrv].NRACK <= CS104MasterLink[pdrv].NR)
    {
        num = CS104MasterLink[pdrv].NR - CS104MasterLink[pdrv].NRACK;
    }
    else
    {
        num = 32767 + CS104MasterLink[pdrv].NR - CS104MasterLink[pdrv].NRACK;
    }

    if (num >= CS104_MASTER_W)
    {
//        log_i("CS104_Master_KillTimer(%d)",pdrv);
        CS104MasterLink[pdrv].NRACK = CS104MasterLink[pdrv].NR;
        CS104_Master_KillTimer(pdrv, 2);
        CS104_MASTER_S104SendCtrlFrame(pdrv, CS104_MASTER_S_FRAME);
    }

    if (CS104MasterLink[pdrv].Tick.Flag & CS104_MASTER_T2FLAG)
    {
        CS104MasterLink[pdrv].Tick.timeEndT2 = timeMs;
        timeOut = CS104MasterLink[pdrv].Tick.timeEndT2 - CS104MasterLink[pdrv].Tick.timeBeginT2;
        if (timeOut > CS104_Master_Pad[pdrv].TickValue[2])
        {
//            log_e("T2 reset(%d)",pdrv);
//            log_w("T2超时时间(%d):%llu end:%llu begin:%llu ",pdrv,timeOut,CS104MasterLink[pdrv].Tick.timeEndT2,CS104MasterLink[pdrv].Tick.timeBeginT2);
            CS104_Master_KillTimer(pdrv, 2);
            CS104_MASTER_S104SendCtrlFrame(pdrv, CS104_MASTER_S_FRAME);			//S帧
        }
    }

    if (CS104MasterLink[pdrv].Tick.Flag & CS104_MASTER_T3FLAG)
    {
        CS104MasterLink[pdrv].Tick.timeEndT3 = timeMs;
        timeOut = CS104MasterLink[pdrv].Tick.timeEndT3 - CS104MasterLink[pdrv].Tick.timeBeginT3;
        if (timeOut > CS104_Master_Pad[pdrv].TickValue[3])
        {
//            log_e("T3 reset(%d)",pdrv);
//            log_w("PeerNoAckNum:%d",CS104MasterLink[pdrv].PeerNoAckNum);
//            log_w("T3超时时间(%d):%llu end:%llu begin:%llu ",pdrv,timeOut,CS104MasterLink[pdrv].Tick.timeEndT3,CS104MasterLink[pdrv].Tick.timeBeginT3);
            CS104_Master_KillTimer(pdrv, 3);

            if (CS104MasterLink[pdrv].PeerNoAckNum == 0)
            {
                CS104_MASTER_S104SendCtrlFrame(pdrv, CS104_MASTER_U_TESTFRACT);	//呼吸U帧
            }
            else if (!(CS104MasterLink[pdrv].Tick.Flag & CS104_MASTER_T1FLAG))
            {
                CS104_Master_SetTimer(pdrv, 1);
            }
        }
    }

    if(CS104MasterApp[pdrv].FirstInit < 2)/*发送I帧(初始化)：对时0 总召1*/
    {
        CS104_Master_sendDateType(pdrv,CS104MasterApp[pdrv].FirstInit);
        CS104MasterApp[pdrv].FirstInit++;
        return;
    }

    if(sendI_InitFlg == FALSE)/*建立链路后启动计时*/
    {
        for(i = 0; i < SEND_S_FLAG_MAX; i++)
        {
            CS104MasterLink[pdrv].Tick.timeBeginSendI[i] = timeMs;
        }
        sendI_InitFlg = TRUE;//初始计时
    }
    if(sendI_InitFlg == TRUE)
    {
        for(i = 0; i < SEND_S_FLAG_MAX; i++)
        {
            if(CS104_Master_Pad[pdrv].sendIFlag[i] == TRUE)			//定时发送对应标识为True命令
            {
                CS104MasterLink[pdrv].Tick.timeEndSendI[i] = timeMs;
                timeOut = CS104MasterLink[pdrv].Tick.timeEndSendI[i] - CS104MasterLink[pdrv].Tick.timeBeginSendI[i];
                if(timeOut >= CS104_Master_Pad[pdrv].sendIClock[i])
                {
                    CS104_Master_sendDateType(pdrv,i);					//发送类型标志,
                    CS104MasterLink[pdrv].Tick.timeBeginSendI[i] = timeMs;/*重新计时*/
                }
            }
        }
    }
}

/**
  * @brief : Read MISI Data.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-04-09][Chen][BRIEF]
  */
static void CS104_MASTER_ReadMISIData(uint8_t pdrv, uint8_t Flag)
{
    uint16_t count = 0;

    /*读取接收缓冲区*/
    if (CS104MasterLink[pdrv].RxdTail < sizeof(CS104MasterLink[pdrv].RxdBuf) - CS104_MASTER_FRAMEBUFSIZE)
    {
        count = CS104_Master_ReadData(pdrv, (uint8_t *)&CS104MasterLink[pdrv].RxdBuf[CS104MasterLink[pdrv].RxdTail], CS104_MASTER_FRAMEBUFSIZE);
    }

    if (Flag)
    {
        CS104MasterLink[pdrv].RxdTail += count;
        CS104MasterLink[pdrv].RxdLength += count;
    }

    //    if(!DLT634_ChannelToMonitor_MASTER_SearchMonitorFrame(pdrv, DLT634_5104, CS104MasterLink[pdrv].RxdBuf, 256))
    {
        CS104_MASTER_SearchValidFrame(pdrv);
    }
}

/* PUBLIC FUNCTION PROTOTYPES ------------------------------------------------*/
/**
  * @brief : 遥控交互发送.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-06-12][Chen][New]
  */
uint8_t CS104_Master_otherYkSendData(uint8_t pdrv)
{
    if(CS104MasterLink[pdrv].Connect)
    {
        CS104_Master_sendDateType(pdrv,REMOTE);
        return TRUE;
    }
    return FALSE;
}

/**
  * @brief : 对时交互发送.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-06-24][Chen][New]
  */
uint8_t CS104_Master_otherSYNSendData(uint8_t pdrv)
{
    if(CS104MasterLink[pdrv].Connect)
    {
//        log_w("CS104_Master_sendDateType CLOCKSYN(%d)",pdrv);
        CS104_Master_sendDateType(pdrv,CLOCKSYN);
        return TRUE;
    }
    return FALSE;
}

/**
  * @brief : App Init.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS104_Master_Appinit(uint8_t pdrv)
{
    if (CS104MasterLink == NULL)
    {
        CS104MasterLink = malloc(sizeof(struct CS104_MASTER_LINKINFO) * CS104_MASTER_VOLUMES);
        memset(CS104MasterLink, 0, sizeof(struct CS104_MASTER_LINKINFO) * CS104_MASTER_VOLUMES);
    }
    if (CS104MasterApp == NULL)
    {
        CS104MasterApp = malloc(sizeof(struct CS104_MASTER_APPINFO) * CS104_MASTER_VOLUMES);
        memset(CS104MasterApp, 0, sizeof(struct CS104_MASTER_APPINFO) * CS104_MASTER_VOLUMES);
    }
    if (CS104_Master_Param == NULL)
    {
        CS104_Master_Param = malloc(sizeof(struct sMasterParam) * CS104_MASTER_VOLUMES);
        memset(CS104_Master_Param, 0, sizeof(struct sMasterParam) * CS104_MASTER_VOLUMES);
    }
    CS104_Master_AppInitReset(pdrv);

    return (TRUE);
}

/**
  * @brief : Clock.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-04-09][Chen][BRIEF]
  */
uint8_t CS104_Master_Clock(uint8_t pdrv)
{
    CS104_MASTER_NetTest(pdrv);/*网络判断*/

    if(CS104MasterLink[pdrv].Connect)
    {
        CS104_MASTER_ReadMISIData(pdrv, TRUE);/*接收数据*/

        CS104_MASTER_SendProcess(pdrv);/*发送数据*/

        CS104_MASTER_LinkOnTimer(pdrv);/*定时器*/
    }

    CS104_Master_writeCommunicationSoe(pdrv,CS104MasterApp[pdrv].Connect);
    
    return (CS104MasterApp[pdrv].Connect);
}

/* END OF FILE ---------------------------------------------------------------*/
