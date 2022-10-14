#pragma once

#include <pthread.h>
#include <assert.h>

#define NTHREAD 64

enum
{
    T_FREE,
    T_LIVE,
    T_DEAD,
};

typedef struct ThreadStruct
{
    int id;
    int state;
    pthread_t thread;
    void (*tfn)(int);
} Thread;

Thread tpool[NTHREAD], *tcur = tpool;

void *wrapper(void *arg)
{
    Thread *thread = (Thread *)arg;
    thread->tfn(thread->id);
    return NULL;
}

// 创造线程
void create(void *fn)
{
    assert(tcur - tpool < NTHREAD);
    *tcur = (struct ThreadStruct){
        .id = (int)(tcur - tpool + 1),
        .state = T_LIVE,
        .tfn = fn,
    };
    pthread_create(&(tcur->thread), NULL, wrapper, tcur);
}

// 等待所有线程结束
void join()
{
    for (int i = 0; i < NTHREAD; i++)
    {
        Thread *t = &tpool[i];
        if (t->state == T_LIVE)
        {
            pthread_join(t->thread, NULL);
            t->state = T_DEAD;
        }
    }
}

// 退出程序时先结束所有线程
__attribute__((destructor)) void cleanup() { join(); }
