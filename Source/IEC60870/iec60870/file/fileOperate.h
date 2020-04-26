/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      fileOperate.h
  * @brief:		文件操作
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2018-12-27
  * @update:    [2018-12-27][Chen][newly increased]
  */
#ifndef	_FILE_OPERATE_
#define _FILE_OPERATE_
/* INCLUDE FILES -------------------------------------------------------------*/
#include <stdint.h>
#include <dirent.h>

/* DEFINES -------------------------------------------------------------------*/
#define FILEMAXNUM                  1024
#define FAILEMAXNUM                 5

#define SOE_FILE_MAXNUM             512
#define CO_FILE_MAXNUM              64
#define FEVENT_FILE_MAXNUM          64
#define COMTRADE_FILE_MAXNUM        64

struct FileType
{
	char old_file[255];
	int num;
};

struct DIR_ARRAY
{
	int num;
	char Array[FILEMAXNUM][255];
};

struct READ_DIR
{
	unsigned int dir_i;
    unsigned int dirid;
};

struct READ_FILE
{
	unsigned int fileid;
	unsigned int offset;
	unsigned int filesize;
	char filename[100];
};

struct WRITE_FILE
{
	unsigned int fileid;
	unsigned int filesize;
	unsigned int overtime;
	char filename[100];
};

uint8_t file_operate_ReadFile(uint8_t dev,uint8_t *pbuf);
uint8_t file_operate_DirRead(uint8_t dev,uint8_t *pbuf);
uint8_t file_operate_DirCall(uint8_t dev,uint8_t *pbuf);
uint8_t file_operate_ReadFileAct(uint8_t dev,uint8_t *pbuf);
void file_operate_WriteFileAct(uint8_t dev,uint8_t *pbuf);
void file_operate_Init(void);
DIR * Scan_Files (char* path,struct DIR_ARRAY* path_out);
#endif	/*_FILE_OPERATE_*/
/* END OF FILE ---------------------------------------------------------------*/
