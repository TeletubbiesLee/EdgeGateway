/**
  * @copyright  Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      dataSwtich_point_table.h
  * @brief:     The format common of the project.
  * @version:   ver 1.1
  * @author:    Mr.J
  * @date:      2018-10-15
  * @update:    
  */
	
#ifndef __DATA_SWITCH_POINT_TABLE_H__
#define __DATA_SWITCH_POINT_TABLE_H__

/* INCLUDE --------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
//#include "dataSwitch_format_common.h"
//#include "slist.h"

/* DEFINE ---------------------------------------------------------------------------*/
#define VERSION_KERNEL  "KERNEL_2019_04_15_14_04"
#define VERSION_UBOOT   "UBOOT_2019_04_15_14_04"
#define VERSION_DTS     "DTS_2019_04_15_14_04"
#define VERSION_ROOTFS  "ROOTFS_2019_04_15_14_04"
#define VERSION_APP     "D03C.1.C0002.0000.191009"

/* ENUM -----------------------------------------------------------------------------*/
/* STRUCT ---------------------------------------------------------------------------*/
/* PUBLIC FUNCTION ------------------------------------------------------------------*/
/* 时间配置结构体*/
struct ds_cp56Time2a
{
    uint8_t msecondL;
    uint8_t msecondH;
    uint8_t minute;
    uint8_t hour;
    uint8_t dayofWeek;	// WEEK(D7-D5)day(D4-D0)
    uint8_t month;
    uint8_t year;
};
typedef struct ds_cp56Time2a *ds_cp56Time2a_t;

struct ds_privateTime
{
	struct ds_cp56Time2a time_cp56;
	uint64_t time_ms;
};
typedef struct ds_privateTime *ds_privateTime_t;

/* 遥信配置结构体*/
struct tagTelesignalCfg
{
    uint16_t Addr;  // 点号
    uint16_t AddrUsed;  // 映射
    char Name[32];            // 名称
    char MapName[32];         // 映射名称
    uint8_t *pVal;    // 值
    uint8_t IsDoublePoint; // 上送类型
    uint8_t IsSOE; // 上送soe
    uint8_t IsCOS; // 上送cos
    uint8_t IsNegated; // 取反
    char Content[4][32];    // 数据显示内容
};
typedef struct tagTelesignalCfg *tagTelesignalCfg_t;

struct tagTelesignalInfo
{
	uint16_t maxNum;
	tagTelesignalCfg_t firstPoint;
};
typedef struct tagTelesignalInfo *tagTelesignalInfo_t;

/* 遥测配置结构 */
struct tagTelemetryCfg
{
	uint16_t Addr;  // 点号
	uint16_t AddrUsed;  // 映射
    char Name[32];            // 名称
    char MapName[32];         // 映射名称
    float *pVal;           // 值	
    char Unit[16];           // 单位
    float Rate;    // 倍率
    uint8_t DataType; // 上送类型
    uint8_t IsDeadZone; 		// 上送死区
    float Deadband;     // 死区值
    float Rating;    // 额定值
};
typedef struct tagTelemetryCfg *tagTelemetryCfg_t;

struct tagTelemetryInfo
{
	uint16_t maxNum;
	tagTelemetryCfg_t firstPoint;
};
typedef struct tagTelemetryInfo *tagTelemetryInfo_t;
	
/* 遥控配置结构 */
struct tagTelecontrolCfg
{
	uint16_t Addr;  // 点号
	uint16_t AddrUsed;  // 映射
	uint8_t IsNegated; // 取反
};
typedef struct tagTelecontrolCfg *tagTelecontrolCfg_t;

struct tagTelecontrolInfo
{
	uint16_t maxNum;
	tagTelecontrolCfg_t firstPoint;
};
typedef struct tagTelecontrolInfo *tagTelecontrolInfo_t;

/* 开关量配置结构体*/
struct tagSwitchQuantityCfg
{
	uint16_t UID; //模块内部地址
    uint16_t LoopSN;           // 回路ID
	char Name[32];            // 名称
    char MapName[32];         // 映射名称
    uint16_t Addr;  // 点号
    uint8_t *pVal;    // 值
    uint8_t Operation; // 运算
    uint8_t EnableLocalSOE; // 本地存储soe使能
	uint8_t EnableFault;  // 故障记录使能
    uint8_t EnableHMIShow;  // HMI显示使能
    uint8_t EnableUpperShow;  // 上位机显示使能
	char Content[4][32];    // 数据显示内容
	char ContentSOE[4][32];   // 数据显示内容
};
typedef struct tagSwitchQuantityCfg *tagSwitchQuantityCfg_t;

struct tagSwitchQuantityInfo
{
	uint16_t maxNum;
	tagSwitchQuantityCfg_t firstPoint;
	uint16_t valueAarrysize;
	uint8_t *valueAarry;
	uint16_t *mapAarry;
    uint8_t *isNegated;
};
typedef struct tagSwitchQuantityInfo *tagSwitchQuantityInfo_t;

/* 模入配置结构 */
struct tagAnalogQuantityCfg
{
	uint16_t UID; //模块内部地址
    uint16_t LoopSN;           // 回路ID
	char Name[32];           // 名称
    char MapName[32];         // 映射名称
    uint16_t Addr;  // 点号
    float *pVal;           // 值	
    char Unit[16];           // 单位
    float RatedValue;    // 额定值
	float ZeroDrift;     // 零漂
    float TransformerRatio;     // 一次变比

    float CalibrateCoefficientBase;     // 初始系数
    float CalibrateReferBase[2];     // 校准基准值   //为0为角度，与一般测试不同
    float CalibrateAccuracy;     // 校准精度
    float CalibrateDeviation;     // 可校准偏差 //为0，不可校准
    uint8_t EnableHMIShow;  // HMI显示使能
    uint8_t EnableUpperShow;  // 上位机显示使能
};
typedef struct tagAnalogQuantityCfg *tagAnalogQuantityCfg_t;

struct tagAnalogQuantityInfo
{
	uint16_t maxNum;
	tagAnalogQuantityCfg_t firstPoint;
	uint16_t valueAarrysize;
	uint8_t *valueAarry;
	uint16_t *mapAarry;
};
typedef struct tagAnalogQuantityInfo *tagAnalogQuantityInfo_t;

struct tagAnalogQuantityHalfWare
{
    uint16_t matchNum;  
	uint16_t addr;     
	short *halfWare;
};
typedef struct tagAnalogQuantityHalfWare *tagAnalogQuantityHalfWare_t;

/* 控制输出配置结构 */
struct tagControlOutputCfg
{
	uint16_t UID; //模块内部地址
    uint16_t LoopSN;           // 回路ID
	char Name[32];           // 名称
    char MapName[32];         // 映射名称
    uint16_t Addr;  // 点号
    float *pVal;           // 值
    uint8_t IsEnable; // 遥控使能
};
typedef struct tagControlOutputCfg *tagControlOutputCfg_t;

struct tagControlOutputInfo
{
	uint16_t maxNum;
	tagControlOutputCfg_t firstPoint;
	uint16_t valueAarrysize;
	uint8_t *valueAarry;
	uint16_t *mapAarry;
    uint8_t *isNegated;
};
typedef struct tagControlOutputInfo *tagControlOutputInfo_t;

/* 继电器输出配置结构 */
struct tagRelayOutputCfg
{
	uint16_t UID; //模块内部地址
    uint16_t LoopSN;           // 回路ID
	char Name[32];           // 名称
    char MapName[32];         // 映射名称
    uint16_t Addr;  // 点号
    float *pVal;           // 值
    uint8_t IsMapOnce;  // 只映射一次
};
typedef struct tagRelayOutputCfg *tagRelayOutputCfg_t;

struct tagRelayOutputInfo
{
	uint16_t maxNum;
	tagRelayOutputCfg_t firstPoint;
	uint16_t valueAarrysize;
	uint8_t *valueAarry;
};
typedef struct tagRelayOutputInfo *tagRelayOutputInfo_t;

struct tagRelayOutputCtrl
{
	uint16_t addr;     
	uint8_t value;
	uint32_t maxPulse;
};
typedef struct tagRelayOutputCtrl *tagRelayOutputCtrl_t;

/* 定值配置结构 */
struct tagValueParaCfg
{
	uint16_t UID; //模块内部地址
    uint16_t LoopSN;           // 回路ID
	char Name[32];           // 名称
    char MapName[32];         // 映射名称
    uint16_t Addr;  // 点号
    uint8_t Type;  //38单精度浮点，4字符串，其他无效
    uint8_t TypeSize;  // 长度
    void *pVal;           // 值
    char Unit[16];           // 单位
    float MaxValue;	       // 上限值
    float MinValue;          // 下限值
    float DefaultValue;      // 缺省值
	uint8_t DisplayMode;// 数据类型浮点 0数字，其他压板个数 //数字类型字符串，1不可修改
    uint8_t decimals;// 小数点位数
    char Content[10][32];        // 数据显示内容
    char Note[32];           // 备注 //字符串模式默认值
    char SortName[32];           //  菜单名称
};
typedef struct tagValueParaCfg *tagValueParaCfg_t;

struct tagValueParaInfo
{
	uint16_t maxNum;
	tagValueParaCfg_t firstPoint;
	uint16_t valueAarrysize;
	uint8_t *valueAarry;
};
typedef struct tagValueParaInfo *tagValueParaInfo_t;

/* soe配置结构 */
struct tagSoeCfg
{
	uint16_t addr;  // 点号
    uint8_t value; // 值
    struct ds_cp56Time2a time;//时间
}__attribute__ ((packed));
typedef struct tagSoeCfg *tagSoeCfg_t;

struct tagSoeInfo
{
	uint16_t *full;
	uint16_t *maxNum;
	uint16_t *in;
	tagSoeCfg_t firstPoint;
};
typedef struct tagSoeInfo *tagSoeInfo_t;

/* co配置结构 */
struct tagCoCfg
{
	uint16_t addr;  // 点号
    uint8_t value; // 值
    uint8_t mode; // 模式 //合环，普通
    uint8_t oprate; // 操作 //选择，执行
    uint8_t res; // 结果 //成功，失败
    struct ds_cp56Time2a time;//时间
}__attribute__ ((packed));
typedef struct tagCoCfg *tagCoCfg_t;

struct tagCoInfo
{
	uint16_t *full;
	uint16_t *maxNum;
	uint16_t *in;
	tagCoCfg_t firstPoint;
};
typedef struct tagCoInfo *tagCoInfo_t;

/* 变化遥测数据结构体 */
struct tagNvaCfg
{
    uint16_t addr;
    float value;
}__attribute__ ((packed));
typedef struct tagNvaCfg *tagNvaCfg_t;

/* 故障事件数据结构体 */
struct tagFeventCfg
{
    uint8_t yx_num;
    uint8_t yx_type;
    struct tagSoeCfg yx[1];
    uint8_t yc_num;
    uint8_t yc_type;
    struct tagNvaCfg yc[10];
}__attribute__ ((packed));
typedef struct tagFeventCfg *tagFeventCfg_t;

struct tagFeventInfo
{
	uint16_t *full;
	uint16_t *maxNum;
	uint16_t *in;
	tagFeventCfg_t firstPoint;
};
typedef struct tagFeventInfo *tagFeventInfo_t;

/* ULOG数据结构体 */
struct tagUlogCfg
{
    uint16_t addr;
    char description[24];
    uint8_t  value;
    struct ds_cp56Time2a time;
}__attribute__ ((packed));
typedef struct tagUlogCfg *tagUlogCfg_t;

struct tagUlogInfo
{
	uint16_t *full;
	uint16_t *maxNum;
	uint16_t *in;
	tagUlogCfg_t firstPoint;
};
typedef struct tagUlogInfo *tagUlogInfo_t;

/* 故障录波触发信息结构体 */
struct tagWaveControlInfoCfg
{
    uint8_t LoopSN;
    uint8_t waveNum;        //波数
    uint8_t afterNum;       //延迟几个波整体录波
    uint8_t triggerNum;     //触发录波时波数
}__attribute__ ((packed));
typedef struct tagWaveControlInfoCfg *tagWaveControlInfoCfg_t;

struct tagWaveBaseInfoCfg
{
    uint8_t yc_num;
    uint16_t ycAddrArray[10];
    uint8_t yx_num;
    uint16_t yxAddrArray[5];
}__attribute__ ((packed));
typedef struct tagWaveBaseInfoCfg *tagWaveBaseInfoCfg_t;

struct tagWaveRecordCfg
{
    struct tagWaveControlInfoCfg control;
    struct tagWaveBaseInfoCfg base;
}__attribute__ ((packed));
typedef struct tagWaveRecordCfg *tagWaveRecordCfg_t;


/* HMI配置结构 */
struct tagHMIYXStrCfg
{
    uint16_t loopSN;
    uint16_t maxNum;
	tagSwitchQuantityCfg_t firstPoint;
};
typedef struct tagHMIYXStrCfg *tagHMIYXStrCfg_t;

struct tagHMIYCStrCfg
{
    uint16_t loopSN;
    uint16_t maxNum;
	tagAnalogQuantityCfg_t firstPoint;
};
typedef struct tagHMIYCStrCfg *tagHMIYCStrCfg_t;

struct tagHMIDZMenuStrCfg
{
    char SortName[32];
    uint16_t MaxNum;
	tagValueParaCfg_t firstPoint;
};
typedef struct tagHMIDZMenuStrCfg *tagHMIDZMenuStrCfg_t;

struct tagHMIDZStrCfg
{
    uint16_t loopSN;
    uint16_t menuMaxNum;
	tagHMIDZMenuStrCfg_t firstPoint;
};
typedef struct tagHMIDZStrCfg *tagHMIDZStrCfg_t;

struct tagHMIInitStrCfg
{
    uint16_t YXLoopNum;
    tagHMIYXStrCfg_t YXCfg;      // 遥信
    uint16_t YCLoopNum;
    tagHMIYCStrCfg_t YCCfg;		//遥测
    uint16_t DZLoopNum;
    tagHMIDZStrCfg_t DZCfg;		//定值
};
typedef struct tagHMIInitStrCfg *tagHMIInitStrCfg_t;

/* 控制数据结构 */
struct tagControlComCfg
{
    uint16_t dir;        //报文方向     //必填
    uint16_t tpye;       //命令类型  //必填
    uint64_t timer;     //计时      
    uint16_t send;      //发起方    //下行报文必填
    uint16_t reply;     //回复方    //上行行报文必填
};
typedef struct tagControlComCfg *tagControlComCfg_t;

struct tagControlRemoteCfg
{
    uint16_t addr;      
    uint16_t cmd;       //命令类型 预制，执行，撤销
    uint16_t sta;       //状态 下发，等待，成功，失败，信息不匹配
    uint16_t work;      //动作 分，合
};
typedef struct tagControlRemoteCfg *tagControlRemoteCfg_t;

struct tagControlCfg
{
    struct tagControlComCfg com;    // 公用头
	void *pdata;                 // 内容，强制类型处理//tagControlRemoteCfg_t
    uint32_t backDataLen;           // 为发起方暂存数据长度
	void *pBackData;                // 为发起方暂存数据
};
typedef struct tagControlCfg *tagControlCfg_t;

struct tagLoopValueCfg
{
    uint16_t LoopSN;        // 回路号
    char *pName;           // 匹配名称
    uint16_t startAddr;        // 起始点号
//    tagCommonMoldIn_t pInfo;    // 返回值
};
typedef struct tagLoopValueCfg *tagLoopValueCfg_t;

struct tagControlMemCfg
{
    struct tagControlCfg info;   // 数据
//    struct slist memList;        // 内存申请释放管理
};
typedef struct tagControlMemCfg *tagControlMemCfg_t;

/* 全局函数 */
struct tagFunctionCfg
{
	//get 获取
	int (*get_struct_point)(uint8_t *pAreaCode,uint16_t addr,void **pStruct);//获取某个结构体指针
	int (*get_value_point)(uint8_t *pAreaCode,uint16_t addr,void **pValue,uint8_t *pSize);//获取某个数据指针
	int (*get_currentTime_info)(ds_privateTime_t pTime);//获取时间信息
    int (*get_cp56Time2a_to_ms)(ds_cp56Time2a_t pTime,uint64_t *pMS);//时标转换
	//write 写入
	int (*write_currentTime_info)(ds_cp56Time2a_t pTime);//写入时间信息
	int (*write_valuePara_info)(uint8_t *pAreaCode,uint16_t addr,void *pValue,uint8_t size);//获取定值区信息，区号指针为空，写入默认区域
	int (*write_soe_intern_info)(tagSoeCfg_t pPoint);//写入内部soe信息
    int (*write_soe_extern_info)(tagSoeCfg_t pPoint);//写入外部soe信息
	int (*write_co_info)(tagCoCfg_t pPoint);//写入Co信息
    int (*write_fevent_info)(tagFeventCfg_t pPoint);//写入故障信息
    void (*write_value_comb_info)(void);//组合遥信赋值
	//operate 操作
    void (*operate_covertSendHandle)(void);//转发点表初始化
	int (*operate_areaCode_info)(uint8_t areaCode);//切换运行定值区
    int (*operate_HalfWaveSend_info)(tagAnalogQuantityHalfWare_t pWaveSend);//半波传递操作
	int (*operate_WaveRecord_info)(tagWaveControlInfoCfg_t pWaveControl);//录波操作
    int (*operate_indirect_info)(tagControlCfg_t pInfo);//间接控制
	int (*operate_Control_info)(tagControlCfg_t pInfo);//控制操作
	int (*operate_RelayOutput_info)(tagRelayOutputCtrl_t pInfo);//继电器操作
    int (*operate_LoopValue_info)(tagLoopValueCfg_t pInfo);//获取当前回路值信息
    int (*operate_test_info)(uint8_t *pInfo, uint8_t type,void *pOther);               //测试
    int (*operate_restHandle_info)(void);               //复位
};
typedef struct tagFunctionCfg *tagFunctionCfg_t;

enum
{
	AREACODE_RUN,
	AREACODE_ZONE1,
	AREACODE_ZONE2,
    _AREACODE_VOLUMES
};
#define TELESIGNAL_STARTADDR 0x0001					//遥信
#define TELEMETRY_STARTADDR 0x4001					//遥测
#define TELECONTROL_STARTADDR 0x6001				//遥控

#define SWITCHQUANTITY_STARTADDR 0x0001
#define ANALOGQUANTITY_STARTADDR 0x4001
#define CONTROLOUTPUT_STARTADDR 0x6001
#define RELAYOUTPUT_STARTADDR 0x6201
#define VALUEPARABASE_STARTADDR 0x8001
#define VALUEPARAZOON_STARTADDR 0x8301

/* 全局变量 */
struct tagVariableCfg
{
//	struct tagValueParaIn currentFixedAreaCode;//当前定值区号
//	struct tagValueParaIn dishakingtime;//开入防抖时间
//	struct tagValueParaIn reversetime;//反校时间
//    struct tagValueParaIn angleReferenceChannel;//角度基准通道
    char *ip1;//网口1IP
    char *ip2;//网口2IP
	struct tagTelesignalInfo telesignal;//获取转换遥信信息，变化遥信由通讯程序自行处理（通讯程序专用）
	struct tagTelemetryInfo telemetry;//获取转换遥测信息，变化遥测由通讯程序自行处理（通讯程序专用）
	struct tagTelecontrolInfo telecontrol;//获取转换遥控信息（通讯程序专用）
	struct tagSoeInfo changeSoe;//获取soe(通讯程序专用)
    struct tagCoInfo changeCo;//获取Co(通讯程序专用)
    struct tagFeventInfo changeFevent;//获取fevent(通讯程序专用)
	struct tagSwitchQuantityInfo switchQuantity;//获取开关量（基础遥信）
    struct tagSwitchQuantityInfo combQuantity;//获取开关量（组合基础遥信）
	struct tagAnalogQuantityInfo analogQuantity;//获取模拟量（基础遥测）
	struct tagControlOutputInfo controlOutput;//获取控制输出（基础遥控）
	struct tagRelayOutputInfo relayOutput;//获取继电器输出
	struct tagSoeInfo baseSoe;//获取soe(基础soe)
	struct tagCoInfo baseCo;//获取Co(基础Co)
    struct tagFeventInfo baseFevent;//获取fevent(基础fevent)
	struct tagValueParaInfo valueParaBase;//0
	struct tagValueParaInfo valueParaZone[_AREACODE_VOLUMES]; //0:运行区 其他：定值区
    struct tagHMIInitStrCfg hmiInitStr;
    struct slist *pSetModuleList;
    struct slist *pRunSModuleList;
    struct slist *pDownSModuleList;
    struct slist *pProModuleList;
    struct slist *pRunPModuleList;
    struct slist *pDownPModuleList;
    int test_en;//测试使能
    uint64_t taskMonitoring;
};
typedef struct tagVariableCfg *tagVariableCfg_t;

#define RETURN_TRUE 0
#define RETURN_ERROR -1
#define SWITCH_OFF 1
#define SWITCH_ON 2
#define PLAIT_OFF (0.0f)
#define PLAIT_ON (1.0f)
#define PLAIT_ALARM (2.0f)
#define TI_M_SP_NA_1 1      //单点
#define TI_M_DP_NA_1 3      //双点
#define TI_M_ME_NA_1 9      //归一化值     
#define TI_M_ME_NB_1 11     //标度化值 
#define TI_M_ME_NC_1 13     //短浮点数
#define TI_M_SP_TB_1 30     //带时标单点
#define TI_M_DP_TB_1 31     //带时标双点
#define DATATYPE_FLOAT 38
#define DATATYPE_CHAR 4

enum
{
    TEST_OPREAT_NULL = 1,
    TEST_OPREAT_START,
    TEST_OPREAT_CHECK,
    TEST_OPREAT_CANCEL,
    TEST_RES_STANDBY,
    TEST_RES_SUCCEED,
    TEST_RES_FAIL,
    TEST_RES_WAIT,
};

enum
{
    CONTROL_OFF = 1,
    CONTROL_ON,
    CONTROL_OPREAT_CHOICE,
    CONTROL_OPREAT_EXECUTE,
    CONTROL_OPREAT_CANCEL,
    CONTROL_MODE_LOOP,
    CONTROL_MODE_RULE,
    CONTROL_RES_SUCCEED,
    CONTROL_RES_FAIL,
    CONTROL_RES_WAIT,
    CONTROL_RES_RELEASE,
    CONTROL_RES_INFOMIS,
};

enum
{
    MESSAGE_DOWN = 1,
    MESSAGE_UP,
    TYPE_REMOTE,
    TYPE_SYNC,
};

/* 全局宏定义 */
struct tagDefineCfg
{
	int return_TRUE;//返回值正确
	int return_ERROR;//返回值错误
	uint8_t switch_OFF;//off
	uint8_t switch_ON;//on
	float plate_OFF;//退
	float plate_ON;//投
	float plate_ALARM;//告警
    uint8_t DB_handle_sem;//数据操作信号量
    uint8_t Ti_M_SP_NA_1;//单点
    uint8_t Ti_M_DP_NA_1;//双点
    uint8_t Ti_M_ME_NA_1;//归一化值     
    uint8_t Ti_M_ME_NB_1;//标度化值 
    uint8_t Ti_M_ME_NC_1;//短浮点数
    uint8_t Ti_M_SP_TB_1;//带时标单点
    uint8_t Ti_M_DP_TB_1;//带时标双点
	uint8_t dataType_FLOAT;//单精度浮点
	uint8_t dataType_CHAR;//字符串

    uint8_t message_DOWN;//下行
    uint8_t message_UP;//上行
    uint8_t type_REMOTE;//命令-遥控
    uint8_t type_SYNC;//命令-遥控

    uint8_t control_OFF;//分
    uint8_t control_ON;//合
	uint8_t control_OPREAT_CHOICE;//选择
	uint8_t control_OPREAT_EXECUTE;//执行
    uint8_t control_OPREAT_CANCEL;//撤销
	uint8_t control_MODE_LOOP;//合环
	uint8_t control_MODE_RULE;//常规
	uint8_t control_RES_SUCCEED;//成功
	uint8_t control_RES_FAIL;//失败
    uint8_t control_RES_WAIT;//等待
    uint8_t control_RES_RELEASE;//下发
    uint8_t control_RES_INFOMIS;//信息不匹配

    uint8_t test_OPREAT_NULL;//空命令
    uint8_t test_OPREAT_START;//启动
	uint8_t test_OPREAT_CHECK;//查询
    uint8_t test_OPREAT_CANCEL;//撤销
    uint8_t test_RES_STANDBY;//待机
    uint8_t test_RES_SUCCEED;//成功
	uint8_t test_RES_FAIL;//失败
    uint8_t test_RES_WAIT;//等待
};
typedef struct tagDefineCfg *tagDefineCfg_t;

/* 全局配置结构 */
struct tagOverallCfg
{
	struct tagFunctionCfg fun;
	struct tagVariableCfg var;
	struct tagDefineCfg def;
};
typedef struct tagOverallCfg *tagOverallCfg_t;

/* 转发点号配置结构 */
struct tagCovertSendAddrStrCfg
{
    uint16_t YXNumber;
    tagTelesignalCfg_t YXCfg;      // 遥信
    uint16_t YCNumber;
    tagTelemetryCfg_t YCCfg;		//遥测
    uint16_t YKNumber;
    tagTelecontrolCfg_t YKCfg;		//遥控
};
typedef struct tagCovertSendAddrStrCfg *tagCovertSendAddrStrCfg_t;

/* 下发点号配置结构 */
struct tagDowmModuleAddrStrCfg
{
	char Name[96];           // 模块名称
    uint16_t ModuleType;        // 模块类型
    uint16_t LoopSN;           // 回路ID
    uint16_t ModuleSN;           //模块ID
    uint16_t Number;
	int *UIDArray;		//引脚号
    uint16_t *AddrArray;      // 点号非0
    int *SizeArray;			//类型
    float *ValueArray;				//4字节传递
};
typedef struct tagDowmModuleAddrStrCfg *tagDowmModuleAddrStrCfg_t;

/* 结构体配置结构 */
struct tagRunModuleStrCfg
{       
	tagDowmModuleAddrStrCfg_t cfg;
	void *overall;
	void *externVariable;
	void *insideVariable;
	int (*taskStart)(tagOverallCfg_t pOverall, void **pExternVariable, void **pInsideVariable);
	void (*funcRun)(tagOverallCfg_t pOverall, void **pExternVariable, void **pInsideVariable);
    void (*funcRest)(tagOverallCfg_t pOverall, void **pExternVariable, void **pInsideVariable);
	int (*funcPoint)(tagOverallCfg_t pOverall, void **pExternVariable, void **pInsideVariable, void *pOther);
    void (*testRun)(tagOverallCfg_t pOverall, void **pExternVariable, void **pInsideVariable);
    int (*testPoint)(tagOverallCfg_t pOverall, void **pExternVariable, void **pInsideVariable, void *pOther);
};
typedef struct tagRunModuleStrCfg *tagRunModuleStrCfg_t;

/* 结构体配置结构 */
struct tagRunModuleListStrCfg
{
	uint16_t moduleType;
	void *list;
};
typedef struct tagRunModuleListStrCfg *tagRunModuleListStrCfg_t;  

/* 模块类型枚举 */
enum
{//不要改变数字，按顺序添加，如果修改需要上位机重新上载模块
    _DS_MODTYPE_COMMON = 0x0010,
    _DS_MODTYPE_CAN = 0x0020,
    _DS_MODTYPE_SWITCHCONTROL = 0x0030, 
    _DS_MODTYPE_ANALOG = 0x0040,
    _DS_MODTYPE_PROTECT = 0x1000, 
    _DS_MODTYPE_ACTIVATION = 0x2010, 
    _DS_MODTYPE_SMALLEARTH = 0x2020, 
    _DS_MODTYPE_RECORDWAVECON = 0x2030,
    _DS_MODTYPE_IEC60870 = 0x2040,
    _DS_MODTYPE_LIMIT = 0x2050,
    _DS_MODTYPE_POWERCAL = 0x2060,

    _DS_MODTYPE_YK_RESET = 0x3010,
    _DS_MODTYPE_COMBYX = 0x3020, 
    _DS_MODTYPE_FIXTOYX = 0x3030,
    _DS_MODTYPE_FIXTOYC = 0x3040, 
    _DS_MODTYPE_FIXTOYK = 0x3050,
    _DS_MODTYPE_YX_COUNT = 0x3060,

    _DS_MODTYPE_BASEMODULE = 0x4010, 
    _DS_MODTYPE_PANEL = 0x4020,    
    _DS_MODTYPE_CASCADE = 0xF000,    
};


#define _DS_REENTRANT_UN 0x00
#define _DS_REENTRANT_EN 0x01

/* 结构体配置结构 */
struct tagModuleStrCfg
{
    uint16_t reentrant;         // 可重入
	char moduleName[96];           // 模块名称
    uint16_t moduleType;           // 命令标识 //用于进入时调用对应的不同功能的函数，运行函数链表里检测与之匹配的形参结构体，主要是形参不同，处理结果不同
    uint16_t checkSum;         // 版本校验
    int (*init)(tagOverallCfg_t pOverall, void **pExternVariable, void **pInsideVariable ,tagDowmModuleAddrStrCfg_t pDowmModuleAddr);
    int (*taskStart)(tagOverallCfg_t pOverall, void **pExternVariable, void **pInsideVariable);
    void (*funcRun)(tagOverallCfg_t pOverall, void **pExternVariable, void **pInsideVariable);
    void (*funcRest)(tagOverallCfg_t pOverall, void **pExternVariable, void **pInsideVariable);
    int (*funcPoint)(tagOverallCfg_t pOverall, void **pExternVariable, void **pInsideVariable, void *pOther);
    void (*testRun)(tagOverallCfg_t pOverall, void **pExternVariable, void **pInsideVariable);
    int (*testPoint)(tagOverallCfg_t pOverall, void **pExternVariable, void **pInsideVariable, void *pOther);

    uint16_t existSwitchQuantitySize;
    tagSwitchQuantityCfg_t existSwitchQuantityCfg;
    uint16_t newSwitchQuantitySize;
    tagSwitchQuantityCfg_t newSwitchQuantityCfg;

    uint16_t existAnalogQuantitySize;
    tagAnalogQuantityCfg_t existAnalogQuantityCfg;
    uint16_t newAnalogQuantitySize;
    tagAnalogQuantityCfg_t newAnalogQuantityCfg;

    uint16_t existControlOutputSize;
    tagControlOutputCfg_t existControlOutputCfg;
    uint16_t newControlOutputSize;
    tagControlOutputCfg_t newControlOutputCfg;

    uint16_t existRelayOutputSize;
	tagRelayOutputCfg_t existRelayOutputCfg;
    uint16_t newRelayOutputSize;
    tagRelayOutputCfg_t newRelayOutputCfg;

    uint16_t existValueParaSizeBase;
    tagValueParaCfg_t existValueParaCfgBase;
    uint16_t newValueParaSizeBase;
    tagValueParaCfg_t newValueParaCfgBase;

    uint16_t existValueParaSizeZone;
    tagValueParaCfg_t existValueParaCfgZone;
    uint16_t newValueParaSizeZone;
    tagValueParaCfg_t newValueParaCfgZone;
};
typedef struct tagModuleStrCfg *tagModuleStrCfg_t;

//任务监听
enum
{
    TASK_DATASWTICH = 0,
    TASK_FILE,
    TASK_CMD,
    TASK_AD7616,
    TASK_TEMP,
    TASK_XADC,
    // TASK_TEMP_TEST,
    
    TASK_MAXNUM,
};

#endif /* __DATA_SWTICH_POINT_TABLE_H__ */

/* END OF FILE ---------------------------------------------------------------*/
