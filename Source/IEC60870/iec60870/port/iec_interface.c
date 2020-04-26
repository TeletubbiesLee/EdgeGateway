/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      iec_interface.c
  * @brief:		通用对外接口
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2018-11-30
  * @update:    [2018-11-30][Chen][newly increased]
  */
#define LOG_TAG    "iec_interface"
/* INCLUDE FILES -------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "iec_interface.h"
#include "..\slave101\cs101SlaveDisk.h"
#include "..\slave101\cs101SlaveApp.h"
#include "..\slave104\cs104SlaveDisk.h"
#include "..\slave104\cs104SlaveApp.h"
#include "..\master104\cs104MasterDisk.h"
#include "..\master104\cs104MasterApp.h"
#include "..\serial\serial.h"
//#include "elog.h"
#include "..\file\fileOperate.h"
#include "..\master101\101master.h"
//#include "modbus_master.h"
#include "..\tcp_socket\tcp_interface.h"
/* PRIVATE VARIABLES ---------------------------------------------------------*/
static uint8_t 	portNo = 0;
static int 		baudRate = 9600;
static char 	parity = 'N';
static uint8_t s101_pdrv = 0;
static uint8_t s104_pdrv = 0;
static uint8_t m104_pdrv = 0;
static struct sIecParam_t m104Param;
/* PUBLIC VARIABLE -----------------------------------------------------------*/
uint8_t g_CommunicatFlag[COM_MAX]; // 通讯互锁标志
// uint8_t g_CommunicatFileOper;
/* PRIVATE FUNCTION PROTOTYPES -----------------------------------------------*/
/**
  * @brief : SerialPortParamCheck.
  * @param : [void]
  * @return: void
  * @updata: [2018-12-04][Chen][newly increased]
  */
static uint8_t SerialPortParamCheck(IecParam sParam)
{
	/*串口号*/
	portNo = sParam->portNo - 1;
    if(portNo > 4)//串口越限
    {
        return FALSE;
    }
	/*波特率*/
	switch(sParam->baudRate)
	{
		case 0:
			baudRate = 2400;
			break;
		case 1:
			baudRate = 4800;
			break;
		case 2:
			baudRate = 9600;
			break;
		case 3:
			baudRate = 38400;
			break;
		case 4:
			baudRate = 115200;
			break;
		default:
			baudRate = 9600;
			break;
	}
	/*奇偶校验*/
	switch(sParam->parity)
	{
		case 0:
			parity = 'N';
			break;
		case 1:
			parity = 'O';
			break;
		case 2:
			parity = 'E';
			break;
		default:
			parity = 'N';
			break;
	}
	return TRUE;
}

/**
  * @brief : backupDataOfsParam.
  * @param : [void]
  * @return: void
  * @updata: [2019-07-01][Chen][new]
  */
static void backupDataOfsParam(IecParam sParam)
{
	m104Param.stype        = sParam->stype;
	m104Param.portNo       = sParam->portNo;
	m104Param.baudRate 	   = sParam->baudRate;
	m104Param.parity 	   = sParam->parity;
	m104Param.ip 		   = sParam->ip;
	m104Param.LocalNetPort = sParam->LocalNetPort;
	m104Param.ModuleSN     = sParam->ModuleSN;
	m104Param.encrypt      = sParam->encrypt;
	m104Param.balanMode    = sParam->balanMode;
	m104Param.sourceAddr   = sParam->sourceAddr;
	m104Param.linkAddrSize = sParam->linkAddrSize;
	m104Param.ASDUCotSize  = sParam->ASDUCotSize;
	m104Param.ASDUAddr 	   = sParam->ASDUAddr;
	m104Param.ASDUAddrSize = sParam->ASDUAddrSize;
	m104Param.InfoAddrSize = sParam->InfoAddrSize;
//    m104Param.allInfoDisk = malloc(sizeof(struct tagOverallCfg));		TXL注释
    m104Param.allInfoDisk = sParam->allInfoDisk;
}

/* PUBLIC FUNCTION PROTOTYPES ------------------------------------------------*/
/**
  * @brief : iec60870初始化接口函数.
  * @param : [void]
  * @return: void
  * @updata: [2018-12-04][Chen][newly increased]
  */
int iec_init(IecParam sParam)
{
	int i,status = -1;
	int portFd = 0;
	uint8_t server_no = 0;
	static uint8_t onceInit = TRUE;
	SerialPort serialPort;

	if(onceInit)
	{
		onceInit = FALSE;
		file_operate_Init();
		for(i=0;i<COM_MAX;i++)/*通讯互锁初始化*/
		{g_CommunicatFlag[i] = COMMUNICATLOCKSTATUS;}
	}
	switch(sParam->stype) /*协议类型*/
	{
		case SLAVE101:
//			log_i("iec101 init");

			/*串口参数*/
			if(SerialPortParamCheck(sParam) == FALSE)
            {
//                log_e("portNo error:%d",portNo);
                break;
            }
//            log_i("portNo:%d baudRate:%d parity:%c",portNo, baudRate, parity);
            
			/*串口创建 初始化*/
			serialPort = SerialPort_create(portNo, baudRate, 8, parity, 1);

			/*打开串口*/
			portFd = SerialPort_open(serialPort,portNo);
			if(portFd < 0)
			{
//				log_e("SerialPort_open error:%d",portFd);
			}
//			log_w("portFd:%d,baudRate:%d,parity:%c",portFd,baudRate,parity);

			/*初始化*/
			CS101_Slave_create(s101_pdrv,portFd,sParam);
			s101_pdrv++;
			status = sParam->stype;
			break;

		case SLAVE104:
//			log_i("iec104 init");
			if(sParam->ip == NULL)
			{
//				log_e("ip = null");
				status = -1;
				break;
			}
			server_no = tcp_start(SERVER,sParam->ip);/*网口号*/
//			log_w("server_no:%d",server_no);
			CS104_Slave_create(s104_pdrv,server_no,sParam);
			s104_pdrv++;
			status = sParam->stype;
			break;

		case MASTER101:
//			log_i("MASTER101 init");

			/*串口参数*/
			if(SerialPortParamCheck(sParam) == FALSE)
            {
//                log_e("portNo error:%d",portNo);
                break;
            }
//            log_i("portNo:%d",portNo);
			/*串口创建 初始化*/
			serialPort = SerialPort_create(portNo, baudRate, 8, parity, 1);

			/*打开串口*/
			portFd = SerialPort_open(serialPort,portNo);
			if(portFd < 0)
			{
//				log_e("SerialPort_open error:%d",portFd);
			}
//			log_w("portFd:%d,baudRate:%d,parity:%c",portFd,baudRate,parity);

			/*初始化*/
			if (cs101_master_comconfig_init(portFd, sParam) == -1)
            {
//                log_e("config_101master_init error");
            }
			status = sParam->stype;
			break;

		case MASTER104:
//			log_i("MASTER104 init");
			backupDataOfsParam(sParam);
			status = sParam->stype;
			break;
//
//        case MASTERMODBUS:
//			log_i("MASTERMODBUS init");
//
//			/*串口参数*/
//            if(SerialPortParamCheck(sParam) == FALSE)
//            {
//                log_e("portNo error:%d",portNo);
//                break;
//            }
//            log_i("portNo:%d baudRate:%d parity:%c",portNo,baudRate,parity);
//
//			if (modbus_master_comconfig_init(sParam) == -1)
//            {
//                log_e("modbus_master_comconfig_init error");
//            }
//			status = sParam->stype;
//			break;
		
		default:
			break;
	}
	return status;
}

/**
  * @brief : 级联模块参数.
  * @param : 
  * @return: void
  * @updata: [2019-05-23][Chen][new]
  */
 uint8_t MasterModleParam(sMasterParam_t sParam)
 {
    uint8_t  portNo;
    uint8_t  client_no;

    portNo = sParam->portNo;
//    log_w("MasterModleParam");
    if(portNo)
    {
        if (cs101_master_appconfig_init(sParam) == 0)
        {
            return true;
        }
//        if (modbus_master_appconfig_init(sParam) == 0)
//        {
//            return true;
//        }
    }
    if(sParam->netEn)
    {
        if(m104Param.stype != MASTER104)
        {
//            log_e("not find iec_init");
            return FALSE;
        }
		if(sParam->ip == NULL)
		{
//			log_e("ip = null");
			return FALSE;
		}
//        log_e("IP:%s",sParam->ip);
        client_no = tcp_start(CLIENT,sParam->ip);/*网口号*/
//        log_w("client_no:%d m104_pdrv:%d",client_no,m104_pdrv);

		CS104_Master_create(m104_pdrv,client_no,&m104Param);
        if (CS104_Master_AppConfigInit(m104_pdrv,client_no,sParam) == FALSE)
        {
//            log_e("CS104_Master_AppConfigInit error");
			return FALSE;
        }
		m104_pdrv++;
		return TRUE;
    }
	return FALSE;
 }

/**
  * @brief : 接收回复 上发数据.
  * @param : 
  * @return: void
  * @updata: [2019-05-31][Chen][new]
  */
//uint8_t iec_receiveData(tagControlCfg_t reInfo)
//{
//	uint8_t pdrv;
//    log_w("reInfo->com.send:%d",reInfo->com.send);
//	for(pdrv = 0; pdrv < s101_pdrv; pdrv++)
//	{
//		if(CS101_Slave_Pad[pdrv].ModuleSN == reInfo->com.send)
//		{
//			CS101_Slave_C_REPLY(pdrv,reInfo);
//			return TRUE;
//		}
//	}
//	for(pdrv = 0; pdrv < s104_pdrv; pdrv++)
//	{
//		if(CS104_Slave_Pad[pdrv].ModuleSN == reInfo->com.send)
//		{
//			CS104_Slave_C_REPLY(pdrv,reInfo);
//			return TRUE;
//		}
//	}
//	return FALSE;
//}

/**
  * @brief : 暂停线程.
  * @param : [stype]
  * @return: void
  * @updata: [2019-04-24][Chen][new]
  */
uint8_t thread_pause(uint8_t stype)
{
	switch(stype)
	{
		case SLAVE101:/*slave101线程暂停*/
			CS101_Slave_pause();
		 	break;
		
		case SLAVE104:/*slave104线程暂停*/
			CS104_Slave_pause();
			break;
		
		default:
			break;
	}
	return 0;
}

/**
  * @brief : 恢复线程.
  * @param : [stype]
  * @return: void
  * @updata: [2019-04-24][Chen][new]
  */
uint8_t thread_resume(uint8_t stype)
{
	switch(stype)
	{
		case SLAVE101:/*slave101线程恢复*/
			CS101_Slave_resume(s101_pdrv);
		 	break;
		
		case SLAVE104:/*slave104线程恢复*/
			CS104_Slave_resume(s104_pdrv);
			break;
		
		default:
			break;
	}
	return 0;
}

/**
  * @brief : iec60870线程启动接口函数.
  * @param : [stype]
  * @return: void
  * @updata: [2018-12-04][Chen][newly increased]
  */
int iec_start(uint8_t stype)
{
    static uint8_t statr_flag = 0;
	int ret = 0;
	pthread_t tid;

	struct sched_param param;
	
	pthread_attr_t attr;

    if ((statr_flag >> stype) & 0x01)		//检查是否启动了线程
    {
        return FALSE;
    }
    statr_flag |= (0x01 << stype);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setschedpolicy(&attr, SCHED_RR);
	pthread_attr_setschedparam(&attr, &param);
	//pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED); //要使优先级其作用必须要有这句话

	switch(stype)
	{
		case SLAVE101:
//			log_i("iec101 start");
			if(s101_pdrv <= 0)
			{
//				log_e("s101_pdrv:%d,pthread101 cannot create",s101_pdrv);
				break;
			}
			/* 101子站线程*/
      		param.sched_priority = 95;
			ret = pthread_create(&tid, &attr, CS101_Slave_Startup, &s101_pdrv);
			if (ret != 0)
			{
				fprintf(stderr, "101 pthread_create error:%s\n", strerror(ret));
                break;
			}
			pthread_detach(tid);
			break;

		case SLAVE104:
//			log_i("iec104 start");
			if(s104_pdrv <= 0)
			{
//				log_e("s104_pdrv:%d,pthread104 cannot create",s104_pdrv);
				break;
			}
			/* 104子站线程*/
      		param.sched_priority = 16;
			ret = pthread_create(&tid, &attr, CS104_Slave_Startup, &s104_pdrv);
			if (ret != 0)
			{
				fprintf(stderr, "104 pthread_create error:%s\n", strerror(ret));
			}
			pthread_detach(tid);
			break;
//
		case MASTER101:
			ret = cs101_master_start();
            if (ret == -1)
            {
//                log_e("master101 start error = %d", ret);
            }
			break;
//
		case MASTER104:
//			log_i("master104 start");
			/* 104主站线程*/
			ret = CS104_Master_Pthread(m104_pdrv);
			if(ret == FALSE)
			{
//				log_e("create cs104 master err");
			}
			break;
//
//        case MASTERMODBUS:
//			ret = modbus_master_start();
//            if (ret == -1)
//            {
//                log_e("modbus_master_start = %d", ret);
//            }
//			break;
	}
	return TRUE;
}

/* END OF FILE ----------------------------------------------------------------*/
