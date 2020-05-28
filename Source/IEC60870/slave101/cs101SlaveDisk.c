/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      cs101SlaveDisk.c
  * @brief:			101子站接口
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2018-11-07
  * @update:    [2018-11-07][Chen][newly increased]
  */
#define LOG_TAG    "CS101_Slave_Disk"
/* INCLUDE FILES -------------------------------------------------------------*/
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include "../port/iec_interface.h"
#include "cs101SlaveApp.h"
#include "cs101SlaveDisk.h"
//#include "encrypt_disk.h"
#include "string.h"
//#include "dataSwitch_point_table.h"
#include "../serial/serial.h"
//#include "elog.h"
#include "../file/fileOperate.h"
// #include "example/example.h"

/* PRIVATE VARIABLES ---------------------------------------------------------*/
static uint8_t file_array[CS101_SLAVE_DISK_VOLUMES][256];
static uint8_t temp_array[CS101_SLAVE_DISK_VOLUMES][256];
static uint8_t g_CommunicatFileOper = 0x00;/*文件操作*/

static uint8_t thread_Stop = FALSE;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static uint16_t writeNum = 0;/*写入定值数量*/
static uint8_t preOperation = FALSE;
static struct tagValueDZ writeDZbuf[512];/*写入定值BUFF*/
static uint8_t dzPreFlag = FALSE;
static uint64_t dzPreTimeOutStart;
static uint64_t dzPreTimeOutEnd;

//static struct tagControlCfg info;//下发数据

struct CS101_SLAVE_DISK *CS101SlaveDisk = NULL;
static struct tagOverallCfg *allInfoDisk = NULL;
/* PUBLIC FUNCTION PROTOTYPES ------------------------------------------------*/
/**
  * @brief : read data from drv.
  * @param : [pdrv]
  * @param : [pbuf]
  * @param : [count]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint16_t CS101_Slave_Read_drv(uint8_t port, uint8_t *pbuf, uint16_t count)
{
    int len=0;
    uint8_t pdrv;
    // uint16_t i;

//    log_i("CS101_Slave_Read_drv");
    for(pdrv=0;pdrv<CS101_SLAVE_DISK_VOLUMES;pdrv++)
    {
        if(CS101_Slave_Pad[pdrv].Port == port)
        {
        	len = SerialPort_readByte(port,pbuf,count);
        }
    }
    if(len == -1)
    {
//    	log_w("len == -1");
    	len = 0;
    }

  //  if(len)
  //  {
  //       log_w("read %d data",len);
  //       for(i=0;i<len;i++)
  //       {
  //           printf("%02x ",pbuf[i]);
  //       }
  //       printf("\n");
  //  }

	return(len);
}

/**
  * @brief : write data to drv.
  * @param : [pdrv]
  * @param : [pbuf]
  * @param : [count]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint16_t CS101_Slave_Write_drv(uint8_t port, uint8_t *pbuf, uint16_t count)
{
    uint8_t pdrv;

    for(pdrv=0;pdrv<CS101_SLAVE_DISK_VOLUMES;pdrv++)
    {
        if(CS101_Slave_Pad[pdrv].Port == port)
        {
            SerialPort_write(port, pbuf, 0, count);
        }
    }
   if(count)
   {
//   	log_w("write %d data",count);
//   	for(uint16_t i=0;i<count;i++)
//   	{
//   		printf("%02x ",pbuf[i]);
//   	}
//   	printf("\n");
   }
	return(count);
}

/**
  * @brief : read data from drv.
  * @param : [pdrv]
  * @param : [pbuf]
  * @param : [count]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint16_t cs101SlaveReadData(uint8_t pdrv, uint8_t *pbuf, uint16_t count)
{
    uint16_t len = 0;
    if(CS101_Slave_Pad[pdrv].Encrypt == 0)
    {
        len = CS101_Slave_Read_drv(CS101_Slave_Pad[pdrv].Port, pbuf, count);
    }
    else
    {
//        len = Encrypt_Readx(pbuf, CS101_Slave_Pad[pdrv].Port);
    }

	return(len);
}

/**
  * @brief : write data to drv.
  * @param : [pdrv]
  * @param : [pbuf]
  * @param : [count]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint16_t CS101_Slave_WriteData(uint8_t pdrv, uint8_t *pbuf, uint16_t count)
{
    if(CS101_Slave_Pad[pdrv].Encrypt == 0)
    {
        return(CS101_Slave_Write_drv(CS101_Slave_Pad[pdrv].Port,pbuf,count));
    }
    else
    {
        if(pbuf[0] == CS101_SLAVE_STARTCODE10)
        {
//			return(Encrypt_WriteX(pbuf, count, 0x00, CS101_Slave_Pad[pdrv].Port));
        }
        else if(pbuf[0] == CS101_SLAVE_STARTCODE68)
        {
            switch(pbuf[4+1+CS101_Slave_Pad[pdrv].LinkAddrSize])
            {
                case CS101_SLAVE_C_SC_NA_1:
                case CS101_SLAVE_C_SC_NB_1:
                case CS101_SLAVE_C_WS_NA_1:
				case CS101_SLAVE_F_SR_NA_1:
					return (0);						//TXL添加
//					if((pbuf[4+1+CS101_Slave_Pad[pdrv].LinkAddrSize+1+1+CS101_Slave_Pad[pdrv].ASDUCotSize+CS101_Slave_Pad[pdrv].ASDUAddrSize+2]&0x80)&&
//                        (pbuf[4+1+CS101_Slave_Pad[pdrv].LinkAddrSize+1+1]==CS101_SLAVE_COT_ACTCON))
//                    {return(Encrypt_WriteX(pbuf, count, 0x82, CS101_Slave_Pad[pdrv].Port));}	//启动确认
//                    else
//                    {return(Encrypt_WriteX(pbuf, count, 0x80, CS101_Slave_Pad[pdrv].Port));}	//结束确认
//
//                default:
//                    return(Encrypt_WriteX(pbuf, count, 0x80, CS101_Slave_Pad[pdrv].Port));
            }
        }
    }
    return(0);
}

/**
  * @brief : all data command.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS101_Slave_C_IC(uint8_t pdrv, uint8_t *pbuf)//总召					//解析主端
{
	CS101SlaveDisk[pdrv].TelesignalCosOut = CS101SlaveDisk[pdrv].TelesignalCosIn;
	CS101SlaveDisk[pdrv].TelemetryNVAOut = CS101SlaveDisk[pdrv].TelemetryNVAIn;
}

/**
  * @brief : 检查遥控信息.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [2019-05-30][Chen][new]
  */
static uint8_t CS101_Slave_CheckCtlRemoteCfg(uint8_t pdrv, uint8_t *pbuf,tagCtlRomteCfg_t tmp)
{
	tmp->addr = pbuf[8]|(pbuf[9]<<8);//遥控信息对象地址
	tmp->cot  = pbuf[4]|(pbuf[5]<<8);//传送原因
	tmp->oper = pbuf[10];//操作
//    log_w("YK checkParam: addr:0x%02x,value:0x%02x",tmp->addr,tmp->oper);
  if(pbuf[2] == CS101_SLAVE_C_SC_NA_1)	/*单点*/
	{
		tmp->value = (pbuf[10]&0x01)+1;
	}
	else
	{
		if((tmp->oper&0x03)==0||(tmp->oper&0x03)==3)
		{
			temp_array[pdrv][4] = CS101_SLAVE_COT_ACTCON|CS101_SLAVE_COT_NO;/*双点错误*/
			CS101_Slave_Reply(pdrv, temp_array[pdrv]);
			return FALSE;
		}
		tmp->value = pbuf[10]&0x03;
	}
//	if((allInfoDisk->var.telecontrol.firstPoint + (tmp->addr-TELECONTROL_STARTADDR))->IsNegated == TRUE)/*取反*/			TXL注释
//	{
//			tmp->value = (~tmp->value)&0x03;
//	}
    
	if(!((tmp->cot == CS101_SLAVE_COT_ACT)||(tmp->cot == CS101_SLAVE_COT_DEACT)))
	{
		temp_array[pdrv][4] = CS101_SLAVE_COT_COTERR;	/*传输原因错误*/
		CS101_Slave_Reply(pdrv, temp_array[pdrv]);
		return FALSE;
	}

//	if(tmp->addr < TELECONTROL_STARTADDR || tmp->addr > TELECONTROL_STARTADDR + allInfoDisk->var.telecontrol.maxNum)	TXL注释
//	{
//		temp_array[pdrv][4] = CS101_SLAVE_COT_ADDRERR;	/*信息体地址错误*/
//		CS101_Slave_Reply(pdrv, temp_array[pdrv]);
//		return FALSE;
//	}

	if((g_CommunicatFlag[COM_YK] == COMMUNICATLOCKSTATUS)||(g_CommunicatFlag[COM_YK] == CS101_Slave_Pad[pdrv].DevId))
	{
//		log_i("g_CommunicatFlag[COM_YK] TRUE:%02x",g_CommunicatFlag[COM_YK]);
	}
	else
	{
		temp_array[pdrv][4] = CS101_SLAVE_COT_ADDRERR;
		CS101_Slave_Reply(pdrv, temp_array[pdrv]);
		return FALSE;
	}

	if(tmp->oper&0x80)//预制
	{
//		if((Encrypt_ApplyType_Read(pdrv) != 0x05)&&(CS101_Slave_Pad[pdrv].Encrypt == 1))//加密数据判断
//		{
//			Encrypt_ReturnFaultInfo_IN(0x1f,0x91,0x01,CS101_Slave_Pad[pdrv].DevId);
//			return FALSE;
//		}
	}
	else//执行
	{
//		if((Encrypt_ApplyType_Read(pdrv) != 0x07)&&(CS101_Slave_Pad[pdrv].Encrypt == 1))//加密数据判断
//		{
//			Encrypt_ReturnFaultInfo_IN(0x1f,0x91,0x01,CS101_Slave_Pad[pdrv].DevId);
//			return FALSE;
//		}
	}
	
	return TRUE;
}
/**
  * @brief : remote command.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][Chen][增加加密判断]
  */
void CS101_Slave_C_SC(uint8_t pdrv, uint8_t *pbuf)//遥控							//解析主端
{//LENTH/Lock_ID/TypeID/VSQ/COT_L/COT_H/PubAddr_L/PubAddr_H/InfoAddr_L/InfoAddr_H/DCO_L/DCO_H

//	static uint8_t reinfo = 0;
	struct tagCtlRomteCfg tmp;//临时结构体
//	struct tagControlRemoteCfg ykCmd;														TXL注释

//	info.pdata = &ykCmd;																	TXL注释
	memcpy(temp_array[pdrv],pbuf,pbuf[0]);										//将得到ASDU的值赋到临时变量

	if(CS101_Slave_CheckCtlRemoteCfg(pdrv,pbuf,&tmp) == FALSE)/*数据检查*/
	{
		return;
	}

	/*参数赋值*/
//	info.com.dir = allInfoDisk->def.message_DOWN;											TXL注释
//	info.com.send = CS101_Slave_Pad[pdrv].ModuleSN;//模块ID
//	info.com.tpye = allInfoDisk->def.type_REMOTE;
//	info.pBackData = temp_array[pdrv]; // 为发起方暂存数据
//	info.backDataLen = pbuf[0]; // 为发起方暂存数据长度
//    for(int i=0; i<pbuf[0]; i++)
//        log_i("temp_array[pdrv][%d]:%02x,info.pBackData:%02x",i,temp_array[pdrv][i],*(uint8_t*)(info.pBackData+i));
//	ykCmd.addr = (allInfoDisk->var.telecontrol.firstPoint + (tmp.addr-TELECONTROL_STARTADDR))->AddrUsed;
//	ykCmd.sta  = allInfoDisk->def.control_RES_RELEASE;
//	ykCmd.work = tmp.value;																	TXL注释
//	log_w("tmp.addr:0x%x",tmp.addr);
//	log_w("ykCmd.addr:0x%x",ykCmd.addr);
	/*逻辑判断*/
	if((tmp.cot == CS101_SLAVE_COT_ACT) && (tmp.oper&0x80))/*预制*/
	{
//		log_i("YK select");
//		ykCmd.cmd = allInfoDisk->def.control_OPREAT_CHOICE;									TXL注释
	}
	else if((tmp.cot == CS101_SLAVE_COT_ACT) && ((tmp.oper&0x80) == 0))/*执行*/
	{
//		log_i("YK operate");
//		ykCmd.cmd = allInfoDisk->def.control_OPREAT_EXECUTE;								TXL注释
	}
	else if((tmp.cot == CS101_SLAVE_COT_DEACT) && ((tmp.oper&0x80) == 0))/*撤销*/
	{
//		log_i("YK cancel");
//		ykCmd.cmd = allInfoDisk->def.control_OPREAT_CANCEL;									TXL注释
	}
	else
	{
		temp_array[pdrv][4] = CS101_SLAVE_COT_ADDRERR;	/*未知错误*/
		CS101_Slave_Reply(pdrv, temp_array[pdrv]);
		return;
	}

//    log_i("((tagControlRemoteCfg_t)info.pdata)->sta = %d ",((tagControlRemoteCfg_t)info.pdata)->sta);			TXL注释
//
//	reinfo = allInfoDisk->fun.operate_Control_info(&info);/*发送遥控命令 */

//    log_i("reinfo= %d",reinfo);

//	if(reinfo == allInfoDisk->def.control_RES_WAIT)/*等待*/											TXL注释
//	{
//		log_e("YK select:control_RES_WAIT");
//		return;
//	}
//	else if(reinfo == allInfoDisk->def.control_RES_FAIL)/*失败*/
//	{
//		log_w("YK select:RETURN_ERROR");
//		temp_array[pdrv][4] = CS101_SLAVE_COT_ACTCON|CS101_SLAVE_COT_NO;//选择|执行
//		if((tmp.cot == CS101_SLAVE_COT_DEACT) && ((tmp.oper&0x80) == 0))//撤销
//		{
//			temp_array[pdrv][4] = CS101_SLAVE_COT_DEACTCON|CS101_SLAVE_COT_NO;
//		}
//		CS101_Slave_Reply(pdrv, temp_array[pdrv]);
//	}
//	else if(reinfo == allInfoDisk->def.control_RES_SUCCEED)/*成功*/
//	{
//		log_w("YK select:RETURN_TRUE");
//		if((tmp.cot == CS101_SLAVE_COT_DEACT) && ((tmp.oper&0x80) == 0))//撤销
//		{
//			temp_array[pdrv][4] = CS101_SLAVE_COT_DEACTCON;
//		}
//		else if((tmp.cot == CS101_SLAVE_COT_ACT) && (tmp.oper&0x80))//选择
//		{
//			temp_array[pdrv][4] = CS101_SLAVE_COT_ACTCON;
//		}
//		else//执行
//		{
//			temp_array[pdrv][4] = CS101_SLAVE_COT_ACTCON;
//			CS101_Slave_Reply(pdrv, temp_array[pdrv]);
//			temp_array[pdrv][4] = CS101_SLAVE_COT_ACCTTERM;/*遥控结束*/
//		}
//		CS101_Slave_Reply(pdrv, temp_array[pdrv]);
//	}
}

/**
  * @brief : clock synchronization command.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS101_Slave_C_CS(uint8_t pdrv, uint8_t *pbuf)//对时				//解析主端
{//LENTH/Lock_ID/TypeID/VSQ/COT_L/COT_H/PubAddr_L/PubAddr_H/InfoAddr_L/InfoAddr_H/CP56Time2a
//	struct ds_privateTime getTime;												TXL注释
//	struct ds_cp56Time2a setTime;												TXL注释

    memcpy(temp_array[pdrv],pbuf,sizeof(temp_array[pdrv]));

    switch(pbuf[4])
    {
        case CS101_SLAVE_COT_REQ:
            temp_array[pdrv][4] = CS101_SLAVE_COT_REQ;
//            allInfoDisk->fun.get_currentTime_info(&getTime);						TXL注释
//            memcpy(&temp_array[pdrv][10],(uint8_t *)&getTime,sizeof(struct ds_cp56Time2a));	TXL注释
            break;
        case CS101_SLAVE_COT_ACT:
            temp_array[pdrv][4] = CS101_SLAVE_COT_ACTCON;
//            timep = *(struct ds_cp56Time2a *)&temp_array[pdrv][10];
//            time = *(struct ds_privateTime *)&timep;
//            memcpy(&setTime,&temp_array[pdrv][10],sizeof(struct ds_cp56Time2a));	TXL注释
//            allInfoDisk->fun.write_currentTime_info(&setTime);					TXL注释
            break;
        default:
            temp_array[pdrv][4] = CS101_SLAVE_COT_COTERR;
            break;
    }

    CS101_Slave_Reply(pdrv, temp_array[pdrv]);
}

/**
  * @brief : get alarm of ms.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: time_ms
  * @updata: [2019-04-22][chen][new]
  */
uint64_t CS101_Slave_getAlarmOfMs(uint8_t pdrv)
{
//	struct ds_privateTime getTime;											TXL注释
//	allInfoDisk->fun.get_currentTime_info(&getTime);						TXL注释
//	return(getTime.time_ms);												TXL注释
	return 0;
}

/**
  * @brief : reset command.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS101_Slave_C_RP(uint8_t pdrv, uint8_t *pbuf)//复位进程					//解析主端
{//LENTH/Lock_ID/TypeID/VSQ/COT_L/COT_H/PubAddr_L/PubAddr_H/InfoAddr_L/InfoAddr_H/QRP
//    memcpy(temp_array[pdrv],pbuf,sizeof(temp_array[pdrv]));
//    temp_array[pdrv][4] = 7;
//    CS101_Slave_Reply(pdrv, temp_array[pdrv]);
    CS101_Slave_Pad[pdrv].resetFlag = 1;
}

/**
  * @brief : 线程退出.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-07-08][Chen][new]
  */
void CS101_Slave_exit(uint8_t pdrv)
{
//	log_e("exit(0)");
	exit(0);
}

/**
  * @brief : 获取定值预制超时时间.
  * @param : [void]
  * @return: true超时复位
  * @updata: [2019-03-11][Chen][new]
  */
uint8_t getDzPreTimeOut101(uint8_t pdrv)
{
	uint64_t timeMs = CS101_Slave_getAlarmOfMs(pdrv);
	if(dzPreFlag == TRUE)
	{
		dzPreTimeOutEnd = timeMs;
		if((dzPreTimeOutEnd - dzPreTimeOutStart) >= 60*1000)
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;

}

/**
  * @brief : 定值撤销操作.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-03-11][Chen][new]
  */
uint8_t CS101_Slave_DZ_RevokeOperation(uint8_t pdrv)
{
//	log_w("CS101_Slave_DZ_RevokeOperation");
	dzPreFlag = FALSE;/*预制超时标志*/
	preOperation = FALSE;
	writeNum = 0;
	g_CommunicatFlag[COM_PARA] = COMMUNICATLOCKSTATUS;
	return TRUE;
}

/**
  * @brief : reset command.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS101_Slave_C_SR(uint8_t pdrv, uint8_t *pbuf)//定值参数					//解析主端
{
	uint8_t j = 0;
	uint16_t i = 0;
//	uint8_t size = 0;
	uint16_t addr = 0;/*地址*/
	uint16_t num = 0;/*数量*/
	uint16_t sendNum = 0;/*发送给主站数量*/
	uint16_t sendNumGroup = 0;/*发送给主站数量*/
	uint8_t areaCode;/*区号*/
//	tagValueParaCfg_t pvalueParaStr;										TXL注释
	float *pValue = NULL;/*值*/
	uint16_t readAddrBuf[256];/*要读取的地址*/
	uint16_t readBufSize = 0;
	uint64_t timeMs = CS101_Slave_getAlarmOfMs(pdrv);

//    log_w("DZ receive all");
	memcpy(temp_array[pdrv],pbuf,pbuf[0]);		//pbuf[0] 为ASDU长度 + 2  ASDU类型TI从pbuf[2]开始

	if((g_CommunicatFlag[COM_PARA] == COMMUNICATLOCKSTATUS)||(g_CommunicatFlag[COM_PARA] == CS101_Slave_Pad[pdrv].DevId))
	{
//		log_i("g_CommunicatFlag[COM_PARA] TRUE:%02x",g_CommunicatFlag[COM_PARA]);
	}
	else
	{
		temp_array[pdrv][4] = CS101_SLAVE_COT_ADDRERR;
		CS101_Slave_Reply(pdrv, temp_array[pdrv]);
		return;
	}

	switch(pbuf[2]) /*应用类型*/
	{
		case CS101_SLAVE_C_SR_NA_1:/*切换定值区号*/
//			if((Encrypt_ApplyType_Read(pdrv) != 0x01)&&(CS101_Slave_Pad[pdrv].Encrypt == 1))
//			{
//				Encrypt_ReturnFaultInfo_IN(0x1f,0x91,0x01,CS101_Slave_Pad[pdrv].DevId);
//				return;
//			}
//			log_i("operate_areaCode_info");
//			if(allInfoDisk->fun.operate_areaCode_info(pbuf[10]) == RETURN_TRUE)								TXL注释
//			{
//				log_i("RETURN_TRUE");
//				temp_array[pdrv][4]=7;
//			}
//			else
//			{
//				log_w("RETURN_ERROR");
//				temp_array[pdrv][4]=47;
//			}
			CS101_Slave_Reply(pdrv, temp_array[pdrv]);
			break;

		case CS101_SLAVE_C_RR_NA_1:/*读定值区号*/
//			log_w("read currentFixedAreaCodeAddr ");
//			addr = allInfoDisk->var.currentFixedAreaCode.addr;												TXL注释
//			if(allInfoDisk->fun.get_value_point(&areaCode,addr,(void*)&pValue,&size) == RETURN_ERROR)
//			{
//				log_w("currentFixedAreaCodeAddr RETURN_ERROR");
//			}
			areaCode = (uint8_t)(*(float*)pValue);

            temp_array[pdrv][0] = temp_array[pdrv][0] + 6;
            temp_array[pdrv][4] = 7;
            temp_array[pdrv][10] = areaCode;
            temp_array[pdrv][11] = 0;
//            temp_array[pdrv][12] = AREACODE_RUN;									TXL注释
            temp_array[pdrv][13] = 0;
//            temp_array[pdrv][14] = AREACODE_ZONE2;								TXL注释
            temp_array[pdrv][15] = 0;
            CS101_Slave_Reply(pdrv, temp_array[pdrv]);
			break;

		case CS101_SLAVE_C_RS_NA_1:/*读参数和定值*/
			areaCode = pbuf[8];

			if((pbuf[3]&0x7f)==0)//vsq	读全部参数
			{
                if(areaCode == 0) /*0区*/
                {
//                	addr = VALUEPARABASE_STARTADDR;									TXL注释
//                	num  = allInfoDisk->var.valueParaBase.maxNum;
                }
                else
                {
//                    addr = VALUEPARAZOON_STARTADDR;								TXL注释
//                    num  = allInfoDisk->var.valueParaZone[areaCode].maxNum;
                }
                for(i = 0; i < num; i++)
                {
                	readAddrBuf[i] = addr+i;
                }
			}
			else	/*读部分参数*/
			{
				num = pbuf[3]&0x7f;												//可变帧长限定词VSQ 判断
				for(i = 0; i < num; i++)
				{
					readAddrBuf[i] = *(uint16_t *)&pbuf[10+(i*2)];				//要读取地址 存储
				}
			}
			memcpy(temp_array[pdrv],pbuf,pbuf[0]);
			if(num == 0)
			{
				temp_array[pdrv][4] = 47;
				CS101_Slave_Reply(pdrv, temp_array[pdrv]);
				break;
			}
//			log_i("num:%d",num);
//			log_i("sendNum:%d",sendNum);
			for(;sendNum<num;)
			{
				readBufSize = 0;
				sendNumGroup = 0;/*分组上送计数清零*/
				for(i = sendNum; i < num; i++)
				{
					memcpy(&temp_array[pdrv][11+readBufSize],&readAddrBuf[i],sizeof(uint16_t));/*信息体地址*/
//					// log_w("get_value_point");
//					if(allInfoDisk->fun.get_struct_point(&areaCode,readAddrBuf[i],(void*)&pvalueParaStr) == RETURN_ERROR)	//TXL注释
//					{
//						log_w("get_value_point RETURN_ERROR %x",readAddrBuf[i]);
//						return;
//					}
//					//{log_w("%d,%d,%f",pvalueParaStr->Type,pvalueParaStr->TypeSize,*(float *)pvalueParaStr->pVal);}
//					temp_array[pdrv][13+readBufSize+0] = pvalueParaStr->Type;												//TXL注释
//					temp_array[pdrv][13+readBufSize+1] = pvalueParaStr->TypeSize;
//					memcpy(&temp_array[pdrv][13+readBufSize+2],pvalueParaStr->pVal,pvalueParaStr->TypeSize);/*地址对应值*/
//					readBufSize += (pvalueParaStr->TypeSize+2+2);
					/*----------------此处随便赋的值---------------------*/
					temp_array[pdrv][13+readBufSize+0] = 1;			//tag类型										//TXL注释
					temp_array[pdrv][13+readBufSize+1] = 1;			//数据长度
					uint8_t val = 5;								//数据
					memcpy(&temp_array[pdrv][13+readBufSize+2],&val,1);/*地址对应值*/
					readBufSize += (1+2+2);
					/*--------------------------------------------------*/
					sendNumGroup++;
					if(readBufSize >200)
					{
						break;
					}
				}
				sendNum += sendNumGroup;
//				log_i("sendNum:%d",sendNum);
				temp_array[pdrv][0] = 0x0a + 1 + readBufSize;
				temp_array[pdrv][3] = sendNumGroup;
				temp_array[pdrv][4] = 7;								//激活确认
				temp_array[pdrv][5] = 0;
				if(sendNum<num)
				{temp_array[pdrv][10] = 0x01;}
				else
				{temp_array[pdrv][10] = 0x00;}
				CS101_Slave_Reply(pdrv, temp_array[pdrv]);
			}
//			log_i("break");
			break;

		case CS101_SLAVE_C_WS_NA_1:/*写参数和定值*/

			 areaCode = pbuf[8];
			 if(pbuf[10]&0x40)/*取消预制*/
			 {
//				 if((Encrypt_ApplyType_Read(pdrv) != 0x03)&&(CS101_Slave_Pad[pdrv].Encrypt == 1))
//				 {
//					Encrypt_ReturnFaultInfo_IN(0x1f,0x91,0x01,CS101_Slave_Pad[pdrv].DevId);
//					return;
//				 }
				 dzPreFlag = FALSE;/*预制超时标志*/
				 preOperation = FALSE;
				 writeNum = 0;
				 temp_array[pdrv][4] = 9;
				 temp_array[pdrv][5] = 0;
				 g_CommunicatFlag[COM_PARA] = COMMUNICATLOCKSTATUS;
				 CS101_Slave_Reply(pdrv, temp_array[pdrv]);
			 }
			 else if(pbuf[10]&0x80)/*预制*/
			 {
//                  log_w("DZ preSet");
//				 if((Encrypt_ApplyType_Read(pdrv) != 0x01)&&(CS101_Slave_Pad[pdrv].Encrypt == 1))
//				 {
//					Encrypt_ReturnFaultInfo_IN(0x1f,0x91,0x01,CS101_Slave_Pad[pdrv].DevId);
//					break;
//				 }
				 num = pbuf[3]&0x7f;
				 if(num == 0)
				 {
					 temp_array[pdrv][4] = 47;
					 CS101_Slave_Reply(pdrv, temp_array[pdrv]);
					 break;
				 }
//                 log_i("DZ preSet num:%d",num);
				 if(g_CommunicatFlag[COM_PARA] == COMMUNICATLOCKSTATUS)
				 {
					 preOperation  = FALSE;
					 g_CommunicatFlag[COM_PARA] = CS101_Slave_Pad[pdrv].DevId;
				 }

			if (preOperation == FALSE)
			{
				for (i = 0; i < num; i++)
				{
					writeDZbuf[writeNum + i].addr  = *(uint16_t *)&pbuf[11 + sendNum];
					writeDZbuf[writeNum + i].tag   = pbuf[13 + sendNum];
					writeDZbuf[writeNum + i].size  = pbuf[14 + sendNum];
					memcpy(writeDZbuf[writeNum + i].value, &pbuf[15 + sendNum], pbuf[14 + sendNum]);

					sendNum += (writeDZbuf[writeNum + i].size + 4);
//					log_i("writeDZbuf addr:0x%04x,tag:0x%02x,size:0x%02x,value:", writeDZbuf[writeNum + i].addr, writeDZbuf[writeNum + i].tag, writeDZbuf[writeNum + i].size);
					for (j = 0; j < writeDZbuf[writeNum + i].size; j++)
						printf("%02x ", writeDZbuf[writeNum + i].value[j]);
					printf("\n");
				}
				writeNum += num;
//				log_i("DZ preSet writeNum:%d",writeNum);
				if (!(pbuf[10] & 0x01))
				{
					preOperation = TRUE;
				}
//				log_i("DZ preSet preOperation:%d", preOperation);
				dzPreFlag = TRUE;/*预制超时标志*/
				dzPreTimeOutStart = dzPreTimeOutEnd = timeMs;
				temp_array[pdrv][4] = 7;
				temp_array[pdrv][5] = 0;
				g_CommunicatFlag[COM_PARA] = CS101_Slave_Pad[pdrv].DevId;
				CS101_Slave_Reply(pdrv, temp_array[pdrv]);
			}
			else
			{
				temp_array[pdrv][4] = 47;
				temp_array[pdrv][5] = 0;
				CS101_Slave_Reply(pdrv, temp_array[pdrv]);
				preOperation = FALSE;
				writeNum = 0;
				g_CommunicatFlag[COM_PARA] = COMMUNICATLOCKSTATUS;
			}
		}
		else /*固化*/
		{
//			log_w("DZ Set");
//			if ((Encrypt_ApplyType_Read(pdrv) != 0x03) && (CS101_Slave_Pad[pdrv].Encrypt == 1))
//			{
//				Encrypt_ReturnFaultInfo_IN(0x1f, 0x91, 0x01, CS101_Slave_Pad[pdrv].DevId);
//				break;
//			}
			if ((g_CommunicatFlag[COM_PARA] == CS101_Slave_Pad[pdrv].DevId) && (preOperation == TRUE))
			{
//				log_w("write_valuePara_info");
				for (i = 0; i < writeNum; i++)
				{
//					if (allInfoDisk->fun.write_valuePara_info(&areaCode, writeDZbuf[i].addr,					TXL注释
//						 writeDZbuf[i].value, writeDZbuf[i].size) == RETURN_ERROR)
//					{
//						log_w("write_valuePara_info RETURN_ERROR");
//					}
				}
				temp_array[pdrv][4] = 7;
				temp_array[pdrv][5] = 0;
				CS101_Slave_Reply(pdrv, temp_array[pdrv]);
			}
			else
			{
				temp_array[pdrv][4] = 47;
				temp_array[pdrv][5] = 0;
				CS101_Slave_Reply(pdrv, temp_array[pdrv]);
			}
			dzPreFlag = FALSE;/*预制超时标志*/
			preOperation = FALSE;
			writeNum = 0;
			g_CommunicatFlag[COM_PARA] = COMMUNICATLOCKSTATUS;
		}
		break;

		default:
			break;
	}
}

/**
  * @brief : 定时启动 比较遥信变化.
  * @param : [pdrv]
  * @param :
  * @return: none
  * @updata: [2018-12-06][Chen][newly increased]
  */
uint8_t CS101_Slave_C_YX(uint8_t pdrv)
{
	uint16_t i,maxnum = 0;
	uint8_t value = 0;
	uint8_t negate = 0;
	uint8_t upTypeCos = 0;
	uint8_t upCos = 0;
//	struct ds_privateTime nowtime;											TXL注释

//	allInfoDisk->fun.get_currentTime_info(&nowtime);															TXL注释
//
//	maxnum = allInfoDisk->var.telesignal.maxNum;

	for(i=0; i<maxnum; i++)
	{
//		value  = *(allInfoDisk->var.telesignal.firstPoint+i)->pVal; /*获取原值*/									TXL注释
//		negate = (allInfoDisk->var.telesignal.firstPoint+i)->IsNegated;/*取反标志*/
//		upTypeCos = (allInfoDisk->var.telesignal.firstPoint+i)->IsDoublePoint;/*上送类型*/
//		upCos  = (allInfoDisk->var.telesignal.firstPoint+i)->IsCOS;

		if(negate == TRUE)/*取反*/
		{
			value = (~value)&0x03;
		}
		if(upTypeCos == CS101_SLAVE_M_SP_NA_1)/*单点*/
		{
			value = (value - 1)&0x01;
		}


		if(value != CS101SlaveDisk[pdrv].telesignalBackup[i])
		{
			if(upCos == TRUE)
			{
//				CS101SlaveDisk[pdrv].TelesignalCosIn->addr = i+TELESIGNAL_STARTADDR;	/*地址*/					TXL注释
				CS101SlaveDisk[pdrv].TelesignalCosIn->value = value;	/*值*/
				CS101SlaveDisk[pdrv].TelesignalCosIn->uptype = upTypeCos;/*上送类型*/
				// log_i("TelesignalCosIn->addr %02x ",TelesignalCosIn->addr);
				// log_i("TelesignalCosIn->value %02x ",TelesignalCosIn->value);
				// log_i("TelesignalCosIn->uptype %02x ",TelesignalCosIn->uptype);
				if(++CS101SlaveDisk[pdrv].TelesignalCosIn >= CS101SlaveDisk[pdrv].TelesignalCosRep+COS_MAX_NUM)
				{
					CS101SlaveDisk[pdrv].TelesignalCosIn = CS101SlaveDisk[pdrv].TelesignalCosRep;
				}
			}
			CS101SlaveDisk[pdrv].telesignalBackup[i] = value;
		}
	}
	return (0);
}

/**
  * @brief : judg cos.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS101_Slave_H_COS(uint8_t pdrv)//判断是否有COS
{
	CS101_Slave_C_YX(pdrv);/*定时比较 暂时放在这里*/

	if(CS101SlaveDisk[pdrv].TelesignalCosOut != CS101SlaveDisk[pdrv].TelesignalCosIn)
	{
		return (1);	/*有变化遥信*/
	}
	return (0);
}

/**
  * @brief : read cos.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS101_Slave_R_COS(uint8_t pdrv, uint8_t *pbuf)//读cos
{
	uint8_t i = 0;
	uint8_t sendnum = 0;
	uint8_t uptype = 0;

	for(i=0; CS101SlaveDisk[pdrv].TelesignalCosOut != CS101SlaveDisk[pdrv].TelesignalCosIn; i++)
	{
		uptype = CS101SlaveDisk[pdrv].TelesignalCosOut->uptype;
//		log_w("CS101SlaveDisk[pdrv].TelesignalCosOut->uptype:(%d)",CS101SlaveDisk[pdrv].TelesignalCosOut->uptype);
		memcpy(&temp_array[pdrv][sendnum*sizeof(struct COS_Str) + 8],CS101SlaveDisk[pdrv].TelesignalCosOut,sizeof(struct COS_Str));

		if(++CS101SlaveDisk[pdrv].TelesignalCosOut >= CS101SlaveDisk[pdrv].TelesignalCosRep+COS_MAX_NUM)
		{
			CS101SlaveDisk[pdrv].TelesignalCosOut = CS101SlaveDisk[pdrv].TelesignalCosRep;
		}

		if(++sendnum >= 10)	/*每包上送数量*/
		{
			break;
		}
		if(uptype != CS101SlaveDisk[pdrv].TelesignalCosOut->uptype)/*与下一个上送类型不同*/
		{
			break;
		}
	}
//	log_i("send %d COS",sendnum);
	temp_array[pdrv][0] = 8 + sendnum*sizeof(struct COS_Str);;	/*LENTH*/
	temp_array[pdrv][2] = uptype; /*类型*/
	temp_array[pdrv][3] = sendnum;
	temp_array[pdrv][4] = CS101_SLAVE_COT_SPONT;
	temp_array[pdrv][5] = 0;
	temp_array[pdrv][6] = LOBYTE(CS101_Slave_Pad[pdrv].ASDUAddr);
	temp_array[pdrv][7] = HIBYTE(CS101_Slave_Pad[pdrv].ASDUAddr);

	memcpy(pbuf,temp_array[pdrv],temp_array[pdrv][0]);
}

/**
  * @brief : judg soe.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS101_Slave_H_SOE(uint8_t pdrv)//判断是否有soe
{
//	if(CS101SlaveDisk[pdrv].SoeOut != *allInfoDisk->var.changeSoe.in)				TXL注释
//	{
//		return(TRUE);
//	}
//	return (FALSE);
	return 0;
}

/**
  * @brief : read soe.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS101_Slave_R_SOE(uint8_t pdrv, uint8_t *pbuf)//读soe
{
//	uint16_t addr = 0;
	uint8_t sendnum = 0;
	uint8_t value = 0;
	uint8_t negate = 0;
	uint8_t upTypeCos = 0;
	uint8_t upTypeSoe = 0;
//	uint8_t upSoe;

//	for(; CS101SlaveDisk[pdrv].SoeOut != *allInfoDisk->var.changeSoe.in;)									TXL注释
//	{
//		addr = (allInfoDisk->var.changeSoe.firstPoint+CS101SlaveDisk[pdrv].SoeOut)->addr;					TXL注释
//		negate = (allInfoDisk->var.telesignal.firstPoint+addr-1)->IsNegated;/*取反标志*/
//		upTypeCos = (allInfoDisk->var.telesignal.firstPoint+addr-1)->IsDoublePoint;/*上送类型*/
//		upSoe  = (allInfoDisk->var.telesignal.firstPoint+addr-1)->IsSOE;
//		value = (allInfoDisk->var.changeSoe.firstPoint+CS101SlaveDisk[pdrv].SoeOut)->value;
//		log_w("soe: addr(%x),negate(%d),upTypeCos(%d),upSoe(%d),value(%d)",addr,negate,upTypeCos,upSoe,value);
		if(upTypeCos == CS101_SLAVE_M_SP_NA_1)/*单点*/
		{
			upTypeSoe = CS101_SLAVE_M_SP_TB_1;
		}
		if(upTypeCos == CS101_SLAVE_M_DP_NA_1)/*双点*/
		{
			upTypeSoe = CS101_SLAVE_M_DP_TB_1;
		}

		if(negate == TRUE)/*取反*/
		{
			value = (~value)&0x03;
		}
		if(upTypeCos == CS101_SLAVE_M_SP_NA_1)/*单点*/
		{
			value = (value - 1)&0x01;
		}

//		(allInfoDisk->var.changeSoe.firstPoint+CS101SlaveDisk[pdrv].SoeOut)->value = value;								TXL注释
//		if(upSoe == TRUE)
//		{
//			memcpy(&temp_array[pdrv][sendnum*sizeof(struct SOE_Str) + 8],&(allInfoDisk->var.changeSoe.firstPoint+CS101SlaveDisk[pdrv].SoeOut)->addr,sizeof(struct SOE_Str));  TXL注释
//		}
		
//		if(++CS101SlaveDisk[pdrv].SoeOut >= *allInfoDisk->var.changeSoe.maxNum)											TXL注释
//		{
//			CS101SlaveDisk[pdrv].SoeOut = 0;
//		}

		if(++sendnum >= 10)	/*每包上送数量*/
		{
//			break;																										TXL注释
		}
//		if(upTypeCos != (allInfoDisk->var.telesignal.firstPoint+addr)->IsDoublePoint)/*与下一个上送类型不同*/				TXL注释
//		{
//			break;
//		}
//	}
//	log_i("send %d SOE",sendnum);
	temp_array[pdrv][0] = 8 + sendnum*sizeof(struct SOE_Str);;	/*LENTH*/
	temp_array[pdrv][2] = upTypeSoe; /*类型*/
	temp_array[pdrv][3] = sendnum;
	temp_array[pdrv][4] = CS101_SLAVE_COT_SPONT;
	temp_array[pdrv][5] = 0;
	temp_array[pdrv][6] = LOBYTE(CS101_Slave_Pad[pdrv].ASDUAddr);
	temp_array[pdrv][7] = HIBYTE(CS101_Slave_Pad[pdrv].ASDUAddr);
	memcpy(pbuf,temp_array[pdrv],temp_array[pdrv][0]);
}

/**
  * @brief : 定时启动 比较遥测变化.
  * @param : [pdrv]
  * @param :
  * @return: none
  * @updata: [2018-12-10][Chen][newly increased]
  */
uint8_t CS101_Slave_C_YC(uint8_t pdrv)
{
	uint16_t i,maxnum = 0;

	float tempf = 0;
	uint8_t  IsDeadZone = 0; // 上送类型
	float	overValue = 0;
//	maxnum = allInfoDisk->var.telemetry.maxNum;																TXL注释

	for(i=0; i<maxnum; i++)
	{
//		tempf  = *(allInfoDisk->var.telemetry.firstPoint+i)->pVal; /*获取原值*/								TXL注释
//		IsDeadZone = (allInfoDisk->var.telemetry.firstPoint+i)->IsDeadZone;

		if(IsDeadZone == TRUE)/*是否上送NVA*/
		{
//			overValue = (allInfoDisk->var.telemetry.firstPoint+i)->Rating *(allInfoDisk->var.telemetry.firstPoint+i)->Deadband;	TXL注释
			if(fabsf(tempf - CS101SlaveDisk[pdrv].telemetryBackup[i]) > overValue)
			{
//				CS101SlaveDisk[pdrv].TelemetryNVAIn->addr = i + TELEMETRY_STARTADDR;/*地址*/										TXL注释
				CS101SlaveDisk[pdrv].TelemetryNVAIn->value = tempf;/*值*/
//				CS101SlaveDisk[pdrv].TelemetryNVAIn->uptype = (allInfoDisk->var.telemetry.firstPoint+i)->DataType;/*上送类型*/	TXL注释
//				CS101SlaveDisk[pdrv].TelemetryNVAIn->MultiplRate = (allInfoDisk->var.telemetry.firstPoint+i)->Rate;/*倍率*/		TXL注释
				if(++CS101SlaveDisk[pdrv].TelemetryNVAIn >= CS101SlaveDisk[pdrv].TelemetryNVARep+NVA_MAX_NUM)
				{
					CS101SlaveDisk[pdrv].TelemetryNVAIn = CS101SlaveDisk[pdrv].TelemetryNVARep;
				}
				CS101SlaveDisk[pdrv].telemetryBackup[i] = tempf;
			}
		}
	}
	return (0);
}
/**
  * @brief : judg nva.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS101_Slave_H_NVA(uint8_t pdrv)//判断是否有NVA
{
	CS101_Slave_C_YC(pdrv);
	if(CS101SlaveDisk[pdrv].TelemetryNVAOut != CS101SlaveDisk[pdrv].TelemetryNVAIn)
	{
		return (1);
	}
	return (0);
}

/**
  * @brief : read nva.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS101_Slave_R_NVA(uint8_t pdrv, uint8_t *pbuf)//读NVA
{
	uint8_t i = 0;
	uint8_t sendnum = 0;
	int16_t tempu;
	float MultiplRate;
	uint8_t  uptype = 0; // 上送类型
	struct NVA_Str nva_temp;

	for(i=0; CS101SlaveDisk[pdrv].TelemetryNVAOut != CS101SlaveDisk[pdrv].TelemetryNVAIn; i++)
	{
		uptype = CS101SlaveDisk[pdrv].TelemetryNVAOut->uptype;
		nva_temp.value  = CS101SlaveDisk[pdrv].TelemetryNVAOut->value;
		nva_temp.addr   = CS101SlaveDisk[pdrv].TelemetryNVAOut->addr;
		MultiplRate = CS101SlaveDisk[pdrv].TelemetryNVAOut->MultiplRate;
		switch(uptype)
		{
			case CS101_SLAVE_M_ME_NA_1:
				tempu = (int16_t)(nva_temp.value*MultiplRate);
				temp_array[pdrv][sendnum*(sizeof(nva_temp.addr)+ sizeof(int16_t) +1) + sizeof(nva_temp.addr)+ sizeof(int16_t) + 8] = 0x00;//QDS
				memcpy(&temp_array[pdrv][sendnum*(sizeof(nva_temp.addr)+ sizeof(int16_t)+1) + 8],&nva_temp,sizeof(nva_temp.addr));
				memcpy(&temp_array[pdrv][sendnum*(sizeof(nva_temp.addr)+ sizeof(int16_t)+1) + sizeof(nva_temp.addr) + 8],&tempu,sizeof(int16_t));
			break;
			case CS101_SLAVE_M_ME_NB_1:
				tempu = (int16_t)(nva_temp.value*MultiplRate);
				temp_array[pdrv][sendnum*(sizeof(nva_temp.addr)+ sizeof(int16_t) +1) + sizeof(nva_temp.addr)+ sizeof(int16_t) + 8] = 0x00;//QDS
				memcpy(&temp_array[pdrv][sendnum*(sizeof(nva_temp.addr)+ sizeof(int16_t)+1) + 8],&nva_temp,sizeof(nva_temp.addr));
				memcpy(&temp_array[pdrv][sendnum*(sizeof(nva_temp.addr)+ sizeof(int16_t)+1) + sizeof(nva_temp.addr) + 8],&tempu,sizeof(int16_t));
				break;
			case CS101_SLAVE_M_ME_NC_1:/*短浮点*/
				temp_array[pdrv][sendnum*(sizeof(struct NVA_Str)+1) + sizeof(struct NVA_Str) + 8] = 0x00;//QDS
				memcpy(&temp_array[pdrv][sendnum*(sizeof(struct NVA_Str)+1) + 8],&nva_temp,sizeof(struct NVA_Str));
				break;
			default:
				break;
		}
		if(++CS101SlaveDisk[pdrv].TelemetryNVAOut >= CS101SlaveDisk[pdrv].TelemetryNVARep+NVA_MAX_NUM)
		{
			CS101SlaveDisk[pdrv].TelemetryNVAOut = CS101SlaveDisk[pdrv].TelemetryNVARep;
		}

		if(++sendnum >= 10)	/*每包上送数量*/
		{
			break;
		}
		if(uptype != CS101SlaveDisk[pdrv].TelemetryNVAOut->uptype)/*与下一个上送类型不同*/
		{
			break;
		}
	}
//	log_i("send %d NVA",sendnum);
	switch(uptype)
	{
		case CS101_SLAVE_M_ME_NA_1:
		case CS101_SLAVE_M_ME_NB_1:
			temp_array[pdrv][0] = 8 + sendnum*(sizeof(nva_temp.addr)+ sizeof(int16_t)+1);
			break;
		case CS101_SLAVE_M_ME_NC_1:
			temp_array[pdrv][0] = 8 + sendnum*(sizeof(struct NVA_Str)+1);
			break;
	}

	temp_array[pdrv][2] = uptype; /*类型*/
	temp_array[pdrv][3] = sendnum;
	temp_array[pdrv][4] = CS101_SLAVE_COT_SPONT;
	temp_array[pdrv][5] = 0;
	temp_array[pdrv][6] = LOBYTE(CS101_Slave_Pad[pdrv].ASDUAddr);
	temp_array[pdrv][7] = HIBYTE(CS101_Slave_Pad[pdrv].ASDUAddr);
	memcpy(pbuf,temp_array[pdrv],temp_array[pdrv][0]);
}

/**
  * @brief : judg fevent.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS101_Slave_H_FEvent(uint8_t pdrv)//判断是否有FEvent
{
    return(0);
}

/**
  * @brief : read fevent.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS101_Slave_R_FEvent(uint8_t pdrv, uint8_t *pbuf)//读FEvent
{//LENTH/Lock_ID/TypeID/VSQ/COT_L/COT_H/PubAddr_L/PubAddr_H/Array
}

/**
  * @brief : 获取总召遥信 组包.
  * @param : [pdrv]
  * @param : [addr]
  * @param : [num]
  * @param : [pbuf]
  * @return: none
  * @updata: [2018-11-29][Chen][BRIEF]
  */
uint16_t CS101_Slave_R_YXDATA(uint8_t pdrv,uint16_t addr,uint16_t num, uint8_t *pbuf)//读YXDATA
{//LENTH/Lock_ID/TypeID/VSQ/COT_L/COT_H/PubAddr_L/PubAddr_H/InfoAddr_L/InfoAddr_H/Array(Value)

	uint8_t i = 0;
	uint8_t sendnum = 0;
	uint8_t value = 0;
	uint8_t negate = 0;
	uint8_t uptype = 0;

//	log_i("read YXDATA addr:%x,num:%d",addr,num);

	for(i=0; i<num; i++)
	{
//		value  = *(allInfoDisk->var.telesignal.firstPoint +addr-TELESIGNAL_STARTADDR +i)->pVal; /*获取原值*/				TXL注释
//		negate = (allInfoDisk->var.telesignal.firstPoint +addr-TELESIGNAL_STARTADDR +i)->IsNegated;/*取反标志*/
//		uptype = (allInfoDisk->var.telesignal.firstPoint +addr-TELESIGNAL_STARTADDR +i)->IsDoublePoint;/*上送类型*/
//		log_w("YX:negate(%d),uptype(%d),value(%d)",negate,uptype,value);
		if(negate == TRUE)/*取反*/
		{
			value = (~value)&0x03;
		}

		if(uptype == CS101_SLAVE_M_SP_NA_1)/*单点*/
		{
			value = (value - 1)&0x01;
		}

//		CS101SlaveDisk[pdrv].telesignalBackup[addr-TELESIGNAL_STARTADDR +i] = value&0x03;	/*总召备份YX*/				TXL注释
		value = 1;	//测试赋值
		temp_array[pdrv][sendnum + 10] = value;
		sendnum++;

//		if(sendnum +(addr-CS101_Slave_Pad[pdrv].YX_FirstAddr) >= allInfoDisk->var.telesignal.maxNum)	/*大于 需要总数*/		TXL注释
//		{
//			break;
//		}
//
//		if(uptype != (allInfoDisk->var.telesignal.firstPoint +i+1)->IsDoublePoint)			TXL注释
//		{
//			break;
//		}
	}
//    log_i("YXDATA uptype:%x,sendnum:%x",uptype,sendnum);
	temp_array[pdrv][0] = 10 + sendnum;	/*LENTH*/
	temp_array[pdrv][2] = uptype; /*类型*/
	temp_array[pdrv][3] = sendnum|0x80;
	temp_array[pdrv][4] = CS101_SLAVE_COT_INTROGEN;
	temp_array[pdrv][5] = 0;
	temp_array[pdrv][6] = LOBYTE(CS101_Slave_Pad[pdrv].ASDUAddr);
	temp_array[pdrv][7] = HIBYTE(CS101_Slave_Pad[pdrv].ASDUAddr);
	temp_array[pdrv][8] = LOBYTE(addr);
	temp_array[pdrv][9] = HIBYTE(addr);
	memcpy(pbuf,temp_array[pdrv],temp_array[pdrv][0]);

	return(addr + sendnum);
}

/**
  * @brief : 总召 获取遥测 组包.
  * @param : [pdrv]
  * @param : [addr]
  * @param : [num]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint16_t CS101_Slave_R_YCDATA(uint8_t pdrv,uint16_t addr,uint16_t num, uint8_t *pbuf)//读YCDATA
{
	/*allInfoDisk->var.telemetry  遥测*/

	uint8_t  i = 0;
	uint8_t  sendnum = 0;
	int16_t  tempu = 0;
	uint8_t  upType = 0; // 上送类型
	float 	 Rate = 0,value = 0;

//	log_i("read YCDATA addr:%x,num:%d",addr,num);

	for(i=0; i<num; i++)
	{
//		upType = (allInfoDisk->var.telemetry.firstPoint +addr-TELEMETRY_STARTADDR +i)->DataType;/*获取类型*/       TXL注释
//		value = *((allInfoDisk->var.telemetry.firstPoint +addr-TELEMETRY_STARTADDR +i)->pVal);
//		Rate  = (allInfoDisk->var.telemetry.firstPoint +addr-TELEMETRY_STARTADDR +i)->Rate;
//		CS101SlaveDisk[pdrv].telemetryBackup[addr-TELEMETRY_STARTADDR +i] = value;/*备份YC*/
//		log_w("YC:upType(%d),value(%f),Rate(%f)",upType,value,Rate);
		switch(upType)
		{
			case CS101_SLAVE_M_ME_NA_1:
				tempu = (int16_t)(value*Rate);
				temp_array[pdrv][sendnum*(1+sizeof(uint16_t)) + sizeof(uint16_t) + 10] = 0x00;//QDS
                memcpy(&temp_array[pdrv][sendnum*(1+sizeof(uint16_t)) + 10],&tempu,sizeof(uint16_t));
			break;
			case CS101_SLAVE_M_ME_NB_1:
				tempu = (int16_t)(value*Rate);
				temp_array[pdrv][sendnum*(1+sizeof(uint16_t)) + sizeof(uint16_t) + 10] = 0x00;//QDS
				memcpy(&temp_array[pdrv][sendnum*(1+sizeof(uint16_t)) + 10],&tempu,sizeof(uint16_t));
				break;
			case CS101_SLAVE_M_ME_NC_1:/*短浮点*/
				temp_array[pdrv][sendnum*(1+sizeof(uint32_t)) + sizeof(uint32_t) + 10] = 0x00;//QDS
				memcpy(&temp_array[pdrv][sendnum*(1+sizeof(uint32_t)) + 10],&value,sizeof(uint32_t));
				break;
			default:
				break;
		}
		temp_array[pdrv][10+i] = 0x34;	//测试使用
		sendnum++;

//		if(sendnum+(addr-CS101_Slave_Pad[pdrv].YC_FirstAddr) >= allInfoDisk->var.telemetry.maxNum)	/*大于 需要总数*/   TXL注释
//		{
//			break;
//		}
//		if(upType != (allInfoDisk->var.telemetry.firstPoint+i+1)->DataType)	/*与下一个类型不同*/			TXL注释
//		{
//			break;
//		}
	}

	switch(upType)
	{
		case CS101_SLAVE_M_ME_NA_1:
		case CS101_SLAVE_M_ME_NB_1:
			temp_array[pdrv][0] = 10 + sendnum*(1+sizeof(uint16_t));
			break;
		case CS101_SLAVE_M_ME_NC_1:
			temp_array[pdrv][0] = 10 + sendnum*(1+sizeof(uint32_t));
			break;
	}
//    log_i("YCDATA uptype:%x,sendnum:%x",upType,sendnum);

	temp_array[pdrv][2] = upType;
	temp_array[pdrv][3] = sendnum|0x80;
	temp_array[pdrv][4] = CS101_SLAVE_COT_INTROGEN;
	temp_array[pdrv][5] = 0;
	temp_array[pdrv][6] = LOBYTE(CS101_Slave_Pad[pdrv].ASDUAddr);
	temp_array[pdrv][7] = HIBYTE(CS101_Slave_Pad[pdrv].ASDUAddr);
	temp_array[pdrv][8] = LOBYTE(addr);
	temp_array[pdrv][9] = HIBYTE(addr);
	memcpy(pbuf,temp_array[pdrv],temp_array[pdrv][0]);

	return(addr + sendnum);
}

/**
  * @brief : updata command.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS101_Slave_F_SR(uint8_t pdrv, uint8_t *pbuf)//软件升级			//解析主端
{
	memcpy(temp_array[pdrv],pbuf,sizeof(temp_array[pdrv]));

	switch(pbuf[4])
	{
		case 6:
			temp_array[pdrv][4] = 7;
			if(pbuf[10]&0x80)
			{
//				if((Encrypt_ApplyType_Read(pdrv) != 0x01)&&(CS101_Slave_Pad[pdrv].Encrypt == 1))
//				{
//					Encrypt_ReturnFaultInfo_IN(0x1f,0x91,0x01,CS101_Slave_Pad[pdrv].DevId);
//					return;
//				}
				//启动升级
			}
			else
			{
				if(CS101_Slave_Pad[pdrv].Encrypt)
				{
					CS101_Slave_Pad[pdrv].updateFlag = 1;
				}
				CS101_Slave_Pad[pdrv].resetFlag = 1;
			}
			break;
		case 8:
			temp_array[pdrv][4] = 9;
			//升级过程中停止升级
			break;
		default:
			temp_array[pdrv][4] = 47;
			break;
	}
    CS101_Slave_Reply(pdrv,temp_array[pdrv]);
}

/**
  * @brief : file command.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS101_Slave_F_FR(uint8_t pdrv, uint8_t *pbuf)//文件读写				//解析主端
{
    if((g_CommunicatFlag[COM_FILE] == CS101_Slave_Pad[pdrv].DevId)||(g_CommunicatFlag[COM_FILE] == COMMUNICATLOCKSTATUS))
		{
        switch(pbuf[11])
        {
            case 7:/*写文件激活*/
                memcpy(temp_array[pdrv],pbuf,pbuf[0]);
//                log_i("file_operate_WriteFileAct 7 ");
                file_operate_WriteFileAct(CS101_Slave_Pad[pdrv].DevId,temp_array[pdrv]);
//                log_i("file_operate_WriteFileAct 7 over");
                memcpy(pbuf,temp_array[pdrv],temp_array[pdrv][0]);
                if(temp_array[pdrv][0] == 0)
                {
                    return;
                }
                CS101_Slave_Reply(pdrv, pbuf);
                g_CommunicatFlag[COM_FILE] = CS101_Slave_Pad[pdrv].DevId;
                break;

            case 9:/*写文件数据传输*/
                memcpy(temp_array[pdrv],pbuf,pbuf[0]);
//                log_i("file_operate_WriteFileAct 9 ");
                file_operate_WriteFileAct(CS101_Slave_Pad[pdrv].DevId,temp_array[pdrv]);
//                log_i("file_operate_WriteFileAct 9 over");
                memcpy(pbuf,temp_array[pdrv],temp_array[pdrv][0]);
                if(temp_array[pdrv][0] == 0)
                {
//                	log_i("unSendReceive");
                    g_CommunicatFlag[COM_FILE] = CS101_Slave_Pad[pdrv].DevId;
                    return;
                }
                CS101_Slave_Reply(pdrv, pbuf);
                g_CommunicatFlag[COM_FILE] = COMMUNICATLOCKSTATUS;
                break;

            case 1:/*读目录*/
            case 3:/*读文件激活*/
            case 6:/*读文件数据传输确认*/
                memcpy(file_array[pdrv],pbuf,pbuf[0]);
                g_CommunicatFlag[COM_FILE] = CS101_Slave_Pad[pdrv].DevId;
                break;
        }
    }
    else
    {
        pbuf[4] = 47;
        CS101_Slave_Reply(pdrv, pbuf);
    }
}

/**
  * @brief : send idle.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS101_Slave_R_IDLE(uint8_t pdrv, uint8_t *pbuf)//发送空闲回调函数
{
    if(file_array[pdrv][0] != 0)
    {
        switch(file_array[pdrv][11])
        {
            case 1://读目录
                if(file_operate_DirCall(CS101_Slave_Pad[pdrv].DevId,file_array[pdrv]))
                {
                    g_CommunicatFileOper |= COMMUNICATLOCKREADDIR;
                    g_CommunicatFlag[COM_FILE] = CS101_Slave_Pad[pdrv].DevId;
                }
                else
                {
                    g_CommunicatFileOper &= ~COMMUNICATLOCKREADDIR;
                    g_CommunicatFlag[COM_FILE] = COMMUNICATLOCKSTATUS;
                }
                memcpy(pbuf,file_array[pdrv],file_array[pdrv][0]);
                break;

            case 3://读文件激活
            case 6://读文件确认
                if(file_operate_ReadFileAct(CS101_Slave_Pad[pdrv].DevId,file_array[pdrv]))
                {
                    g_CommunicatFileOper |= COMMUNICATLOCKREADFILE;
                    g_CommunicatFlag[COM_FILE] = CS101_Slave_Pad[pdrv].DevId;
                }
                else
                {
                    g_CommunicatFileOper &= ~COMMUNICATLOCKREADFILE;
                    g_CommunicatFlag[COM_FILE] = COMMUNICATLOCKSTATUS;
                }
                memcpy(pbuf,file_array[pdrv],file_array[pdrv][0]);
                break;
            default:
                break;
        }
        memset(file_array[pdrv],0,file_array[pdrv][0]);
    }
    else
    {
        if((g_CommunicatFileOper&COMMUNICATLOCKREADDIR)&&(g_CommunicatFlag[COM_FILE] == CS101_Slave_Pad[pdrv].DevId))
				{
            if(file_operate_DirRead(CS101_Slave_Pad[pdrv].DevId,pbuf))
            {
                g_CommunicatFileOper |= COMMUNICATLOCKREADDIR;
                g_CommunicatFlag[COM_FILE] = CS101_Slave_Pad[pdrv].DevId;
            }
            else
            {
                g_CommunicatFileOper &= ~COMMUNICATLOCKREADDIR;
                g_CommunicatFlag[COM_FILE] = COMMUNICATLOCKSTATUS;
            }
        }
        if((g_CommunicatFileOper&COMMUNICATLOCKREADFILE)&&(g_CommunicatFlag[COM_FILE] == CS101_Slave_Pad[pdrv].DevId))
        {
            if(file_operate_ReadFile(CS101_Slave_Pad[pdrv].DevId,pbuf))
            {
                g_CommunicatFileOper |= COMMUNICATLOCKREADFILE;
                g_CommunicatFlag[COM_FILE] = CS101_Slave_Pad[pdrv].DevId;
            }
            else
            {
                g_CommunicatFileOper &= ~COMMUNICATLOCKREADFILE;
                g_CommunicatFlag[COM_FILE] = COMMUNICATLOCKSTATUS;
            }
        }
    }
}

/**
  * @brief : Encrypt.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS101_Slave_H_Encrypt(uint8_t pdrv)//判断是否有加密数据发送
{
//	return(Encrypt_CheckSend(CS101_Slave_Pad[pdrv].Port));
	return 0;
}

/**
  * @brief : other reply.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
//uint8_t CS101_Slave_C_REPLY(uint8_t pdrv,tagControlCfg_t reInfo)//其他设备回复			TXL注释
//{
//	//reInfo->backDataLen
//	// reInfo->pBackData
//	// reInfo->com.dir//报文方向
//	// reInfo->com.reply//回复方
//	// reInfo->com.send//发起方
//	// reInfo->com.timer
//	// reInfo->com.tpye//命令类型
//	struct tagCtlRomteCfg tmp;//临时结构体
//	tagControlRemoteCfg_t ykCmd;
//
//	ykCmd = reInfo->pdata;
//    log_w("CS101_Slave_C_REPLY ykCmd->sta:%d,reInfo->backDataLen:%d",ykCmd->sta,reInfo->backDataLen);
//	memcpy(temp_array[pdrv],reInfo->pBackData,reInfo->backDataLen);
//    for(int i=0; i<reInfo->backDataLen; i++)
//        log_i("temp_array[pdrv][%d]:%02x,reInfo->pBackData:%02x",i,temp_array[pdrv][i],*(uint8_t*)(reInfo->pBackData+i));
//	tmp.cot  = temp_array[pdrv][4];//传送原因
//	tmp.oper = temp_array[pdrv][10];//操作
//
//	if(ykCmd->sta == allInfoDisk->def.control_RES_FAIL)/*失败*/
//	{
//        log_w("CS101_Slave_C_REPLY control_RES_FAIL");
//		temp_array[pdrv][4] = CS101_SLAVE_COT_ACTCON|CS101_SLAVE_COT_NO;//选择|执行
//		if((tmp.cot == CS101_SLAVE_COT_DEACT) && ((tmp.oper&0x80) == 0))//撤销
//		{
//			temp_array[pdrv][4] = CS101_SLAVE_COT_DEACTCON|CS101_SLAVE_COT_NO;
//		}
//		CS101_Slave_Reply(pdrv, temp_array[pdrv]);
//	}
//	else if(ykCmd->sta == allInfoDisk->def.control_RES_SUCCEED)/*成功*/
//	{
//        log_w("CS101_Slave_C_REPLY control_RES_SUCCEED tmp.cot:%d tmp.oper:%d",tmp.cot,tmp.oper);
//		if((tmp.cot == CS101_SLAVE_COT_DEACT) && ((tmp.oper&0x80) == 0))//撤销
//		{
//			temp_array[pdrv][4] = CS101_SLAVE_COT_DEACTCON;
//		}
//		else if((tmp.cot == CS101_SLAVE_COT_ACT) && (tmp.oper&0x80))//选择
//		{
//			temp_array[pdrv][4] = CS101_SLAVE_COT_ACTCON;
//		}
//		else//执行
//		{
//			temp_array[pdrv][4] = CS101_SLAVE_COT_ACTCON;
//			CS101_Slave_Reply(pdrv, temp_array[pdrv]);
//			temp_array[pdrv][4] = CS101_SLAVE_COT_ACCTTERM;/*遥控结束*/
//		}
//		CS101_Slave_Reply(pdrv, temp_array[pdrv]);
//	}
//	else
//	{
//		return FALSE;
//	}
//
//	return TRUE;
//}

/**
  * @brief : 遥信遥测备份初始化.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-03-13][Chen][newly increased]
  */
static uint8_t CS101_Slave_YxYcBackupInit(uint8_t pdrv)
{
	uint16_t i,num;
	uint8_t value = 0;
	uint8_t negate = 0;
	uint8_t uptype = 0;

	num = CS101_Slave_Pad[pdrv].YX_AllNum;
  for(i=0; i<num; i++)
	{
//		value  = *(allInfoDisk->var.telesignal.firstPoint +i)->pVal; /*获取原值*/					TXL注释
//		negate = (allInfoDisk->var.telesignal.firstPoint +i)->IsNegated;/*取反标志*/					TXL注释
//		uptype = (allInfoDisk->var.telesignal.firstPoint +i)->IsDoublePoint;/*上送类型*/				TXL注释

		if(negate == TRUE)/*取反*/
		{
			value = (~value)&0x03;
		}
		if(uptype == CS101_SLAVE_M_SP_NA_1)/*单点*/
		{
			value = (value - 1)&0x01;
		}

		CS101SlaveDisk[pdrv].telesignalBackup[i] = value&0x03;	/*总召备份YX*/
	}

	num = CS101_Slave_Pad[pdrv].YC_AllNum;
	for(i=0; i<num; i++)
	{
//		CS101SlaveDisk[pdrv].telemetryBackup[i] = *((allInfoDisk->var.telemetry.firstPoint +i)->pVal);/*备份YC*/     TXL注释
	}
	return TRUE;
}

/**
  * @brief : disk初始化.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-03-22][Chen][newly increased]
  */
static void CS101_Slave_DiskInit(uint8_t pdrv)
{
	if (CS101SlaveDisk == NULL)
	{
		CS101SlaveDisk = malloc(sizeof(struct CS101_SLAVE_DISK) * CS101_SLAVE_DISK_VOLUMES);
    memset (CS101SlaveDisk, 0, sizeof (struct CS101_SLAVE_DISK) * CS101_SLAVE_DISK_VOLUMES);
	}

//	CS101SlaveDisk[pdrv].SoeOut = *allInfoDisk->var.changeSoe.in;							TXL注释

	memset(CS101SlaveDisk[pdrv].TelesignalCosRep,0,sizeof(CS101SlaveDisk[pdrv].TelesignalCosRep));
	CS101SlaveDisk[pdrv].TelesignalCosIn = CS101SlaveDisk[pdrv].TelesignalCosRep;
	CS101SlaveDisk[pdrv].TelesignalCosOut = CS101SlaveDisk[pdrv].TelesignalCosRep;
//	int i = 0;

	memset(CS101SlaveDisk[pdrv].TelemetryNVARep,0,sizeof(CS101SlaveDisk[pdrv].TelemetryNVARep));
	CS101SlaveDisk[pdrv].TelemetryNVAIn = CS101SlaveDisk[pdrv].TelemetryNVARep;
	CS101SlaveDisk[pdrv].TelemetryNVAOut = CS101SlaveDisk[pdrv].TelemetryNVARep;
}

/**
  * @brief : 101初始化默认值设置.
  * @param : [void]
  * @return: void
  * @updata: [2018-11-09][Chen][newly increased]
  */
static void CS101_Slave_ParamInit(uint8_t pdrv, int portFd, IecParam sParam)
{
//	log_i("CS101_Slave_ParamInit");
	if(sParam->allInfoDisk == NULL)
	{
//		allInfoDisk = malloc(sizeof(struct tagOverallCfg));				TXL注释
//		memset (allInfoDisk, 0, sizeof (struct tagOverallCfg));			TXL注释
	}
	else
	{
		allInfoDisk = sParam->allInfoDisk;
	}
	// if (allInfoDisk == NULL)
	// {
	// 	allInfoDisk = malloc(sizeof(struct tagOverallCfg));
	// 	memset (allInfoDisk, 0, sizeof (struct tagOverallCfg));
	// 	allInfoDisk = &g_overall;
	// }

	// if (writeDZbuf == NULL)
	// {
	// 	writeDZbuf = malloc(sizeof(struct tagValueDZ));
	// }
//	log_w("allInfoDisk->var.telesignal.maxNum:%02x", allInfoDisk->var.telesignal.maxNum);  TXL注释

	if(pdrv == 0)
	{CS101_Slave_Pad[pdrv].DevId = SLAVE101_ID0;}
	if(pdrv == 1)
	{CS101_Slave_Pad[pdrv].DevId = SLAVE101_ID1;}
	if(pdrv == 2)
	{CS101_Slave_Pad[pdrv].DevId = SLAVE101_ID2;}
	if(pdrv == 3)
	{CS101_Slave_Pad[pdrv].DevId = SLAVE101_ID3;}

	CS101_Slave_Pad[pdrv].Port = portFd;					/*端口*/
	CS101_Slave_Pad[pdrv].ModuleSN = sParam->ModuleSN;  /*模块ID*/
	CS101_Slave_Pad[pdrv].Encrypt = sParam->encrypt;		/*加密*/
	CS101_Slave_Pad[pdrv].BalanMode = sParam->balanMode;		/*平衡*/
	CS101_Slave_Pad[pdrv].IEC_DIR = 0x80;	/*DIR*/
	CS101_Slave_Pad[pdrv].SourceAddr = sParam->sourceAddr;		/*本机地址*/
	CS101_Slave_Pad[pdrv].LinkAddrSize = sParam->linkAddrSize;	/*链路地址长度*/
	CS101_Slave_Pad[pdrv].ASDUCotSize = sParam->ASDUCotSize;	/*传送原因长度*/
	CS101_Slave_Pad[pdrv].ASDUAddr = sParam->ASDUAddr;			/*ASDU地址*/
	CS101_Slave_Pad[pdrv].ASDUAddrSize = sParam->ASDUAddrSize;	/*ASDU地址长度*/

	CS101_Slave_Pad[pdrv].FixFrmLength = 4 + CS101_Slave_Pad[pdrv].LinkAddrSize;

	CS101_Slave_Pad[pdrv].ClockTimers = 1;
	CS101_Slave_Pad[pdrv].TimeOutValue = 1000;	/*重发超时 MS*/
	CS101_Slave_Pad[pdrv].AskStaOutValue = 5000;	/*链路请求超时 MS*/
	CS101_Slave_Pad[pdrv].TimeOutLink = 70000;

	CS101_Slave_Pad[pdrv].updateFlag = 0;
	CS101_Slave_Pad[pdrv].resetFlag = 0;
	CS101_Slave_Pad[pdrv].TimeOutReset = 5000;	/*复位进程超时时间 MS*/
	/*----------------测试随意赋值---------------------*/
	CS101_Slave_Pad[pdrv].YX_AllNum = 200;			/*遥信总数*/
	CS101_Slave_Pad[pdrv].YX_FirstAddr = 0x0001;	/*遥信首地址*/
	CS101_Slave_Pad[pdrv].YX_FrameNum = 100;		/*每组遥信数*/
	CS101_Slave_Pad[pdrv].YC_AllNum = 30;			/*遥测总数*/
	CS101_Slave_Pad[pdrv].YC_FirstAddr = 0x6001;	/*遥测首地址*/
	CS101_Slave_Pad[pdrv].YC_FrameNum = 24;			/*每组遥测数*/
	/*------------------------------------------------*/
//	CS101_Slave_Pad[pdrv].YX_AllNum = allInfoDisk->var.telesignal.maxNum;	/*遥信总数*/			TXL注释
//	CS101_Slave_Pad[pdrv].YX_FirstAddr = TELESIGNAL_STARTADDR;		/*遥信首地址*/				TXL注释
//	CS101_Slave_Pad[pdrv].YX_FrameNum = 100;		/*每组遥信数*/								TXL注释
//	CS101_Slave_Pad[pdrv].YC_AllNum = allInfoDisk->var.telemetry.maxNum;	/*遥测总数*/			TXL注释
//	CS101_Slave_Pad[pdrv].YC_FirstAddr = TELEMETRY_STARTADDR;	/*遥测首地址*/					TXL注释
//	CS101_Slave_Pad[pdrv].YC_FrameNum = 24;	/*每组遥测数*/										TXL注释

    if (CS101_Slave_Pad[pdrv].Encrypt)
    {
//        if(!Encrypt_AddDevice(CS101_Slave_Pad[pdrv].DevId,CS101_Slave_Pad[pdrv].Port,CS101_Slave_Read_drv,CS101_Slave_Write_drv))
//        {
//        	log_e("Encrypt_AddDevice failed");
//        	CS101_Slave_Pad[pdrv].Encrypt = 0;
//        }
    }

//	log_i("CS101_Slave_Pad[pdrv].Port:%02x",CS101_Slave_Pad[pdrv].Port);
//	log_i("CS101_Slave_Pad[pdrv].ModuleSN:%d",CS101_Slave_Pad[pdrv].ModuleSN);
//	log_i("CS101_Slave_Pad[pdrv].Encrypt:%02x",CS101_Slave_Pad[pdrv].Encrypt);
//	log_i("CS101_Slave_Pad[pdrv].BalanMode:%02x",CS101_Slave_Pad[pdrv].BalanMode);
//	log_i("CS101_Slave_Pad[pdrv].SourceAddr:%02x",CS101_Slave_Pad[pdrv].SourceAddr);
//	log_i("CS101_Slave_Pad[pdrv].LinkAddrSize:%02x",CS101_Slave_Pad[pdrv].LinkAddrSize);
//	log_i("CS101_Slave_Pad[pdrv].ASDUCotSize:%02x",CS101_Slave_Pad[pdrv].ASDUCotSize);
//	log_i("CS101_Slave_Pad[pdrv].ASDUAddrSize:%02x",CS101_Slave_Pad[pdrv].ASDUAddrSize);
//	log_i("CS101_Slave_Pad[pdrv].ASDUAddr:%02x",CS101_Slave_Pad[pdrv].ASDUAddr);
//
//
//	log_i("CS101_Slave_Pad[pdrv].YX_AllNum:%02x",CS101_Slave_Pad[pdrv].YX_AllNum);
//	log_i("CS101_Slave_Pad[pdrv].YC_AllNum:%02x",CS101_Slave_Pad[pdrv].YC_AllNum);


}

/**
  * @brief : 101初始化.
  * @param : [void]
  * @return: void
  * @updata: [2018-11-07][Chen][newly increased]
  */
int CS101_Slave_create(uint8_t pdrv, int portFd, IecParam sParam)
{
	CS101_Slave_ParamInit(pdrv,portFd,sParam);/*参数默认初始化值*/
	CS101_Slave_DiskInit(pdrv);/*disk初始化*/
	CS101_Slave_YxYcBackupInit(pdrv);/*初始化遥信遥测备份数组*/
	CS101_Slave_Appinit();/*app参数初始化*/
	return 0;
}

/**
  * @brief : 改变转发表初始化.
  * @param : [void]
  * @return: void
  * @updata: [2019-04-19][Chen][newly increased]
  */
uint8_t CS101_Slave_changePointTable(uint8_t num)
{
	uint8_t pdrv = 0;
	for(pdrv = 0; pdrv < num; pdrv++)
	{
//		CS101_Slave_Pad[pdrv].YX_AllNum = allInfoDisk->var.telesignal.maxNum;			TXL注释
//		CS101_Slave_Pad[pdrv].YC_AllNum = allInfoDisk->var.telemetry.maxNum;			TXL注释
		CS101SlaveDisk[pdrv].TelesignalCosOut = CS101SlaveDisk[pdrv].TelesignalCosIn;
		CS101SlaveDisk[pdrv].TelemetryNVAOut = CS101SlaveDisk[pdrv].TelemetryNVAIn;
		CS101_Slave_YxYcBackupInit(pdrv);/*初始化遥信遥测备份数组*/
	}
	return TRUE;
}

/**
  * @brief : 101线程暂停.
  * @param : [void]
  * @return: true
  * @updata: [2019-04-24][Chen][new]
  */
uint8_t CS101_Slave_pause(void)
{
	if (thread_Stop == FALSE)
	{
		pthread_mutex_lock(&mut);
		thread_Stop = TRUE;
//		log_w("slave101 thread pause!");
		pthread_mutex_unlock(&mut);
	}
	else
	{  
//		log_e("slave101 pthread pause already!");
	}
	return TRUE;
}

/**
  * @brief : 101线程恢复.
  * @param : [num]
  * @return: true
  * @updata: [2019-04-24][Chen][new]
  */
uint8_t CS101_Slave_resume(uint8_t num)
{
	if (thread_Stop == TRUE)
	{
		CS101_Slave_changePointTable(num);
		pthread_mutex_lock(&mut);
		thread_Stop = FALSE;
		pthread_cond_signal(&cond);
//		log_w("slave101 run pause!");
		pthread_mutex_unlock(&mut);
	}
	else
	{
//		log_e("slave101 pthread run already!");
	}
	return TRUE;
}

/**
  * @brief : 101运行启动 接口函数.
  * @param : [void]
  * @return: void
  * @updata: [2018-11-07][Chen][newly increased]
  */
void *CS101_Slave_Startup(void *arg)
{
  static uint16_t i = 0;
	uint8_t disk,num;
	disk = 0;
	num = *(uint8_t*)arg;

	for(;;)
	{
		pthread_mutex_lock(&mut);
		while (thread_Stop == TRUE)
		{
			pthread_cond_wait(&cond, &mut);
		}
		pthread_mutex_unlock(&mut);

		/*运行程序*/
		CS101_Slave_Clock(disk);

		if(++disk >= num)
		{
			disk = 0;
		}

		if (++i >= 200)
		{
			i = 0;
//			log_e("CS101_Slave_run");
		}
		usleep(10000);
	}
	return 0;
}





/* END OF FILE ----------------------------------------------------------------*/
