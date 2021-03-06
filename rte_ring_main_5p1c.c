#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "rte_ring.h"

#define RING_SIZE 1<<4

static struct rte_ring *r;

static __inline__ unsigned long long nstime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ((unsigned long long)ts.tv_sec)*1e9 + ts.tv_nsec;
}


void *enqueue_fun(void *data)
{
    int n = (int)data;
    int i;
    int ret;
    void *p;

    for (i = 0; i < n; i++) {
        p = (void*)(i+1);
        ret = -1;
        while (ret) {
            ret = rte_ring_mp_enqueue(r, p);
        }
    }

    return NULL;
}

void *dequeue_func(void *data)
{
    int ret;
    int i = 0;
    int sum = 0;
    int n = (int)data;
    void *p;

    for (i = 0; i < n; i++) {
        p = (void*)(i+1);
        ret = -1;
        while (ret) {
            ret = rte_ring_sc_dequeue(r, (void **)&p);
        }
    }

    return NULL;
}


int main(int argc, char *argv[])
{
    int ret = 0;
    pthread_t pid1, pid2, pid3, pid4, pid5, pid6;
    pthread_attr_t pthread_attr;
    int count = 100000000;
    int per_count = count/5;

    r = rte_ring_create("test", RING_SIZE, 0);

    if (r == NULL) {
        return -1;
    }

    printf("starting...\n");

    unsigned long long t1, t2;

    t1 = nstime();

    // producer
    pthread_create(&pid1, NULL, enqueue_fun, (void *)per_count);
    pthread_create(&pid2, NULL, enqueue_fun, (void *)per_count);
    pthread_create(&pid3, NULL, enqueue_fun, (void *)per_count);
    pthread_create(&pid4, NULL, enqueue_fun, (void *)per_count);
    pthread_create(&pid5, NULL, enqueue_fun, (void *)per_count);

    // consumer
    dequeue_func((void *)count);

    pthread_join(pid1, NULL);
    pthread_join(pid2, NULL);
    pthread_join(pid3, NULL);
    pthread_join(pid4, NULL);
    pthread_join(pid5, NULL);

    t2 = nstime();
    printf("complete: count=%d, ns diff=%llu\n", count, t2-t1);

    rte_ring_free(r);

    return 0;
}
