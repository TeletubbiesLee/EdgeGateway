/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      cs104MasterDisk.c
  * @brief:		104主站接口
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2019-03-18
  * @update:    [2019-03-18][Chen][newly increased]
  */
 #define LOG_TAG    "CS104_Master_Disk"
/* INCLUDE FILES -------------------------------------------------------------*/
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
//#include "cascadeModule.h"
#include "../port/iec_interface.h"
#include "cs104MasterApp.h"
#include "cs104MasterDisk.h"
//#include "encrypt_disk.h"
#include "../common/common.h"
#include "../tcp_socket/client.h"
#include "../file/fileOperate.h"
//#include "elog.h"
/* PRIVATE VARIABLES ----------------------------------------------------------*/
static uint8_t temp_array[CS104_MASTER_VOLUMES][256];
static struct tagOverallCfg *allInfoDisk = NULL;
static struct tagControlCfg sYKInfo;/*遥控数据*/
static struct tagControlRemoteCfg sYKData;/*遥控内容*/
static uint8_t disk[CS104_MASTER_VOLUMES];
/* PUBLIC VARIABLE -----------------------------------------------------------*/
/* PRIVATE FUNCTION PROTOTYPES -----------------------------------------------*/
/**
  * @brief : read data from drv.
  * @param : [pdrv]
  * @param : [pbuf]
  * @param : [count]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint16_t CS104_Master_Read_drv(uint8_t port, uint8_t *pbuf, uint16_t count)
{
	uint16_t len=0;
	uint16_t i;

	len = ethernet_client_get(port, pbuf, count);

   if(len)
   {
//		log_w("(%d)read %d data",port,len);
//	   	printf("read:  ");
//		for(i=0;i<len;i++)
//		{
//			printf("%02x ",pbuf[i]);
//		}
//		printf("\n");
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
uint16_t CS104_Master_Write_drv(uint8_t port, uint8_t *pbuf, uint16_t count)
{
    uint16_t i; 
    
    ethernet_client_put(port, pbuf, count);

    if(count)
    {
//    	log_w("(%d)write %d data",port,count);
//    	printf("write: ");
//    	for(i=0;i<count;i++)
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
uint16_t CS104_Master_ReadData(uint8_t pdrv, uint8_t *pbuf, uint16_t count)
{ 
  uint16_t len = 0; 
        
	len = CS104_Master_Read_drv(CS104_Master_Pad[pdrv].Port, pbuf, count);

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
uint16_t CS104_Master_WriteData(uint8_t pdrv, uint8_t *pbuf, uint16_t count)
{ 
	uint16_t len = 0;  

  len = CS104_Master_Write_drv(CS104_Master_Pad[pdrv].Port,pbuf,count);

	return(len);
}

/**
  * @brief : Check Link.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS104_Master_CheckLink(uint8_t pdrv)
{ 
    uint8_t res = 0;  

		res = getClientLinkState(CS104_Master_Pad[pdrv].Port);

    return(res);
}

/**
  * @brief : Stop Link.
  * @param : [pdrv]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS104_Master_StopLink(uint8_t pdrv)
{
		setClientCloseLink(CS104_Master_Pad[pdrv].Port);

    return(TRUE);
}

/**
  * @brief : 检查遥控信息.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [2019-06-20][Chen][new]
  */
static uint8_t CS104_Master_CheckCtlRemoteCfg(uint8_t pdrv, uint8_t *pbuf,tagCtlRomteCfg_t tmp)
{
	tmp->addr = pbuf[8]|(pbuf[9]<<8);//地址
	tmp->cot  = pbuf[4]|(pbuf[5]<<8);//传送原因
	tmp->oper = pbuf[11];//操作

  if(pbuf[2] == CS104_MASTER_C_SC_NA_1)	/*单点*/
	{
		tmp->value = (pbuf[11]&0x01)+1;
	}
	else
	{
		tmp->value = pbuf[11]&0x03;
	}
	return TRUE;
}

/**
  * @brief : 接收到遥控信息.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [2019-06-20][Chen][new]
  */
uint8_t CS104_Master_ReadRemoteCtlProcess(uint8_t pdrv, uint8_t *pbuf)
{
	struct tagCtlRomteCfg tmp;//临时结构体
	struct tagControlRemoteCfg ykCmd;

	sYKInfo.pdata = &ykCmd;

	if(CS104_Master_CheckCtlRemoteCfg(pdrv,pbuf,&tmp) == FALSE)/*数据检查*/
	{
		return FALSE;
	}
	/*参数赋值*/
	sYKInfo.com.dir   = allInfoDisk->def.message_UP;
	sYKInfo.com.reply = CS104_Master_Param[pdrv].module_id;//模块ID
	sYKInfo.com.tpye  = allInfoDisk->def.type_REMOTE;

//	ykCmd.addr = (CS104_Master_Param[pdrv].p_YK_data->addr + tmp.addr - IEC60870_YK_STARTADDR);		TXL注释
	ykCmd.work = tmp.value;
    
	/*逻辑判断*/
  if(tmp.oper&0x80)/*预制*/
  {
    if(tmp.cot == CS104_MASTER_COT_ACTCON)
    {
//      log_i("YK select true");
      ykCmd.cmd = allInfoDisk->def.control_OPREAT_CHOICE;
      ykCmd.sta  = allInfoDisk->def.control_RES_SUCCEED;
    }
    else
    {
//      log_i("YK select false");
      ykCmd.cmd = allInfoDisk->def.control_OPREAT_CHOICE;
      ykCmd.sta  = allInfoDisk->def.control_RES_FAIL;
    }
  }
  else
  {
    if(tmp.cot == CS104_MASTER_COT_ACTCON)
    {
//      log_i("YK operate true");
      ykCmd.cmd = allInfoDisk->def.control_OPREAT_EXECUTE;
      ykCmd.sta  = allInfoDisk->def.control_RES_SUCCEED;
    }
    else if(tmp.cot == CS104_MASTER_COT_DEACTCON)
    {
//      log_i("YK cancel true");
      ykCmd.cmd = allInfoDisk->def.control_OPREAT_CANCEL;
      ykCmd.sta  = allInfoDisk->def.control_RES_SUCCEED;
    }
    else if(tmp.cot == (CS104_MASTER_COT_ACTCON|CS104_MASTER_COT_NO))
    {
//      log_i("YK operate false");
      ykCmd.cmd = allInfoDisk->def.control_OPREAT_EXECUTE;
      ykCmd.sta  = allInfoDisk->def.control_RES_FAIL;
    }
    else if(tmp.cot == (CS104_MASTER_COT_DEACTCON|CS104_MASTER_COT_NO))
    {
//      log_i("YK cancel false");
      ykCmd.cmd = allInfoDisk->def.control_OPREAT_CANCEL;
      ykCmd.sta  = allInfoDisk->def.control_RES_FAIL;
    }
    else
    {
        return FALSE;
    }
    
  }
//  log_i("ykCmd.addr:%x work:%d cmd:%d sta:%d",ykCmd.addr, ykCmd.work, ykCmd.cmd, ykCmd.sta);
  allInfoDisk->fun.operate_Control_info(&sYKInfo);/*发送遥控命令 */
  return TRUE;
}

/**
  * @brief : 遥控下发处理信息.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][Chen][]
  */
uint8_t CS104_Master_getRemoteCtlInfo(uint8_t pdrv, uint8_t *pbuf)
{//LENTH/Lock_ID/TypeID/VSQ/COT_L/COT_H/PubAddr_L/PubAddr_H/Array
  uint16_t addr;
  uint8_t value;
  tagControlRemoteCfg_t ykCmd;
//  log_w("CS104_Master_getRemoteCtlInfo(%d)",pdrv);
  ykCmd = sYKInfo.pdata;
  memcpy(temp_array[pdrv],pbuf,pbuf[0]);
//   log_e("((tagControlRemoteCfg_t)sYKInfo.pdata)->addr = %x",((tagControlRemoteCfg_t)sYKInfo.pdata)->addr);
//  log_w("ykCmd->addr:%x cmd:%x sta:%x work:%x",ykCmd->addr,ykCmd->cmd,ykCmd->sta,ykCmd->work);
//  addr = (ykCmd->addr - CS104_Master_Param[pdrv].p_YK_data->addr + IEC60870_YK_STARTADDR);	TXL注释
  temp_array[pdrv][8] = addr & 0xff;
  temp_array[pdrv][9] = addr>>8 & 0xff;
  temp_array[pdrv][10] = 0;

  if(CS104_Master_Param[pdrv].yk_type == allInfoDisk->def.Ti_M_SP_NA_1)//单点
  {
    temp_array[pdrv][2] = CS104_MASTER_C_SC_NA_1;
    value = ykCmd->work - 1;
  }
  else
  {
    temp_array[pdrv][2] = CS104_MASTER_C_SC_NB_1;
    value = ykCmd->work;
  }

  if(ykCmd->cmd == allInfoDisk->def.control_OPREAT_CHOICE)/*预制*/
  {
    temp_array[pdrv][4] = CS104_MASTER_COT_ACT;
    temp_array[pdrv][5] = 0;
    temp_array[pdrv][11] = 0x80|value;
  }
  else if(ykCmd->cmd == allInfoDisk->def.control_OPREAT_EXECUTE)/*执行*/
  {
    temp_array[pdrv][4] = CS104_MASTER_COT_ACT;
    temp_array[pdrv][5] = 0;
    temp_array[pdrv][11] = 0x00|value;
  }
  else if(ykCmd->cmd == allInfoDisk->def.control_OPREAT_CANCEL)/*撤销*/
  {
    temp_array[pdrv][4] = CS104_MASTER_COT_DEACT;
    temp_array[pdrv][5] = 0;
    temp_array[pdrv][11] = 0x00|value;
  }
  memcpy(pbuf,temp_array[pdrv],temp_array[pdrv][0]);
  return TRUE;
}

/**
  * @brief : 其他设备下发的对时.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [2019-06-24][Chen][new]
  */
uint8_t CS104_Master_SYN_Other(tagControlCfg_t infoSyn)
{
  uint8_t pdrv;
  for(pdrv = 0; pdrv < CS104_MASTER_VOLUMES; pdrv++)
  {
    if(CS104_Master_Pad[pdrv].ModuleSN == infoSyn->com.reply)
    {
      CS104_Master_otherSYNSendData(pdrv);
      return TRUE;
    }
  }
  return FALSE;
}

/**
  * @brief : 对时 获取时间.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t CS104_Master_SYNProcess(uint8_t pdrv, ds_privateTime_t time)
{
  allInfoDisk->fun.get_currentTime_info(time);
  return TRUE;
}

/**
  * @brief : get alarm of ms.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: time_ms
  * @updata: [2019-04-22][chen][new]
  */
uint64_t CS104_Master_getAlarmOfMs(uint8_t pdrv)
{
	struct ds_privateTime getTime;
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
void CS104_Master_ResetProcess(uint8_t pdrv, uint8_t *pbuf)
{
	g_CommunicatFlag[COM_FILE] |= COMMUNICATLOCKRESET;
}

/**
  * @brief : 总召命令判断.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: true or false
  * @updata: [2019-06-19][Chen][new]
  */
uint16_t CS104_Master_ReadAllDataProcess(uint8_t pdrv, uint8_t *pbuf)
{
  uint8_t cotL = pbuf[4];/*传送原因*/
  
  if(cotL == CS104_MASTER_COT_ACCTTERM)//激活终止
  {
    return TRUE;
  }
	return FALSE;
}

/**
  * @brief : 处理接收到YX数据.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [2019-06-19][Chen][new]
  */
uint16_t CS104_Master_ReadYxProcess(uint8_t pdrv, uint8_t *pbuf)
{
  //总召 或者 cos 单点双点
  //LENTH/Lock_ID/TypeID/VSQ/COT_L/COT_H/PubAddr_L/PubAddr_H/Array
  uint16_t i,addr,offsetAddr;
  uint8_t value;
  uint8_t typeID = pbuf[2];/*应用类型*/
  uint8_t num = pbuf[3]&0x7f;/*数量*/
  uint8_t cotL = pbuf[4];/*传送原因*/
  if(cotL == CS104_MASTER_COT_INTROGEN)/*总召*/
  {
    addr = (pbuf[8] | pbuf[9]<<8)&0xffff;
//    log_i("(%d)yx num:%d YX_len:%d",pdrv,num,CS104_Master_Param[pdrv].YX_len);
    for(i=0; i<num; i++)
    {
      offsetAddr = (addr -IEC60870_YX_STARTADDR +i);
      value = pbuf[11 +i];
      if(typeID == CS104_MASTER_M_SP_NA_1)//单点
      {
        value += 1;
      }
      if(offsetAddr >= CS104_Master_Param[pdrv].YX_len)
      {
//        log_w("(%d)moduleCascadeWriteInput yx over",pdrv);
        break;
      }
//      log_w("(%d)moduleCascadeWriteInput addr:%x,value:%x",pdrv,(CS104_Master_Param[pdrv].p_YX_data+offsetAddr)->addr,value);
//      moduleCascadeWriteInput((CS104_Master_Param[pdrv].p_YX_data+offsetAddr)->addr,value);/*写YX*/		TXL注释
    }
  }
	return TRUE;
}

/**
  * @brief : 处理接收到Soe数据.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [2019-06-19][Chen][new]
  */
uint16_t CS104_Master_ReadSoeProcess(uint8_t pdrv, uint8_t *pbuf)
{
  //LENTH/Lock_ID/TypeID/VSQ/COT_L/COT_H/PubAddr_L/PubAddr_H/Array
  uint16_t i,offsetAddr;
  struct tagSoeCfg tmp_Soe;
  uint8_t typeID = pbuf[2];/*应用类型*/
  uint8_t num = pbuf[3]&0x7f;/*数量*/
  uint8_t cycleLen = 11;//循环数据长度

  for(i=0; i<num; i++)
  {
    offsetAddr = (pbuf[8+cycleLen*i] | pbuf[9+cycleLen*i]<<8)&0xffff;
//    tmp_Soe.addr = (CS104_Master_Param[pdrv].p_YX_data + offsetAddr - IEC60870_YX_STARTADDR)->addr;	TXL注释
    tmp_Soe.value = pbuf[11+cycleLen*i];
    memcpy(&tmp_Soe.time,&pbuf[12+cycleLen*i],sizeof(struct CP56Time2a_t));

    if(typeID == CS104_MASTER_M_SP_TB_1)//单点
    {
      tmp_Soe.value += 1;
    }
    if(offsetAddr >= CS104_Master_Param[pdrv].YX_len)
    {
//      log_w("(%d)moduleCascadeWriteSoe over",pdrv);
      break;
    }
//    log_w("(%d)moduleCascadeWriteSoe,%x,%x",pdrv,(CS104_Master_Param[pdrv].p_YX_data+offsetAddr)->addr,tmp_Soe.addr);
//    moduleCascadeWriteSoe(&tmp_Soe);/*写SOE*/					TXL注释
  }

	return TRUE;
}

/**
  * @brief : 处理接收到YC数据.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [2019-04-17][Chen][new]
  */
uint16_t CS104_Master_ReadYcProcess(uint8_t pdrv, uint8_t *pbuf)
{
  //总召 或者 NVA 09 11 13
  //LENTH/Lock_ID/TypeID/VSQ/COT_L/COT_H/PubAddr_L/PubAddr_H/Array
  uint16_t i,offsetAddr,addr = 0;
  float value;
  uint8_t typeID = pbuf[2];/*应用类型*/
  uint8_t vsq = pbuf[3];/*vsq*/
  uint8_t num = pbuf[3]&0x7f;/*数量*/
//  log_i("(%d)yc num:%d YC_len:%d",pdrv,num,CS104_Master_Param[pdrv].YC_len);
  if(vsq&0x80)/*连续*/
  {
    addr = (pbuf[8] | pbuf[9]<<8)&0xffff;

    for(i=0; i<num; i++)
    {
      switch(typeID)
      {
        case CS104_MASTER_M_ME_NA_1://归一化值
          value = (float)(pbuf[11+3*i] | pbuf[12+3*i]<<8);
          break;
        case CS104_MASTER_M_ME_NB_1://标度化值
          value = (float)(pbuf[11+3*i] | pbuf[12+3*i]<<8);
          break;
        case CS104_MASTER_M_ME_NC_1://短浮点数
          memcpy(&value,&pbuf[11+5*i],sizeof(uint32_t));
          break;

        default:
          break;
      }
      offsetAddr = (addr -IEC60870_YC_STARTADDR +i);
//      log_i("addr:%x,offsetAddr:%x",addr,offsetAddr);
      if(offsetAddr >= CS104_Master_Param[pdrv].YC_len)
      {
//        log_w("write yc over");
        break;
      }
//      log_w("yc addr:%x,value:%f",(CS104_Master_Param[pdrv].p_YC_data+offsetAddr)->addr,value);
      *(CS104_Master_Param[pdrv].p_YC_data +offsetAddr)->pValue = value;/*写YC*/
    }
  }
  else
  {
    for(i=0; i<num; i++)
    {
      switch(typeID)
      {
        case CS104_MASTER_M_ME_NA_1://归一化值
          addr = (pbuf[8+6*i] | pbuf[9+6*i]<<8)&0xffff;
          value = (float)(pbuf[11+6*i] | pbuf[12+6*i]<<8);
          break;
        case CS104_MASTER_M_ME_NB_1://标度化值
          addr = (pbuf[8+6*i] | pbuf[9+6*i]<<8)&0xffff;
          value = (float)(pbuf[11+6*i] | pbuf[12+6*i]<<8);
          break;
        case CS104_MASTER_M_ME_NC_1://短浮点数
          addr = (pbuf[8+8*i] | pbuf[9+8*i]<<8)&0xffff;
          memcpy(&value,&pbuf[11+8*i],sizeof(uint32_t));
          break;

        default:
          break;
      }
      offsetAddr = (addr -IEC60870_YC_STARTADDR +i);
//      log_i("addr:%x,offsetAddr:%x",addr,offsetAddr);
      if((i >= CS104_Master_Param[pdrv].YC_len) || (offsetAddr >= CS104_Master_Param[pdrv].YC_len))
      {
//        log_w("write yc over");
        break;
      }
//      log_w("yc addr:%x,value:%f",(CS104_Master_Param[pdrv].p_YC_data+offsetAddr)->addr,value);
      *(CS104_Master_Param[pdrv].p_YC_data +offsetAddr)->pValue = value;/*写YC*/
    }
  }
  return TRUE;
}

/**
  * @brief : updata command.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS104_Master_SoftwareUpdate(uint8_t pdrv, uint8_t *pbuf)//升级操作
{

}

/**
  * @brief : 文件操作.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS104_Master_FileHandleProcess(uint8_t pdrv, uint8_t *pbuf)//文件操作
{//LENTH/Lock_ID/TypeID/VSQ/COT_L/COT_H/PubAddr_L/PubAddr_H/InfoAddr_L/InfoAddr_M/InfoAddr_H/sty/Array

}

/**
  * @brief : 空闲文件读操作响应
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CS104_Master_IdleProcess(uint8_t pdrv, uint8_t *pbuf)//发送空闲回调函数
{//LENTH/Lock_ID/TypeID/VSQ/COT_L/COT_H/PubAddr_L/PubAddr_H/Array(InfoAddr_L/InfoAddr_M/InfoAddr_H/Value/QDS)

}

/**
  * @brief : 写通讯状态.
  * @param : [pdrv]
  * @return: none
  * @updata: [2019-07-03][Chen][new]
  */
uint8_t CS104_Master_writeCommunicationSoe(uint8_t pdrv, uint8_t Connect)
{
    struct tagSoeCfg tmp_Soe;
    struct ds_privateTime time;
    Connect++;//双点
//    if(Connect == *CS104_Master_Param[pdrv].communication_state.pValue)		TXL注释
//    {
//        return FALSE;
//    }

    CS104_Master_SYNProcess(pdrv,&time);

//    tmp_Soe.addr  = CS104_Master_Param[pdrv].communication_state.addr;		TXL注释
    tmp_Soe.value = Connect &0x03;
    memcpy(&tmp_Soe.time,&time.time_cp56,sizeof(struct CP56Time2a_t));
//    log_w("tmp_Soe.value:%d tmp_Soe.addr:%x",tmp_Soe.value,tmp_Soe.addr);
//    moduleCascadeWriteSoe(&tmp_Soe);/*写通讯状态SOE*/							TXL注释

	return TRUE;
}

/**
  * @brief : 其他设备回复.
  * @param : [pdrv]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
int CS104_Master_C_REPLY(tagControlCfg_t info)
{
  uint8_t pdrv;
//  log_i("CS104_Master_C_REPLY ModuleId:%d type:%d",info->com.reply,info->com.tpye);
  for(pdrv = 0; pdrv < CS104_MASTER_VOLUMES; pdrv++)
  {
    if(CS104_Master_Pad[pdrv].ModuleSN == info->com.reply)/*找到设备*/
    {
      if(info->com.tpye == allInfoDisk->def.type_SYNC)/*对时*/
      {
//        log_i("CS104_Master_otherSYNSendData(%d)",pdrv);
        if(CS104_Master_otherSYNSendData(pdrv) == FALSE)
        {
          return FALSE;
        }
      }
      if(info->com.tpye == allInfoDisk->def.type_REMOTE)/*遥控*/
      {
//        log_i("CS104_Master_otherYkSendData(%d)",pdrv);
//        log_e("((tagControlRemoteCfg_t)info->pdata)->addr = %x",((tagControlRemoteCfg_t)info->pdata)->addr);

        memcpy(&sYKData,info->pdata,sizeof(struct tagControlRemoteCfg));                   
        memcpy(&sYKInfo,info,sizeof(struct tagControlCfg));
        sYKInfo.pdata = &sYKData;
//        log_e("((tagControlRemoteCfg_t)sYKInfo.pdata)->addr = %x",((tagControlRemoteCfg_t)sYKInfo.pdata)->addr);
        if(CS104_Master_otherYkSendData(pdrv) == FALSE)
        {
//          log_e("control_RES_FAIL(%d)",pdrv);
          return allInfoDisk->def.control_RES_FAIL;
        }
//        log_i("control_RES_WAIT(%d)",pdrv);
        return allInfoDisk->def.control_RES_WAIT;
      }
      return FALSE;
    }
  }
  return FALSE;
}

/**
  * @brief : 104初始化默认值设置.
  * @param : [void]
  * @return: void
  * @updata: [2018-12-13][Chen][newly increased]
  */
static void CS104_Master_ParamInit(uint8_t pdrv, int client_no, IecParam sParam)
{
	if(sParam->allInfoDisk == NULL)
	{
//    log_e("(%d)sParam->allInfoDisk == NULL",pdrv);
		allInfoDisk = malloc(sizeof(struct tagOverallCfg));
		memset (allInfoDisk, 0, sizeof (struct tagOverallCfg));
	}
	else
	{
		allInfoDisk = sParam->allInfoDisk;
	}

	CS104_Master_Pad[pdrv].Port = client_no; // 端口号
  CS104_Master_Pad[pdrv].ModuleSN = sParam->ModuleSN;  /*模块ID*/
	CS104_Master_Pad[pdrv].LinkAddress = sParam->sourceAddr; // 链路地址
	CS104_Master_Pad[pdrv].PubAddress  = sParam->ASDUAddr; // 公共地址
	CS104_Master_Pad[pdrv].CotSize = sParam->ASDUCotSize;        // 传输原因字节数
	CS104_Master_Pad[pdrv].InfoAddrSize = sParam->InfoAddrSize;   // 信息体地址字节数
	CS104_Master_Pad[pdrv].LinkAddrSize = sParam->linkAddrSize;   // 链路层地址字节数
	CS104_Master_Pad[pdrv].PubAddrSize = sParam->ASDUAddrSize;    // 公共地址字节数

	CS104_Master_Pad[pdrv].TickValue[0] = 30000;  // 30s 连接建立的超时
	CS104_Master_Pad[pdrv].TickValue[1] = 15000;  // 15s 发送或测试APDU的超时
	CS104_Master_Pad[pdrv].TickValue[2] = 10000;  // 10s 无数据报文t2 t1状态下发送测试帧的超时
	CS104_Master_Pad[pdrv].TickValue[3] = 20000;  // 20s 若在t3内未接收到任何报文，将向对方发送测试链路帧。

  CS104_Master_Pad[pdrv].sendIFlag[CLOCKSYN]  = FALSE;/*发送时钟同步命令*/
  CS104_Master_Pad[pdrv].sendIFlag[ALLDATA]   = TRUE;/*发送总召命令*/
  CS104_Master_Pad[pdrv].sendIFlag[ELEDATA]   = FALSE;/*发送电度总召命令*/
  CS104_Master_Pad[pdrv].sendIFlag[RESETPRO]  = FALSE;/*发送复位进程命令*/
  CS104_Master_Pad[pdrv].sendIFlag[TEST]      = FALSE;/*发送测试命令*/
  CS104_Master_Pad[pdrv].sendIFlag[SWICHAREA] = FALSE;/*发送切换定值区命令*/
  CS104_Master_Pad[pdrv].sendIFlag[RDAREA]    = FALSE;/*发送读定值区号命令*/
  CS104_Master_Pad[pdrv].sendIFlag[RDSETIG]   = FALSE;/*发送读定值参数命令*/
  CS104_Master_Pad[pdrv].sendIFlag[WRSETIG]   = FALSE;/*发送写定值参数命令*/

  CS104_Master_Pad[pdrv].sendUSTARTClock  = 1000;/*1s 启动链路*/

  CS104_Master_Pad[pdrv].sendIClock[ALLDATA]   = 20000;/*60s 总召*/
  CS104_Master_Pad[pdrv].sendIClock[CLOCKSYN]  = 0;
  CS104_Master_Pad[pdrv].sendIClock[ELEDATA]   = 0;
  CS104_Master_Pad[pdrv].sendIClock[RESETPRO]  = 0;
  CS104_Master_Pad[pdrv].sendIClock[TEST]      = 0;
  CS104_Master_Pad[pdrv].sendIClock[SWICHAREA] = 0;
  CS104_Master_Pad[pdrv].sendIClock[RDAREA]    = 0;
  CS104_Master_Pad[pdrv].sendIClock[RDSETIG]   = 0;
  CS104_Master_Pad[pdrv].sendIClock[WRSETIG]   = 0;


//	log_i("CS104_Master_Pad[%d].Port:%d",pdrv,CS104_Master_Pad[pdrv].Port);
//  log_i("CS104_Master_Pad[%d].ModuleSN:%d",pdrv,CS104_Master_Pad[pdrv].ModuleSN);
//	log_i("CS104_Master_Pad[%d].LinkAddress:%d",pdrv,CS104_Master_Pad[pdrv].LinkAddress);
//	log_i("CS104_Master_Pad[%d].PubAddress:%d",pdrv,CS104_Master_Pad[pdrv].PubAddress);
//	log_i("CS104_Master_Pad[%d].CotSize:%d",pdrv,CS104_Master_Pad[pdrv].CotSize);
//	log_i("CS104_Master_Pad[%d].InfoAddrSize:%d",pdrv,CS104_Master_Pad[pdrv].InfoAddrSize);
//	log_i("CS104_Master_Pad[%d].LinkAddrSize:%d",pdrv,CS104_Master_Pad[pdrv].LinkAddrSize);
//	log_i("CS104_Master_Pad[%d].PubAddrSize:%d",pdrv,CS104_Master_Pad[pdrv].PubAddrSize);
}

/**
  * @brief : 104初始化.
  * @param : [void]
  * @return: void
  * @updata: [2018-12-13][Chen][newly increased]
  */
int CS104_Master_create(uint8_t pdrv, int client_no, IecParam sParam)
{
	CS104_Master_ParamInit(pdrv,client_no,sParam);/*参数默认初始化值*/
	CS104_Master_Appinit(pdrv);/*app参数初始化*/
	return 0;
}

/**
  * @brief : 104主站模块参数配置
  * @param : [配置参数]
  * @return: ture or false
  * @update: [2019-06-19][Chen][new]
  */
uint8_t CS104_Master_AppConfigInit(uint8_t pdrv, int client_no, sMasterParam_t sParam)
{
  uint8_t i;
  CS104_Master_Pad[pdrv].ModuleSN = sParam->module_id;
  CS104_Master_Param[pdrv].module_id = sParam->module_id;
  CS104_Master_Param[pdrv].communication_state = sParam->communication_state;
  CS104_Master_Param[pdrv].YX_len = sParam->YX_len;
  CS104_Master_Param[pdrv].p_YX_data = sParam->p_YX_data;
  CS104_Master_Param[pdrv].YC_len = sParam->YC_len;
  CS104_Master_Param[pdrv].p_YC_data = sParam->p_YC_data;
  CS104_Master_Param[pdrv].YK_len = sParam->YK_len;
  CS104_Master_Param[pdrv].p_YK_data = sParam->p_YK_data;
  CS104_Master_Param[pdrv].sourceAddr = sParam->sourceAddr;
  CS104_Master_Param[pdrv].portNo = sParam->portNo;
  CS104_Master_Param[pdrv].yk_type = sParam->yk_type;
  sParam->master_YK_fun = CS104_Master_C_REPLY;
  sParam->master_CLOCK_SYNC_fun = CS104_Master_C_REPLY;
  
//  log_i("CS104_Master_Param[%d].module_id:%d",pdrv,CS104_Master_Param[pdrv].module_id);
//  log_i("CS104_Master_Param[%d].YX_len:%d",pdrv,CS104_Master_Param[pdrv].YX_len);
//  log_i("CS104_Master_Param[%d].YC_len:%d",pdrv,CS104_Master_Param[pdrv].YC_len);
//  log_i("CS104_Master_Param[%d].YK_len:%d",pdrv,CS104_Master_Param[pdrv].YK_len);
//  log_i("CS104_Master_Param[%d].sourceAddr:%d",pdrv,CS104_Master_Param[pdrv].sourceAddr);
//  log_i("CS104_Master_Param[%d].portNo:%d",pdrv,CS104_Master_Param[pdrv].portNo);
//  log_i("CS104_Master_Param[%d].yk_type:%d",pdrv,CS104_Master_Param[pdrv].yk_type);
  for(i=0;i<sParam->YX_len;i++)
  {

  }
//    log_i("(%d)p_YX_data->addr:%x,value:%x",pdrv,(CS104_Master_Param[pdrv].p_YX_data+i)->addr,*(CS104_Master_Param[pdrv].p_YX_data+i)->pValue);
  for(i=0;i<sParam->YC_len;i++)
  {

  }
//    log_i("(%d)p_YC_data->addr:%x,value:%x",pdrv,(CS104_Master_Param[pdrv].p_YC_data+i)->addr,*(CS104_Master_Param[pdrv].p_YC_data+i)->pValue);
  for(i=0;i<sParam->YK_len;i++)
  {

  }
//    log_i("(%d)p_YK_data->addr:%x",pdrv,(CS104_Master_Param[pdrv].p_YK_data+i)->addr);
//  log_i("CS104_Master_Param[%d].yk_type:%d",pdrv,CS104_Master_Param[pdrv].yk_type);
  return TRUE;
}

/**
  * @brief : 104Master运行启动.
  * @param : [void]
  * @return: void
  * @updata: [2019-03-18][Chen][newly increased]
  */
void *CS104_Master_Startup(void *arg)
{
  uint16_t i[256] = {0};
	uint8_t pdrv = *(uint8_t*)arg;
//  log_w("pdrv:%d",pdrv);
	for(;;)
	{
		/*运行程序*/
		CS104_Master_Clock(pdrv);
		if (++i[pdrv] >= 200)
		{
			i[pdrv] = 0;
//			log_e("CS104_Master(%d)_run %llu",pdrv,CS104_Master_getAlarmOfMs(pdrv)/1000);
		}
		usleep(20000);
	}
	return 0;
}

/**
  * @brief : 104Master线程创建.
  * @param : [num]
  * @return: ture or false
  * @updata: [2019-06-18][Chen][new]
  */
int CS104_Master_Pthread(uint8_t num)
{
  uint8_t i = 0;
  
  pthread_t tid;
  int ret = -1;
//  log_w("num:%d",num);
  for(i = 0; i < num; i++)
  {
    disk[i] = i;
//    log_w("disk[%d]:%d",i,disk[i]);
    ret = pthread_create(&tid, NULL, CS104_Master_Startup, &disk[i]);

    if(ret != 0)
    {
      fprintf(stderr, "104 master pthread_create error:%s\n", strerror(ret));
      return FALSE;
    }
    pthread_detach(tid);
  }
  return TRUE;
}
/* END OF FILE ---------------------------------------------------------------*/

