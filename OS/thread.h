#pragma once

#include <pthread.h>

void create(void *fn);
void joinAll();
void thread_mutex_init(pthread_mutex_t *mutex);
void thread_mutex_lock(pthread_mutex_t *mutex);
