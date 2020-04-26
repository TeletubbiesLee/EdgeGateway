/**
  * @copyright  Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      comtrade.c
  * @brief:     the device driver of read comtrade
  * @version:   V1.0.0
  * @author:    Sunxr
  * @date:      2018-09-04
  * @update:    [2018-09-04][Sunxr][newly increased]
  */

/* INCLUDE --------------------------------------------------------------------------*/
#include "slist.h"

#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



/* PRIVATE VARIABLES ----------------------------------------------------------------*/


/* PRIVATE FUNCTION -----------------------------------------------------------------*/


/* PUBLIC FUNCTION ------------------------------------------------------------------*/
/**
  * @brief : 链表初始化
  * @param : [list]-链表指针
  * @param : [(*destroy)(void *data)]-用户自定义的数据内存释放函数，不释放写NULL
  * @return: none
  * @update: [2018-10-25][Sunxr][newly increased]
  */
void slist_new(struct slist *list, void (*destroy)(void *data))
{
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    list->destroy = destroy;
}

/**
  * @brief : 向链表插入节点 若ins_node == NULL 头插法插入新节点 若ins_node != NULL 将新节点插入到ins_node节点后方
  * @param : [list]-链表指针
  * @param : [ins_node]-插入点
  * @param : [data]-节点数据
  * @return: [-1]-失败
  * @return: [0]-成功
  * @update: [2018-10-25][Sunxr][newly increased]
  */
int slist_ins_next(struct slist *list, node *ins_node, const void *data)
{
	node *new_node = NULL;

	/* 申请空间*/
    if ((new_node = (node *)malloc(sizeof(node))) == NULL)
    {
    	return -1;
    }

    new_node->data = (void *)data;

    if (ins_node == NULL)
    {
        if (list_size(list) == 0)
        {
        	list->tail = new_node;
        }

        /* 在头部插入新节点 */
        new_node->next = list->head;
        list->head = new_node;
    }
    else
    {
    	if (ins_node->next == NULL)
    	{
            list->tail = new_node;
    	}

    	/* 将新节点插入到插入点后方 */
    	new_node->next = ins_node->next;
    	ins_node->next = new_node;
    }

    list->size++;

    return 0;
}

/**
  * @brief : 从链表中移除节点
  * @param : [list]-链表指针
  * @param : [ins_node]-移除点
  * @param : [data]-节点数据
  * @return: [-1]-失败
  * @return: [0]-成功
  * @update: [2018-10-25][Sunxr][newly increased]
  */
int slist_rem_next(struct slist *list, node *rem_node, void **data)
{
	node *old_node = NULL;

	/* 链表为空  直接返回 */
    if (list_size(list) == 0)
    {
    	return -1;
    }

    if (rem_node == NULL)
    {
    	/* 移除头节点 */
    	*data = list->head->data;
        old_node = list->head;
        list->head = list->head->next;

        if (list_size(list) == 1)
        {
        	list->tail = NULL;
        }
    }
    else
    {
        if (rem_node->next == NULL)
        {
            return -1;
        }

        /* 移除rem_node节点后的节点 */
        *data = rem_node->next->data;
        old_node = rem_node->next;
        rem_node->next = rem_node->next->next;

        if (rem_node->next == NULL)
        {
        	list->tail = rem_node;
        }
    }

    free(old_node);
    list->size--;

	return 0;
}

/**
  * @brief : 从链表中移除当前节点
  * @param : [list]-链表指针
  * @param : [ins_node]-当前移除点
  * @param : [data]-节点数据
  * @return: [-1]-失败
  * @return: [0]-成功
  * @update: [2018-10-25][Sunxr][newly increased]
  */
int slist_rem_node(struct slist *list, node *rem_node, void **data)
{
    node *temp_node = list->head;

    if (list == NULL || rem_node == NULL || data == NULL)
    {
        return -1;
    }

    if (rem_node == temp_node)      /* 如果是头结点 */
    {
        slist_rem_next(list, NULL, data);
    }

    while (temp_node->next != NULL) 
    {
        if (temp_node->next == rem_node) 
        {
            slist_rem_next(list, temp_node, data);
            break;
        }
        temp_node = temp_node->next;
    }

    return 0;
}

/**
  * @brief : 销毁链表
  * @param : [list]-链表指针
  * @return: none
  * @update: [2018-10-25][Sunxr][newly increased]
  */
void slist_destory(struct slist *list)
{
    void *data = NULL;

    while (list_size(list) > 0)
    {
    	if (slist_rem_next(list, NULL, (void **)&data) == 0 && list->destroy != NULL)
    	{
            list->destroy(data);
    	}
    }

    memset(list, 0, sizeof(struct slist));
}

/* FILE END  ------------------------------------------------------------------------*/

