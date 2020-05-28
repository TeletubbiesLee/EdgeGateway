/**
  *             Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      fileOperate.c
  * @brief:		文件操作
  * @version:   V1.0.0
  * @author:    Chen
  * @date:      2018-12-27
  * @update:    [2018-12-27][Chen][newly increased]
  */
#define LOG_TAG    "fileOperate"
/* INCLUDE FILES -------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include "fileOperate.h"
//#include "elog.h"
#include "../common/common.h"
/* PRIVATE VARIABLES ---------------------------------------------------------*/
static int MyFile; /* File object */
//static int MyFile_SOE; /* File object */
//static int MyFile_CO; /* File object */
static struct stat FileSta;
static char content[512];
static char FileName[255]; /* File Name */
static char DirName[255]; /*目录名*/
//static int failnum = 0;
static uint8_t temp_array[256];

static struct DIR_ARRAY *Dir_Array;
static struct DIR_ARRAY *Dir_ArrayClear;// __attribute__((at(0x00050000 + SDRAM_ADDR_FILE)));
static struct READ_DIR Read_Dir[DEV_MAX_NUM];
static struct READ_FILE Read_File[DEV_MAX_NUM];
static struct WRITE_FILE Write_File[DEV_MAX_NUM];


/* PRIVATE FUNCTION PROTOTYPES -----------------------------------------------*/
/**
  * @brief : 获取时间信息.
  * @param : [time]-时间
  * @return: RETURN_TRUE
  * @return: RETURN_ERROR
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
static int get_currentTime(struct CP56Time2a_t time)
{
	struct tm *t;
	struct timeval tv;

	gettimeofday(&tv, NULL);

	t = localtime(&tv.tv_sec);

	time.msecondL = (t->tm_sec*1000 + tv.tv_usec/1000)&0xff;
	time.msecondH = ((t->tm_sec*1000 + tv.tv_usec/1000)>>8)&0xff;
	time.minute = t->tm_min;
	time.hour = t->tm_hour;
	time.dayofWeek = (t->tm_mday&0x1f)|((t->tm_wday&0x07)<<5);
	time.month = 1+t->tm_mon;
	time.year = 1970+t->tm_year-2000;
	return 0;
}

/**
  * @brief : Scan_Files.
  * @param : [path]
  * @param : [path_out]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
DIR * Scan_Files (char* path,struct DIR_ARRAY* path_out)
{
    DIR *dir;
    DIR *dirtemp;
    struct dirent *fno;
    char base[1000];

//    log_w("path:%s",path);
    dir = opendir(path); //打开目录
    perror("opendir");
    if (dir != NULL)
    {
        for (;;)
        {
            fno = readdir(dir);/* Read a directory item */
            perror("readdir");

            if (fno == NULL || fno->d_reclen == 0)/* Break on error or end of dir */
            	{break;}

//            log_i("fno->d_name:%s",fno->d_name);
//            log_i("fno->d_type:%d",fno->d_type);
//            log_i("fno->d_reclen:%d",fno->d_reclen);

            if(strcmp(fno->d_name,".")==0 || strcmp(fno->d_name,"..")==0)
                continue;

            if (fno->d_type == DT_DIR)  /* It is a directory */
            {
                memset(base,'\0',sizeof(base));
                strcpy(base,path);
                strcat(base,"/");
                strcat(base,fno->d_name);
                dirtemp = Scan_Files(base, path_out);
                if (dirtemp == NULL)
                	{break;}
            }
            else if(fno->d_type == DT_REG)/* It is a file. */
            {
                sprintf((*path_out).Array[(*path_out).num], "%s/%s\n", path, fno->d_name);
				(*path_out).num++;
            }
        }

        closedir(dir);
        perror("closedir");
    }
    return dir;
}

/**
  * @brief : TIME_TO_STR.
  * @param : [content]
  * @param : [time]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void TIME_TO_STR(char *content,struct CP56Time2a_t time)
{

}

/**
  * @brief : file_Dir_Clear.
  * @param : [DIR]
  * @param : [maxnum]
  * @param : [cmpsize]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void file_Dir_Clear(char *dir,uint16_t maxnum,uint16_t cmpsize)
{

}

/**
  * @brief : CreatDoc_FEVENT.
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CreatDoc_FEVENT(void)
{

}

/**
  * @brief : CreatDoc_SOE.
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CreatDoc_SOE(void)
{

}

/**
  * @brief : CreatDoc_CO.
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CreatDoc_CO(void)
{

}

/**
  * @brief : file_operate_DirRead.
  * @param : [dev]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t file_operate_DirRead(uint8_t dev,uint8_t *pbuf)
{//LENTH/Lock_ID/TypeID/VSQ/COT_L/COT_H/PubAddr_L/PubAddr_H/InfoAddr_L/InfoAddr_H/sty/Array
    struct CP56Time2a_t time;
	int j=0,txnum=0,size;
    unsigned int *i;

    i = &Read_Dir[dev].dir_i;

    pbuf[1] = dev;

    txnum = 2;
    pbuf[txnum++] = 210;//TypeID
    pbuf[txnum++] = 0;
    pbuf[txnum++] = 5;
    pbuf[txnum++] = 0;
    pbuf[txnum++] = 1;
    pbuf[txnum++] = 0;
    txnum = 8;
    pbuf[txnum++] = 0;
    pbuf[txnum++] = 0;
    pbuf[txnum++] = 1;

    while(1)
    {
        if(*Dir_Array->Array[*i] != 0)
        {
            //填表
            if(j==0)
            {txnum = 19;}
            Dir_Array->Array[*i][strlen(Dir_Array->Array[*i])-1] = '\0';
            pbuf[txnum++] = strlen(Dir_Array->Array[*i]);
            memcpy(&pbuf[txnum],Dir_Array->Array[*i],strlen(Dir_Array->Array[*i]));
            txnum += strlen(Dir_Array->Array[*i]);
            pbuf[txnum++] = 0;
            MyFile = open(Dir_Array->Array[*i], O_RDONLY, 0);
            if(MyFile == -1)
            {return(0);}
            fstat(MyFile, &FileSta);
            size = FileSta.st_size;
            close(MyFile);
            pbuf[txnum++] = size&0xff;
            pbuf[txnum++] = (size>>8)&0xff;
            pbuf[txnum++] = (size>>16)&0xff;
            pbuf[txnum++] = (size>>24)&0xff;
            if(strncmp(&Dir_Array->Array[*i][strlen(Dir_Array->Array[*i])-8],"20",2))
            {
            	get_currentTime(time);
                pbuf[txnum++] = time.msecondL;
                pbuf[txnum++] = time.msecondH;
                pbuf[txnum++] = time.minute;
                pbuf[txnum++] = time.hour;
                pbuf[txnum++] = time.dayofWeek;
                pbuf[txnum++] = time.month;
                pbuf[txnum++] = time.year;
            }
            else
            {
                pbuf[txnum++] = 0;
                pbuf[txnum++] = 0;
                pbuf[txnum++] = 0;
                pbuf[txnum++] = 0;
                pbuf[txnum++] = (Dir_Array->Array[*i][strlen(Dir_Array->Array[*i])-6]-30)*10 + (Dir_Array->Array[*i][strlen(Dir_Array->Array[*i])-5]-30);
                pbuf[txnum++] = (Dir_Array->Array[*i][strlen(Dir_Array->Array[*i])-4]-30)*10 + (Dir_Array->Array[*i][strlen(Dir_Array->Array[*i])-3]-30);;
                pbuf[txnum++] = (Dir_Array->Array[*i][strlen(Dir_Array->Array[*i])-2]-30)*10 + (Dir_Array->Array[*i][strlen(Dir_Array->Array[*i])-1]-30);;
            }
            //j>6,发送j=0
            if(++j>2)
            {
                pbuf[18] = j;
                pbuf[0] = txnum;
                txnum = 11;
                pbuf[1] = dev;
                pbuf[txnum++] = 2;
                pbuf[txnum++] = 0;
                memcpy(&pbuf[txnum],&Read_Dir[dev].dirid,sizeof(Read_Dir[dev].dirid));
                txnum += sizeof(Read_Dir[dev].dirid);
                if(*Dir_Array->Array[*i+1] != 0)
                {pbuf[txnum++] = 1;}
                else
                {pbuf[txnum++] = 0;}
                j=0;
                ++(*i);
                return(1);
            }
            ++(*i);
        }
        else
        {
            //如果可发送目录个数为0，发送
            if(*i==0)
            {
                txnum = 11;
                pbuf[1] = (pbuf[1]>>4);
                pbuf[txnum++] = 2;
                pbuf[txnum++] = 0;
                memcpy(&pbuf[txnum],&Read_Dir[dev].dirid,sizeof(Read_Dir[dev].dirid));
                txnum += sizeof(Read_Dir[dev].dirid);
                pbuf[txnum++] = 0;
                pbuf[txnum++] = 0;
                pbuf[0] = txnum;
                return(0);
            }
            //如果j！=0发送已打包目录数据
            if(j!=0)
            {
                pbuf[18] = j;
                pbuf[0] = txnum;
                txnum = 11;
                pbuf[1] = dev;
                pbuf[txnum++] = 2;
                pbuf[txnum++] = 0;
                memcpy(&pbuf[txnum],&Read_Dir[dev].dirid,sizeof(Read_Dir[dev].dirid));
                txnum += sizeof(Read_Dir[dev].dirid);
                pbuf[txnum++] = 0;
                j=0;
                return(0);
            }
            return(0);
        }
    }
	return 0;
}

/**
  * @brief : file_operate_DirCall.
  * @param : [dev]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t file_operate_DirCall(uint8_t dev,uint8_t *pbuf)
{
	int txnum=0;

	memcpy(temp_array, pbuf,pbuf[0]);

	if(pbuf[16])//读指定目录
	{
        memset(DirName,0,sizeof(DirName));
		memcpy(DirName, &pbuf[17],pbuf[16]);//建立查询目录
		DirName[pbuf[16]] = '\0';
	}
	else//读默认目录
	{
		memset(DirName,0,sizeof(DirName));
		strcpy(DirName,"/home/ftpuser/file_manager");
		strcat(DirName,"/history");//建立查询目录
	}
//	log_w("DirName:%s",DirName);

	memset(Dir_Array,0,sizeof(struct DIR_ARRAY));

	Scan_Files(DirName, Dir_Array);

//	log_w("Dir_Array->num:%d",Dir_Array->num);
	if(Dir_Array->num == 0)//失败
	{
		temp_array[1] = dev;
        txnum = 8;
        temp_array[txnum++] = 0;
		temp_array[txnum++] = 0;
        temp_array[txnum++] = 1;
		txnum = 11;
		temp_array[txnum++] = 2;
		temp_array[txnum++] = 1;
		temp_array[txnum++] = pbuf[12];
		temp_array[txnum++] = pbuf[13];
		temp_array[txnum++] = pbuf[14];
		temp_array[txnum++] = pbuf[15];
		temp_array[txnum++] = 0;
		temp_array[txnum++] = 0;
		temp_array[0] = txnum;
        memcpy(pbuf,temp_array,temp_array[0]);
        return(0);
	}
	else//成功
	{
        Read_Dir[dev].dir_i = 0;
        memcpy(&Read_Dir[dev].dirid,&pbuf[12],sizeof(Read_Dir[dev].dirid));
        return(file_operate_DirRead(dev,pbuf));
	}
	return 0;
}

/**
  * @brief : file_operate_ReadFile.
  * @param : [dev]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t file_operate_ReadFile(uint8_t dev,uint8_t *pbuf)
{//LENTH/Lock_ID/TypeID/VSQ/COT_L/COT_H/PubAddr_L/PubAddr_H/InfoAddr_L/InfoAddr_H/sty/Array
	int i,txnum;
	int eachoffset = 200;
	int bytesread;

//	log_i("dev:%x Read_File[dev].filename:%s Read_File[dev].offset:%d",dev,Read_File[dev].filename,Read_File[dev].offset);
    pbuf[1] = (dev);
    MyFile = open(Read_File[dev].filename, O_RDONLY);

    lseek(MyFile, Read_File[dev].offset, SEEK_SET);

    bytesread = read(MyFile, content, eachoffset);

    close(MyFile);

//    log_i("bytesread:%d",bytesread);
    if(MyFile == -1)
    {return(0);}
    if(bytesread)
    {
        txnum = 2;
        pbuf[txnum++] = 210;//TypeID
        pbuf[txnum++] = 0;
        pbuf[txnum++] = 5;
        pbuf[txnum++] = 0;
        pbuf[txnum++] = 1;
        pbuf[txnum++] = 0;
        txnum = 8;
        pbuf[txnum++] = 0;
        pbuf[txnum++] = 0;
        pbuf[txnum++] = 2;
        txnum = 11;
        pbuf[txnum++] = 5;
        pbuf[txnum++] = Read_File[dev].fileid&0xff;
        pbuf[txnum++] = (Read_File[dev].fileid>>8)&0xff;
        pbuf[txnum++] = (Read_File[dev].fileid>>16)&0xff;
        pbuf[txnum++] = (Read_File[dev].fileid>>24)&0xff;
        pbuf[txnum++] = Read_File[dev].offset&0xff;
        pbuf[txnum++] = (Read_File[dev].offset>>8)&0xff;
        pbuf[txnum++] = (Read_File[dev].offset>>16)&0xff;
        pbuf[txnum++] = (Read_File[dev].offset>>24)&0xff;
        if((bytesread < eachoffset)|(Read_File[dev].filesize == eachoffset ))
        {pbuf[txnum++] = 0;}
        else
        {pbuf[txnum++] = 1;}
        memcpy(&pbuf[txnum],content,bytesread);
        pbuf[txnum+bytesread] = 0;
        for(i=0;i<bytesread;i++)
        {pbuf[txnum+bytesread] = (pbuf[txnum+bytesread] + pbuf[txnum+i])&0xff;}
        pbuf[0] = txnum+bytesread+1;
        Read_File[dev].offset += eachoffset;
        Read_File[dev].filesize -= eachoffset;
        if((bytesread < eachoffset)|(Read_File[dev].filesize == eachoffset ))
        {memset(&Read_File,0,sizeof(Read_File));}
        if((bytesread < eachoffset)|(Read_File[dev].filesize == eachoffset ))
        {return(0);}
        else
        {return(1);}
    }
    else
    {
        {memset(&Read_File,0,sizeof(Read_File));}
        return(0);
    }
}

/**
  * @brief : file_operate_ReadFileAct.
  * @param : [dev]
  * @param : [pbuf]
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t file_operate_ReadFileAct(uint8_t dev,uint8_t *pbuf)
{//LENTH/Lock_ID/TypeID/VSQ/COT_L/COT_H/PubAddr_L/PubAddr_H/InfoAddr_L/InfoAddr_H/sty/Array
	int i,txnum,filesize;
	int fileid = 0;

	memcpy(temp_array, pbuf,pbuf[0]);

	switch(pbuf[11])
	{
		//读文件激活
		case 3:
            temp_array[4] = 7;
			temp_array[1] = dev;
			memcpy(FileName,&pbuf[13],pbuf[12]);
			FileName[pbuf[12]] = '\0';
            MyFile = open(FileName, O_WRONLY, 0);
            fstat(MyFile, &FileSta);
			filesize = FileSta.st_size;
			close(MyFile);
			txnum = 11;
			temp_array[txnum++] = 4;
			if(MyFile == -1)
			{temp_array[txnum++] = 1;}
			else
			{temp_array[txnum++] = 0;}
			temp_array[txnum++] = strlen(FileName);
			memcpy(&temp_array[txnum],FileName,strlen(FileName));
			txnum += strlen(FileName);
			for(i=0;i<strlen(FileName);i++)
			{fileid += FileName[i];}
			temp_array[txnum++] = fileid&0xff;
			temp_array[txnum++] = (fileid>>8)&0xff;
			temp_array[txnum++] = (fileid>>16)&0xff;
			temp_array[txnum++] = (fileid>>24)&0xff;
			temp_array[txnum++] = filesize&0xff;
			temp_array[txnum++] = (filesize>>8)&0xff;
			temp_array[txnum++] = (filesize>>16)&0xff;
			temp_array[txnum++] = (filesize>>24)&0xff;
			temp_array[0] = txnum;
            memcpy(pbuf, temp_array,temp_array[0]);
			if(!temp_array[12])//激活成功
			{
                memset(&Read_File,0,sizeof(Read_File));
				Read_File[dev].fileid = fileid;
				Read_File[dev].filesize = filesize;
				memcpy(Read_File[dev].filename,FileName,strlen(FileName)+1);
                return(1);
			}
			return(0);
		//读文件确认
		case 6:
			memset(&Read_File[dev],0,sizeof(Read_File));
            return(0);
		default:
			break;
	}
	return(1);
}

/**
  * @brief : ReadDoc_Record.
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void ReadDoc_Record(void)
{

}

/**
  * @brief : file_operate_WriteFileAct.
  * @return: none
  * @param : [dev]
  * @param : [pbuf]
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void file_operate_WriteFileAct(uint8_t dev,uint8_t *pbuf)
{//LENTH/Lock_ID/TypeID/VSQ/COT_L/COT_H/PubAddr_L/PubAddr_H/InfoAddr_L/InfoAddr_H/sty/Array
    unsigned long i;
    unsigned long crc;

	memcpy(temp_array, pbuf,pbuf[0]);

	switch(pbuf[11])
	{
		//写文件激活
		case 7:
            temp_array[4] = 7;//COT_L
			temp_array[1] = (pbuf[1]>>4);
			temp_array[11] = 8;
			temp_array[12] = 0;//0 成功
            memcpy(&temp_array[13],&pbuf[12],sizeof(temp_array)-12);
			temp_array[0] = pbuf[0]+1;
            memset(&Write_File,0,sizeof(Write_File));
            Write_File[dev].fileid = *(unsigned long *)&pbuf[11 + 2 + pbuf[12]];//记录id
            Write_File[dev].filesize = *(unsigned long *)&pbuf[11 + 6 + pbuf[12]];//文件长度
            for(i=0;i<pbuf[12];i++)
            {
                if(!strncmp("ConfigurationSet",(char *)&pbuf[i+13],sizeof("ConfigurationSet") - 1))
                {
                    strcpy(Read_File[dev].filename,"/sojo");
                    strcat(Read_File[dev].filename,"/ConfigurationSet.cfg");
                    break;
                }
                if(!strncmp("UpdateProgram",(char *)&pbuf[i+13],sizeof("UpdateProgram") - 1))
                {
                    strcpy(Read_File[dev].filename,"/sojo");
                    strcat(Read_File[dev].filename,"/UpdateProgram.bin");
//                    log_i("start recive UpdateProgram.bin,size:%d",(unsigned long *)&Write_File[dev].filesize);
                    break;
                }
            }
            if(i>=pbuf[12])
            {
                temp_array[12] = 2;
                memset(&Write_File,0,sizeof(Write_File));
            }
            MyFile = open(Read_File[dev].filename, O_WRONLY, 0);
            perror("Update open");
            ftruncate(MyFile,0);/*清空内容*/
            perror("Update ftruncate");
			break;
		//写文件确认
		case 9:
			temp_array[4] = 5;
			temp_array[0] = 21;
			temp_array[11] = 10;//写文件数据传输确认
			for(crc=0,i=0;i<pbuf[0]-1-21;i++)
			{
				crc += pbuf[i+21];
			}
			if((crc&0xff) != pbuf[pbuf[0]-1])
			{
				temp_array[20] = 2;/*校验和错误*/
//				log_e("crc:%x",crc&0xff);
//				log_e("pbuf[pbuf[0]-1]:%x",pbuf[pbuf[0]-1]);
			}
			else if(Write_File[dev].fileid != *(unsigned long *)&pbuf[11 + 1])
			{
				temp_array[20] = 4;/*文件ID与激活ID不一致*/
//				log_e("temp_array[20] = 4");
			}
			else if(Write_File[dev].filesize < (i + *(unsigned long *)&pbuf[11 + 5]))
			{
				temp_array[20] = 3;/*文件长度不对应*/
//				log_e("temp_array[20] = 3");
			}
			else
			{
				temp_array[20] = 0;/*0成功*/

                {
//					log_i("open Update file:%s",Read_File[dev].filename);
                    MyFile = open(Read_File[dev].filename, O_WRONLY, 0);
                    perror("Update open");

//                    log_i("Update lseek:%d",*(unsigned long *)&pbuf[11 + 5]);
                    lseek(MyFile, *(unsigned long *)&pbuf[11 + 5], SEEK_SET);
                    perror("Update lseek");

//                    log_i("Update write:%d",i);
                    write(MyFile,&pbuf[21], i);
                    perror("Update write");

                    close(MyFile);
                    perror("Update close");
                }

				if(pbuf[20] ==0)//传输完成
                {
//					log_i("recive success");
                    if(strcmp(Read_File[dev].filename,"/sojo/ConfigurationSet.cfg") == 0)//读配置文件
                    {
//                        ReadDoc_Record();///////////////////////////////
                    }
                }
                else
                {
                    temp_array[0] = 0;//不回复数据
                }
			}
			break;
		default:
			break;
	}
    memcpy(pbuf,temp_array,sizeof(temp_array));
}

/**
  * @brief : CreatDoc_Record.
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void CreatDoc_Record(void)
{

}

/**
  * @brief : file_operate_Format.
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void file_operate_Format(void)
{


}

/**
  * @brief : AddDoc_FEVNET.
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t AddDoc_FEVNET(void)
{
	return 0;
}

/**
  * @brief : AddDoc_SOE_CO.
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t AddDoc_SOE_CO(void)
{
	return 0;
}

/**
  * @brief : AddDoc_Random.
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t AddDoc_Random(void)
{
	return 0;
}

/**
  * @brief : start_Comtrade.
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t start_Comtrade(void)
{
	return 0;
}

/**
  * @brief : start_Comtrade.
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
uint8_t save_LinelossFile(void)
{
	return 0;
}

/**
  * @brief : file_operate.
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void file_operate(void)
{

}

/**
  * @brief : file_operate_Clear.
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void file_operate_Clear(void)
{

}

/**
  * @brief : file_operate_Init.
  * @return: none
  * @updata: [YYYY-MM-DD][NAME][BRIEF]
  */
void file_operate_Init(void)
{
    Dir_Array = (struct DIR_ARRAY *)malloc(sizeof(struct DIR_ARRAY));
    Dir_ArrayClear = (struct DIR_ARRAY *)malloc(sizeof(struct DIR_ARRAY));

    memset(&Read_Dir, 0, sizeof(Read_Dir));
    memset(&Read_File, 0, sizeof(Read_File));
}

/* END OF FILE ----------------------------------------------------------------*/
