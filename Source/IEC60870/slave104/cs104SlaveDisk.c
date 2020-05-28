/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      cs104SlaveDisk.c
  * @brief:			104子站接口
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2018-11-07
  * @update:    [2018-11-07][Chen][newly increased]
  */
#define LOG_TAG    "CS104_Slave_Disk"
/* INCLUDE FILES -------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "../port/iec_interface.h"
#include "cs104SlaveApp.h"
#include "cs104SlaveDisk.h"
//#include "encrypt_disk.h"
#include "../point/dataSwitch_point_table.h"
#include "../tcp_socket/server.h"
#include "../file/fileOperate.h"
//#include "elog.h"
// #include "example/example.h"
/* PRIVATE VARIABLES ----------------------------------------------------------*/
static uint8_t file_array[CS104_SLAVE_VOLUMES][256];
static uint8_t temp_array[CS104_SLAVE_VOLUMES][256];
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

//static struct tagControlCfg info;//下发数据										TXL注释

struct CS104_SLAVE_DISK *CS104SlaveDisk = NULL;
static struct tagOverallCfg *allInfoDisk = NULL;
/* PRIVATE FUNCTION PROTOTYPES -----------------------------------------------*/
/**
  * @brief : read data from drv.
  * @param : [pdrv]
  * @param : [pbuf]
  * @param : [count]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint16_t CS104_Slave_Read_drv(uint8_t port, uint8_t *pbuf, uint16_t count)
{
  uint16_t len=0;
  // uint16_t i;

	len = ethernet_server_get(port, pbuf, count);

  //  if(len)
  //  {
  //  	printf("\n");
  //  	log_w("read %d data",len);
  //  	for(i=0;i<len;i++)
  //  	{
  //  		printf("%02x ",pbuf[i]);
  //  	}
  //  	printf("\n");
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
uint16_t CS104_Slave_Write_drv(uint8_t port, uint8_t *pbuf, uint16_t count)
{
    ethernet_server_put(port, pbuf, count);

    if(count)
    {
//    	log_w("write %d data",count);
//    	for(uint16_t i=0;i<count;i++)
//    	{
//    		printf("%02x ",pbuf[i]);
//    	}
//    	printf("\n");
    }
	return(count);
}

/**
  * @brief : read data to drv.
  * @param : [pdrv]
  * @param : [pbuf]
  * @param : [count]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint16_t CS104_Slave_ReadData(uint8_t pdrv, uint8_t *pbuf, uint16_t count)
{ 
    uint16_t len = 0; 
	
    if(CS104_Slave_Pad[pdrv].Encrypt == 0)
    {
        len = CS104_Slave_Read_drv(CS104_Slave_Pad[pdrv].Port, pbuf, count);
    }
    else
    {
//        len = Encrypt_Readx(pbuf, CS104_Slave_Pad[pdrv].Port);		TXL注释
    }
	return(len);
}

/**
  * @brief : write data from drv.
  * @param : [pdrv]
  * @param : [pbuf]
  * @param : [count]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint16_t CS104_Slave_WriteData(uint8_t pdrv, uint8_t *pbuf, uint16_t count)
{ 
	uint16_t len = 0;  

    if(CS104_Slave_Pad[pdrv].Encrypt == 0)		//加密
    {
        len = CS104_Slave_Write_drv(CS104_Slave_Pad[pdrv].Port,pbuf,count);
    }
    else
    {
        if(pbuf[2]&0x01)//u帧s帧
        {
//            len = Encrypt_WriteX(pbuf, count, 0x00, CS104_Slave_Pad[pdrv].Port);				TXL注释
        }
        else//i帧
        {                   
            switch(pbuf[6])
            {
                case CS104_SLAVE_C_SC_NA_1:
                case CS104_SLAVE_C_SC_NB_1:
				case CS104_SLAVE_F_SR_NA_1:
//										if((pbuf[15]&0x80)&&(pbuf[8]==CS104_SLAVE_COT_ACTCON))							TXL注释
//                    {return(Encrypt_WriteX(pbuf, count, 0x82, CS104_Slave_Pad[pdrv].Port));}	//启动确认
//                    else
//                    {return(Encrypt_WriteX(pbuf, count, 0x80, CS104_Slave_Pad[pdrv].Port));}	//结束确认
				
				case CS104_SLAVE_C_WS_NA_1:
//										if((pbuf[14]&0x80)&&(pbuf[8]==CS104_SLAVE_COT_ACTCON))							TXL注释
//                    {return(Encrypt_WriteX(pbuf, count, 0x82, CS104_Slave_Pad[pdrv].Port));}	//启动确认
//                    else
//                    {return(Encrypt_WriteX(pbuf, count, 0x80, CS104_Slave_Pad[pdrv].Port));}	//结束确认
					break;	//TXL注释			消除错误
//                default:
//                    return(Encrypt_WriteX(pbuf, count, 0x80, CS104_Slave_Pad[pdrv].Port));							TXL注释
            }
        }
    }
	return(len);
}

/**
  * @brief : Check Link.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS104_Slave_CheckLink(uint8_t pdrv)
{ 
    uint8_t res = 0;  
    switch(pdrv)
    {
		case CS104_SLAVE_DISK0:
			res = getServerLinkState(CS104_SLAVE_DISK0);
            break;
						
        case CS104_SLAVE_DISK1:
        	res = getServerLinkState(CS104_SLAVE_DISK1);
            break;
						
        default:
            break;
    }
    return(res);
}

/**
  * @brief : Stop Link.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS104_SLAVE_StopLink(uint8_t pdrv)
{ 
    switch(pdrv)
    {
		case CS104_SLAVE_DISK0:
			setServerCloseLink(CS104_SLAVE_DISK0);
            break;
		
        case CS104_SLAVE_DISK1:
        	setServerCloseLink(CS104_SLAVE_DISK1);
            break;
				
        default:
            break;
    }
    return(TRUE);
}

/**
  * @brief : all data command.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS104_SLAVE_CallAllDataProcess(uint8_t pdrv, uint8_t *pbuf)//总召
{
	CS104SlaveDisk[pdrv].TelesignalCosOut = CS104SlaveDisk[pdrv].TelesignalCosIn;
	CS104SlaveDisk[pdrv].TelemetryNVAOut = CS104SlaveDisk[pdrv].TelemetryNVAIn;
}

/**
  * @brief : 检查遥控信息.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [2019-05-30][Chen][new]
  */
static uint8_t CS104_Slave_CheckCtlRemoteCfg(uint8_t pdrv, uint8_t *pbuf,tagCtlRomteCfg_t tmp)
{
	tmp->addr = pbuf[8]|(pbuf[9]<<8);//地址
	tmp->cot  = pbuf[4]|(pbuf[5]<<8);//传送原因
	tmp->oper = pbuf[11];//操作

  if(pbuf[2] == CS104_SLAVE_C_SC_NA_1)	/*单点*/
	{
		tmp->value = (pbuf[11]&0x01)+1;
	}
	else
	{
		if((tmp->oper&0x03)==0||(tmp->oper&0x03)==3)
		{
			temp_array[pdrv][4] = CS104_SLAVE_COT_ACTCON|CS104_SLAVE_COT_NO;/*双点错误*/
			CS104_Slave_Reply(pdrv, temp_array[pdrv]);
			return FALSE;
		}
		tmp->value = pbuf[11]&0x03;
	}
//	if((allInfoDisk->var.telecontrol.firstPoint + (tmp->addr-TELECONTROL_STARTADDR))->IsNegated == TRUE)/*取反*/				TXL注释
//	{
//			tmp->value = (~tmp->value)&0x03;
//	}
    
	if(!((tmp->cot == CS104_SLAVE_COT_ACT)||(tmp->cot == CS104_SLAVE_COT_DEACT)))
	{
		temp_array[pdrv][4] = CS104_SLAVE_COT_COTERR;	/*传输原因错误*/
		CS104_Slave_Reply(pdrv, temp_array[pdrv]);
		return FALSE;
	}

//	if(tmp->addr < TELECONTROL_STARTADDR || tmp->addr > TELECONTROL_STARTADDR + allInfoDisk->var.telecontrol.maxNum)		TXL注释
//	{
//		temp_array[pdrv][4] = CS104_SLAVE_COT_ADDRERR;	/*信息体地址错误*/
//		CS104_Slave_Reply(pdrv, temp_array[pdrv]);
//		return FALSE;
//	}

	if((g_CommunicatFlag[COM_YK] == COMMUNICATLOCKSTATUS)||(g_CommunicatFlag[COM_YK] == CS104_Slave_Pad[pdrv].DevId))
	{
//		log_i("g_CommunicatFlag[COM_YK] TRUE:%02x",g_CommunicatFlag[COM_YK]);
	}
	else
	{
		temp_array[pdrv][4] = CS104_SLAVE_COT_ADDRERR;
		CS104_Slave_Reply(pdrv, temp_array[pdrv]);
		return FALSE;
	}

	if(tmp->oper&0x80)//预制
	{
//		if((Encrypt_ApplyType_Read(pdrv) != 0x05)&&(CS104_Slave_Pad[pdrv].Encrypt == 1))//加密数据判断						TXL注释
//		{
//			Encrypt_ReturnFaultInfo_IN(0x1f,0x91,0x01,CS104_Slave_Pad[pdrv].DevId);
//			return FALSE;
//		}
	}
	else//执行
	{
//		if((Encrypt_ApplyType_Read(pdrv) != 0x07)&&(CS104_Slave_Pad[pdrv].Encrypt == 1))//加密数据判断						TXL注释
//		{
//			Encrypt_ReturnFaultInfo_IN(0x1f,0x91,0x01,CS104_Slave_Pad[pdrv].DevId);
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
void CS104_SLAVE_HandleCtrlProcess(uint8_t pdrv, uint8_t *pbuf)
{
//	static uint8_t reinfo = 0;
	struct tagCtlRomteCfg tmp;//临时结构体
//	struct tagControlRemoteCfg ykCmd;												TXL注释

//	info.pdata = &ykCmd;															TXL注释
	memcpy(temp_array[pdrv],pbuf,pbuf[0]);

	if(CS104_Slave_CheckCtlRemoteCfg(pdrv,pbuf,&tmp) == FALSE)/*数据检查*/
	{
		return;
	}
	/*参数赋值*/
//	info.com.dir = allInfoDisk->def.message_DOWN;									TXL注释
//	info.com.send = CS104_Slave_Pad[pdrv].ModuleSN;//模块ID
//	info.com.tpye = allInfoDisk->def.type_REMOTE;
//	info.pBackData = temp_array[pdrv]; // 为发起方暂存数据
//	info.backDataLen = pbuf[0]; // 为发起方暂存数据长度
//
//	ykCmd.addr = (allInfoDisk->var.telecontrol.firstPoint + (tmp.addr-TELECONTROL_STARTADDR))->AddrUsed;
//	ykCmd.sta  = allInfoDisk->def.control_RES_RELEASE;
//	ykCmd.work = tmp.value;

	/*逻辑判断*/
	if((tmp.cot == CS104_SLAVE_COT_ACT) && (tmp.oper&0x80))/*预制*/
	{
//		log_i("YK select");
//		ykCmd.cmd = allInfoDisk->def.control_OPREAT_CHOICE;							TXL注释
	}
	else if((tmp.cot == CS104_SLAVE_COT_ACT) && ((tmp.oper&0x80) == 0))/*执行*/
	{
//		log_i("YK operate");
//		ykCmd.cmd = allInfoDisk->def.control_OPREAT_EXECUTE;						TXL注释
	}
	else if((tmp.cot == CS104_SLAVE_COT_DEACT) && ((tmp.oper&0x80) == 0))/*撤销*/
	{
//		log_i("YK cancel");
//		ykCmd.cmd = allInfoDisk->def.control_OPREAT_CANCEL;							TXL注释
	}
	else
	{
		temp_array[pdrv][4] = CS104_SLAVE_COT_ADDRERR;	/*未知错误*/
		CS104_Slave_Reply(pdrv, temp_array[pdrv]);
		return;
	}

//	reinfo = allInfoDisk->fun.operate_Control_info(&info);/*发送遥控命令 */			TXL注释
//
//	if(reinfo == allInfoDisk->def.control_RES_WAIT)/*等待*/
//	{
//		log_e("YK select:control_RES_WAIT");
//		return;
//	}
//	else if(reinfo == allInfoDisk->def.control_RES_FAIL)/*失败*/
//	{
//		log_w("YK select:RETURN_ERROR");
//		temp_array[pdrv][4] = CS104_SLAVE_COT_ACTCON|CS104_SLAVE_COT_NO;//选择|执行
//		if((tmp.cot == CS104_SLAVE_COT_DEACT) && ((tmp.oper&0x80) == 0))//撤销
//		{
//			temp_array[pdrv][4] = CS104_SLAVE_COT_DEACTCON|CS104_SLAVE_COT_NO;
//		}
//		CS104_Slave_Reply(pdrv, temp_array[pdrv]);
//	}
//	else if(reinfo == allInfoDisk->def.control_RES_SUCCEED)/*成功*/
//	{
//		log_w("YK select:RETURN_TRUE");
//		if((tmp.cot == CS104_SLAVE_COT_DEACT) && ((tmp.oper&0x80) == 0))//撤销
//		{
//			temp_array[pdrv][4] = CS104_SLAVE_COT_DEACTCON;
//		}
//		else if((tmp.cot == CS104_SLAVE_COT_ACT) && (tmp.oper&0x80))//选择
//		{
//			temp_array[pdrv][4] = CS104_SLAVE_COT_ACTCON;
//		}
//		else//执行
//		{
//			temp_array[pdrv][4] = CS104_SLAVE_COT_ACTCON;
//			CS104_Slave_Reply(pdrv, temp_array[pdrv]);
//			temp_array[pdrv][4] = CS104_SLAVE_COT_ACCTTERM;/*遥控结束*/
//		}
//		CS104_Slave_Reply(pdrv, temp_array[pdrv]);
//	}
}

/**
  * @brief : clock synchronization command.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS104_SLAVE_SYNProcess(uint8_t pdrv, uint8_t *pbuf)
{
//	struct ds_privateTime getTime;								TXL注释
//	struct ds_cp56Time2a setTime;

    memcpy(temp_array[pdrv],pbuf,sizeof(temp_array[pdrv]));

    switch(pbuf[4])
    {
        case CS104_SLAVE_COT_REQ:
            temp_array[pdrv][4] = CS104_SLAVE_COT_REQ;
//            allInfoDisk->fun.get_currentTime_info(&getTime);									TXL注释
//            memcpy(&temp_array[pdrv][11],(uint8_t *)&getTime,sizeof(struct ds_cp56Time2a));
            break;
        case CS104_SLAVE_COT_ACT:
            temp_array[pdrv][4] = CS104_SLAVE_COT_ACTCON;
            //timep = *(struct ds_cp56Time2a *)&temp_array[pdrv][10];
            //time = *(struct ds_privateTime *)&timep;
//            memcpy(&setTime,&temp_array[pdrv][11],sizeof(struct ds_cp56Time2a));				TXL注释
//            allInfoDisk->fun.write_currentTime_info(&setTime);
            break;
        default:
            temp_array[pdrv][4] = CS104_SLAVE_COT_COTERR;
            break;
    }

    CS104_Slave_Reply(pdrv, temp_array[pdrv]);
}

/**
  * @brief : get alarm of ms.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: time_ms
  * @updata: [2019-04-22][chen][new]
  */
uint64_t CS104_SLAVE_getAlarmOfMs(uint8_t pdrv)
{
	struct ds_privateTime getTime;														//TXL注释
	allInfoDisk->fun.get_currentTime_info(&getTime);
	return(getTime.time_ms);
}

/**
  * @brief : reset command.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS104_SLAVE_ResetProcess(uint8_t pdrv, uint8_t *pbuf)
{
	CS104_Slave_Pad[pdrv].resetFlag = 1;
}

/**
  * @brief : 线程退出.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-07-08][Chen][new]
  */
void CS104_Slave_exit(uint8_t pdrv)
{
//	log_e("exit(0)");													TXL注释
//	exit(0);
}

/**
  * @brief : 获取定值预制超时时间.
  * @param : [void]
  * @return: true超时复位
  * @updata: [2019-03-11][Chen][new]
  */
uint8_t getDzPreTimeOut104(uint8_t pdrv)
{
	uint64_t timeMs = CS104_SLAVE_getAlarmOfMs(pdrv);
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
uint8_t CS104_Slave_DZ_RevokeOperation(uint8_t pdrv)
{
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
void CS104_SLAVE_FixedParaProcess(uint8_t pdrv, uint8_t *pbuf)//定值操作
{
	uint16_t i = 0;
//	uint8_t size = 0;
	uint16_t addr = 0;/*地址*/
	uint16_t num;/*数量*/
	uint16_t sendNum = 0;/*发送给主站数量*/
	uint16_t sendNumGroup = 0;/*发送给主站数量*/
	uint8_t areaCode;/*区号*/
//	tagValueParaCfg_t pvalueParaStr;												TXL注释
	float *pValue = NULL;/*值*/
	uint16_t readAddrBuf[256];/*要读取的地址*/
	uint16_t readBufSize = 0;
	uint64_t timeMs = CS104_SLAVE_getAlarmOfMs(pdrv);

//	log_w("DZ receive all");
	memcpy(temp_array[pdrv],pbuf,pbuf[0]);

	if((g_CommunicatFlag[COM_PARA] == COMMUNICATLOCKSTATUS)||(g_CommunicatFlag[COM_PARA] == CS104_Slave_Pad[pdrv].DevId))
	{
//		log_i("g_CommunicatFlag[COM_PARA] TRUE:%02x",g_CommunicatFlag[COM_PARA]);
	}
	else
	{
		temp_array[pdrv][4] = CS104_SLAVE_COT_ADDRERR;
		CS104_Slave_Reply(pdrv, temp_array[pdrv]);
		return;
	}

	switch(pbuf[2]) /*应用类型*/
	{
		case CS104_SLAVE_C_SR_NA_1:/*切换定值区号*/
//			if((Encrypt_ApplyType_Read(pdrv) != 0x01)&&(CS104_Slave_Pad[pdrv].Encrypt == 1))		TXL注释
//			{
//				Encrypt_ReturnFaultInfo_IN(0x1f,0x91,0x01,CS104_Slave_Pad[pdrv].DevId);
//				return;
//			}
//			log_i("operate_areaCode_info");
//			if(allInfoDisk->fun.operate_areaCode_info(pbuf[11]) == RETURN_TRUE)
//			{
//				log_i("RETURN_TRUE");
//				temp_array[pdrv][4]=7;
//			}
//			else
//			{
//				log_w("RETURN_ERROR");
//				temp_array[pdrv][4]=47;
//			}
			CS104_Slave_Reply(pdrv, temp_array[pdrv]);
			break;

		case CS104_SLAVE_C_RR_NA_1:/*读定值区号*/
//			log_w("read currentFixedAreaCodeAddr ");
//			addr = allInfoDisk->var.currentFixedAreaCode.addr;					TXL注释
//			if(allInfoDisk->fun.get_value_point(&areaCode,addr,(void*)&pValue,&size) == RETURN_ERROR)		TXL注释
//			{
//				log_w("currentFixedAreaCodeAddr RETURN_ERROR");
//			}
			areaCode = (uint8_t)(*(float*)pValue);

            temp_array[pdrv][0] = temp_array[pdrv][0] + 6;
            temp_array[pdrv][4] = 7;
            temp_array[pdrv][11] = areaCode;
            temp_array[pdrv][12] = 0;
//            temp_array[pdrv][13] = AREACODE_RUN;															TXL注释
            temp_array[pdrv][14] = 0;
//            temp_array[pdrv][15] = AREACODE_ZONE2;														TXL注释
            temp_array[pdrv][16] = 0;
            CS104_Slave_Reply(pdrv, temp_array[pdrv]);
			break;

		case CS104_SLAVE_C_RS_NA_1:/*读参数和定值*/

			areaCode = pbuf[8];

			if((pbuf[3]&0x7f)==0)//vsq	读全部参数
			{
                if(areaCode == 0) /*0区*/
                {
                	addr = 0x8101;
                	num = 83;
//                	addr = VALUEPARABASE_STARTADDR;															TXL注释
//                	num  = allInfoDisk->var.valueParaBase.maxNum;
                }
                else
                {
                	addr = 0x8301;
                	num = 83;
//                    addr = VALUEPARAZOON_STARTADDR;														TXL注释
//                    num  = allInfoDisk->var.valueParaZone[areaCode].maxNum;
                }
                for(i = 0; i < num; i++)
                {
                	readAddrBuf[i] = addr+i;
                }
			}
			else	/*读部分参数*/
			{
				num = pbuf[3]&0x7f;
				for(i = 0; i < num; i++)
				{
					readAddrBuf[i] = *(uint16_t *)&pbuf[10+(i*3)];
				}
			}
			memcpy(temp_array[pdrv],pbuf,pbuf[0]);
			if(num == 0)
			{
				temp_array[pdrv][4] = 47;
				CS104_Slave_Reply(pdrv, temp_array[pdrv]);
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
					temp_array[pdrv][13+readBufSize] = 0;
					// log_w("get_value_point");
//					if(allInfoDisk->fun.get_struct_point(&areaCode,readAddrBuf[i],(void*)&pvalueParaStr) == RETURN_ERROR)			TXL注释
//					{
//						log_w("get_value_point RETURN_ERROR %x",readAddrBuf[i]);
//						return;
//					}
					// {log_w("%d,%d,%f",pvalueParaStr->Type,pvalueParaStr->TypeSize,*(float *)pvalueParaStr->pVal);}
//					temp_array[pdrv][14+readBufSize+0] = pvalueParaStr->Type;														TXL注释
//					temp_array[pdrv][14+readBufSize+1] = pvalueParaStr->TypeSize;
//					memcpy(&temp_array[pdrv][14+readBufSize+2],pvalueParaStr->pVal,pvalueParaStr->TypeSize);/*地址对应值*/
//					readBufSize += (pvalueParaStr->TypeSize+2+3);
					/*----------------此处随便赋的值---------------------*/
					temp_array[pdrv][14+readBufSize+0] = 4;			//tag类型										//TXL注释
					temp_array[pdrv][14+readBufSize+1] = 24;		//数据长度
					//D03C.1.CXXXX.XXXX.190624
					uint8_t tmp = 0;
					tmp = 0x44;
					memcpy(&temp_array[pdrv][14+readBufSize+2],&tmp,1);
					tmp = 0x30;
					memcpy(&temp_array[pdrv][14+readBufSize+3],&tmp,1);
					tmp = 0x33;
					memcpy(&temp_array[pdrv][14+readBufSize+4],&tmp,1);
					tmp = 0x43;
					memcpy(&temp_array[pdrv][14+readBufSize+5],&tmp,1);
					tmp = 0x2e;
					memcpy(&temp_array[pdrv][14+readBufSize+6],&tmp,1);
					tmp = 0x31;
					memcpy(&temp_array[pdrv][14+readBufSize+7],&tmp,1);
					tmp = 0x2e;
					memcpy(&temp_array[pdrv][14+readBufSize+8],&tmp,1);
					tmp = 0x43;
					memcpy(&temp_array[pdrv][14+readBufSize+9],&tmp,1);
					tmp = 0x58;
					memcpy(&temp_array[pdrv][14+readBufSize+10],&tmp,1);
					tmp = 0x58;
					memcpy(&temp_array[pdrv][14+readBufSize+11],&tmp,1);
					tmp = 0x58;
					memcpy(&temp_array[pdrv][14+readBufSize+12],&tmp,1);
					tmp = 0x58;
					memcpy(&temp_array[pdrv][14+readBufSize+13],&tmp,1);
					tmp = 0x2e;
					memcpy(&temp_array[pdrv][14+readBufSize+14],&tmp,1);
					tmp = 0x58;
					memcpy(&temp_array[pdrv][14+readBufSize+15],&tmp,1);
					tmp = 0x58;
					memcpy(&temp_array[pdrv][14+readBufSize+16],&tmp,1);
					tmp = 0x58;
					memcpy(&temp_array[pdrv][14+readBufSize+17],&tmp,1);
					tmp = 0x58;
					memcpy(&temp_array[pdrv][14+readBufSize+18],&tmp,1);
					tmp = 0x2e;
					memcpy(&temp_array[pdrv][14+readBufSize+19],&tmp,1);
					tmp = 0x31;
					memcpy(&temp_array[pdrv][14+readBufSize+20],&tmp,1);
					tmp = 0x39;
					memcpy(&temp_array[pdrv][14+readBufSize+21],&tmp,1);
					tmp = 0x30;
					memcpy(&temp_array[pdrv][14+readBufSize+22],&tmp,1);
					tmp = 0x36;
					memcpy(&temp_array[pdrv][14+readBufSize+23],&tmp,1);
					tmp = 0x32;
					memcpy(&temp_array[pdrv][14+readBufSize+24],&tmp,1);
					tmp = 0x34;
					memcpy(&temp_array[pdrv][14+readBufSize+25],&tmp,1);
					readBufSize += (24+2+3);
					/*--------------------------------------------------*/
					sendNumGroup++;
					if(readBufSize >200)
					{
						break;
					}
				}
				sendNum += sendNumGroup;
//				log_i("sendNum:%d",sendNum);																						TXL注释
				temp_array[pdrv][0] = 0x0a + 1 + readBufSize;
				temp_array[pdrv][3] = sendNumGroup;
				temp_array[pdrv][4] = 7;
				temp_array[pdrv][5] = 0;
				if(sendNum<num)
				{temp_array[pdrv][10] = 0x01;}
				else
				{temp_array[pdrv][10] = 0x00;}
				CS104_Slave_Reply(pdrv, temp_array[pdrv]);
			}
//			log_i("break");																											TXL注释
			break;

		case CS104_SLAVE_C_WS_NA_1:/*写参数和定值*/

			 areaCode = pbuf[8];
			 if(pbuf[10]&0x40)/*取消预制*/
			 {
//				 if((Encrypt_ApplyType_Read(pdrv) != 0x03)&&(CS104_Slave_Pad[pdrv].Encrypt == 1))									TXL注释
//				 {
//					Encrypt_ReturnFaultInfo_IN(0x1f,0x91,0x01,CS104_Slave_Pad[pdrv].DevId);
//					break;
//				 }
				 dzPreFlag = FALSE;/*预制超时标志*/
				 preOperation = FALSE;
				 writeNum = 0;
				 temp_array[pdrv][4] = 9;
				 temp_array[pdrv][5] = 0;
				 g_CommunicatFlag[COM_PARA] = COMMUNICATLOCKSTATUS;
				 CS104_Slave_Reply(pdrv, temp_array[pdrv]);
			 }
			 else if(pbuf[10]&0x80)/*预制*/
			 {
//				 if((Encrypt_ApplyType_Read(pdrv) != 0x01)&&(CS104_Slave_Pad[pdrv].Encrypt == 1))									TXL注释
//				 {
//					Encrypt_ReturnFaultInfo_IN(0x1f,0x91,0x01,CS104_Slave_Pad[pdrv].DevId);
//					break;
//				 }
				 num = pbuf[3]&0x7f;
				 if(num == 0)
				 {
					 temp_array[pdrv][4] = 47;
					 CS104_Slave_Reply(pdrv, temp_array[pdrv]);
					 break;
				 }
//                 log_i("DZ preSet num:%d",num);
				 if(g_CommunicatFlag[COM_PARA] == COMMUNICATLOCKSTATUS)
				 {
					 preOperation  = FALSE;
					 g_CommunicatFlag[COM_PARA] = CS104_Slave_Pad[pdrv].DevId;
				 }

				 if(preOperation == FALSE)
				 {
					 for(i = 0; i < num; i++)
					 {
						 writeDZbuf[writeNum + i].addr = *(uint16_t *)&pbuf[11+sendNum];
						 writeDZbuf[writeNum + i].tag  = pbuf[14+sendNum];
						 writeDZbuf[writeNum + i].size = pbuf[15+sendNum];
						 memcpy(writeDZbuf[writeNum + i].value,&pbuf[16+sendNum],pbuf[15+sendNum]);
						 sendNum += (writeDZbuf[writeNum + i].size+5);
					 }
					 writeNum += num;
					 if(!(pbuf[10]&0x01))
					 {
						 preOperation = TRUE;
					 }
					dzPreFlag = TRUE;/*预制超时标志*/
					dzPreTimeOutStart = dzPreTimeOutEnd = timeMs;	
					 temp_array[pdrv][4] = 7;
					 temp_array[pdrv][5] = 0;
					 g_CommunicatFlag[COM_PARA] = CS104_Slave_Pad[pdrv].DevId;
					 CS104_Slave_Reply(pdrv, temp_array[pdrv]);
				 }
				 else
				 {
					 temp_array[pdrv][4] = 47;
					 temp_array[pdrv][5] = 0;
					 CS104_Slave_Reply(pdrv, temp_array[pdrv]);
					 preOperation  = FALSE;
					 writeNum = 0;
					 g_CommunicatFlag[COM_PARA] = COMMUNICATLOCKSTATUS;
				 }
			 }
			 else/*固化*/
			 {
//				 if((Encrypt_ApplyType_Read(pdrv) != 0x03)&&(CS104_Slave_Pad[pdrv].Encrypt == 1))							TXL注释
//				 {
//					Encrypt_ReturnFaultInfo_IN(0x1f,0x91,0x01,CS104_Slave_Pad[pdrv].DevId);
//					break;
//				 }
			    if((g_CommunicatFlag[COM_PARA] == CS104_Slave_Pad[pdrv].DevId)&&(preOperation == TRUE))
				 {
					 for(i = 0; i < writeNum; i++)
					 {
//						 if(allInfoDisk->fun.write_valuePara_info(&areaCode, writeDZbuf[i].addr,							TXL注释
//								 	 writeDZbuf[i].value, writeDZbuf[i].size) == RETURN_ERROR)
//						 {log_w("write_valuePara_info RETURN_ERROR");}
					 }
					 temp_array[pdrv][4] = 7;
					 temp_array[pdrv][5] = 0;
					 CS104_Slave_Reply(pdrv, temp_array[pdrv]);
				 }
				 else
				 {
					 temp_array[pdrv][4] = 47;
					 temp_array[pdrv][5] = 0;
					 CS104_Slave_Reply(pdrv, temp_array[pdrv]);
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
  * @updata: [2018-12-14][Chen][newly increased]
  */
uint8_t CS104_Slave_C_YX(uint8_t pdrv)
{
	uint16_t i,maxnum = 0;
	uint8_t value = 0;
	uint8_t negate = 0;
	uint8_t upTypeCos = 0;
	uint8_t upCos = 0;
//	struct ds_privateTime nowtime;																				TXL注释
//
//	allInfoDisk->fun.get_currentTime_info(&nowtime);
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
		if(upTypeCos == CS104_SLAVE_M_SP_NA_1)/*单点*/
		{
			value = (value - 1)&0x01;
		}

		if(value != CS104SlaveDisk[pdrv].telesignalBackup[i])
		{
			if(upCos == TRUE)
			{
//				CS104SlaveDisk[pdrv].TelesignalCosIn->addr = i+TELESIGNAL_STARTADDR;	/*地址*/					TXL注释
				CS104SlaveDisk[pdrv].TelesignalCosIn->value = value;	/*值*/
				CS104SlaveDisk[pdrv].TelesignalCosIn->uptype = upTypeCos;/*上送类型*/

				if(++CS104SlaveDisk[pdrv].TelesignalCosIn >= CS104SlaveDisk[pdrv].TelesignalCosRep+COS_MAX_NUM)
				{
					CS104SlaveDisk[pdrv].TelesignalCosIn = CS104SlaveDisk[pdrv].TelesignalCosRep;
				}
			}
			CS104SlaveDisk[pdrv].telesignalBackup[i] = value;
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
uint8_t CS104_SLAVE_H_COS(uint8_t pdrv)//判断是否有COS
{
	CS104_Slave_C_YX(pdrv);/*定时比较 暂时放在这里*/

	if(CS104SlaveDisk[pdrv].TelesignalCosOut != CS104SlaveDisk[pdrv].TelesignalCosIn)
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
void CS104_SLAVE_R_COS(uint8_t pdrv, uint8_t *pbuf)//读cos
{
	uint8_t i = 0;
	uint8_t sendnum = 0;
	uint8_t uptype = 0;

	for(i=0; CS104SlaveDisk[pdrv].TelesignalCosOut != CS104SlaveDisk[pdrv].TelesignalCosIn; i++)
	{
		uptype = CS104SlaveDisk[pdrv].TelesignalCosOut->uptype;
        temp_array[pdrv][sendnum*(sizeof(struct COS_Str)+1) + 8] = LOBYTE(CS104SlaveDisk[pdrv].TelesignalCosOut->addr);
        temp_array[pdrv][sendnum*(sizeof(struct COS_Str)+1) + 9] = HIBYTE(CS104SlaveDisk[pdrv].TelesignalCosOut->addr);
        temp_array[pdrv][sendnum*(sizeof(struct COS_Str)+1) + 10] = 0;
        memcpy(&temp_array[pdrv][sendnum*(sizeof(struct COS_Str)+1) + 11],&CS104SlaveDisk[pdrv].TelesignalCosOut->value,sizeof(struct COS_Str)-2);

		if(++CS104SlaveDisk[pdrv].TelesignalCosOut >= CS104SlaveDisk[pdrv].TelesignalCosRep+COS_MAX_NUM)
		{
			CS104SlaveDisk[pdrv].TelesignalCosOut = CS104SlaveDisk[pdrv].TelesignalCosRep;
		}

		if(++sendnum >= 10)	/*每包上送数量*/
		{
			break;
		}
		if(uptype != CS104SlaveDisk[pdrv].TelesignalCosOut->uptype)/*与下一个上送类型不同*/
		{
			break;
		}
	}

	temp_array[pdrv][0] = 8 + sendnum*(sizeof(struct COS_Str)+1);	/*LENTH*/
	temp_array[pdrv][2] = uptype; /*类型*/
	temp_array[pdrv][3] = sendnum;
	temp_array[pdrv][4] = CS104_SLAVE_COT_SPONT;
	temp_array[pdrv][5] = 0;
	temp_array[pdrv][6] = LOBYTE(CS104_Slave_Pad[pdrv].PubAddress);
	temp_array[pdrv][7] = HIBYTE(CS104_Slave_Pad[pdrv].PubAddress);

	memcpy(pbuf,temp_array[pdrv],temp_array[pdrv][0]);
}

/**
  * @brief : judg soe.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS104_SLAVE_H_SOE(uint8_t pdrv)//判断是否有soe
{
//	if(CS104SlaveDisk[pdrv].SoeOut != *allInfoDisk->var.changeSoe.in)										TXL注释
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
void CS104_SLAVE_R_SOE(uint8_t pdrv, uint8_t *pbuf)//读soe
{
//	uint16_t addr = 0;
	uint8_t sendnum = 0;
//	uint8_t value = 0;
//	uint8_t negate = 0;
//	uint8_t upTypeCos = 0;
	uint8_t upTypeSoe = 0;
//	uint8_t upSoe;

//	for(; CS104SlaveDisk[pdrv].SoeOut != *allInfoDisk->var.changeSoe.in;)										TXL注释
//	{
//		addr = (allInfoDisk->var.changeSoe.firstPoint+CS104SlaveDisk[pdrv].SoeOut)->addr;
//		negate = (allInfoDisk->var.telesignal.firstPoint+addr-1)->IsNegated;/*取反标志*/
//		upTypeCos = (allInfoDisk->var.telesignal.firstPoint+addr-1)->IsDoublePoint;/*上送类型*/
//		upSoe  = (allInfoDisk->var.telesignal.firstPoint+addr-1)->IsSOE;
//		value = (allInfoDisk->var.changeSoe.firstPoint+CS104SlaveDisk[pdrv].SoeOut)->value;
//		log_w("upTypeCos:%d,addr:0x%02x,value:%d",upTypeCos,addr,value);
//		if(upTypeCos == 0)
//		{
//			exit(1);///////////////////////////////////////////////////////////////////////////////
//		}
//		if(upTypeCos == CS104_SLAVE_M_SP_NA_1)/*单点*/
//		{
//			upTypeSoe = CS104_SLAVE_M_SP_TB_1;
//		}
//		if(upTypeCos == CS104_SLAVE_M_DP_NA_1)/*双点*/
//		{
//			upTypeSoe = CS104_SLAVE_M_DP_TB_1;
//		}
//
//		if(negate == TRUE)/*取反*/
//		{
//			value = (~value)&0x03;
//		}
//		if(upTypeCos == CS104_SLAVE_M_SP_NA_1)/*单点*/
//		{
//			value = (value - 1)&0x01;
//		}
//
//		if(upSoe == TRUE)
//		{
//			temp_array[pdrv][sendnum*(sizeof(struct SOE_Str)+1) + 8] = LOBYTE((allInfoDisk->var.changeSoe.firstPoint+CS104SlaveDisk[pdrv].SoeOut)->addr);
//			temp_array[pdrv][sendnum*(sizeof(struct SOE_Str)+1) + 9] = HIBYTE((allInfoDisk->var.changeSoe.firstPoint+CS104SlaveDisk[pdrv].SoeOut)->addr);
//			temp_array[pdrv][sendnum*(sizeof(struct SOE_Str)+1) + 10] = 0;
//			temp_array[pdrv][sendnum*(sizeof(struct SOE_Str)+1) + 11] = value;
//			memcpy(&temp_array[pdrv][sendnum*(sizeof(struct SOE_Str)+1) + 12],&(allInfoDisk->var.changeSoe.firstPoint+CS104SlaveDisk[pdrv].SoeOut)->value,sizeof(struct CP56Time2a_t));
//		}
//
//		if(++CS104SlaveDisk[pdrv].SoeOut >= *allInfoDisk->var.changeSoe.maxNum)
//		{
//			CS104SlaveDisk[pdrv].SoeOut = 0;
//		}
//
//		if(++sendnum >= 10)	/*每包上送数量*/
//		{
//			break;
//		}
//		if(upTypeCos != (allInfoDisk->var.telesignal.firstPoint+addr)->IsDoublePoint)/*与下一个上送类型不同*/
//		{
//			break;
//		}
//	}

	temp_array[pdrv][0] = 8 + sendnum*(sizeof(struct SOE_Str)+1);	/*LENTH*/
	temp_array[pdrv][2] = upTypeSoe; /*类型*/
	temp_array[pdrv][3] = sendnum;
	temp_array[pdrv][4] = CS104_SLAVE_COT_SPONT;
	temp_array[pdrv][5] = 0;
	temp_array[pdrv][6] = LOBYTE(CS104_Slave_Pad[pdrv].PubAddress);
	temp_array[pdrv][7] = HIBYTE(CS104_Slave_Pad[pdrv].PubAddress);
	memcpy(pbuf,temp_array[pdrv],temp_array[pdrv][0]);
}

/**
  * @brief : 定时启动 比较遥测变化.
  * @param : [pdrv]
  * @param :
  * @return: none
  * @updata: [2018-12-14][Chen][newly increased]
  */
uint8_t CS104_Slave_C_YC(uint8_t pdrv)
{
	uint16_t i,maxnum = 0;

	float tempf = 0;
	uint8_t  IsDeadZone = 0; // 上送类型
	float	overValue = 0;

//	maxnum = allInfoDisk->var.telemetry.maxNum;															TXL注释
	for(i=0; i<maxnum; i++)
	{
//		tempf  = *(allInfoDisk->var.telemetry.firstPoint+i)->pVal; /*获取原值*/							TXL注释
//		IsDeadZone = (allInfoDisk->var.telemetry.firstPoint+i)->IsDeadZone;
		if(IsDeadZone == TRUE)/*是否上送NVA*/
		{
//			overValue = (allInfoDisk->var.telemetry.firstPoint+i)->Rating *(allInfoDisk->var.telemetry.firstPoint+i)->Deadband;				TXL注释
			if(fabsf(tempf - CS104SlaveDisk[pdrv].telemetryBackup[i]) > overValue)
			{
//				CS104SlaveDisk[pdrv].TelemetryNVAIn->addr = i + TELEMETRY_STARTADDR;/*地址*/													TXL注释
				CS104SlaveDisk[pdrv].TelemetryNVAIn->value = tempf;/*值*/
//				CS104SlaveDisk[pdrv].TelemetryNVAIn->uptype = (allInfoDisk->var.telemetry.firstPoint+i)->DataType;/*上送类型*/				TXL注释
//				CS104SlaveDisk[pdrv].TelemetryNVAIn->MultiplRate = (allInfoDisk->var.telemetry.firstPoint+i)->Rate;/*倍率*/
				if(++CS104SlaveDisk[pdrv].TelemetryNVAIn >= CS104SlaveDisk[pdrv].TelemetryNVARep+NVA_MAX_NUM)
				{
					CS104SlaveDisk[pdrv].TelemetryNVAIn = CS104SlaveDisk[pdrv].TelemetryNVARep;
				}
				CS104SlaveDisk[pdrv].telemetryBackup[i] = tempf;
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
uint8_t CS104_SLAVE_H_NVA(uint8_t pdrv)//判断是否有NVA
{
	CS104_Slave_C_YC(pdrv);
	if(CS104SlaveDisk[pdrv].TelemetryNVAOut != CS104SlaveDisk[pdrv].TelemetryNVAIn)
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
void CS104_SLAVE_R_NVA(uint8_t pdrv, uint8_t *pbuf)//读NVA
{
	uint8_t i = 0;
	uint8_t sendnum = 0;
	int16_t tempu;
	float MultiplRate;
	uint8_t  uptype = 0; // 上送类型
	struct NVA_Str nva_temp;

	for(i=0; CS104SlaveDisk[pdrv].TelemetryNVAOut != CS104SlaveDisk[pdrv].TelemetryNVAIn; i++)
	{
		uptype = CS104SlaveDisk[pdrv].TelemetryNVAOut->uptype;
		nva_temp.value  = CS104SlaveDisk[pdrv].TelemetryNVAOut->value;
		nva_temp.addr   = CS104SlaveDisk[pdrv].TelemetryNVAOut->addr;
		MultiplRate = CS104SlaveDisk[pdrv].TelemetryNVAOut->MultiplRate;
//		log_w("uptype:%d,addr:0x%02x,value:%f",uptype,nva_temp.addr,nva_temp.value);
		if(uptype == 0)
		{
			exit(1);//////////////////////////////////////////////////////////////////////////////////////
		}
		switch(uptype)
		{
			case CS104_SLAVE_M_ME_NA_1:
				tempu = (int16_t)(nva_temp.value*MultiplRate);
				temp_array[pdrv][sendnum*(sizeof(nva_temp.addr)+ sizeof(int16_t)+1+1) + 8] = LOBYTE(nva_temp.addr);
				temp_array[pdrv][sendnum*(sizeof(nva_temp.addr)+ sizeof(int16_t)+1+1) + 9] = HIBYTE(nva_temp.addr);
				temp_array[pdrv][sendnum*(sizeof(nva_temp.addr)+ sizeof(int16_t)+1+1) + 10] = 0;
				temp_array[pdrv][sendnum*(sizeof(nva_temp.addr)+ sizeof(int16_t)+1+1) + sizeof(nva_temp.addr)+ sizeof(int16_t)+1 + 8] = 0x00;//QDS
				memcpy(&temp_array[pdrv][sendnum*(sizeof(nva_temp.addr)+ sizeof(int16_t)+1+1) + 11],(uint8_t *)&tempu,sizeof(int16_t));
				break;
			case CS104_SLAVE_M_ME_NB_1:
				tempu = (int16_t)(nva_temp.value*MultiplRate);
				temp_array[pdrv][sendnum*(sizeof(nva_temp.addr)+ sizeof(int16_t)+1+1) + 8] = LOBYTE(nva_temp.addr);
				temp_array[pdrv][sendnum*(sizeof(nva_temp.addr)+ sizeof(int16_t)+1+1) + 9] = HIBYTE(nva_temp.addr);
				temp_array[pdrv][sendnum*(sizeof(nva_temp.addr)+ sizeof(int16_t)+1+1) + 10] = 0;
				temp_array[pdrv][sendnum*(sizeof(nva_temp.addr)+ sizeof(int16_t)+1+1) + sizeof(nva_temp.addr)+ sizeof(int16_t)+1 + 8] = 0x00;//QDS
				memcpy(&temp_array[pdrv][sendnum*(sizeof(nva_temp.addr)+ sizeof(int16_t)+1+1) + 11],(uint8_t *)&tempu,sizeof(int16_t));
				break;
			case CS104_SLAVE_M_ME_NC_1:/*短浮点*/
				temp_array[pdrv][sendnum*(sizeof(struct NVA_Str)+1+1) + 8] = LOBYTE(nva_temp.addr);
				temp_array[pdrv][sendnum*(sizeof(struct NVA_Str)+1+1) + 9] = HIBYTE(nva_temp.addr);
				temp_array[pdrv][sendnum*(sizeof(struct NVA_Str)+1+1) + 10] = 0;
				temp_array[pdrv][sendnum*(sizeof(struct NVA_Str)+1+1) + sizeof(struct NVA_Str)+1 + 8] = 0x00;//QDS
				memcpy(&temp_array[pdrv][sendnum*(sizeof(struct NVA_Str)+1+1) + 11],(uint8_t *)&nva_temp.value,sizeof(struct NVA_Str)-2);
				break;
			default:
				break;
		}
		if(++CS104SlaveDisk[pdrv].TelemetryNVAOut >= CS104SlaveDisk[pdrv].TelemetryNVARep+NVA_MAX_NUM)
		{
			CS104SlaveDisk[pdrv].TelemetryNVAOut = CS104SlaveDisk[pdrv].TelemetryNVARep;
		}

		if(++sendnum >= 10)	/*每包上送数量*/
		{
			break;
		}
		if(uptype != CS104SlaveDisk[pdrv].TelemetryNVAOut->uptype)/*与下一个上送类型不同*/
		{
			break;
		}
	}
	switch(uptype)
	{
		case CS104_SLAVE_M_ME_NA_1:
		case CS104_SLAVE_M_ME_NB_1:
			temp_array[pdrv][0] = 8 + sendnum*(sizeof(nva_temp.addr)+ sizeof(int16_t)+1+1);
			break;
		case CS104_SLAVE_M_ME_NC_1:
			temp_array[pdrv][0] = 8 + sendnum*(sizeof(struct NVA_Str)+1+1);
			break;
	}

	temp_array[pdrv][2] = uptype; /*类型*/
	temp_array[pdrv][3] = sendnum;
	temp_array[pdrv][4] = CS104_SLAVE_COT_SPONT;
	temp_array[pdrv][5] = 0;
	temp_array[pdrv][6] = LOBYTE(CS104_Slave_Pad[pdrv].PubAddress);
	temp_array[pdrv][7] = HIBYTE(CS104_Slave_Pad[pdrv].PubAddress);
	memcpy(pbuf,temp_array[pdrv],temp_array[pdrv][0]);
}

/**
  * @brief : judg fevent.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS104_SLAVE_H_FEvent(uint8_t pdrv)//判断是否有FEvent
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
void CS104_SLAVE_R_FEvent(uint8_t pdrv, uint8_t *pbuf)//读FEvent
{//LENTH/Lock_ID/TypeID/VSQ/COT_L/COT_H/PubAddr_L/PubAddr_H/Array
}

/**
  * @brief : 获取总召遥信 组包.
  * @param : [pdrv]
  * @param : [addr]
  * @param : [num]
  * @param : [pbuf]
  * @return: none
  * @updata: [2018-12-14][Chen][BRIEF]
  */
uint16_t CS104_SLAVE_ReadYxData(uint8_t pdrv, uint16_t addr, uint16_t num, uint8_t *pbuf)//读YXDATA
{
	uint8_t i = 0;
	uint8_t sendnum = 0;
	uint8_t value = 0;
	uint8_t negate = 0;
	uint8_t uptype = 0;
	uptype = 1;//单点遥信
//	log_i("CS104_SLAVE_ReadYxData读遥信数量:%d",num);
	for(i=0; i<num; i++)
	{
//		value  = *(allInfoDisk->var.telesignal.firstPoint +addr-TELESIGNAL_STARTADDR +i)->pVal; /*获取原值*/					TXL注释
//		negate = (allInfoDisk->var.telesignal.firstPoint +addr-TELESIGNAL_STARTADDR +i)->IsNegated;/*取反标志*/
//		uptype = (allInfoDisk->var.telesignal.firstPoint +addr-TELESIGNAL_STARTADDR +i)->IsDoublePoint;/*上送类型*/
//		log_w("uptype:%d,negate:%d,value:%d",uptype,negate,value);

		if(uptype == 0)
		{
			exit(1);///////////////////////////////////////////////////////////////////////////////////
		}
		if(negate == TRUE)/*取反*/
		{
			value = (~value)&0x03;
		}

		if(uptype == CS104_SLAVE_M_SP_NA_1)/*单点*/
		{
			value = (value - 1)&0x01;
		}
//		CS104SlaveDisk[pdrv].telesignalBackup[addr-TELESIGNAL_STARTADDR +i] = value&0x03;	/*总召备份YX*/					TXL注释
		value = 0xAA;			//遥信赋值
		temp_array[pdrv][sendnum + 11] = value;
		sendnum++;
//
//		if(sendnum +(addr-CS104_Slave_Pad[pdrv].YX_FirstAddr) >= allInfoDisk->var.telesignal.maxNum)	/*大于 需要总数*/		TXL注释
//		{
//			break;
//		}
//
//		if(uptype != (allInfoDisk->var.telesignal.firstPoint +i+1)->IsDoublePoint)
//		{
//			break;
//		}
	}

	temp_array[pdrv][0] = 11 + sendnum;	/*LENTH*/
	temp_array[pdrv][2] = uptype; /*类型*/					//上送类型 顶替TI位置
	temp_array[pdrv][3] = sendnum|0x80;						//VSQ
	temp_array[pdrv][4] = CS104_SLAVE_COT_INTROGEN;			//传输原因L
	temp_array[pdrv][5] = 0;								//传输原因H
	temp_array[pdrv][6] = LOBYTE(CS104_Slave_Pad[pdrv].PubAddress);	//公共地址
	temp_array[pdrv][7] = HIBYTE(CS104_Slave_Pad[pdrv].PubAddress);
	temp_array[pdrv][8] = LOBYTE(addr);						//信息体地址
	temp_array[pdrv][9] = HIBYTE(addr);
	temp_array[pdrv][10] = 0x00;
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
uint16_t CS104_SLAVE_ReadYcData(uint8_t pdrv, uint16_t addr, uint16_t num, uint8_t *pbuf)//读YCDATA
{
	/*allInfoDisk->var.telemetry  遥测*/

	uint8_t  i = 0;
	uint8_t  sendnum = 0;
	int16_t  tempu = 0;
	uint8_t  upType = 0; // 上送类型
	float 	 Rate = 0,value = 0;
//	log_i("CS104_SLAVE_ReadYcData读遥测数量:%d",num);
	upType = 9;	// 测量值，归一化值
	for(i=0; i<num; i++)
	{
//		upType = (allInfoDisk->var.telemetry.firstPoint +addr-TELEMETRY_STARTADDR +i)->DataType;/*获取类型*/				TXL注释
//		value = *((allInfoDisk->var.telemetry.firstPoint +addr-TELEMETRY_STARTADDR +i)->pVal);
//		Rate  = (allInfoDisk->var.telemetry.firstPoint +addr-TELEMETRY_STARTADDR +i)->Rate;
//		CS104SlaveDisk[pdrv].telemetryBackup[addr-TELEMETRY_STARTADDR +i] = value;/*备份YC*/
//		log_w("upType:%d,Rate:%f,value:%f",upType,Rate,value);
		if(upType == 0)
		{
			exit(1);///////////////////////////////////////////////////////////////////////////
		}
		switch(upType)
		{
			case CS104_SLAVE_M_ME_NA_1:
				tempu = (int16_t)(value*Rate);
				temp_array[pdrv][sendnum*(1+sizeof(uint16_t)) + sizeof(uint16_t) + 11] = 0x00;//QDS
				memcpy(&temp_array[pdrv][sendnum*(1+sizeof(uint16_t)) + 11],&tempu,sizeof(uint16_t));
			break;
			case CS104_SLAVE_M_ME_NB_1:
				tempu = (int16_t)(value*Rate);
				temp_array[pdrv][sendnum*(1+sizeof(uint16_t)) + sizeof(uint16_t) + 11] = 0x00;//QDS
				memcpy(&temp_array[pdrv][sendnum*(1+sizeof(uint16_t)) + 11],&tempu,sizeof(uint16_t));
				break;
			case CS104_SLAVE_M_ME_NC_1:/*短浮点*/
				temp_array[pdrv][sendnum*(1+sizeof(uint32_t)) + sizeof(uint32_t) + 11] = 0x00;//QDS
				memcpy(&temp_array[pdrv][sendnum*(1+sizeof(uint32_t)) + 11],&value,sizeof(uint32_t));
				break;
			default:
				break;
		}
		sendnum++;

//		if(sendnum +(addr-CS104_Slave_Pad[pdrv].YC_FirstAddr) >= allInfoDisk->var.telemetry.maxNum)	/*大于 需要总数*/			TXL注释
//		{
//			break;
//		}
//		if(upType != (allInfoDisk->var.telemetry.firstPoint+i+1)->DataType)	/*与下一个类型不同*/
//		{
//			break;
//		}
	}

	switch(upType)
	{
		case CS104_SLAVE_M_ME_NA_1:
		case CS104_SLAVE_M_ME_NB_1:
			temp_array[pdrv][0] = 11 + sendnum*(1+sizeof(uint16_t));
			break;
		case CS104_SLAVE_M_ME_NC_1:
			temp_array[pdrv][0] = 11 + sendnum*(1+sizeof(uint32_t));
			break;
	}

	temp_array[pdrv][2] = upType;
	temp_array[pdrv][3] = sendnum|0x80;
	temp_array[pdrv][4] = CS104_SLAVE_COT_INTROGEN;
	temp_array[pdrv][5] = 0;
	temp_array[pdrv][6] = LOBYTE(CS104_Slave_Pad[pdrv].PubAddress);
	temp_array[pdrv][7] = HIBYTE(CS104_Slave_Pad[pdrv].PubAddress);
	temp_array[pdrv][8] = LOBYTE(addr);
	temp_array[pdrv][9] = HIBYTE(addr);
	temp_array[pdrv][10] = 0x00;
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
void CS104_SLAVE_SoftwareUpdate(uint8_t pdrv, uint8_t *pbuf)//升级操作
{
	memcpy(temp_array[pdrv],pbuf,sizeof(temp_array[pdrv]));
	
	switch(pbuf[4])
	{
		case 6:
			temp_array[pdrv][4] = 7;
			if(pbuf[11]&0x80)
			{
//				if((Encrypt_ApplyType_Read(pdrv) != 0x01)&&(CS104_Slave_Pad[pdrv].Encrypt == 1))					TXL注释
//				{
//					Encrypt_ReturnFaultInfo_IN(0x1f,0x91,0x01,CS104_Slave_Pad[pdrv].DevId);
//					return;
//				}
				//启动升级
			}
			else
			{
				if(CS104_Slave_Pad[pdrv].Encrypt)
				{
					CS104_Slave_Pad[pdrv].updateFlag = 1;
				}
				//结束升级
				CS104_Slave_Pad[pdrv].resetFlag = 1;
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
	CS104_Slave_Reply(pdrv,temp_array[pdrv]);
}

/**
  * @brief : 文件操作.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS104_SLAVE_FileHandleProcess(uint8_t pdrv, uint8_t *pbuf)//文件操作
{//LENTH/Lock_ID/TypeID/VSQ/COT_L/COT_H/PubAddr_L/PubAddr_H/InfoAddr_L/InfoAddr_M/InfoAddr_H/sty/Array
	if((g_CommunicatFlag[COM_FILE] == CS104_Slave_Pad[pdrv].DevId)||(g_CommunicatFlag[COM_FILE] == COMMUNICATLOCKSTATUS))
	{
		switch(pbuf[12])
		{

			case 7:/*写文件激活*/
				if(pbuf[0] < 11)
				{return;}
				memcpy(&pbuf[10],&pbuf[11],pbuf[0]-11);
				pbuf[0] -= 1;
				memcpy(temp_array[pdrv],pbuf,pbuf[0]);
//				log_i("file_operate_WriteFileAct 7 ");
				file_operate_WriteFileAct(CS104_Slave_Pad[pdrv].DevId,temp_array[pdrv]);
//				log_i("file_operate_WriteFileAct 7 over");
				memcpy(pbuf,temp_array[pdrv],temp_array[pdrv][0]);
				if(temp_array[pdrv][0] < 10)
				{return;}
				memcpy(&pbuf[11],&temp_array[pdrv][10],temp_array[pdrv][0]-10);
				pbuf[10] = 0;
				pbuf[0] += 1;
				CS104_Slave_Reply(pdrv, pbuf);
        g_CommunicatFlag[COM_FILE] = CS104_Slave_Pad[pdrv].DevId;
				break;

			case 9:/*写文件数据传输*/
				if(pbuf[0] < 11)
				{return;}
				memcpy(&pbuf[10],&pbuf[11],pbuf[0]-11);
				pbuf[0] -= 1;
				memcpy(temp_array[pdrv],pbuf,pbuf[0]);
//				log_i("file_operate_WriteFileAct 9 ");
				file_operate_WriteFileAct(CS104_Slave_Pad[pdrv].DevId,temp_array[pdrv]);
//				log_i("file_operate_WriteFileAct 9 over");
				memcpy(pbuf,temp_array[pdrv],temp_array[pdrv][0]);
				if(temp_array[pdrv][0] < 10)
				{
//					log_i("unSendReceive");
          g_CommunicatFlag[COM_FILE] = CS104_Slave_Pad[pdrv].DevId;
					return;
				}
				memcpy(&pbuf[11],&temp_array[pdrv][10],temp_array[pdrv][0]-10);
				pbuf[10] = 0;
				pbuf[0] += 1;
				CS104_Slave_Reply(pdrv, pbuf);
				g_CommunicatFlag[COM_FILE] = COMMUNICATLOCKSTATUS;
				break;

			case 1:/*读目录*/
			case 3:/*读文件激活*/
			case 6:/*读文件数据传输确认*/
				memcpy(file_array[pdrv],pbuf,pbuf[0]);
        g_CommunicatFlag[COM_FILE] = CS104_Slave_Pad[pdrv].DevId;
				break;
			default:
				break;
		}
	}
    else
    {
        pbuf[4] = 47;
        CS104_Slave_Reply(pdrv, pbuf);
    }
}

/**
  * @brief : 空闲文件读操作响应
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS104_SLAVE_IdleProcess(uint8_t pdrv, uint8_t *pbuf)//发送空闲回调函数
{//LENTH/Lock_ID/TypeID/VSQ/COT_L/COT_H/PubAddr_L/PubAddr_H/Array(InfoAddr_L/InfoAddr_M/InfoAddr_H/Value/QDS)
    if(file_array[pdrv][0] != 0)
    {
        if(file_array[pdrv][0] < 11)
        {return;}
        memcpy(&file_array[pdrv][10],&file_array[pdrv][11],file_array[pdrv][0]-11);
        file_array[pdrv][0] -= 1;
        switch(file_array[pdrv][11])
        {
            case 1:/*读目录*/
//            	log_w("file_operate_DirCall");
                if(file_operate_DirCall(CS104_Slave_Pad[pdrv].DevId,file_array[pdrv]))
                {
                    g_CommunicatFileOper |= COMMUNICATLOCKREADDIR;
                    g_CommunicatFlag[COM_FILE] = CS104_Slave_Pad[pdrv].DevId;
                }
                else
                {
                    g_CommunicatFileOper &= ~COMMUNICATLOCKREADDIR;
                    g_CommunicatFlag[COM_FILE] = COMMUNICATLOCKSTATUS;
                }
//                log_i("file_operate_DirCall over");
                memcpy(pbuf,file_array[pdrv],file_array[pdrv][0]);
                break;

            case 3:/*读文件激活*/
            case 6:/*读文件数据传输确认*/
                if(file_operate_ReadFileAct(CS104_Slave_Pad[pdrv].DevId,file_array[pdrv]))
                {
                    g_CommunicatFileOper |= COMMUNICATLOCKREADFILE;
                    g_CommunicatFlag[COM_FILE] = CS104_Slave_Pad[pdrv].DevId;
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
        memcpy(temp_array[pdrv],pbuf,pbuf[0]);
        if(pbuf[0] < 10)
        {return;}
        memcpy(&pbuf[11],&temp_array[pdrv][10],temp_array[pdrv][0]-10);
        pbuf[10] = 0;
        pbuf[0] += 1;
        memset(file_array[pdrv],0,file_array[pdrv][0]);
    }
    else
    {
        if((g_CommunicatFileOper&COMMUNICATLOCKREADDIR)&&(g_CommunicatFlag[COM_FILE] == CS104_Slave_Pad[pdrv].DevId))
        {
            if(file_operate_DirRead(CS104_Slave_Pad[pdrv].DevId,pbuf))
            {
                g_CommunicatFileOper |= COMMUNICATLOCKREADDIR;
                g_CommunicatFlag[COM_FILE] = CS104_Slave_Pad[pdrv].DevId;
            }
            else
            {
                g_CommunicatFileOper &= ~COMMUNICATLOCKREADDIR;
                g_CommunicatFlag[COM_FILE] = COMMUNICATLOCKSTATUS;
            }
            memcpy(temp_array[pdrv],pbuf,pbuf[0]);
            if(pbuf[0] < 10)
            {return;}
            memcpy(&pbuf[11],&temp_array[pdrv][10],temp_array[pdrv][0]-10);
            pbuf[10] = 0;
            pbuf[0] += 1;
        }
        if((g_CommunicatFileOper&COMMUNICATLOCKREADFILE)&&(g_CommunicatFlag[COM_FILE] == CS104_Slave_Pad[pdrv].DevId))
        {
            if(file_operate_ReadFile(CS104_Slave_Pad[pdrv].DevId,pbuf))
            {
                g_CommunicatFileOper |= COMMUNICATLOCKREADFILE;
                g_CommunicatFlag[COM_FILE] = CS104_Slave_Pad[pdrv].DevId;
            }
            else
            {
                g_CommunicatFileOper &= ~COMMUNICATLOCKREADFILE;
                g_CommunicatFlag[COM_FILE] = COMMUNICATLOCKSTATUS;
            }
            memcpy(temp_array[pdrv],pbuf,pbuf[0]);
            if(pbuf[0] < 10)
            {return;}
            memcpy(&pbuf[11],&temp_array[pdrv][10],temp_array[pdrv][0]-10);
            pbuf[10] = 0;
            pbuf[0] += 1;
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
uint8_t CS104_SLAVE_H_Encrypt(uint8_t pdrv)//判断是否有加密数据发送
{
//	return(Encrypt_CheckSend(CS104_Slave_Pad[pdrv].Port));										TXL注释
	return 0;
}

/**
  * @brief : other reply.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
//uint8_t CS104_Slave_C_REPLY(uint8_t pdrv,tagControlCfg_t reInfo)//其他设备回复					TXL注释
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
//
//    log_w(" 接收到YK信息：addr = %x",((tagControlRemoteCfg_t)reInfo->pdata)->addr);
//    log_w(" 接收到YK信息：sta = %x",((tagControlRemoteCfg_t)reInfo->pdata)->sta);
//    log_w("reInfo->backDataLen :%d",reInfo->backDataLen);
//
//	memcpy(temp_array[pdrv],reInfo->pBackData,reInfo->backDataLen);
//	tmp.cot  = temp_array[pdrv][4];//传送原因
//	tmp.oper = temp_array[pdrv][11];//操作
//    log_i("tmp.cot:%d",tmp.cot);
//    log_i("tmp.oper:%d",tmp.oper);
//	if(ykCmd->sta == allInfoDisk->def.control_RES_FAIL)/*失败*/
//	{
//        log_w("遥控失败");
//		temp_array[pdrv][4] = CS104_SLAVE_COT_ACTCON|CS104_SLAVE_COT_NO;//选择|执行
//		if((tmp.cot == CS104_SLAVE_COT_DEACT) && ((tmp.oper&0x80) == 0))//撤销
//		{
//			temp_array[pdrv][4] = CS104_SLAVE_COT_DEACTCON|CS104_SLAVE_COT_NO;
//		}
//		CS104_Slave_Reply(pdrv, temp_array[pdrv]);
//	}
//	else if(ykCmd->sta == allInfoDisk->def.control_RES_SUCCEED)/*成功*/
//	{
//		if((tmp.cot == CS104_SLAVE_COT_DEACT) && ((tmp.oper&0x80) == 0))//撤销
//		{
//            log_w("遥控撤销成功");
//			temp_array[pdrv][4] = CS104_SLAVE_COT_DEACTCON;
//		}
//		else if((tmp.cot == CS104_SLAVE_COT_ACT) && (tmp.oper&0x80))//选择
//		{
//            log_w("遥控选择成功");
//			temp_array[pdrv][4] = CS104_SLAVE_COT_ACTCON;
//		}
//		else//执行
//		{
//            log_w("遥控执行成功");
//			temp_array[pdrv][4] = CS104_SLAVE_COT_ACTCON;
//			CS104_Slave_Reply(pdrv, temp_array[pdrv]);
//			temp_array[pdrv][4] = CS104_SLAVE_COT_ACCTTERM;/*遥控结束*/
//		}
//		CS104_Slave_Reply(pdrv, temp_array[pdrv]);
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
static uint8_t CS104_Slave_YxYcBackupInit(uint8_t pdrv)
{
	uint16_t i,num;
	uint8_t value = 0;
	uint8_t negate = 0;
	uint8_t uptype = 0;

	num = CS104_Slave_Pad[pdrv].YX_AllNum;
  for(i=0; i<num; i++)
	{
//		value  = *(allInfoDisk->var.telesignal.firstPoint +i)->pVal; /*获取原值*/					TXL注释
//		negate = (allInfoDisk->var.telesignal.firstPoint +i)->IsNegated;/*取反标志*/
//		uptype = (allInfoDisk->var.telesignal.firstPoint +i)->IsDoublePoint;/*上送类型*/

		if(negate == TRUE)/*取反*/
		{
			value = (~value)&0x03;
		}
		if(uptype == CS104_SLAVE_M_SP_NA_1)/*单点*/
		{
			value = (value - 1)&0x01;
		}

		CS104SlaveDisk[pdrv].telesignalBackup[i] = value&0x03;	/*总召备份YX*/
	}

	num = CS104_Slave_Pad[pdrv].YC_AllNum;
	for(i=0; i<num; i++)
	{
//		CS104SlaveDisk[pdrv].telemetryBackup[i] = *((allInfoDisk->var.telemetry.firstPoint +i)->pVal);/*备份YC*/				TXL注释
	}
	return TRUE;
}

/**
  * @brief : disk初始化.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-03-21][Chen][newly increased]
  */
static void CS104_Slave_DiskInit(uint8_t pdrv)
{
	if (CS104SlaveDisk == NULL)
	{
		CS104SlaveDisk = malloc(sizeof(struct CS104_SLAVE_DISK) * CS104_SLAVE_VOLUMES);
		memset (CS104SlaveDisk, 0, sizeof (struct CS104_SLAVE_DISK) * CS104_SLAVE_VOLUMES);
	}

//	CS104SlaveDisk[pdrv].SoeOut = *allInfoDisk->var.changeSoe.in;						TXL注释

	memset(CS104SlaveDisk[pdrv].TelesignalCosRep,0,sizeof(CS104SlaveDisk[pdrv].TelesignalCosRep));
	CS104SlaveDisk[pdrv].TelesignalCosIn = CS104SlaveDisk[pdrv].TelesignalCosRep;
	CS104SlaveDisk[pdrv].TelesignalCosOut = CS104SlaveDisk[pdrv].TelesignalCosRep;
	
	memset(CS104SlaveDisk[pdrv].TelemetryNVARep,0,sizeof(CS104SlaveDisk[pdrv].TelemetryNVARep));
	CS104SlaveDisk[pdrv].TelemetryNVAIn = CS104SlaveDisk[pdrv].TelemetryNVARep;
	CS104SlaveDisk[pdrv].TelemetryNVAOut = CS104SlaveDisk[pdrv].TelemetryNVARep;
}

/**
  * @brief : 104初始化默认值设置.
  * @param : [void]
  * @return: void
  * @updata: [2018-12-13][Chen][newly increased]
  */
static void CS104_Slave_ParamInit(uint8_t pdrv, int server_no, IecParam sParam)
{
	if(sParam->allInfoDisk == NULL)
	{
		allInfoDisk = malloc(sizeof(struct tagOverallCfg));
		memset (allInfoDisk, 0, sizeof (struct tagOverallCfg));
	}
	else
	{
		allInfoDisk = sParam->allInfoDisk;
	}

	 if (allInfoDisk == NULL)
	 {
	 	allInfoDisk = malloc(sizeof(struct tagOverallCfg));
	 	memset (allInfoDisk, 0, sizeof (struct tagOverallCfg));
//	 	allInfoDisk = &g_overall;
	 }

	 if (writeDZbuf == NULL)
	 {
//	 	writeDZbuf = malloc(sizeof(struct tagValueDZ));
	 }
//	log_w("allInfoDisk->var.telesignal.maxNum:%02x", allInfoDisk->var.telesignal.maxNum);

	if(pdrv == 0)
	{CS104_Slave_Pad[pdrv].DevId = SLAVE104_ID0;}
	if(pdrv == 1)
	{CS104_Slave_Pad[pdrv].DevId = SLAVE104_ID1;}

	CS104_Slave_Pad[pdrv].Port = server_no; // 端口号
	CS104_Slave_Pad[pdrv].ModuleSN = sParam->ModuleSN;  /*模块ID*/
	CS104_Slave_Pad[pdrv].Encrypt = sParam->encrypt;

	CS104_Slave_Pad[pdrv].LinkAddress = sParam->sourceAddr; // 链路地址
	CS104_Slave_Pad[pdrv].PubAddress  = sParam->ASDUAddr; // 公共地址
	CS104_Slave_Pad[pdrv].CotSize = sParam->ASDUCotSize;        // 传输原因字节数
	CS104_Slave_Pad[pdrv].InfoAddrSize = sParam->InfoAddrSize;   // 信息体地址字节数
	CS104_Slave_Pad[pdrv].LinkAddrSize = sParam->linkAddrSize;   // 链路层地址字节数
	CS104_Slave_Pad[pdrv].PubAddrSize = sParam->ASDUAddrSize;    // 公共地址字节数

	CS104_Slave_Pad[pdrv].ClockTimers = 1;    // 非0
//	CS104_Slave_Pad[pdrv].YX_AllNum = allInfoDisk->var.telesignal.maxNum;
//	CS104_Slave_Pad[pdrv].YX_FirstAddr = TELESIGNAL_STARTADDR;
	CS104_Slave_Pad[pdrv].YX_AllNum = 200;					//  遥信总数 测试随意赋值
	CS104_Slave_Pad[pdrv].YX_FirstAddr = 0x0001;			//	遥信起始地址
	CS104_Slave_Pad[pdrv].YX_FrameNum = 100;				//	遥信传输每组最多数据

//	CS104_Slave_Pad[pdrv].YC_AllNum = allInfoDisk->var.telemetry.maxNum;
//	CS104_Slave_Pad[pdrv].YC_FirstAddr = TELEMETRY_STARTADDR;
	CS104_Slave_Pad[pdrv].YC_AllNum = 30;					//遥测总数
	CS104_Slave_Pad[pdrv].YC_FirstAddr = 0x4001;			//遥测起始地址
	CS104_Slave_Pad[pdrv].YC_FrameNum = 24;					//遥测传输每组最多数据

	CS104_Slave_Pad[pdrv].TickValue[0] = 30000;  // 30s 连接建立的超时
	CS104_Slave_Pad[pdrv].TickValue[1] = 15000;  // 15s 发送或测试APDU的超时
	CS104_Slave_Pad[pdrv].TickValue[2] = 10000;  // 10s 无数据报文t2 t1状态下发送测试帧的超时
	CS104_Slave_Pad[pdrv].TickValue[3] = 20000;  // 20s 若在t3内未接收到任何报文，将向对方发送测试链路帧。
	
	CS104_Slave_Pad[pdrv].updateFlag = 0;
	CS104_Slave_Pad[pdrv].resetFlag = 0;
	CS104_Slave_Pad[pdrv].TimeOutReset = 5000;	/*复位进程超时时间 MS*/

    if (CS104_Slave_Pad[pdrv].Encrypt)
    {
//        if(!Encrypt_AddDevice(CS104_Slave_Pad[pdrv].DevId,CS104_Slave_Pad[pdrv].Port,CS104_Slave_Read_drv,CS104_Slave_Write_drv))			TXL注释
//        {
//        	log_e("Encrypt_AddDevice failed");
//        	CS104_Slave_Pad[pdrv].Encrypt = 0;
//        }
    }

//	log_i("CS104_Slave_Pad[pdrv].Port:%d",CS104_Slave_Pad[pdrv].Port);													TXL注释
//	log_i("CS104_Slave_Pad[pdrv].ModuleSN:%d",CS104_Slave_Pad[pdrv].ModuleSN);
//	log_i("CS104_Slave_Pad[pdrv].Encrypt:%d",CS104_Slave_Pad[pdrv].Encrypt);
//	log_i("CS104_Slave_Pad[pdrv].LinkAddress:%d",CS104_Slave_Pad[pdrv].LinkAddress);
//	log_i("CS104_Slave_Pad[pdrv].PubAddress:%d",CS104_Slave_Pad[pdrv].PubAddress);
//	log_i("CS104_Slave_Pad[pdrv].CotSize:%d",CS104_Slave_Pad[pdrv].CotSize);
//	log_i("CS104_Slave_Pad[pdrv].InfoAddrSize:%d",CS104_Slave_Pad[pdrv].InfoAddrSize);
//	log_i("CS104_Slave_Pad[pdrv].LinkAddrSize:%d",CS104_Slave_Pad[pdrv].LinkAddrSize);
//	log_i("CS104_Slave_Pad[pdrv].PubAddrSize:%d",CS104_Slave_Pad[pdrv].PubAddrSize);
//
//	log_i("CS104_Slave_Pad[pdrv].YX_AllNum:%x",CS104_Slave_Pad[pdrv].YX_AllNum);
//	log_i("CS104_Slave_Pad[pdrv].YC_AllNum:%x",CS104_Slave_Pad[pdrv].YC_AllNum);
}

/**
  * @brief : 104初始化.
  * @param : [void]
  * @return: void
  * @updata: [2018-12-13][Chen][newly increased]
  */
int CS104_Slave_create(uint8_t pdrv, int server_no, IecParam sParam)
{
	CS104_Slave_ParamInit(pdrv,server_no,sParam);/*参数默认初始化值*/
	CS104_Slave_DiskInit(pdrv);/*disk初始化*/
	CS104_Slave_YxYcBackupInit(pdrv);/*初始化遥信遥测备份数组*/
	CS104_Slave_Appinit(pdrv);/*app参数初始化*/
	return 0;
}

/**
  * @brief : 改变转发表初始化.
  * @param : [void]
  * @return: void
  * @updata: [2019-04-19][Chen][newly increased]
  */
uint8_t CS104_Slave_changePointTable(uint8_t num)
{
	uint8_t pdrv = 0;
	for(pdrv = 0; pdrv < num; pdrv++)
	{
//		CS104_Slave_Pad[pdrv].YX_AllNum = allInfoDisk->var.telesignal.maxNum;									TXL注释
//		CS104_Slave_Pad[pdrv].YC_AllNum = allInfoDisk->var.telemetry.maxNum;
		CS104SlaveDisk[pdrv].TelesignalCosOut = CS104SlaveDisk[pdrv].TelesignalCosIn;
		CS104SlaveDisk[pdrv].TelemetryNVAOut = CS104SlaveDisk[pdrv].TelemetryNVAIn;
		CS104_Slave_YxYcBackupInit(pdrv);/*初始化遥信遥测备份数组*/
	}
	return TRUE;
}

/**
  * @brief : 104线程暂停.
  * @param : [void]
  * @return: true
  * @updata: [2019-04-24][Chen][new]
  */
uint8_t CS104_Slave_pause(void)
{
	if (thread_Stop == FALSE)
	{
		pthread_mutex_lock(&mut);
		thread_Stop = TRUE;
//		log_w("slave104 thread pause!");
		pthread_mutex_unlock(&mut);
	}
	else
	{  
//		log_e("slave104 pthread pause already!");
	}
	return TRUE;
}

/**
  * @brief : 104线程恢复.
  * @param : [num]
  * @return: true
  * @updata: [2019-04-24][Chen][new]
  */
uint8_t CS104_Slave_resume(uint8_t num)
{
	if (thread_Stop == TRUE)
	{
		CS104_Slave_changePointTable(num);
		pthread_mutex_lock(&mut);
		thread_Stop = FALSE;
		pthread_cond_signal(&cond);
//		log_w("slave104 run pause!");
		pthread_mutex_unlock(&mut);
	}
	else
	{
//		log_e("slave104 pthread run already!");
	}
	return TRUE;
}

/**
  * @brief : 104运行启动 接口函数.
  * @param : [void]
  * @return: void
  * @updata: [2018-12-13][Chen][newly increased]
  */
void *CS104_Slave_Startup(void *arg)
{
  static uint16_t i = 0;
	uint8_t disk,num;

	disk = 0;
	num = *(uint8_t*)arg;

//	log_i("CS104 num:%d",num);

	for(;;)
	{
		pthread_mutex_lock(&mut);
		while (thread_Stop == TRUE)
		{
			pthread_cond_wait(&cond, &mut);
		}
		pthread_mutex_unlock(&mut);

		/*运行程序*/
		CS104_Slave_Clock(disk);

		if(++disk >= num)
		{
			disk = 0;
		}

		if (++i >= 200)
		{
			i = 0;
//			log_e("CS104_Slave_run");
		}
		usleep(20000);
	}
	return 0;
}
/* END OF FILE ---------------------------------------------------------------*/

