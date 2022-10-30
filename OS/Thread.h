#pragma once

#include <assert.h>
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>

#define NTHREAD 64

enum
{
    T_FREE,
    T_LIVE,
    T_DEAD,
};

typedef struct Thread
{
    int       id;
    int       state;
    pthread_t thread;
    void (*tfn)(int);
} Thread;

Thread tpool[NTHREAD], *tcur = tpool;

void* wrapper(void* arg)
{
    Thread* thread = (Thread*)arg;
    thread->tfn(thread->id);
    return NULL;
}

// 创造线程
void create(void* fn)
{
    assert(tcur - tpool < NTHREAD);
    *tcur = (struct Thread) {
        .id    = (int)(tcur - tpool + 1),
        .state = T_LIVE,
        .tfn   = fn,
    };
    pthread_create(&(tcur->thread), NULL, wrapper, tcur);
    tcur++;
}

// 等待所有线程结束
void joinAll()
{
    for (int i = 0; i < NTHREAD; i++)
    {
        Thread* t = &tpool[i];
        if (t->state == T_LIVE)
        {
            pthread_join(t->thread, NULL);
            t->state = T_DEAD;
        }
    }
}

// 退出程序时先结束所有线程
__attribute__((destructor)) void cleanup() { joinAll(); }

static inline int atomatic_xchg(volatile int* addr, int newval)
{
    int result;
    asm volatile("lock xchg %0, %1" : "+m"(*addr), "=a"(result) : "1"(newval) : "memory");
    return result;
}

typedef int SpinlockT;
#define SPIN_INIT() 0
void spin_lock(SpinlockT* lock)
{
    while (atomatic_xchg(lock, 1))
        ;
}
void spin_unlock(SpinlockT* lock) { atomatic_xchg(lock, 0); }

// 这个封装会assert上锁
void mutex_lock(pthread_mutex_t* mutex) { assert(pthread_mutex_lock(mutex) == 0); }
void mutex_unlock(pthread_mutex_t* mutex) { pthread_mutex_unlock(mutex); }
void mutex_init(pthread_mutex_t* mutex) { assert(pthread_mutex_init(mutex, NULL) == 0); }
void cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex) { pthread_cond_wait(cond, mutex); }
void cond_signal(pthread_cond_t* cond) { pthread_cond_signal(cond); }

typedef struct ZemT
{
    int             value;
    pthread_cond_t  cond;
    pthread_mutex_t lock;
} ZemT;

void Zem_init(ZemT* s, int value)
{
    s->value = value;
    pthread_cond_init(&s->cond, NULL);
    pthread_mutex_init(&s->lock, NULL);
}

void Zem_wait(ZemT* s)
{
    mutex_lock(&s->lock);
    while (s->value <= 0)
        cond_wait(&s->cond, &s->lock);
    s->value--; // 这句放到while前会导致逻辑出问题从而死锁
    mutex_unlock(&s->lock);
}

void Zem_post(ZemT* s)
{
    mutex_lock(&s->lock);
    s->value++;
    cond_signal(&s->cond);
    mutex_unlock(&s->lock);
}