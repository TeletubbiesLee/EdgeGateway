#define LOG_TAG    "101 maser"
//#include "fml/elog/inc/elog.h"
#include "101master.h"
#include "../serial/serial.h"
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include "../list/slist.h"
//#include "cascadeModule.h"

static struct slist s_comcfg_101master_List;
static struct slist s_appcfg_101master_List;
static struct slist s_physics_101master_List;
static struct slist s_link_101master_List;
static struct slist s_appdata_101master_List;
static tagOverallCfg_t s_pTagOverall;

/**
  * @brief : 读取数据函数接口
  * @param : [buff]- 数据缓冲区指针
  * @param : [len]- 数据缓冲区长度
  * @return: [read_len]- 读取到的数据长度
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static int cs101_master_read_data_port(int fd, uint8_t *buff, uint32_t len)
{
    int read_len = 0;
    read_len = SerialPort_readByte(fd, buff, len);
    if(read_len)
    {
// 	   	printf("read:  ");
// 		for(int i=0;i<read_len;i++)
// 		{
// 			printf("%02x ",buff[i]);
// 		}
// 		printf("\n");
    }
    return read_len;
}

/**
  * @brief : 写数据函数接口
  * @param : [buff]- 数据缓冲区指针
  * @param : [len]- 数据缓冲区长度
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_write_data_port(int fd, uint8_t *buff, uint32_t len)
{
    SerialPort_write(fd, buff, 0, len);
    if(len)
    {
//    	log_w("(%d)write %d data",port,count);
//    	printf("write: ");
//    	for(int i=0;i<len;i++)
//    	{
//    		printf("%02x ",buff[i]);
//    	}
//    	printf("\n");
    }
}

/**
  * @brief : 接收数据入队
  * @param : [p_buff]- 数据缓冲区指针
  * @param : [p_in]- 队入指针
  * @param : [p_out]- 对出指针
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_data_in_queue(CS101_MASTER_PHYSICS_t *p_physics_data)
{
    uint8_t read_buff[512];
    int read_len = 0;
    int i;

    if ((p_physics_data->rx_queue.in + 1) % CS101_MASTER_RX_QUEUE_LEN == p_physics_data->rx_queue.out)        //队满
    {
//        log_e("queue full!!!");
        sleep(1);
        return;
    }
    read_len = cs101_master_read_data_port(p_physics_data->fd, read_buff, 512);			//读取数据
    if (read_len <= 0)
    {
        return;
    }

    for (i=0; i<read_len; i++)
    {
        p_physics_data->rx_queue.buff[(p_physics_data->rx_queue.in)++] = read_buff[i];
        if (p_physics_data->rx_queue.in >= CS101_MASTER_RX_QUEUE_LEN)
        {
            p_physics_data->rx_queue.in = 0;
        }
        if (p_physics_data->rx_queue.in == p_physics_data->rx_queue.out)		//队满
        {
//            log_e("in = %d, out = %d",p_physics_data->rx_queue.in, p_physics_data->rx_queue.out);
//            log_e("queue full!!!");
            sleep(1);
            break;
        }
    }
}

/**
  * @brief : CP56Time2a转换成时间
  * @param : [p_CP56Time2a]- CP56Time2a时间结构指针
  * @param : [p_time]- 时间结构指针
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_CP56Time2a_to_time(CS101_MASTER_CP56Time2a_t *p_CP56Time2a, CS101_MASTER_PAR_TIME_t *p_time)
{
    p_time->msec    = (((((uint16_t)p_CP56Time2a->milliseconds_H) << 8) & 0xff00) | p_CP56Time2a->milliseconds_L) % 1000;
    p_time->sec     = (((((uint16_t)p_CP56Time2a->milliseconds_H) << 8) & 0xff00) | p_CP56Time2a->milliseconds_L) / 1000;
    p_time->min     = p_CP56Time2a->minutes;
    p_time->hours   = p_CP56Time2a->hours;
    p_time->week    = ((p_CP56Time2a->day_of_week >> 5) & 0x07);
    p_time->day     = (p_CP56Time2a->day_of_week & 0x1f);
    p_time->months  = p_CP56Time2a->months;
    p_time->years   = p_CP56Time2a->years;
}

/**
  * @brief : 获取系统时间
  * @param : [p_buff]- 数据缓冲区指针
  * @param : [p_in]- 队入指针
  * @param : [p_out]- 对出指针
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_get_time(CS101_MASTER_TIME_t *time_101)
{
    uint8_t error = 1;
    struct ds_privateTime sys_time;
	error = s_pTagOverall->fun.get_currentTime_info(&sys_time);
	if(error == RETURN_ERROR)
	{
//		log_e("monitor read time error");
	}
    time_101->CP56Time2a.years = sys_time.time_cp56.year;
    time_101->CP56Time2a.months = sys_time.time_cp56.month;
    time_101->CP56Time2a.day_of_week = sys_time.time_cp56.dayofWeek;
    time_101->CP56Time2a.hours = sys_time.time_cp56.hour;
    time_101->CP56Time2a.minutes = sys_time.time_cp56.minute;
    time_101->CP56Time2a.milliseconds_L = sys_time.time_cp56.msecondL;
    time_101->CP56Time2a.milliseconds_H = sys_time.time_cp56.msecondH;
    
    cs101_master_CP56Time2a_to_time(&time_101->CP56Time2a, &time_101->time_par);
    time_101->time_ms = sys_time.time_ms;
}

/**
  * @brief : 定时器启动
  * @param : [time_ms]- 系统时间ms
  * @param : [timer]- 定时器指针
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_timer_start(CS101_MASTER_TIMER_t *timer)
{
    CS101_MASTER_TIME_t time_101;
    if (timer->enable_flag != CS101_MASTER_TIME_ENABLE)
    {
        cs101_master_get_time(&time_101);
        timer->enable_flag = CS101_MASTER_TIME_ENABLE;
        timer->start_time = time_101.time_ms;
    }
}

/**
  * @brief : 定时器停止
  * @param : [timer]- 定时器指针
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_timer_stop(CS101_MASTER_TIMER_t *timer)
{
    if (timer->enable_flag != CS101_MASTER_TIME_UNABLE)
    {
        timer->enable_flag = CS101_MASTER_TIME_UNABLE;
        timer->start_time = 0;
    }
}

/**
  * @brief : 定时器比较
  * @param : [time_ms]- 系统时间ms
  * @param : [timer]- 定时器指针
  * @return: [-1/0/1] -1:定时器未启动 0:定时器时间未到 1定时时间到
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static int cs101_master_timer_compare(CS101_MASTER_TIMER_t *timer)
{
    CS101_MASTER_TIME_t time_101;
    if (timer->enable_flag != CS101_MASTER_TIME_ENABLE)
    {
        return -1;
    }
    cs101_master_get_time(&time_101);
    if ((time_101.time_ms - timer->start_time) >= timer->timing_time)
    {
        return 1;
    }
    else
    {
        return 0;
    }
    
}

/**
  * @brief : 写遥测数据
  * @param : [module_id]- 模块ID
  * @param : [yc_data]- 遥测数据
  * @return: [void]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
 static void cs101_master_write_YCdata(uint16_t module_id, CS101_MASTER_YC_DATA_t *yc_data)
 {
    struct snode *p_temp_node = NULL;
    uint16_t deviation_addr = 0;
    /* 查找模块ID */
    p_temp_node = s_appdata_101master_List.head;
    while (p_temp_node != NULL)
    {
        if (((CS101_MASTER_APPDATA_t*)(p_temp_node->data))->module_id == module_id)
        {
            deviation_addr = yc_data->addr - CS101_MASTER_YC_START_ADDR;
            /* 写遥测 */
            if (((deviation_addr) < ((CS101_MASTER_APPDATA_t*)(p_temp_node->data))->YC_len) && (deviation_addr >= 0))
            {
                *(((CS101_MASTER_APPDATA_t*)(p_temp_node->data))->p_YC_data + deviation_addr)->pValue = yc_data->value;
                return;
            }
            return;
        }
        p_temp_node = p_temp_node->next;
    }
 }

/**
  * @brief : 写遥信数据
  * @param : [module_id]- 模块ID
  * @param : [yx_data]- 遥信数据
  * @return: [void]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
 static void cs101_master_write_YXdata(uint16_t module_id, CS101_MASTER_YX_DATA_t *yx_data)
 {
//    struct snode *p_temp_node = NULL;
//    uint16_t deviation_addr = 0;
    /* 查找模块ID */
//    p_temp_node = s_appdata_101master_List.head;
//    while (p_temp_node != NULL)
//    {
//        if (((CS101_MASTER_APPDATA_t*)(p_temp_node->data))->module_id == module_id)
//        {
//            deviation_addr = yx_data->addr - CS101_MASTER_YX_START_ADDR;
//            if ((deviation_addr < ((CS101_MASTER_APPDATA_t*)(p_temp_node->data))->YX_len) && (deviation_addr >= 0))
//            {
//                moduleCascadeWriteInput(((((CS101_MASTER_APPDATA_t*)(p_temp_node->data))->p_YX_data) + deviation_addr)->addr, yx_data->value);
//            }
//            return;
//        }
//        p_temp_node = p_temp_node->next;
//    }
 }

/**
  * @brief : 写SOE数据
  * @param : [module_id]- 模块ID
  * @param : [yx_data]- 遥信数据
  * @return: [void]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
 static void cs101_master_write_SOEdata(uint16_t module_id, CS101_MASTER_SOE_DATA_t *SOE_data)
 {
//    struct snode *p_temp_node = NULL;
//    uint16_t deviation_addr = 0;
//    struct tagSoeCfg point;
    /* 查找模块ID */
//    p_temp_node = s_appdata_101master_List.head;
//    while (p_temp_node != NULL)
//    {
//        if (((CS101_MASTER_APPDATA_t*)(p_temp_node->data))->module_id == module_id)
//        {
//            deviation_addr = SOE_data->addr - CS101_MASTER_YX_START_ADDR;
//            if ((deviation_addr < ((CS101_MASTER_APPDATA_t*)(p_temp_node->data))->YX_len) && (deviation_addr >= 0))
//            {
//                point.addr =  ((((CS101_MASTER_APPDATA_t*)(p_temp_node->data))->p_YX_data) + deviation_addr)->addr;
//                point.value = SOE_data->value;
//                point.time.msecondL = SOE_data->CP56Time2a.milliseconds_L;
//                point.time.msecondH = SOE_data->CP56Time2a.milliseconds_H;
//                point.time.minute = SOE_data->CP56Time2a.minutes;
//                point.time.hour = SOE_data->CP56Time2a.hours;
//                point.time.dayofWeek = SOE_data->CP56Time2a.day_of_week;	// WEEK(D7-D5)day(D4-D0)
//                point.time.month = SOE_data->CP56Time2a.months;
//                point.time.year = SOE_data->CP56Time2a.years;
//                moduleCascadeWriteSoe(&point);
//            }
//            return;
//        }
//        p_temp_node = p_temp_node->next;
//    }
 }

/**
  * @brief : 写通讯状态
  * @param : [module_id]- 模块ID
  * @param : [state]- 通讯状态
  * @return: [void]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
 static int cs101_master_write_comm_state(uint16_t module_id, uint8_t state, CS101_MASTER_CP56Time2a_t *p_CP56Time2a)
 {
    struct snode *p_temp_node = NULL;
//    struct tagSoeCfg point;
    /* 查找模块ID */
    p_temp_node = s_appdata_101master_List.head;
    while (p_temp_node != NULL)
    {
        if (((CS101_MASTER_APPDATA_t*)(p_temp_node->data))->module_id == module_id)
        {
            if (*((CS101_MASTER_APPDATA_t*)(p_temp_node->data))->communication_state.pValue != state)
            {
//                point.addr = ((CS101_MASTER_APPDATA_t*)(p_temp_node->data))->communication_state.addr;
//                point.value = state;
//                point.time.msecondL = p_CP56Time2a->milliseconds_L;
//                point.time.msecondH = p_CP56Time2a->milliseconds_H;
//                point.time.minute = p_CP56Time2a->minutes;
//                point.time.hour = p_CP56Time2a->hours;
//                point.time.dayofWeek = p_CP56Time2a->day_of_week;	// WEEK(D7-D5)day(D4-D0)
//                point.time.month = p_CP56Time2a->months;
//                point.time.year = p_CP56Time2a->years;
//                moduleCascadeWriteSoe(&point);
                return 0;
            }
            return -1;
        }
        p_temp_node = p_temp_node->next;
    }
    return -1;
 }

/**
  * @brief : 校验和计算
  * @param : [p_buff]- 数据缓冲区指针
  * @param : [link_addr_len]- 数据长度
  * @return: [sum]- 校验和
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static uint8_t cs101_master_check_sun_cal(uint8_t *p_buff, uint8_t link_addr_len)
{
    uint16_t sum = 0;
    uint16_t j =0;
    uint8_t *p = 0;
    uint8_t i = 0;

    if (p_buff[0] == CS101_MASTER_FIXED_START)                   //固定帧
    {
        for (j=0; j<(link_addr_len + 1); j++)
        {
            sum += p_buff[1 + j];
        }
    }
    else if(p_buff[0] == CS101_MASTER_VARIABLE_START)
    {
        p = p_buff + 4;
        i = p_buff[1];

        while (i--)
        {
            sum += (*p++);
        }
    }
    return (uint8_t)(sum&0xff);
}

/**
  * @brief : 报文校验
  * @param : [p_buff]- 数据缓冲区指针
  * @param : [link_addr]- 链路地址
  * @return: [0/-1]- 0：成功，1：失败
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static int cs101_master_message_verify(uint8_t *p_buff, CS101_MASTER_LINK_ADDR_t *link_addr)
{
    int i = 0;

    if (p_buff[0] == CS101_MASTER_FIXED_START)                   //固定帧
    {
        /* 帧校验和CS校验 */
        if (p_buff[2 + link_addr->len] != (cs101_master_check_sun_cal(p_buff, link_addr->len)&0xFF))	
        {
            return -1;
        }
        /* 结束帧检测 */
        if (p_buff[3 + link_addr->len] != CS101_MASTER_FRAME_FINISH)
        {
            return -1;
        }
        /* 公共地址检测 */
        for (i=0; i<link_addr->len; i++)
        {
            if (p_buff[2 + i] != link_addr->addr[i])
            {
                return -1;
            }
        }
        return 0;
    }
    else if(p_buff[0] == CS101_MASTER_VARIABLE_START)            //可变帧
    {
        /* 报头校验 */
        if ((p_buff[0] != p_buff[3]) || (p_buff[1] != p_buff[2]))
        {
            return -1;
        }
        /* 帧校验和CS校验 */
        if (p_buff[4 + p_buff[1]] != (cs101_master_check_sun_cal(p_buff, link_addr->len)&0xFF))	
        {
            return -1;
        }
        /* 结束帧检测 */
        if (p_buff[5 + p_buff[1]] != CS101_MASTER_FRAME_FINISH)
        {
            return -1;
        }
        /* 公共地址检测 */
        for (i=0; i<link_addr->len; i++)
        {
            if (p_buff[5 + i] != link_addr->addr[i])
            {
                return -1;
            }
        }
        return 0;
    }
    else
    {
        return -1;
    }
}

/**
  * @brief : 总召ASDU组包
  * @param : [ASDU]- ASDU
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_always_ASDU_pack(CS101_MASTER_ASDU_t *ASDU)
{
    int i = 0; 

    ASDU->TI = CS101_MASTER_TI_C_IC_NA_1;
    ASDU->VSQ = 1;
    for (i=0; i<ASDU->COT.len; i++)
    {
        ASDU->COT.cot[i] = ((CS101_MASTER_COT_ACT >> (8 * i)) & 0xff);
    }

    for (i=0; i<CS101_MASTER_INFO_ADDR_LEN; i++)
    {
        ASDU->info_data.C_100.info_addr[i] = 0;
    }
    ASDU->info_data.C_100.QOI = 20;
    ASDU->info_len = CS101_MASTER_INFO_ADDR_LEN + sizeof(ASDU->info_data.C_100.QOI);
}

/**
  * @brief : 总召ASDU解析
  * @param : [ASDU]- ASDU
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_always_ASDU_analy(CS101_MASTER_LINK_t *p_link_data)
{
    switch (p_link_data->ASDU.COT.cot[0])
    {
        case CS101_MASTER_COT_ACTCON: break;           /* 总召确认 */
        case CS101_MASTER_COT_ACTTERM:                 /* 总召结束 */
            p_link_data->link_flag |= CS101_MASTER_LINK_INIT_END_FLAG;       
            
            if (!(p_link_data->link_flag & CS101_MASTER_LINK_CLOCK_SYNC_FLAG))
            {
                p_link_data->link_flag |= CS101_MASTER_LINK_CLOCK_SYNC_FLAG;
                p_link_data->pack_flag |= CS101_M_PACK_CLOCK_SYNC_FLAG;     /* 时钟同步 */
            }   
            break;          
        default: break; 
    }
    if (p_link_data->mode_selection == CS101_MASTER_BALANCE_MODE)     //平衡模式
    {
        p_link_data->pack_flag |= CS101_M_PACK_LINK_ACCEPT_FLAG;   /* 发送认可 */
    }
}

/**
  * @brief : 时钟同步和读取ASDU组包
  * @param : [ASDU]- ASDU指针
  * @param : [CP56Time2a]- 时标指针
  * @param : [COT]- 传送原因
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_clock_synch_ASDU_pack(CS101_MASTER_ASDU_t *ASDU, CS101_MASTER_CP56Time2a_t *CP56Time2a, uint8_t COT)
{
    int i = 0; 

    ASDU->TI = CS101_MASTER_TI_C_CS_NA_1;
    ASDU->VSQ = 1;
    for (i=0; i<ASDU->COT.len; i++)
    {
        ASDU->COT.cot[i] = ((COT >> (8 * i)) & 0xff);
    }

    for (i=0; i<CS101_MASTER_INFO_ADDR_LEN; i++)
    {
        ASDU->info_data.C_103.info_addr[i] = 0;
    }
    ASDU->info_data.C_103.CP56Time2a = *CP56Time2a;
    ASDU->info_len = CS101_MASTER_INFO_ADDR_LEN + sizeof(ASDU->info_data.C_103.CP56Time2a);
}

/**
  * @brief : 时钟同步和读取ASDU解析
  * @param : [p_master_101_data]- 101主站数据结构指针
  * @param : [time_data]- 时间结构指针
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_clock_synch_ASDU_analy(CS101_MASTER_LINK_t *p_link_data)
{
    CS101_MASTER_PAR_TIME_t time_data;

    cs101_master_CP56Time2a_to_time(&p_link_data->ASDU.info_data.C_103.CP56Time2a, &time_data);
    switch (p_link_data->ASDU.COT.cot[0])
    {
        case CS101_MASTER_COT_REQ: break;                /* 时钟读取确认 */
        case CS101_MASTER_COT_ACTCON:                    /* 时钟同步确认 */
            break;           
        default: break;
    }
    if (p_link_data->mode_selection == CS101_MASTER_BALANCE_MODE)     //平衡模式
    {
        p_link_data->pack_flag |= CS101_M_PACK_LINK_ACCEPT_FLAG;   /* 发送认可 */
    }
}

/**
  * @brief : 复位进程ASDU组包
  * @param : [ASDU]- ASDU指针
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_reset_ASDU_pack(CS101_MASTER_ASDU_t *ASDU)
{
    int i = 0; 

    ASDU->TI = CS101_MASTER_TI_C_RP_NA_1;
    ASDU->VSQ = 1;
    for (i=0; i<ASDU->COT.len; i++)
    {
        ASDU->COT.cot[i] = ((CS101_MASTER_COT_ACT >> (8 * i)) & 0xff);
    }

    for (i=0; i<CS101_MASTER_INFO_ADDR_LEN; i++)
    {
        ASDU->info_data.C_105.info_addr[i] = 0;
    }
    ASDU->info_data.C_105.QRP = 1;
    ASDU->info_len = CS101_MASTER_INFO_ADDR_LEN + sizeof(ASDU->info_data.C_105.QRP);
}

/**
  * @brief : 复位进程ASDU解析
  * @param : [p_master_101_data]- 101主站数据结构指针
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_reset_ASDU_analy(CS101_MASTER_LINK_t *p_link_data)
{
    if (p_link_data->ASDU.COT.cot[0] == CS101_MASTER_COT_ACTCON)
    {
        p_link_data->link_flag = 0;
        if (p_link_data->mode_selection == CS101_MASTER_BALANCE_MODE)     //平衡模式
        {
            p_link_data->pack_flag |= CS101_M_PACK_LINK_ACCEPT_FLAG;   /* 发送认可 */
        }
    }
}

/**
  * @brief : 遥控ASDU组包
  * @param : [p_master_101_data]- 101主站数据结构指针
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_YK_ASDU_pack(CS101_MASTER_LINK_t *p_link_data)
{
    int i = 0; 
    uint8_t cot = 0;
    uint8_t S_E = 0;
    uint8_t open_close = 0;

    if (p_link_data->link_flag & CS101_MASTER_YK_SELECT_FLAG)          /* 选择 */
    {
        S_E = CS101_MASTER_YK_SELECT;
        cot = CS101_MASTER_COT_ACT;
    }
    else if (p_link_data->link_flag & CS101_MASTER_YK_EXECUTE_FLAG)     /* 执行 */
    {
        S_E = CS101_MASTER_YK_EXECUTE;
        cot = CS101_MASTER_COT_ACT;
    }
    else if (p_link_data->link_flag & CS101_MASTER_YK_FINAL_FLAG)     /* 终止 */
    {
        S_E = CS101_MASTER_YK_EXECUTE;
        cot = CS101_MASTER_COT_DEACT;
    }

    if (p_link_data->link_flag & CS101_MASTER_YK_OPEN_FLAG)
    {
        open_close = 1;             //双点分
    }
    else
    {
        open_close = 2;             //双点合
    }
    
    if (p_link_data->link_flag & CS101_MASTER_YK_TWO_POINT_FLAG)       /* 双点 */
    {
        p_link_data->ASDU.TI = CS101_MASTER_TI_C_SC_NB_1;
    }
    else
    {
        p_link_data->ASDU.TI = CS101_MASTER_TI_C_SC_NA_1;
    }
    p_link_data->ASDU.VSQ = 1;

    for (i=0; i<p_link_data->ASDU.COT.len; i++)
    {
        p_link_data->ASDU.COT.cot[i] = ((cot >> (8 * i)) & 0xff);
    }

    if (p_link_data->ASDU.TI == CS101_MASTER_TI_C_SC_NA_1)
    {
        for (i=0; i<CS101_MASTER_INFO_ADDR_LEN; i++)
        {
            p_link_data->ASDU.info_data.C_45.info_addr[i] = (uint8_t)((p_link_data->yk_data.yk_addr >> (i * 8)) & 0x00ff);
        }
        p_link_data->ASDU.info_data.C_45.SCO = ((S_E | (open_close - 1)) & 0xff);
        p_link_data->ASDU.info_len = CS101_MASTER_INFO_ADDR_LEN + sizeof(p_link_data->ASDU.info_data.C_45.SCO);
    }

    if (p_link_data->ASDU.TI == CS101_MASTER_TI_C_SC_NB_1)
    {
        for (i=0; i<CS101_MASTER_INFO_ADDR_LEN; i++)
        {
            p_link_data->ASDU.info_data.C_46.info_addr[i] = (uint8_t)((p_link_data->yk_data.yk_addr >> (i * 8)) & 0x00ff);
        }
        p_link_data->ASDU.info_data.C_46.DCO = ((S_E | open_close) & 0xff);
        p_link_data->ASDU.info_len = CS101_MASTER_INFO_ADDR_LEN + sizeof(p_link_data->ASDU.info_data.C_46.DCO);
    }
}

/**
  * @brief : 遥控ASDU解析
  * @param : [p_master_101_data]- 101主站数据结构指针
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_YK_ASDU_analy(CS101_MASTER_LINK_t *p_link_data)
{
    switch (p_link_data->ASDU.COT.cot[0])
    {
        case CS101_MASTER_COT_ACTCON:                    /* 激活确认 */
            p_link_data->link_flag |= CS101_MASTER_YK_ACT_AFFIRM_FLAG;
            break;
        case CS101_MASTER_COT_DEACTCON:                  /* 停止激活确认 */
            p_link_data->link_flag |= CS101_MASTER_YK_FINAL_AFFIRM_FLAG;
            break;
        case CS101_MASTER_COT_ACTTERM:                   /* 激活终止 */
            p_link_data->link_flag |= CS101_MASTER_YK_EXE_END_FLAG;
            break;
        default:
            p_link_data->link_flag |= CS101_MASTER_YK_ERROR_FLAG;
            break;
    }
    if (p_link_data->mode_selection == CS101_MASTER_BALANCE_MODE)     //平衡模式
    {
        p_link_data->pack_flag |= CS101_M_PACK_LINK_ACCEPT_FLAG;   /* 发送认可 */
    }
}

/**
  * @brief : 遥信ASDU解析
  * @param : [p_master_101_data]- 101主站数据结构指针
  * @param : [yx_data]- 遥信数据结构
  * @return: [0/-1]- 0：成功，1：失败
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_YX_ASDU_analy(CS101_MASTER_LINK_t *p_link_data)
{
    uint8_t number = 0;
    uint8_t SQ = 0;
    CS101_MASTER_YX_DATA_t yx_data;
    int i = 0;

    number = ((p_link_data->ASDU.VSQ) & 0x7f);
    SQ = ((p_link_data->ASDU.VSQ & 0x80) >> 7);
    if (SQ == 0)
    {
        for (i=0; i<number; i++)
        {
            /* 单点 */
            if (p_link_data->ASDU.TI == TI_M_SP_NA_1)
            {
                yx_data.addr = (p_link_data->ASDU.info_data.C_1.SQ0.array[i].info_addr[1] << 8) | \
                                 p_link_data->ASDU.info_data.C_1.SQ0.array[i].info_addr[0];
                yx_data.value = p_link_data->ASDU.info_data.C_1.SQ0.array[i].value + 1;  //转换为双点
            }
            /* 双点 */
            if (p_link_data->ASDU.TI == TI_M_DP_NA_1)
            {
                yx_data.addr = (p_link_data->ASDU.info_data.C_3.SQ0.array[i].info_addr[1] << 8) | 
                                 p_link_data->ASDU.info_data.C_3.SQ0.array[i].info_addr[0];
                yx_data.value = p_link_data->ASDU.info_data.C_3.SQ0.array[i].value;
            }
            if (((p_link_data->ASDU.COT.cot[0]) == CS101_MASTER_COT_INTROGEN) && (!(p_link_data->link_flag & CS101_MASTER_LINK_INIT_END_FLAG)))
            {
                cs101_master_write_YXdata(p_link_data->module_id, &yx_data);
            }
        }
    }
    if (SQ == 1)
    {
        for (i=0; i<number; i++)
        {
            /* 单点 */
            if (p_link_data->ASDU.TI == TI_M_SP_NA_1)
            {
                yx_data.addr = ((p_link_data->ASDU.info_data.C_1.SQ1.info_addr[1] << 8) | \
                                  p_link_data->ASDU.info_data.C_1.SQ1.info_addr[0]) + i;
                yx_data.value = p_link_data->ASDU.info_data.C_1.SQ1.array[i].value + 1;  //转换为双点
            }
            /* 双点 */
            if (p_link_data->ASDU.TI == TI_M_DP_NA_1)
            {
                yx_data.addr = ((p_link_data->ASDU.info_data.C_3.SQ1.info_addr[1] << 8) | \
                                  p_link_data->ASDU.info_data.C_3.SQ1.info_addr[0]) + i;
                yx_data.value = p_link_data->ASDU.info_data.C_3.SQ1.array[i].value;
            }
            if (((p_link_data->ASDU.COT.cot[0]) == CS101_MASTER_COT_INTROGEN) && (!(p_link_data->link_flag & CS101_MASTER_LINK_INIT_END_FLAG)))
            {
                cs101_master_write_YXdata(p_link_data->module_id, &yx_data);
            }
        }
    }
    /* 传送原因解析 */
    switch (p_link_data->ASDU.COT.cot[0])
    {
        case CS101_MASTER_COT_BACK: break;           /* 背景扫描 */
        case CS101_MASTER_COT_SPONT: break;          /* 突发，自发 */
        case CS101_MASTER_COT_REQ: break;            /* 请求或被请求*/
        case CS101_MASTER_COT_INTROGEN: break;       /* 响应站召唤 */
        default: break;
    }
    if (p_link_data->mode_selection == CS101_MASTER_BALANCE_MODE)     //平衡模式
    {
        p_link_data->pack_flag |= CS101_M_PACK_LINK_ACCEPT_FLAG;   /* 发送认可 */
    }
}

/**
  * @brief : SOE ASDU解析
  * @param : [p_master_101_data]- 101主站数据结构指针
  * @param : [soe_data]- SOE数据结构
  * @return: [0/-1]- 0：成功，1：失败
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_SOE_ASDU_analy(CS101_MASTER_LINK_t *p_link_data)
{
    uint8_t number = 0;
    uint8_t SQ = 0;
    CS101_MASTER_SOE_DATA_t soe_data;
    int i = 0;

    CS101_MASTER_PAR_TIME_t date_time;

    number = ((p_link_data->ASDU.VSQ) & 0x7f);
    SQ = ((p_link_data->ASDU.VSQ & 0x80) >> 7);
    if (SQ == 0)
    {
        for (i=0; i<number; i++)
        {
            /* 单点 */
            if (p_link_data->ASDU.TI == TI_M_SP_TB_1)
            {
                soe_data.addr = (p_link_data->ASDU.info_data.C_30.SQ0.array[i].info_addr[1] << 8) | \
                                  p_link_data->ASDU.info_data.C_30.SQ0.array[i].info_addr[0];
                soe_data.value = p_link_data->ASDU.info_data.C_30.SQ0.array[i].value + 1;  //转换为双点
                soe_data.CP56Time2a = p_link_data->ASDU.info_data.C_30.SQ0.array[i].CP56Time2a;
            }
            /* 双点 */
            if (p_link_data->ASDU.TI == TI_M_DP_TB_1)
            {
                soe_data.addr = (p_link_data->ASDU.info_data.C_31.SQ0.array[i].info_addr[1] << 8) | \
                                  p_link_data->ASDU.info_data.C_31.SQ0.array[i].info_addr[0];
                soe_data.value = p_link_data->ASDU.info_data.C_31.SQ0.array[i].value;
                soe_data.CP56Time2a = p_link_data->ASDU.info_data.C_31.SQ0.array[i].CP56Time2a;
            }
            cs101_master_CP56Time2a_to_time(&soe_data.CP56Time2a, &date_time);
            cs101_master_write_SOEdata(p_link_data->module_id, &soe_data);
        }
    }
    if (SQ == 1)
    {
        for (i=0; i<number; i++)
        {
            /* 单点 */
            if (p_link_data->ASDU.TI == TI_M_SP_NA_1)
            {
                soe_data.addr = ((p_link_data->ASDU.info_data.C_30.SQ1.info_addr[1] << 8) | \
                                   p_link_data->ASDU.info_data.C_30.SQ1.info_addr[0]) + i;
                soe_data.value = p_link_data->ASDU.info_data.C_30.SQ1.array[i].value + 1;  //转换为双点
                soe_data.CP56Time2a = p_link_data->ASDU.info_data.C_30.SQ0.array[i].CP56Time2a;
            }
            /* 双点 */
            if (p_link_data->ASDU.TI == TI_M_DP_NA_1)
            {
                soe_data.addr = ((p_link_data->ASDU.info_data.C_31.SQ1.info_addr[1] << 8) | \
                                   p_link_data->ASDU.info_data.C_31.SQ1.info_addr[0]) + i;
                soe_data.value = p_link_data->ASDU.info_data.C_31.SQ1.array[i].value + 1;  //转换为双点
                soe_data.CP56Time2a = p_link_data->ASDU.info_data.C_31.SQ0.array[i].CP56Time2a;
            }
            cs101_master_CP56Time2a_to_time(&soe_data.CP56Time2a, &date_time);
            cs101_master_write_SOEdata(p_link_data->module_id, &soe_data);
        }
    }
    /* 传送原因解析 */
    switch (p_link_data->ASDU.COT.cot[0])
    {
        case CS101_MASTER_COT_BACK: break;         /* 背景扫描 */
        case CS101_MASTER_COT_SPONT: break;        /* 突发，自发 */
        case CS101_MASTER_COT_REQ: break;          /* 请求或被请求*/
        case CS101_MASTER_COT_INTROGEN: break;     /* 响应站召唤 */
        default: break;
    }
    if (p_link_data->mode_selection == CS101_MASTER_BALANCE_MODE)     //平衡模式
    {
        p_link_data->pack_flag |= CS101_M_PACK_LINK_ACCEPT_FLAG;   /* 发送认可 */
    }
}

/**
  * @brief : 遥测 ASDU解析
  * @param : [p_master_101_data]- 101主站数据结构指针
  * @param : [soe_data]- SOE数据结构
  * @return: [0/-1]- 0：成功，1：失败
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_YC_ASDU_analy(CS101_MASTER_LINK_t *p_link_data)
{
    uint8_t number = 0;
    uint8_t SQ = 0;
    int i = 0;
    uint16_t yc_value;
    CS101_MASTER_YC_DATA_t yc_data;

    number = ((p_link_data->ASDU.VSQ) & 0x7f);
    SQ = ((p_link_data->ASDU.VSQ & 0x80) >> 7);
    switch (p_link_data->ASDU.TI)
    {
        case TI_M_ME_NA_1:              /* 归一化值 */
            if (SQ == 0)
            {
                for (i=0; i<number; i++)
                {
                    yc_data.addr = (p_link_data->ASDU.info_data.C_9.SQ0.array[i].info_addr[1] << 8) | \
                                    p_link_data->ASDU.info_data.C_9.SQ0.array[i].info_addr[0];
                    yc_value = (p_link_data->ASDU.info_data.C_9.SQ0.array[i].value[1] << 8)| \
                                p_link_data->ASDU.info_data.C_9.SQ0.array[i].value[0];
                    yc_data.value = (float)yc_value;
                    cs101_master_write_YCdata(p_link_data->module_id, &yc_data);
                }
            }
            if (SQ == 1)
            {
                for (i=0; i<number; i++)
                {
                    yc_data.addr = ((p_link_data->ASDU.info_data.C_9.SQ1.info_addr[1] << 8) | \
                                    p_link_data->ASDU.info_data.C_9.SQ1.info_addr[0]) + i;
                    yc_value = (p_link_data->ASDU.info_data.C_9.SQ1.array[i].value[1] << 8)| \
                                p_link_data->ASDU.info_data.C_9.SQ1.array[i].value[0];
                    yc_data.value = (float)yc_value;
                    cs101_master_write_YCdata(p_link_data->module_id, &yc_data);
                }
            }
            break;
        case TI_M_ME_NB_1:              /* 标度化值 */
            if (SQ == 0)
            {
                for (i=0; i<number; i++)
                {
                    yc_data.addr = (p_link_data->ASDU.info_data.C_11.SQ0.array[i].info_addr[1] << 8) | \
                                    p_link_data->ASDU.info_data.C_11.SQ0.array[i].info_addr[0];
                    yc_value = (p_link_data->ASDU.info_data.C_11.SQ0.array[i].value[1] << 8)| \
                                p_link_data->ASDU.info_data.C_11.SQ0.array[i].value[0];
                    yc_data.value = (float)yc_value;
                    cs101_master_write_YCdata(p_link_data->module_id, &yc_data);
                }
            }
            if (SQ == 1)
            {
                for (i=0; i<number; i++)
                {
                    yc_data.addr = ((p_link_data->ASDU.info_data.C_11.SQ1.info_addr[1] << 8) | \
                                    p_link_data->ASDU.info_data.C_11.SQ1.info_addr[0]) + i;
                    yc_value = (p_link_data->ASDU.info_data.C_11.SQ1.array[i].value[1] << 8)| \
                                p_link_data->ASDU.info_data.C_11.SQ1.array[i].value[0];
                    yc_data.value = (float)yc_value;
                    cs101_master_write_YCdata(p_link_data->module_id, &yc_data);
                }
            }
            break;
        case TI_M_ME_NC_1:              /* 短浮点数 */
            if (SQ == 0)
            {
                for (i=0; i<number; i++)
                {
                    yc_data.addr = (p_link_data->ASDU.info_data.C_13.SQ0.array[i].info_addr[1] << 8) | \
                                    p_link_data->ASDU.info_data.C_13.SQ0.array[i].info_addr[0];
                    memcpy(&yc_data.value, p_link_data->ASDU.info_data.C_13.SQ1.array[i].value, sizeof(float));
                    cs101_master_write_YCdata(p_link_data->module_id, &yc_data);
                }
            }   
            if (SQ == 1)
            {
                for (i=0; i<number; i++)
                {
                    yc_data.addr = ((p_link_data->ASDU.info_data.C_13.SQ1.info_addr[1] << 8) | \
                                    p_link_data->ASDU.info_data.C_13.SQ1.info_addr[0]) + i;
                    memcpy(&yc_data.value, p_link_data->ASDU.info_data.C_13.SQ1.array[i].value, sizeof(float));
                    cs101_master_write_YCdata(p_link_data->module_id, &yc_data);
                }
            }
            break;
        default:
            break;
    }

    /* 传送原因解析 */
    switch (p_link_data->ASDU.COT.cot[0])
    {
        
        case CS101_MASTER_COT_PER_CYC: break;    /* 周期循环 */
        case CS101_MASTER_COT_BACK: break;       /* 背景扫描 */
        case CS101_MASTER_COT_SPONT: break;      /* 突发，自发 */
        case CS101_MASTER_COT_REQ: break;        /* 请求或被请求*/
        case CS101_MASTER_COT_INTROGEN: break;   /* 响应站召唤 */
        default: break;
    }
    if (p_link_data->mode_selection == CS101_MASTER_BALANCE_MODE)     //平衡模式
    {
        p_link_data->pack_flag |= CS101_M_PACK_LINK_ACCEPT_FLAG;   /* 发送认可 */
    }
}

/**
  * @brief : 控制域组包
  * @param : [contior]- 控制域
  * @param : [FCB]- FCB位状态
  * @return: [contior]- 返回控制域
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static uint8_t cs101_master_conitor_pack(CS101_MASTER_LINK_t *p_link_data, uint8_t contior)
{
    uint8_t flag = 0;

    flag |= contior;
    if (contior & CS101_MASTER_CONTROL_PRM)
    {
        if (((p_link_data->mode_selection == CS101_MASTER_BALANCE_MODE) && ((contior & 0x0f) == CS101_MASTER_CONTROL_M_FC_3)) || \
            ((p_link_data->mode_selection == CS101_MASTER_NOBALANCE_MODE) && (((contior & 0x0f) == CS101_MASTER_CONTROL_M_FC_3) || \
            ((contior & 0x0f) == CS101_MASTER_CONTROL_M_FC_10) || ((contior & 0x0f) == CS101_MASTER_CONTROL_M_FC_11))))
        {
            if (p_link_data->masterFCB & CS101_MASTER_CONTROL_FCB)      /* FCB翻转 */
            {
                p_link_data->masterFCB &= ~CS101_MASTER_CONTROL_FCB;
            }
            else
            {
                p_link_data->masterFCB |= CS101_MASTER_CONTROL_FCB;
            }
            flag |= CS101_MASTER_CONTROL_FCV;
            flag |= (p_link_data->masterFCB & CS101_MASTER_CONTROL_FCB);
        }
    }
    return flag;
}

/**
  * @brief : 固定帧组包
  * @param : [conitor]- 控制域
  * @param : [link_addr]- 链路地址
  * @param : [tx_frame]- 发送帧
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_fixed_pack(uint8_t control, CS101_MASTER_LINK_t *p_link_data)
{
    int i = 0;
    int j = 0;

    p_link_data->tx_frame.buff[i++] = CS101_MASTER_FIXED_START;                                      //启动字符
    p_link_data->tx_frame.buff[i++] = cs101_master_conitor_pack(p_link_data, control);               //控制域
    for (j=0; j<p_link_data->link_addr.len; j++)                                        //地址域
    {
        p_link_data->tx_frame.buff[i++] = p_link_data->link_addr.addr[j];
    }
    p_link_data->tx_frame.buff[i++] = cs101_master_check_sun_cal(p_link_data->tx_frame.buff, p_link_data->link_addr.len);    //校验和
    p_link_data->tx_frame.buff[i++] = CS101_MASTER_FRAME_FINISH;                                     //结束字符
    p_link_data->tx_frame.len = i;
}
/**
  * @brief : 固定帧从动站数据打包发送 PRM = 0
  * @param : [conitor]- 控制域
  * @param : [link_addr]- 链路地址
  * @param : [tx_frame]- 发送帧
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_fixed_slave_pack(uint8_t FC, CS101_MASTER_LINK_t *p_link_data)
{
    uint8_t flag = 0;

    flag = FC & 0x0f;
    cs101_master_fixed_pack(flag, p_link_data);
}

/**
  * @brief : 固定帧启动站数据打包发送 PRM = 1
  * @param : [conitor]- 控制域
  * @param : [link_addr]- 链路地址
  * @param : [tx_frame]- 发送帧
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_fixed_start_pack(uint8_t FC, CS101_MASTER_LINK_t *p_link_data)
{
    uint8_t flag = 0;

    flag = FC & 0x0f;
    flag |= CS101_MASTER_CONTROL_PRM;
    cs101_master_fixed_pack(flag, p_link_data);
    p_link_data->link_flag |= CS101_MASTER_LINK_WAIT_ACK_FLAG;             //启动站发送，等待应答
}

/**
  * @brief : 可变帧组包
  * @param : [p_master_101_data]- 101主站数据
  * @param : [conitor]- 控制域
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_variable_pack(uint8_t conitor, CS101_MASTER_LINK_t *p_link_data)
{
    int i = 0, j = 0;

    p_link_data->tx_frame.buff[i++] = CS101_MASTER_VARIABLE_START;                                       //启动字符
    p_link_data->tx_frame.buff[i++] = 1 + p_link_data->link_addr.len + 2 + p_link_data->ASDU.COT.len + \
        p_link_data->ASDU.ASDU_addr.len + p_link_data->ASDU.info_len;                       //报文长度
    p_link_data->tx_frame.buff[i++] = 1 + p_link_data->link_addr.len + 2 + p_link_data->ASDU.COT.len + \
        p_link_data->ASDU.ASDU_addr.len + p_link_data->ASDU.info_len;                       //报文长度
    p_link_data->tx_frame.buff[i++] = CS101_MASTER_VARIABLE_START;                                       //启动字符
    p_link_data->tx_frame.buff[i++] = cs101_master_conitor_pack(p_link_data, conitor);                   //控制域
    for (j=0; j<p_link_data->link_addr.len; j++)                                            //地址域
    {
        p_link_data->tx_frame.buff[i++] = p_link_data->link_addr.addr[j];
    }
    p_link_data->tx_frame.buff[i++] = p_link_data->ASDU.TI;                                 //ASDU类型标识
    p_link_data->tx_frame.buff[i++] = p_link_data->ASDU.VSQ;                                //可变帧长限定词
    for (j=0; j<p_link_data->ASDU.COT.len; j++)                                             //传送原因
    {
        p_link_data->tx_frame.buff[i++] = p_link_data->ASDU.COT.cot[j];
    }
    for (j=0; j<p_link_data->ASDU.ASDU_addr.len; j++)                                       //ASDU地址
    {
        p_link_data->tx_frame.buff[i++] = p_link_data->ASDU.ASDU_addr.addr[j];
    }
    memcpy(p_link_data->tx_frame.buff + i, p_link_data->ASDU.info_data.info_buff, p_link_data->ASDU.info_len);   //信息体对象
    i += p_link_data->ASDU.info_len;
    p_link_data->tx_frame.buff[i++] = cs101_master_check_sun_cal(p_link_data->tx_frame.buff, p_link_data->link_addr.len);    //校验和
    p_link_data->tx_frame.buff[i++] = CS101_MASTER_FRAME_FINISH;                                         //结束字符
    p_link_data->tx_frame.len = i;
}

/**
  * @brief : 可变帧启动站组包
  * @param : [p_master_101_data]- 101主站数据
  * @param : [conitor]- 控制域
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_variable_start_pack(uint8_t conitor, CS101_MASTER_LINK_t *p_link_data)
{
    uint8_t flag = 0;

    flag = conitor & 0x0f;
    flag |= CS101_MASTER_CONTROL_PRM;
    cs101_master_variable_pack(flag, p_link_data);
    p_link_data->link_flag |= CS101_MASTER_LINK_WAIT_ACK_FLAG;             //启动站发送，等待应答
}

/**
  * @brief : 固定帧解析
  * @param : [p_master_101_data]- 101主站数据指针
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_fixed_analysis(CS101_MASTER_LINK_t *p_link_data)
{
    uint8_t conitor = 0;
    conitor = p_link_data->rx_frame.buff[1];                                        /* 控制域 */

    if (((p_link_data->mode_selection == CS101_MASTER_BALANCE_MODE) && (conitor & CS101_MASTER_CONTROL_DIR)) || \
        (p_link_data->mode_selection == CS101_MASTER_NOBALANCE_MODE))                            /* 平衡模式终端发出的上行报文  非平衡 */
    {
        if (conitor & CS101_MASTER_CONTROL_PRM)                                                  /* 该帧来自启动站 */
        {
            switch (conitor & 0x0F)
            {
                case CS101_MASTER_CONTROL_M_FC_9:                                                /* 请求链路 */
                    p_link_data->pack_flag |= CS101_M_PACK_LINK_STATE_FLAG;          /* 发送链路状态 */
                    break;
                case CS101_MASTER_CONTROL_M_FC_0:                                                /* 复位链路 */
                    p_link_data->pack_flag |= CS101_M_PACK_LINK_ACCEPT_FLAG;   /* 发送认可 */
                    p_link_data->pack_flag |= CS101_M_PACK_ALWAYS_CALLED_FLAG;      /* 总召 */
                    break;
                default:
                    break;
            }
        }
        else                                                                        /* 该帧来自从动站 */
        {
            switch (conitor & 0x0F)
            {
                case CS101_MASTER_CONTROL_S_FC_0:                                                /* 认可 */
                    if (p_link_data->link_flag & CS101_MASTER_LINK_RESET_FLAG)                   /* 复位链路 */
                    {
                        p_link_data->link_flag &= ~CS101_MASTER_LINK_RESET_FLAG;
                        p_link_data->link_flag |= CS101_MASTER_LINK_ESTABLISH_FLAG;              /* 链路建立 */
                        if (p_link_data->mode_selection == CS101_MASTER_NOBALANCE_MODE)              //非平衡
                        {
                            p_link_data->pack_flag |= CS101_M_PACK_ALWAYS_CALLED_FLAG;      /* 总召 */
                        }
                    }
                    if (p_link_data->mode_selection == CS101_MASTER_NOBALANCE_MODE)              //非平衡
                    {
                        if (p_link_data->link_flag & CS101_MASTER_CLOCK_SYNCH_CLOCK_FLAG)        /* 时钟同步*/
                        {
                            p_link_data->pack_flag |= CS101_M_PACK_SECOND_DATA_FLAG;             /* 访问二级数据 */
                            p_link_data->link_flag &= ~CS101_MASTER_CLOCK_SYNCH_CLOCK_FLAG;
                        }
                    }
                    p_link_data->link_flag &= ~CS101_MASTER_LINK_WAIT_ACK_FLAG;
                    break;
                case CS101_MASTER_CONTROL_S_FC_1:                                                /* 否定认可 */
                    if (p_link_data->link_flag & CS101_MASTER_LINK_RESET_FLAG)                   /* 复位链路 */
                    {
                        p_link_data->link_flag &= ~CS101_MASTER_LINK_RESET_FLAG;
                    }
                    p_link_data->link_flag &= ~CS101_MASTER_LINK_WAIT_ACK_FLAG;
                    break;
                case CS101_MASTER_CONTROL_S_FC_9:                                                /* 无所请求的用户数据 */
                    if (p_link_data->mode_selection == CS101_MASTER_NOBALANCE_MODE)              //非平衡
                    {
                        p_link_data->link_flag &= ~CS101_MASTER_LINK_WAIT_ACK_FLAG;
                    }
                    break;
                case CS101_MASTER_CONTROL_S_FC_11:                                               /* 链路状态 */
                    if (p_link_data->link_flag & CS101_MASTER_LINK_REQUEST_FLAG)                 /* 请求链路 */
                    {
                        p_link_data->link_flag &= ~CS101_MASTER_LINK_REQUEST_FLAG;
                        p_link_data->pack_flag |= CS101_M_PACK_RESET_LINK_FLAG;
                    }
                    p_link_data->link_flag &= ~CS101_MASTER_LINK_WAIT_ACK_FLAG;
                    break;
                default:
                    break;
            }
            if (p_link_data->mode_selection == CS101_MASTER_NOBALANCE_MODE)
            {
                if (conitor & CS101_MASTER_CONTROL_ACD)
                {
                    p_link_data->pack_flag |= CS101_M_PACK_STAIR_DATA_FLAG;                      /* 访问一级数据 */
                }
            }
        }
    } 
}

/**
  * @brief : 可变帧解析
  * @param : [p_master_101_data]- 101主站数据指针
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_variable_analysis(CS101_MASTER_LINK_t *p_link_data)
{
    int i = 0;
    int j = 0;
    int len = 0;
    int conitor = 0;    

    if (p_link_data->link_flag & CS101_MASTER_LINK_ESTABLISH_FLAG)          /* 链路建立 */
    {
        i++;                                                                //启动字符
        len = p_link_data->rx_frame.buff[i++];                               //报文长度
        i++;                                                                //报文长度
        i++;                                                                //启动字符
        conitor = p_link_data->rx_frame.buff[i++];                           //控制域
        for (j=0; j<p_link_data->link_addr.len; j++)                         //地址域
        {
            i++;
        }
        p_link_data->ASDU.TI = p_link_data->rx_frame.buff[i++];               //ASDU类型标识
        p_link_data->ASDU.VSQ = p_link_data->rx_frame.buff[i++];              //可变帧长限定词
        for (j=0; j<p_link_data->ASDU.COT.len; j++)                          //传送原因
        {
            p_link_data->ASDU.COT.cot[j] = p_link_data->rx_frame.buff[i++];
        }
        for (j=0; j<p_link_data->ASDU.ASDU_addr.len; j++)                    //ASDU地址
        {
            p_link_data->ASDU.ASDU_addr.addr[j] = p_link_data->rx_frame.buff[i++];
        }
        p_link_data->ASDU.info_len = len - (1 + p_link_data->link_addr.len + 2 + \
            p_link_data->ASDU.COT.len +  p_link_data->ASDU.ASDU_addr.len);
        memcpy(p_link_data->ASDU.info_data.info_buff, p_link_data->rx_frame.buff + i, p_link_data->ASDU.info_len);   //信息体对象
               
        switch (p_link_data->ASDU.TI)
        {
            
            case CS101_MASTER_TI_M_SP_NA_1:                      /* 单点信息 */
            case CS101_MASTER_TI_M_DP_NA_1:                      /* 双点信息 */
                cs101_master_YX_ASDU_analy(p_link_data);
                break;
            case CS101_MASTER_TI_M_ME_NA_1:                      /* 测量值，归一化值 */
            case CS101_MASTER_TI_M_ME_NB_1:                      /* 测量值，标度化值 */
            case CS101_MASTER_TI_M_ME_NC_1:                      /* 测量值，短浮点数 */
                cs101_master_YC_ASDU_analy(p_link_data);
                break;
            case CS101_MASTER_TI_M_SP_TB_1:                      /* 带时标的单点信息 */
            case CS101_MASTER_TI_M_DP_TB_1:                      /* 带时标的双点信息 */
                cs101_master_SOE_ASDU_analy(p_link_data);
                break;
            case CS101_MASTER_TI_M_FT_NA_1:                      /* 故障事件信息 */
                break;
            case CS101_MASTER_TI_M_IT_NB_1:                      /* 累计量，短浮点数 */
                break;
            case CS101_MASTER_TI_M_IT_TC_1:                      /* 带时标的累计量 */
                break;
            case CS101_MASTER_TI_C_SC_NA_1:                      /* 单点命令 */
            case CS101_MASTER_TI_C_SC_NB_1:                      /* 双点命令 */
                cs101_master_YK_ASDU_analy(p_link_data);
                break;
            case CS101_MASTER_TI_M_EI_NA_1:                      /* 初始化结束 */
                if (p_link_data->mode_selection == CS101_MASTER_BALANCE_MODE)           //平衡
                {
                    p_link_data->pack_flag |= CS101_M_PACK_LINK_ACCEPT_FLAG;   /* 发送认可 */
                }
                break;
            case CS101_MASTER_TI_C_IC_NA_1:                      /* 站总召唤命令 */
                cs101_master_always_ASDU_analy(p_link_data);
                break;
            case CS101_MASTER_TI_C_CI_NA_1:                      /* 电能量召唤命令 */
                break;
            case CS101_MASTER_TI_C_CS_NA_1:                      /* 时钟同步命令 */
                cs101_master_clock_synch_ASDU_analy(p_link_data);
                break;
            case CS101_MASTER_TI_C_TS_NA_1:                      /* 测试命令 */
                break;
            case CS101_MASTER_TI_C_RP_NA_1:                      /* 复位进程命令 */
                cs101_master_reset_ASDU_analy(p_link_data);
                break;
            case CS101_MASTER_TI_C_SR_NA_1:                      /* 切换定值区号 */
                break;
            case CS101_MASTER_TI_C_RR_NA_1:                      /* 读定值区号 */
                break;
            case CS101_MASTER_TI_C_RS_NA_1:                      /* 读参数和定值 */
                break;
            case CS101_MASTER_TI_C_WS_NA_1:                      /* 写参数和定值 */
                break;
            case CS101_MASTER_TI_F_FR_NA_1:                      /* 文件传输 */
                break;
            case CS101_MASTER_TI_F_SR_NA_1:                      /* 软件升级 */
                break;
            default:
                break;
        }
        if (p_link_data->mode_selection == CS101_MASTER_NOBALANCE_MODE)           //非平衡
        {
            p_link_data->link_flag &= ~CS101_MASTER_LINK_WAIT_ACK_FLAG;
            if (conitor & CS101_MASTER_CONTROL_ACD)
            {
                p_link_data->pack_flag |= CS101_M_PACK_STAIR_DATA_FLAG;                      /* 访问一级数据 */
            }
        }
    }
}

/**
  * @brief : 请求链路
  * @param : [p_master_101_data]- 101主站数据指针
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_request_link(CS101_MASTER_LINK_t *p_link_data)
{
    uint32_t link_addr = 0;
    int j = 0;
    /* 复位 */
    p_link_data->link_flag = 0; 
    p_link_data->pack_flag = 0;
    p_link_data->masterFCB = 0;
    p_link_data->retry_num_con = 0;
    p_link_data->subsiteFCB = CS101_MASTER_CONTROL_FCB_FIRST;
    
    for (j=0; j<p_link_data->link_addr.len; j++)
    {
        link_addr |= p_link_data->link_addr.addr[j] << j * 8;
    }
//    log_i("请求链路，串口号%d，链路地址%d", p_link_data->port_number, link_addr);
    if (!(p_link_data->link_flag & CS101_MASTER_LINK_ESTABLISH_FLAG)) 	//链路建立
    {
        p_link_data->pack_flag |= CS101_M_PACK_REQUEST_LINK_FLAG;		//请求链路
    }
}

/**
  * @brief : 遥控状态发送
  * @param : [p_master_101_data]- 101主站数据指针
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
void cs101_master_YK_send(CS101_MASTER_LINK_t *p_link_data, uint16_t yk_sta)
{
    struct snode *p_temp_node_data = NULL;
    struct tagControlCfg info;
    struct tagControlRemoteCfg cmd;

    memset(&info, 0, sizeof(info));
    info.pdata = &cmd;
    info.com.tpye = s_pTagOverall->def.type_REMOTE;
    info.com.dir = s_pTagOverall->def.message_UP;
    info.com.reply = p_link_data->module_id;

    p_temp_node_data = s_appdata_101master_List.head;
    while (p_temp_node_data != NULL)
    {
        if (((CS101_MASTER_APPDATA_t*)(p_temp_node_data->data))->module_id == p_link_data->module_id)
        {
            cmd.addr =  (((CS101_MASTER_APPDATA_t*)(p_temp_node_data->data))->p_YK_data + p_link_data->yk_data.yk_addr - CS101_MASTER_YK_START_ADDR)->addr;
            break;
        }
        p_temp_node_data = p_temp_node_data->next;
    }

    if (p_link_data->link_flag & CS101_MASTER_YK_OPEN_FLAG)
    {
        cmd.work = s_pTagOverall->def.control_OFF;
    }
	else if (p_link_data->link_flag & CS101_MASTER_YK_CLOSE_FLAG)
    {
        cmd.work = s_pTagOverall->def.control_ON;
    }
    
    if (p_link_data->link_flag & CS101_MASTER_YK_SELECT_FLAG)
    {
        cmd.cmd = s_pTagOverall->def.control_OPREAT_CHOICE;
    }
	else if (p_link_data->link_flag & CS101_MASTER_YK_EXECUTE_FLAG)
    {
        cmd.cmd = s_pTagOverall->def.control_OPREAT_EXECUTE;
    }
    else if (p_link_data->link_flag & CS101_MASTER_YK_FINAL_FLAG)
    {
        cmd.cmd = s_pTagOverall->def.control_OPREAT_CANCEL;
    }
    cmd.sta = yk_sta;

    s_pTagOverall->fun.operate_Control_info(&info);
}

/**
  * @brief : 遥控
  * @param : [p_master_101_data]- 101主站数据指针
  * @param : [yk_addr]- 遥控地址
  * @param : [single_double]- 单双点
  * @param : [YK_handle]- 预制，执行，终止
  * @param : [open_close]- 分合操作
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static int cs101_master_YK(CS101_MASTER_LINK_t *p_link_data, uint16_t yk_addr, uint16_t cmd, uint16_t work)
{
    if (p_link_data->link_flag & CS101_MASTER_LINK_ESTABLISH_FLAG) 
    {
        p_link_data->link_flag &= ~CS101_MASTER_YK_FLAG_BITS;   /* 清除遥控标志 */
        /* 单双点判断 */
        if (p_link_data->yk_data.yk_single_double == s_pTagOverall->def.Ti_M_DP_NA_1)
        {
            p_link_data->link_flag |= CS101_MASTER_YK_TWO_POINT_FLAG;
        }
        else
        {
            p_link_data->link_flag &= ~CS101_MASTER_YK_TWO_POINT_FLAG;
        }
        /* 分合操作判断 */
        if (work == s_pTagOverall->def.control_OFF)
        {
            p_link_data->link_flag |= CS101_MASTER_YK_OPEN_FLAG;
        }
        else if (work == s_pTagOverall->def.control_ON)
        {
            p_link_data->link_flag |= CS101_MASTER_YK_CLOSE_FLAG;
        }
        /* 选择执行判断 */
        if (cmd == s_pTagOverall->def.control_OPREAT_CHOICE)         /* 选择 */
        {
            p_link_data->link_flag |= CS101_MASTER_YK_SELECT_FLAG;
        }
        else if (cmd == s_pTagOverall->def.control_OPREAT_EXECUTE)   /* 执行 */
        {
            p_link_data->link_flag |= CS101_MASTER_YK_EXECUTE_FLAG;
        }
        else if (cmd == s_pTagOverall->def.control_OPREAT_CANCEL)    /* 终止 */
        {
            p_link_data->link_flag |= CS101_MASTER_YK_FINAL_FLAG;
        }
        /* 遥控地址传递 */
        int i = 0;
        while (i < CS101_MASTER_INFO_ADDR_LEN)
        {
        	p_link_data->yk_data.yk_addr = yk_addr;
        	i++;
        }

        p_link_data->pack_flag |= CS101_M_PACK_YK_HANDLE_FLAG;      /* 遥控 */
        cs101_master_YK_send(p_link_data, s_pTagOverall->def.control_RES_WAIT);
        return s_pTagOverall->def.control_RES_WAIT;
    }
    else
    {
        p_link_data->link_flag &= ~CS101_MASTER_YK_FLAG_BITS;   /* 清除遥控标志 */
        cs101_master_YK_send(p_link_data, s_pTagOverall->def.control_RES_FAIL);
        return s_pTagOverall->def.control_RES_FAIL;
    }
}

/**
  * @brief : 遥控处理
  * @param : [p_master_101_data]- 101主站数据指针
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_YK_treating(CS101_MASTER_LINK_t *p_link_data)
{
    uint32_t link_addr = 0;
    int i = 0;
    for (i=0; i<p_link_data->link_addr.len; i++)
    {
        link_addr |= p_link_data->link_addr.addr[i] << i * 8;
    }

    if (p_link_data->link_flag & CS101_MASTER_YK_SELECT_FLAG)            /* 遥控选择 */
    {
        p_link_data->link_flag |= CS101_MASTER_YK_ING_FLAG;              /* 遥控中 */
        if (p_link_data->link_flag & CS101_MASTER_YK_ACT_AFFIRM_FLAG)    /* 确认 */
        {
            /* 发送遥控状态 */
//            log_i("串口号%d，链路地址%d，遥控选择成功", p_link_data->port_number, link_addr);
            cs101_master_YK_send(p_link_data, s_pTagOverall->def.control_RES_SUCCEED);
            p_link_data->link_flag &= ~CS101_MASTER_YK_SELECT_FLAG;
            p_link_data->link_flag &= ~CS101_MASTER_YK_ACT_AFFIRM_FLAG;
        }
    }
    if (p_link_data->link_flag & CS101_MASTER_YK_EXECUTE_FLAG)           /* 遥控执行 */
    {
        if (p_link_data->link_flag & CS101_MASTER_YK_ACT_AFFIRM_FLAG)    /* 确认 */
        {
//            log_i("串口号%d，链路地址%d，遥控执行成功", p_link_data->port_number, link_addr);
            cs101_master_YK_send(p_link_data, s_pTagOverall->def.control_RES_SUCCEED);
            p_link_data->link_flag &= ~CS101_MASTER_YK_FLAG_BITS;
        }
    }
    if (p_link_data->link_flag & CS101_MASTER_YK_FINAL_FLAG)             /* 遥控终止 */
    {
        if (p_link_data->link_flag & CS101_MASTER_YK_FINAL_AFFIRM_FLAG)  /* 确认 */
        {
//            log_i("串口号%d，链路地址%d，遥控终止成功", p_link_data->port_number, link_addr);
            cs101_master_YK_send(p_link_data, s_pTagOverall->def.control_RES_SUCCEED);
            p_link_data->link_flag &= ~CS101_MASTER_YK_FLAG_BITS;
        }
    }
    if (p_link_data->link_flag & CS101_MASTER_YK_ERROR_FLAG)             /* 遥控错误 */
    {
//        log_i("串口号%d，链路地址%d，遥控错误", p_link_data->port_number, link_addr);
        cs101_master_YK_send(p_link_data, s_pTagOverall->def.control_RES_FAIL);
        p_link_data->link_flag &= ~CS101_MASTER_YK_FLAG_BITS;
    }
}

/**
  * @brief : 搜索101报文，固定帧和可变帧
  * @param : [p_master_101_data]- 101主站数据指针
  * @return: []- -1：未搜索到报文，0：报文不完整，1：接收到报文
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static int cs101_master_search_message(CS101_MASTER_PHYSICS_t *p_physics_data, CS101_MASTER_LINK_t *p_link_data)
{
    while (p_physics_data->rx_queue.seek_out != p_physics_data->rx_queue.in)
    {
        switch (p_physics_data->search_flag)
        {
            case CS101_M_SEARTCH_START:                             /* 检测报文头 */
                if (p_link_data->rx_frame.len != 0)                   /* 搜索前清空缓冲区 */
                {
                    memset(p_link_data->rx_frame.buff, 0, p_link_data->rx_frame.len);
                    p_link_data->rx_frame.len = 0;
                }
                if ((p_physics_data->rx_queue.buff[p_physics_data->rx_queue.seek_out] == CS101_MASTER_FIXED_START) || \
                    (p_physics_data->rx_queue.buff[p_physics_data->rx_queue.seek_out] == CS101_MASTER_VARIABLE_START))
                {
                    p_link_data->rx_frame.buff[p_link_data->rx_frame.len] = p_physics_data->rx_queue.buff[p_physics_data->rx_queue.seek_out];	//报头从队列中取出放到缓存区
                    p_link_data->rx_frame.len++;				//偏移到第二字节
                    p_physics_data->rx_queue.out = p_physics_data->rx_queue.seek_out + 1;	//偏移到第二字节
                    if (p_physics_data->rx_queue.out >= CS101_MASTER_RX_QUEUE_LEN)
                    {
                        p_physics_data->rx_queue.out = 0;
                    }
                    p_physics_data->search_flag = CS101_M_RECEIVE_MESS;
                    break;
                } 
                break;
            case CS101_M_RECEIVE_MESS:                              /* 接收后续报文 */
                p_link_data->rx_frame.buff[p_link_data->rx_frame.len] = p_physics_data->rx_queue.buff[p_physics_data->rx_queue.seek_out];
                p_link_data->rx_frame.len++;
                if (((p_link_data->rx_frame.buff[0] == CS101_MASTER_FIXED_START) && (p_link_data->rx_frame.len == (4 + p_link_data->link_addr.len))) ||\
                    ((p_link_data->rx_frame.buff[0] == CS101_MASTER_VARIABLE_START) && (p_link_data->rx_frame.len == (p_link_data->rx_frame.buff[1] + 6))))
                {
                    p_physics_data->search_flag = CS101_M_FRAME_VERIFY;
                    break;
                }
                break;
            default:
                break;
        }
        p_physics_data->rx_queue.seek_out++;             /* 搜索尾指针移动 */
        if (p_physics_data->rx_queue.seek_out >= CS101_MASTER_RX_QUEUE_LEN)
        {
            p_physics_data->rx_queue.seek_out = 0;
        }
        if (p_physics_data->search_flag == CS101_M_FRAME_VERIFY) /* 帧校验 */
        {
            if (cs101_master_message_verify(p_link_data->rx_frame.buff, &p_link_data->link_addr) == 0)
            {
                p_physics_data->rx_queue.out = p_physics_data->rx_queue.seek_out;
                p_physics_data->search_flag = CS101_M_SEARTCH_START;
                return 1;                                   /* 校验成功，进行解析 */
            }
            else                                            /* 校验失败，重新搜索 */
            {
                p_physics_data->rx_queue.seek_out = p_physics_data->rx_queue.out;
                p_physics_data->search_flag = CS101_M_SEARTCH_START;
            }
        }
    }
    /* 接收报文超时处理 */
    if (p_physics_data->search_flag == CS101_M_RECEIVE_MESS)
    {
        p_physics_data->seek_count++;
        if (p_physics_data->seek_count >= p_physics_data->seek_time)
        {
            p_physics_data->search_flag = CS101_M_SEARTCH_START;
            p_physics_data->seek_count = 0;
            p_physics_data->rx_queue.out = p_physics_data->rx_queue.in;
            p_physics_data->rx_queue.seek_out = p_physics_data->rx_queue.out;
        }
    }
    else
    {
        p_physics_data->seek_count = 0;
    }
    /* 返回值处理 */
    if (p_physics_data->search_flag == CS101_M_RECEIVE_MESS)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/**
  * @brief : FCB翻转判断
  * @param : [p_master_101_data]- 101主站数据结构指针
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static int cs101_master_FCBflip_judgment(uint8_t conitor, uint8_t *subsiteFCB)
{
    if (conitor & CS101_MASTER_CONTROL_PRM)  /* 启动站方向 */
    {
        if (conitor & CS101_MASTER_CONTROL_FCV)
        {
            if (*subsiteFCB & CS101_MASTER_CONTROL_FCB_FIRST)    //FCB位第一次
            {
                *subsiteFCB = conitor & CS101_MASTER_CONTROL_FCB;
            }
            else
            {
                if (*subsiteFCB & CS101_MASTER_CONTROL_FCB)      /* FCB翻转 */
                {
                    *subsiteFCB &= ~CS101_MASTER_CONTROL_FCB;
                }
                else
                {
                    *subsiteFCB |= CS101_MASTER_CONTROL_FCB;
                }
                if ((conitor & CS101_MASTER_CONTROL_FCB) != *subsiteFCB)
                {
                    return -1;
                }
            }  
        }
    }
    return 0;
}

/**
  * @brief : 报文解析
  * @param : [p_master_101_data]- 101主站数据结构指针
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static int cs101_master_message_analysis(CS101_MASTER_LINK_t *p_link_data)
{
    if (p_link_data->rx_frame.buff[0] == CS101_MASTER_FIXED_START)             /* 固定帧解析 */
    {
        if (cs101_master_FCBflip_judgment(p_link_data->rx_frame.buff[1], &p_link_data->subsiteFCB) == -1)
        {
            return -1;
        }
        cs101_master_fixed_analysis(p_link_data);
    }
    else if (p_link_data->rx_frame.buff[0] == CS101_MASTER_VARIABLE_START)     /* 可变帧解析 */
    {
        if (cs101_master_FCBflip_judgment(p_link_data->rx_frame.buff[4], &p_link_data->subsiteFCB) == -1)
        {
            return -1;
        }
        cs101_master_variable_analysis(p_link_data);
    }
    return 0;
}

/**
  * @brief : 数据处理
  * @param : [p_master_101_data]- 101主站数据结构指针
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_data_treating(CS101_MASTER_LINK_t *p_link_data)
{
    uint32_t link_addr = 0;
    int i = 0;
    for (i=0; i<p_link_data->link_addr.len; i++)
    {
        link_addr |= p_link_data->link_addr.addr[i] << i * 8;	//链路地址
    }

    if (p_link_data->link_flag & CS101_MASTER_LINK_ESTABLISH_FLAG)                                       /* 链路建立 */
    {
    	//写通讯状态  soe
        if (cs101_master_write_comm_state(p_link_data->module_id, s_pTagOverall->def.switch_ON, &p_link_data->time_101.CP56Time2a) == 0)
        {
//            log_i("串口号%d，链路地址%d，连接成功", p_link_data->port_number, link_addr);
        }
        if (!(p_link_data->link_flag & CS101_MASTER_LINK_WAIT_ACK_FLAG))
        {
            /* 平衡模式心跳测试 */
            if (p_link_data->mode_selection == CS101_MASTER_BALANCE_MODE)                               /* 平衡模式 */
            {
                cs101_master_timer_start(&p_link_data->heartbeat_time);                                 /* 启动定时 */
                if (cs101_master_timer_compare(&p_link_data->heartbeat_time) == 1)
                {
                    cs101_master_timer_stop(&p_link_data->heartbeat_time);                              /* 停止定时 */
                    p_link_data->pack_flag |= CS101_M_PACK_LINK_TEST_FLAG;                               /* 心跳测试 */
                }
            }
            /* 非平衡模式访问二级数据 */
            else if (p_link_data->mode_selection == CS101_MASTER_NOBALANCE_MODE)                        /* 非平衡模式 */
            {
                cs101_master_timer_start(&p_link_data->call_data_time); 
                if (cs101_master_timer_compare(&p_link_data->call_data_time) == 1)
                {
                    cs101_master_timer_stop(&p_link_data->call_data_time); 
                    p_link_data->pack_flag |= CS101_M_PACK_SECOND_DATA_FLAG;                            /* 访问二级数据 */
                }
            }
        }
        else
        {
            cs101_master_timer_stop(&p_link_data->heartbeat_time);      
            cs101_master_timer_stop(&p_link_data->call_data_time);                                        /* 停止定时 */
        }
        /* 循环总召 */
        cs101_master_timer_start(&p_link_data->all_call_time);                                 /* 启动定时 */
        if (cs101_master_timer_compare(&p_link_data->all_call_time) == 1)
        {
            cs101_master_timer_stop(&p_link_data->all_call_time);                              /* 停止定时 */
            p_link_data->pack_flag |= CS101_M_PACK_ALWAYS_CALLED_FLAG;                               /* 总召唤 */
        }
        cs101_master_YK_treating(p_link_data);                                                /* 遥控数据处理 */
    }
    else 
    {
    	//写通讯状态  soe
        if (cs101_master_write_comm_state(p_link_data->module_id, s_pTagOverall->def.switch_OFF, &p_link_data->time_101.CP56Time2a) == 0)
        {
//            log_i("串口号%d，链路地址%d，断开连接", p_link_data->port_number, link_addr);
        }
    }
}

/**
  * @brief : 数据发送前组包
  * @param : [p_master_101_data]- 101主站数据结构指针
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_data_pack(CS101_MASTER_LINK_t *p_link_data)
{
    if (!(p_link_data->link_flag & CS101_MASTER_LINK_WAIT_ACK_FLAG))                        	/* 链路空闲 */
    {
        if (p_link_data->pack_flag & CS101_M_PACK_LINK_ACCEPT_FLAG)                          	/* 认可 */
        {
            p_link_data->pack_flag &= ~CS101_M_PACK_LINK_ACCEPT_FLAG;
            cs101_master_fixed_slave_pack(CS101_MASTER_CONTROL_S_FC_0, p_link_data);
        }
        else if (p_link_data->pack_flag & CS101_M_PACK_REQUEST_LINK_FLAG)                       /* 请求链路 */
        {
            p_link_data->pack_flag &= ~CS101_M_PACK_REQUEST_LINK_FLAG;
            cs101_master_fixed_start_pack(CS101_MASTER_CONTROL_M_FC_9, p_link_data);
            p_link_data->link_flag |= CS101_MASTER_LINK_REQUEST_FLAG;                           /* 请求链路 */
        }
        else if (p_link_data->pack_flag & CS101_M_PACK_RESET_LINK_FLAG)                         /* 复位链路 */
        {
            p_link_data->pack_flag &= ~CS101_M_PACK_RESET_LINK_FLAG;
            cs101_master_fixed_start_pack(CS101_MASTER_CONTROL_M_FC_0, p_link_data);
            p_link_data->link_flag |= CS101_MASTER_LINK_RESET_FLAG;                             /* 复位链路 */                         /* 请求链路 */
        }
        else if (p_link_data->pack_flag & CS101_M_PACK_STAIR_DATA_FLAG)                         /* 访问一级数据 */
        {
            p_link_data->pack_flag &= ~CS101_M_PACK_STAIR_DATA_FLAG;
            cs101_master_fixed_start_pack(CS101_MASTER_CONTROL_M_FC_10, p_link_data);
        }
        else if (p_link_data->pack_flag & CS101_M_PACK_LINK_STATE_FLAG)                         /* 链路状态 */
        {
            p_link_data->pack_flag &= ~CS101_M_PACK_LINK_STATE_FLAG;
            cs101_master_fixed_slave_pack(CS101_MASTER_CONTROL_S_FC_11, p_link_data);
        }
        else if (p_link_data->pack_flag & CS101_M_PACK_CLOCK_SYNC_FLAG)                         /* 时钟同步 */
        {
            p_link_data->pack_flag &= ~CS101_M_PACK_CLOCK_SYNC_FLAG;
            cs101_master_clock_synch_ASDU_pack(&p_link_data->ASDU, &p_link_data->time_101.CP56Time2a, CS101_MASTER_COT_ACT);
            cs101_master_variable_start_pack(CS101_MASTER_CONTROL_M_FC_3, p_link_data);
            if (p_link_data->mode_selection == CS101_MASTER_NOBALANCE_MODE)   //非平衡
            {
                p_link_data->link_flag |= CS101_MASTER_CLOCK_SYNCH_CLOCK_FLAG;
            }
        }
        else if (p_link_data->pack_flag & CS101_M_PACK_CLOCK_READ_FLAG)                         /* 时钟读取 */
        {
            p_link_data->pack_flag &= ~CS101_M_PACK_CLOCK_READ_FLAG;
            cs101_master_clock_synch_ASDU_pack(&p_link_data->ASDU, &p_link_data->time_101.CP56Time2a, CS101_MASTER_COT_REQ);
            cs101_master_variable_start_pack(CS101_MASTER_CONTROL_M_FC_3, p_link_data);
            if (p_link_data->mode_selection == CS101_MASTER_NOBALANCE_MODE)   //非平衡
            {
                p_link_data->link_flag |= CS101_MASTER_CLOCK_SYNCH_CLOCK_FLAG;
            }
        }
        else if (p_link_data->pack_flag & CS101_M_PACK_RESET_COURSE_FLAG)                        /* 复位进程 */
        {
            p_link_data->pack_flag &= ~CS101_M_PACK_RESET_COURSE_FLAG;
            cs101_master_reset_ASDU_pack(&p_link_data->ASDU);
            cs101_master_variable_start_pack(CS101_MASTER_CONTROL_M_FC_3, p_link_data);
        }
        else if (p_link_data->pack_flag & CS101_M_PACK_ALWAYS_CALLED_FLAG)                       /* 总召唤 */
        {
            p_link_data->pack_flag &= ~CS101_M_PACK_ALWAYS_CALLED_FLAG;
            cs101_master_always_ASDU_pack(&p_link_data->ASDU);
            cs101_master_variable_start_pack(CS101_MASTER_CONTROL_M_FC_3, p_link_data);
        }
        else if (p_link_data->pack_flag & CS101_M_PACK_YK_HANDLE_FLAG)                           /* 遥控操作 */
        {
            p_link_data->pack_flag &= ~CS101_M_PACK_YK_HANDLE_FLAG;
            cs101_master_YK_ASDU_pack(p_link_data);
            cs101_master_variable_start_pack(CS101_MASTER_CONTROL_M_FC_3, p_link_data);
        }
        else if (p_link_data->pack_flag & CS101_M_PACK_SECOND_DATA_FLAG)                        /* 访问二级数据 */
        {
            p_link_data->pack_flag &= ~CS101_M_PACK_SECOND_DATA_FLAG;
            cs101_master_fixed_start_pack(CS101_MASTER_CONTROL_M_FC_11, p_link_data);
        }
        else if (p_link_data->pack_flag & CS101_M_PACK_LINK_TEST_FLAG)                          /* 链路测试 */
        {
            p_link_data->pack_flag &= ~CS101_M_PACK_LINK_TEST_FLAG;
            cs101_master_fixed_start_pack(CS101_MASTER_CONTROL_M_FC_2, p_link_data);
        }
    }
}

/**
  * @brief : 数据发送以及重发处理
  * @param : [p_master_101_data]- 101主站数据结构指针
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
 static void cs101_master_data_send(CS101_MASTER_PHYSICS_t *p_physics_data, CS101_MASTER_LINK_t *p_link_data)
{
    uint32_t link_addr = 0;
    int i = 0;
    if (p_link_data->tx_frame.len != 0)
    {
        cs101_master_write_data_port(p_physics_data->fd, p_link_data->tx_frame.buff, p_link_data->tx_frame.len);
        p_link_data->link_flag |= CS101_MASTER_LINK_WAIT_JUMP_FLAG;         /* 非平衡时使用，发送数据后需等待跳转 */
        /* 清空重发缓冲区 */
        memset(p_link_data->tx_re_frame.buff, 0, p_link_data->tx_re_frame.len);
        p_link_data->tx_re_frame.len = 0;
        /* 发送数据拷贝到重发缓冲区 */
        memcpy(p_link_data->tx_re_frame.buff, p_link_data->tx_frame.buff, p_link_data->tx_frame.len);
        p_link_data->tx_re_frame.len = p_link_data->tx_frame.len;
        /* 发送缓冲区清零 */
        memset(p_link_data->tx_frame.buff, 0, p_link_data->tx_frame.len);
        p_link_data->tx_frame.len = 0;
        cs101_master_timer_stop(&p_link_data->retry_time);                                        /* 停止定时 */
        cs101_master_timer_stop(&p_link_data->heartbeat_time);                                    /* 停止定时 */
        cs101_master_timer_stop(&p_link_data->call_data_time); 
        cs101_master_timer_stop(&p_link_data->await_time); 
    }
    /* 重发处理 */
    if (p_link_data->link_flag & CS101_MASTER_LINK_ESTABLISH_FLAG) 
    {
        if (p_link_data->link_flag & CS101_MASTER_LINK_WAIT_ACK_FLAG)
        {
            if (p_link_data->retry_num_con < p_link_data->retry_number)
            {
                cs101_master_timer_start(&p_link_data->retry_time);        /* 启动定时 */
                if (cs101_master_timer_compare(&p_link_data->retry_time) == 1)
                {
                    cs101_master_timer_stop(&p_link_data->retry_time);                                    /* 停止定时 */
                    cs101_master_write_data_port(p_physics_data->fd, p_link_data->tx_re_frame.buff, p_link_data->tx_re_frame.len);
                    p_link_data->link_flag |= CS101_MASTER_LINK_WAIT_JUMP_FLAG;         /* 非平衡时使用，发送数据后需等待跳转 */
                    p_link_data->retry_num_con++;
                    for (i=0; i<p_link_data->link_addr.len; i++)
                    {
                        link_addr |= p_link_data->link_addr.addr[i] << i * 8;
                    }
//                    log_i("串口号%d，链路地址%d，%d次重发", p_link_data->port_number, link_addr, p_link_data->retry_num_con);
                    cs101_master_timer_stop(&p_link_data->await_time); 
                }
            }
            else
            {
                cs101_master_timer_stop(&p_link_data->retry_time);                                        /* 停止定时 */
                p_link_data->retry_num_con = 0;
                p_link_data->link_flag = 0;                                                    //重新建立链路
                p_link_data->masterFCB = 0;
                p_link_data->subsiteFCB = CS101_MASTER_CONTROL_FCB_FIRST;
            }
        }
        else
        {
            cs101_master_timer_stop(&p_link_data->retry_time);                                            /* 停止定时 */
            p_link_data->retry_num_con = 0;
            p_link_data->link_flag &= ~CS101_MASTER_LINK_WAIT_JUMP_FLAG;         /* 非平衡时使用，发送数据后需等待跳转 */
        }
    }   
}

/**
  * @brief : 等待跳转
  * @param : [p_master_101_data]- 101主站数据结构指针
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static int cs101_master_wait_jump(CS101_MASTER_LINK_t *p_link_data)
{
    uint32_t link_addr = 0;
    int i = 0;
    if (p_link_data->mode_selection == CS101_MASTER_NOBALANCE_MODE)   //非平衡
    {
        if (!(p_link_data->link_flag & CS101_MASTER_LINK_WAIT_JUMP_FLAG))
        {
            cs101_master_timer_stop(&p_link_data->await_time); 
            return -1;
        }
        else
        {
            cs101_master_timer_start(&p_link_data->await_time);                /* 启动定时 */
            if (cs101_master_timer_compare(&p_link_data->await_time) == 1)
            {
                cs101_master_timer_stop(&p_link_data->await_time); 
                p_link_data->link_flag &= ~CS101_MASTER_LINK_WAIT_JUMP_FLAG;
                for (i=0; i<p_link_data->link_addr.len; i++)
                {
                    link_addr |= p_link_data->link_addr.addr[i] << i * 8;
                }
//                log_i("串口号%d，链路地址%d，链路等待超时", p_link_data->port_number, link_addr);
                return -1;
            }
            return 0;
        }
    }
    else
    {
        return -1;
    }
}

/**
  * @brief : 遥控命令接收
  * @param : [p_master_101_data]- 101主站数据指针
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
int cs101_master_YK_receive(tagControlCfg_t p_control)
{
    struct snode *p_temp_node_link = NULL;
    struct snode *p_temp_node_data = NULL;
    uint16_t yk_addr;
    uint16_t cmd;
    uint16_t work;
    if ((p_control->com.dir == s_pTagOverall->def.message_DOWN) && (p_control->com.tpye == s_pTagOverall->def.type_REMOTE))   /* 下行遥控命令 */
    {
        /* 查找模块ID */
        p_temp_node_link = s_link_101master_List.head;
        while (p_temp_node_link != NULL)
        {
            if (((CS101_MASTER_LINK_t*)(p_temp_node_link->data))->module_id == p_control->com.reply)
            {
                p_temp_node_data = s_appdata_101master_List.head;
                while (p_temp_node_data != NULL)
                {
                    if (((CS101_MASTER_APPDATA_t*)(p_temp_node_data->data))->module_id == p_control->com.reply)
                    {
                        yk_addr = ((tagControlRemoteCfg_t)(p_control->pdata))->addr - ((CS101_MASTER_APPDATA_t*)(p_temp_node_data->data))->p_YK_data->addr + CS101_MASTER_YK_START_ADDR;
                        cmd = ((tagControlRemoteCfg_t)(p_control->pdata))->cmd;
                        work = ((tagControlRemoteCfg_t)(p_control->pdata))->work;
                        return (cs101_master_YK(((CS101_MASTER_LINK_t*)(p_temp_node_link->data)), yk_addr, cmd, work));
                    }
                    p_temp_node_data = p_temp_node_data->next;
                }
            }
            p_temp_node_link = p_temp_node_link->next;
        }
        return s_pTagOverall->def.control_RES_INFOMIS;
    }
    return s_pTagOverall->def.control_RES_FAIL;
}

/**
  * @brief : 对时命令接收
  * @param : [p_master_101_data]- 101主站数据指针
  * @return: [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
int cs101_master_clock_sync_receive(tagControlCfg_t p_control)
{
    struct snode *p_temp_node_link = NULL;
    struct snode *p_temp_node_data = NULL;

    /* 查找模块ID */
    p_temp_node_link = s_link_101master_List.head;
    while (p_temp_node_link != NULL)
    {
        if (((CS101_MASTER_LINK_t*)(p_temp_node_link->data))->module_id == p_control->com.reply)
        {
            p_temp_node_data = s_appdata_101master_List.head;
            while (p_temp_node_data != NULL)
            {
                if (((CS101_MASTER_APPDATA_t*)(p_temp_node_data->data))->module_id == p_control->com.reply)
                {
                    if (((CS101_MASTER_LINK_t*)(p_temp_node_link->data))->link_flag & CS101_MASTER_LINK_ESTABLISH_FLAG) /* 链路建立 */
                    {
                        ((CS101_MASTER_LINK_t*)(p_temp_node_link->data))->pack_flag |= CS101_M_PACK_CLOCK_SYNC_FLAG;
                    }
                }
                p_temp_node_data = p_temp_node_data->next;
            }
        }
        p_temp_node_link = p_temp_node_link->next;
    }

    return 0;
}

/**
  * @brief : 101主站运行函数
  * @param : [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_run(CS101_MASTER_PHYSICS_t *p_physics_data, CS101_MASTER_LINK_t *p_link_data)
{
    while (1)
    {
        /* 获取时间 */
        cs101_master_get_time(&p_link_data->time_101);
        /* 数据处理 */
        cs101_master_data_treating(p_link_data);
        /* 发送数据打包 */
        cs101_master_data_pack(p_link_data);
        /* 发送数据 */
        cs101_master_data_send(p_physics_data, p_link_data);
        /* 搜索101报文 */
        if (cs101_master_search_message(p_physics_data, p_link_data) == 1)
        {
            /* 报文解析 */
            if (cs101_master_message_analysis(p_link_data) == -1)
            {
//                log_e("FCB ERROR\n");
            }
        }
        /* 等待跳转 */
        if (cs101_master_wait_jump(p_link_data) == -1)
        {
            if (p_link_data->mode_selection == CS101_MASTER_NOBALANCE_MODE)   //非平衡
            {
                p_physics_data->search_flag = CS101_M_SEARTCH_START;
                p_physics_data->seek_count = 0;
                p_physics_data->rx_queue.out = p_physics_data->rx_queue.in;
                p_physics_data->rx_queue.seek_out = p_physics_data->rx_queue.out;
            }
            return;
        }
        usleep(1000);

        /* 获取时间 */
        /* 搜索101报文 */
        /* 报文解析 */
        /* 数据处理 */
        /* 发送数据打包 */
        /* 发送数据 */
        /* 等待跳转 */
    }
}

/**
  * @brief : 101主站物理层初始化
  * @param : [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_physics_init(CS101_MASTER_COMCFG_t *p_comcfg_data)
{
    CS101_MASTER_PHYSICS_t *p_physics_data = NULL;

    p_physics_data = (CS101_MASTER_PHYSICS_t *)malloc(sizeof(CS101_MASTER_PHYSICS_t));
    if (p_physics_data == NULL)
    {
//        log_e("malloc p_physics_data error");
    }
    p_physics_data->port_number = p_comcfg_data->port_number;
    p_physics_data->fd = p_comcfg_data->fd;
    memset(p_physics_data->rx_queue.buff, 0, CS101_MASTER_RX_QUEUE_LEN);
    p_physics_data->rx_queue.in = 0;
    p_physics_data->rx_queue.out = 0;
    p_physics_data->rx_queue.seek_out = 0;
    p_physics_data->search_flag = 0;
    p_physics_data->seek_count = 0;
    p_physics_data->seek_time = 500;    /* 每帧等待最长时间 */
    slist_ins_next(&s_physics_101master_List, s_physics_101master_List.tail, p_physics_data);
}

/**
  * @brief : 101主站链路层初始化
  * @param : [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_link_init(CS101_MASTER_COMCFG_t *p_comcfg_data, CS101_MASTER_APPCFG_t *p_appcfg_data)
{
    CS101_MASTER_LINK_t *p_link_data = NULL;

    p_link_data = (CS101_MASTER_LINK_t *)malloc(sizeof(CS101_MASTER_LINK_t));
    if (p_link_data == NULL)
    {
//        log_e("malloc p_link_data error");
    }

    p_link_data->mode_selection = p_comcfg_data->mode;             		/* 模式配置 */
    p_link_data->yk_data.yk_single_double = p_appcfg_data->YK_single_double;
    p_link_data->module_id = p_appcfg_data->module_id;					//模块ID
    p_link_data->port_number = p_appcfg_data->port_number;
    memset(p_link_data->rx_frame.buff, 0, CS101_MASTER_RX_FRAME_MAX_LEN);    /* 接收帧缓冲区初始化 */
    p_link_data->rx_frame.len = 0;
    memset(p_link_data->tx_frame.buff, 0, CS101_MASTER_RX_FRAME_MAX_LEN);    /* 发送帧缓冲区初始化 */
    p_link_data->tx_frame.len = 0;
    memset(p_link_data->tx_re_frame.buff, 0, CS101_MASTER_RX_FRAME_MAX_LEN); /* 重发帧缓冲区初始化 */
    p_link_data->tx_re_frame.len = 0;
    p_link_data->masterFCB = 0;                                 /* 主站FCB从0翻转 */
    p_link_data->subsiteFCB = CS101_MASTER_CONTROL_FCB_FIRST;                /* 从站FCB初始化为第一次 */
    p_link_data->link_addr.addr[0] = (uint8_t)(p_appcfg_data->link_addr & 0x00ff);	//链路地址
    p_link_data->link_addr.addr[1] = (uint8_t)((p_appcfg_data->link_addr >> 8) & 0x00ff);
    p_link_data->link_addr.len = p_comcfg_data->link_addr_len;				 //链路地址长度

    p_link_data->ASDU.COT.cot[0] = 0;
    p_link_data->ASDU.COT.cot[1] = 0;
    p_link_data->ASDU.COT.len = p_comcfg_data->cot_len;						 //传输原因

    p_link_data->ASDU.ASDU_addr.addr[0] = (uint8_t)(p_comcfg_data->ASDU_addr & 0x00ff);			//ASDU地址
    p_link_data->ASDU.ASDU_addr.addr[1] = (uint8_t)((p_comcfg_data->ASDU_addr >> 8) & 0x00ff);
    p_link_data->ASDU.ASDU_addr.len = p_comcfg_data->ASDU_len;		//ASDU地址长度
    memset(p_link_data->ASDU.info_data.info_buff, 0, CS101_MASTER_INFO_OBJECT_LEN);
    p_link_data->ASDU.info_len = 0;

    p_link_data->link_flag = 0;
    p_link_data->pack_flag = 0;
    p_link_data->heartbeat_time.timing_time = CS101_MASTER_HEARTBEAT_TIME;      //心跳测试时间
    p_link_data->call_data_time.timing_time = CS101_MASTER_CALL_DATA_TIME;      //召唤二级数据
    p_link_data->retry_time.timing_time = CS101_MASTER_RETRY_TIME;              //重发时间
    p_link_data->retry_num_con = 0;
    p_link_data->retry_number = CS101_MASTER_RETRY_NUMBER;                      //重发次数
    p_link_data->await_time.timing_time = CS101_MASTER_AWAIT_TIME;              //等待链路时间
    p_link_data->all_call_time.timing_time = CS101_MASTER_ALL_CALL_TIME;        //循环总召时间
    slist_ins_next(&s_link_101master_List, s_link_101master_List.tail, p_link_data);
}

/**
  * @brief : 101主站应用数据初始化
  * @param : [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static void cs101_master_appdata_init(CS101_MASTER_APPCFG_t *p_appcfg_data)
{
    CS101_MASTER_APPDATA_t *p_appdata = NULL;
    
    p_appdata = (CS101_MASTER_APPDATA_t *)malloc(sizeof(CS101_MASTER_APPDATA_t));
    if (p_appdata == NULL)
    {
//        log_e("malloc appdata_101_init error");
    }

    p_appdata->module_id = p_appcfg_data->module_id;                        /* 模块ID */
    p_appdata->communication_state = p_appcfg_data->communication_state;    /* 通讯状态 */
    p_appdata->YX_len = p_appcfg_data->YX_len;                              /* 遥信长度 */
    p_appdata->p_YX_data = p_appcfg_data->p_YX_data;                        /* 遥信数据 */
    p_appdata->YC_len = p_appcfg_data->YC_len;                              /* 遥测长度 */
    p_appdata->p_YC_data = p_appcfg_data->p_YC_data;                        /* 遥测数据 */
    p_appdata->YK_len = p_appcfg_data->YK_len;                              /* 遥控长度 */
    p_appdata->p_YK_data = p_appcfg_data->p_YK_data;                        /* 遥控数据 */
    slist_ins_next(&s_appdata_101master_List, s_appdata_101master_List.tail, p_appdata);
}

/**
  * @brief : 串口数据读取线程
  * @param : [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
void *cs101_master_read_data_thread(void *arg)
{
    while(1)
    {
        cs101_master_data_in_queue(arg);
    }
    return (void *)0;
}

/**
  * @brief : 101主站运行线程
  * @param : [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
void *cs101_master_run_thread(void *arg)
{
    int i = 0, j = 0;
    uint32_t link_addr;
    CS101_MASTER_TIMER_t link_time;                                             /* 链路重新连接定时器 */
    link_time.timing_time = CS101_MASTER_TIMING_TIME;                           /* 链路重连时间 */
    int now_module_id = -1;                                                     /* 故障链路ID */
    struct snode *p_temp_node_link_fault = s_link_101master_List.head;          /* 故障链路节点 */
    struct snode *p_temp_node_link_run = NULL;                                  /* 链路运行节点 */
    CS101_MASTER_PHYSICS_t *p_physics_data = (CS101_MASTER_PHYSICS_t*)arg;		//物理数据块
    /* 打开链路 */
    p_temp_node_link_run = s_link_101master_List.head;							//链路头部
    while (p_temp_node_link_run != NULL)
    {
        /* 启动该串口下所有链路 */
        if (((CS101_MASTER_LINK_t*)(p_temp_node_link_run->data))->port_number == p_physics_data->port_number)
        {
            cs101_master_request_link((CS101_MASTER_LINK_t*)(p_temp_node_link_run->data));
        }
        p_temp_node_link_run = p_temp_node_link_run->next;
    }

    while (1)
    {
        p_temp_node_link_run = s_link_101master_List.head;						//链路头部
        while (p_temp_node_link_run != NULL)
        {
            if (((CS101_MASTER_LINK_t*)(p_temp_node_link_run->data))->port_number == p_physics_data->port_number)	//串口判断
            {
                /* 故障模块重连 */
                if (((CS101_MASTER_LINK_t*)(p_temp_node_link_run->data))->module_id == now_module_id)
                {
                    cs101_master_timer_start(&link_time);        /* 启动定时 */
                    if (cs101_master_timer_compare(&link_time) == 1)
                    {
                        cs101_master_timer_stop(&link_time);
                        if (!(((CS101_MASTER_LINK_t*)(p_temp_node_link_run->data))->link_flag & CS101_MASTER_LINK_ESTABLISH_FLAG))
                        {
                            link_addr = 0;
                            for (j=0; j<((CS101_MASTER_LINK_t*)(p_temp_node_link_run->data))->link_addr.len; j++)
                            {
                                link_addr |= ((CS101_MASTER_LINK_t*)(p_temp_node_link_run->data))->link_addr.addr[j] << j * 8;
                            }
//                            log_i("串口%d下链路地址%d重连", ((CS101_MASTER_LINK_t*)(p_temp_node_link_run->data))->port_number, link_addr);
                            cs101_master_request_link((CS101_MASTER_LINK_t*)(p_temp_node_link_run->data));
                        }
                        now_module_id = -1;
                    }
                }
                cs101_master_run(p_physics_data, (CS101_MASTER_LINK_t*)(p_temp_node_link_run->data));	//101主站运行函数
            }
            p_temp_node_link_run = p_temp_node_link_run->next;
        }
        /* 查找故障节点 */
        if (now_module_id == -1)        /* 当前无故障模块处理 */
        {
            for (i=0; i<s_link_101master_List.size; i++)	//链路个数
            {
                if (((CS101_MASTER_LINK_t*)(p_temp_node_link_fault->data))->port_number == p_physics_data->port_number)
                {
                    /* 链路断开 */
                    if (!(((CS101_MASTER_LINK_t*)(p_temp_node_link_fault->data))->link_flag & CS101_MASTER_LINK_ESTABLISH_FLAG))	//当前链路断开
                    {
                        now_module_id = ((CS101_MASTER_LINK_t*)(p_temp_node_link_fault->data))->module_id;
                        p_temp_node_link_fault = p_temp_node_link_fault->next;          /* 故障处理节点后移 */
                        if (p_temp_node_link_fault == NULL)
                        {
                            p_temp_node_link_fault = s_link_101master_List.head;
                        }
                        break;
                    }
                }
                p_temp_node_link_fault = p_temp_node_link_fault->next;
                if (p_temp_node_link_fault == NULL)
                {
                    p_temp_node_link_fault = s_link_101master_List.head;
                }
            }
        }
        usleep(1000);
    }
    return (void *)0;
}

/**
  * @brief : 创建线程
  * @param : [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
static int cs101_master_create_thread(uint8_t port_number)
{
    pthread_t tid1, tid2;
    int err;
    struct snode *p_temp_node_physics = NULL;
    printf("pthread_create\r\n");
    /* 根据端口号查找端口配置 */
    p_temp_node_physics = s_physics_101master_List.head;
    while (p_temp_node_physics != NULL)
    {
        if (((CS101_MASTER_PHYSICS_t*)(p_temp_node_physics->data))->port_number == port_number)
        {
            /* 创建读串口数据线程 */
            err = pthread_create(&tid1, NULL, cs101_master_read_data_thread, ((CS101_MASTER_PHYSICS_t*)(p_temp_node_physics->data)));
            if(err != 0)
            {
//                log_e("创建串口读线程失败   tid = %d", tid1);
                return -1;
            }

            /* 创建运行线程 */
            err = pthread_create(&tid2, NULL, cs101_master_run_thread, ((CS101_MASTER_PHYSICS_t*)(p_temp_node_physics->data)));
            if(err != 0)
            {
//                log_e("创建运行线程失败   tid = %d", tid2);
                return -1;
            }

            return 0;
        }
        p_temp_node_physics = p_temp_node_physics->next;
    }
    return 0;
}

void cs101_master_init(void)
{
    static uint8_t first_flag = 0;

    if (first_flag == 0)
    {
        /* 初始化链表 */
        first_flag = 1;
        slist_new(&s_comcfg_101master_List,NULL);
        slist_new(&s_appcfg_101master_List,NULL);
        slist_new(&s_appdata_101master_List,NULL);
        slist_new(&s_link_101master_List,NULL);
//        s_pTagOverall = dataSwitch_getOverall();
    }
}
/**
  * @brief : 101主站配置初始化
  * @param : [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
int cs101_master_comconfig_init(int fd, IecParam sParam)
{
    CS101_MASTER_COMCFG_t *p_comcfg_data = NULL;
    struct snode *p_temp_node = NULL;
    s_pTagOverall = sParam->allInfoDisk;
    cs101_master_init();
//    log_w("配置串口%d", sParam->portNo);
    /* 查找端口号，判断端口号是否重复 */
    p_temp_node = s_comcfg_101master_List.head;

    while (p_temp_node != NULL)
    {
        if (((CS101_MASTER_COMCFG_t*)(p_temp_node->data))->port_number == sParam->portNo)
        {
//            log_e("串口号%d配置重复", sParam->portNo);
            return -1;
        }
        p_temp_node = p_temp_node->next;
    }
    /* 申请配置结构空间 */
    p_comcfg_data = (CS101_MASTER_COMCFG_t *)malloc(sizeof(CS101_MASTER_COMCFG_t)); 
    if (p_comcfg_data == NULL)
    {
//        log_e("malloc p_comcfg_data error");
        return -1;
    }
    p_comcfg_data->mode = sParam->balanMode;
    p_comcfg_data->port_number = sParam->portNo;
    p_comcfg_data->fd = fd;
    p_comcfg_data->link_addr_len = sParam->linkAddrSize;
    p_comcfg_data->cot_len = sParam->ASDUCotSize;
    p_comcfg_data->ASDU_len = sParam->ASDUAddrSize;
    p_comcfg_data->ASDU_addr = sParam->ASDUAddr;
    slist_ins_next(&s_comcfg_101master_List, s_comcfg_101master_List.tail, p_comcfg_data);
    return 0;
}

/**
  * @brief : 101主站应用初始化
  * @param : [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
int cs101_master_appconfig_init(sMasterParam_t sParam)//此函数调用一次初始化一个从站
{
    int slave_num = 0;
    CS101_MASTER_APPCFG_t *p_appcfg_data = NULL;
    struct snode *p_temp_node_app = NULL;
    struct snode *p_temp_node_com = NULL;
    cs101_master_init();
    sParam->master_CLOCK_SYNC_fun = NULL;
    sParam->master_YK_fun = NULL;

//    log_w("配置串口%d下链路地址%d", sParam->portNo, sParam->sourceAddr);
    /* 查找101端口下的模块 */
    p_temp_node_com = s_comcfg_101master_List.head;

    while(p_temp_node_com != NULL)
    {
        if (((CS101_MASTER_COMCFG_t*)(p_temp_node_com->data))->port_number == sParam->portNo)   /* 串口号相同 */
        {
            /* 查询相同端口下的子站数量 */
            p_temp_node_app = s_appcfg_101master_List.head;
            while (p_temp_node_app != NULL)
            {
                if (((CS101_MASTER_APPCFG_t*)(p_temp_node_app->data))->port_number == sParam->portNo)	//串口号相同
                {
                    /* 判断相同端口下链路地址是否重复 */
                    if (((CS101_MASTER_APPCFG_t*)(p_temp_node_app->data))->link_addr == sParam->sourceAddr)//从站链路地址不同
                    {
//                        log_e("串口号%d下链路地址%d重复", sParam->portNo, sParam->sourceAddr);
                        return -1;
                    }
                    slave_num++;													//从站配置端口数
                }
                p_temp_node_app = p_temp_node_app->next;
            }
            if (slave_num >= CS101_MASTER_SLAVE_MAX)	//从站最大连接数
            {
//                log_e("串口号%d下从站数量大于%d", sParam->portNo, CS101_MASTER_SLAVE_MAX);
                return -1;
            }
            p_appcfg_data = (CS101_MASTER_APPCFG_t *)malloc(sizeof(CS101_MASTER_APPCFG_t)); /* 应用配置结构空间 */
            if (p_appcfg_data == NULL)
            {
//                log_e("malloc p_appcfg_data error");
                return -1;
            }
            p_appcfg_data->module_id = sParam->module_id;						//模块ID， 从站id
            p_appcfg_data->link_addr = sParam->sourceAddr;						//从站地址
            p_appcfg_data->YK_single_double = sParam->yk_type;
            p_appcfg_data->port_number = sParam->portNo;						//串口
            p_appcfg_data->communication_state = sParam->communication_state;   /* 通讯状态 */
            p_appcfg_data->YX_len = sParam->YX_len;                             /* 遥信长度 */
            p_appcfg_data->p_YX_data = sParam->p_YX_data;                       /* 遥信数据 */
            p_appcfg_data->YC_len = sParam->YC_len;                             /* 遥测长度 */
            p_appcfg_data->p_YC_data = sParam->p_YC_data;                       /* 遥测数据 */
            p_appcfg_data->YK_len = sParam->YK_len;                             /* 遥控长度 */
            p_appcfg_data->p_YK_data = sParam->p_YK_data;                       /* 遥控数据 */
            slist_ins_next(&s_appcfg_101master_List, s_appcfg_101master_List.tail, p_appcfg_data);
            sParam->master_YK_fun = cs101_master_YK_receive;
            sParam->master_CLOCK_SYNC_fun = cs101_master_clock_sync_receive;
            return 0;
        }
        p_temp_node_com = p_temp_node_com->next;
    }
//    log_e("串口号%d未配置", sParam->portNo);
    return -1;
}

/**
  * @brief : 101主站启动
  * @param : [无]
  * @update: [2019-5-5][WangHuaRun][newly increased]
  */
int cs101_master_start(void)
{
    struct snode *p_temp_node_cfg = NULL;
    struct snode *p_temp_node_app = NULL;
    int slave_num = 0;

//    log_w("101主站启动");
    /* 查询端口配置并初始化 */
    p_temp_node_cfg = s_comcfg_101master_List.head;
    while (p_temp_node_cfg != NULL)
    {
        /* 初始化物理层 */
        cs101_master_physics_init((CS101_MASTER_COMCFG_t*)(p_temp_node_cfg->data));	//主站配置物理参数，用于接收串口中的数据
        /* 初始化该端口下的应用层 */
        p_temp_node_app = s_appcfg_101master_List.head;
        while (p_temp_node_app != NULL)
        {
            if (((CS101_MASTER_APPCFG_t*)(p_temp_node_app->data))->port_number == \
                ((CS101_MASTER_COMCFG_t*)(p_temp_node_cfg->data))->port_number)		//主站配置和主站APP串口检查
            {
                cs101_master_link_init((CS101_MASTER_COMCFG_t*)(p_temp_node_cfg->data), (CS101_MASTER_APPCFG_t*)(p_temp_node_app->data));	//合并配置信息和app信息
                cs101_master_appdata_init((CS101_MASTER_APPCFG_t*)(p_temp_node_app->data));	//配置信息赋值给APP信息
                slave_num++;								//当前串口下的从站数量
            }
            p_temp_node_app = p_temp_node_app->next;
        }
        if ((((CS101_MASTER_COMCFG_t*)(p_temp_node_cfg->data))->mode == CS101_MASTER_BALANCE_MODE) && (slave_num > 1))  //平衡模式从站大于1
        {
//            log_e("平衡模式从站数量大于1");
            return -1;
        }
        if (slave_num == 0)
        {
//            log_e("串口号：%d下从站数量为0", ((CS101_MASTER_COMCFG_t*)(p_temp_node_cfg->data))->port_number);
            return -1;
        }
        /* 创建线程 */

        if (cs101_master_create_thread(((CS101_MASTER_COMCFG_t*)(p_temp_node_cfg->data))->port_number) == -1)
        {
//            log_e("创建线程失败 串口号：%d", ((CS101_MASTER_COMCFG_t*)(p_temp_node_cfg->data))->port_number);
            return -1;
        }
        slave_num = 0;
        p_temp_node_cfg = p_temp_node_cfg->next;
    }
    return 0;
}


