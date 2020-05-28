/**
  * @copyright  Copyright (C) SOJO Electric CO., Ltd. 2018-2019. All right reserved.
  * @file:      slist.h
  * @brief:     the device driver of single list
  * @version:   V1.0.0
  * @author:    Sunxr
  * @date:      2018-10-25
  * @update:    [2018-10-25][Sunxr][newly increased]
  */

#ifndef _SLIST_H_
#define _SLIST_H_

/* INCLUDE --------------------------------------------------------------------------*/
#include <stdint.h>

/* DEFINE ---------------------------------------------------------------------------*/

/* ENUM -----------------------------------------------------------------------------*/

/* STRUCT ---------------------------------------------------------------------------*/
typedef struct snode
{
	void *data;
	struct snode *next;
}node;

struct slist
{
	int size;
	node *head;
	node *tail;
	void (*destroy)(void *data);
};

/* PUBLIC FUNCTION ------------------------------------------------------------------*/
void slist_new(struct slist *list, void (*destroy)(void *data));
void slist_destory(struct slist *list);
int slist_ins_next(struct slist *list, node *ins_node, const void *data);
int slist_rem_next(struct slist *list, node *rem_node, void **data);

#define list_size(list)                             ((list)->size)
#define list_head(list)                             ((list)->head)
#define list_tail(list)                             ((list)->head)
#define list_data(node)                             ((node)->data)
#define list_next(node)                             ((node)->next)
#define list_is_head(list, node)                    ((node) == (list)->head ? 1 : 0)
#define list_is_tail(node)                          ((node)->next == NULL ? 1 : 0)

#endif /* _SLIST_H_ */

/* FILE END  ------------------------------------------------------------------------*/
