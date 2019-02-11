#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "rte_ring.h"

#define RING_SIZE 4

const int count = 100000000;

static struct rte_ring *r;

static __inline__ unsigned long long nstime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ((unsigned long long)ts.tv_sec)*1e9 + ts.tv_nsec;
}

uint8_t buffer[4][1024]; // 4个1024大小的缓冲区

void *enqueue_fun(void *data)
{
    int n = count;
    int i;
    int ret;
    uint8_t *p;

    for (i = 0; i < n; i++) {
        ret = -1;
        while (ret) {
            ret = rte_ring_mc_dequeue(r, (void **)&p);
        }

        // 写入数据
        p[0] = (uint8_t)i;
        printf("produce: %d, %d\n", p[0], i);

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
    int n = count;
    uint8_t *p;

    for (i = 0; i < n; i++) {
        ret = -1;
        while (ret) {
            ret = rte_ring_mc_dequeue(r, (void **)&p);
        }

        // 读取数据并处理
        // printf("dequeue: %d\n", p[0]);

        ret = -1;
        while (ret) {
            ret = rte_ring_mp_enqueue(r, p);
        }
    }

    return NULL;
}


int main(int argc, char *argv[])
{
    int ret = 0;
    pthread_t pid1, pid2, pid3, pid4, pid5, pid6, pid7, pid8;
    pthread_attr_t pthread_attr;
    // int count = 100000000;
    // int per_count = count/2;

    r = rte_ring_create("test", RING_SIZE, 0);

    if (r == NULL) {
        return -1;
    }

    // 先把4个缓冲区都放进去
    rte_ring_mp_enqueue(r, &buffer[0]);
    rte_ring_mp_enqueue(r, &buffer[1]);
    rte_ring_mp_enqueue(r, &buffer[2]);
    rte_ring_mp_enqueue(r, &buffer[3]);

    unsigned long long t1, t2;

    t1 = nstime();

    // producers
    pthread_create(&pid1, NULL, enqueue_fun, NULL);
    // pthread_create(&pid3, NULL, enqueue_fun, (void *)per_count);
    // pthread_create(&pid4, NULL, enqueue_fun, (void *)per_count);

    // consumers
    pthread_create(&pid5, NULL, dequeue_func, NULL);
    pthread_create(&pid6, NULL, dequeue_func, NULL);
    pthread_create(&pid7, NULL, dequeue_func, NULL);
    // pthread_create(&pid8, NULL, dequeue_func, (void *)per_count);

    pthread_join(pid1, NULL);
    t2 = nstime();
    printf("complete: count=%d, ns diff=%llu\n", count, t2-t1);
    // pthread_join(pid2, NULL);
    // pthread_join(pid3, NULL);
    // pthread_join(pid4, NULL);
    pthread_join(pid5, NULL);
    pthread_join(pid6, NULL);
    pthread_join(pid7, NULL);
    // pthread_join(pid8, NULL);


    rte_ring_free(r);

    return 0;
}
