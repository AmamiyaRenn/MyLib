#pragma once

#include "OS/Thread.h"
#include <bits/pthreadtypes.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct TListNode
{
    int               data;
    struct TListNode* next;
} TListNode;

// 并发链表
typedef struct TList
{
    TListNode*      head;
    pthread_mutex_t lock;
} TList;

void TList_init(TList* list)
{
    list->head       = malloc(sizeof(TListNode));
    list->head->data = 0;
    list->head->next = NULL;
    mutex_init(&list->lock);
}

TListNode* TList_insert(TList* list, int e)
{
    TListNode* new_node = malloc(sizeof(TListNode));
    if (new_node == NULL)
    {
        perror("malloc");
        return NULL; // fail
    }
    mutex_lock(&list->lock); // 假设malloc线程安全，则只需对真正的临界区上锁
    list->head->data++;      // 首节点记录链表数据个数
    new_node->data   = e;
    new_node->next   = list->head->next;
    list->head->next = new_node;
    mutex_unlock(&list->lock);
    return new_node;
}

TListNode* TList_find(TList* list, int e)
{
    mutex_lock(&list->lock);
    TListNode* curr_node = list->head->next;
    TListNode* ret_val   = NULL;
    while (curr_node != NULL)
    {
        if (curr_node->data == e)
        {
            ret_val = curr_node;
            break; // 减少上锁/解锁的次数
        }
        curr_node = curr_node->next;
    }
    mutex_unlock(&list->lock);
    return ret_val;
}
