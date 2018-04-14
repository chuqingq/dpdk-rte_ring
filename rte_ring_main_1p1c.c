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
    // int ret;
    void *p;

    for (i = 0; i < n; i++) {
        // p = (void*)(i+1);
        // ret = -1;
        // while (ret) {
        //     ret = rte_ring_sp_enqueue(r, p);
        // }
        while (rte_ring_sp_enqueue(r, (void*)(i+1)))
            ;
    }

    return NULL;
}

void *dequeue_func(void *data)
{
    // int ret;
    int i = 0;
    int n = (int)data;
    void *p;
    int total_dequeue = 0;

    for (i = 0; i < n; i++) {
        //p = (void*)(i+1);
        // ret = -1;
        // while (ret) {
        //     ret = rte_ring_sc_dequeue(r, (void **)&p);
        //     total_dequeue++;
        // }
        while (rte_ring_sc_dequeue(r, (void **)&p))
            total_dequeue++;
    }

    printf("dequeue_func: total_dequeue=%d\n", total_dequeue);
    return NULL;
}


int main(int argc, char *argv[])
{
    int ret = 0;
    pthread_t pid1, pid2, pid3, pid4, pid5, pid6, pid7, pid8;
    pthread_attr_t pthread_attr;
    int count = 100000000;
    int per_count = count;

    r = rte_ring_create("test", RING_SIZE, 0);

    if (r == NULL) {
        return -1;
    }

    unsigned long long t1, t2;

    t1 = nstime();

    // producers
    pthread_create(&pid1, NULL, enqueue_fun, (void *)per_count);

    // consumers
    // dequeue_func((void *)per_count);
    pthread_create(&pid8, NULL, dequeue_func, (void *)per_count);

    pthread_join(pid1, NULL);
    pthread_join(pid8, NULL);


    t2 = nstime();
    printf("complete: count=%d, ns diff=%llu\n", count, t2-t1);

    rte_ring_free(r);

    return 0;
}
