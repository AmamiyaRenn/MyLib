#pragma once

#include "OS/Thread.h"
#include <string.h>

#define CPUNUMS 4

typedef struct LazyCounterT
{
    int             global;
    pthread_mutex_t g_lock;
    int             local[CPUNUMS];
    pthread_mutex_t l_lock[CPUNUMS];
    int             threshold;
} LazyCounterT;

void LazyCounter_init(LazyCounterT* counter, int threshold)
{
    counter->threshold = threshold;

    counter->global = 0;
    pthread_mutex_init(&counter->g_lock, NULL);

    memset(&counter->local, 0, CPUNUMS * sizeof(int));
    for (int i = 0; i < CPUNUMS; i++)
        pthread_mutex_init(&counter->l_lock[i], NULL);
};

void LazyCounter_update(LazyCounterT* counter, int threadID, int amount)
{
    mutex_lock(&counter->l_lock[threadID]);
    counter->local[threadID] += amount;
    if (counter->local[threadID] >= counter->threshold)
    {
        mutex_lock(&counter->g_lock);
        counter->global += counter->local[threadID];
        mutex_unlock(&counter->g_lock);
        counter->local[threadID] = 0;
    }
    mutex_unlock(&counter->l_lock[threadID]);
}

int LazyCounter_get(LazyCounterT* counter)
{
    mutex_lock(&counter->g_lock);
    int val = counter->global;
    mutex_unlock(&counter->g_lock);
    return val;
}