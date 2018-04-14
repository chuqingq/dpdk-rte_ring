// g++ -o producerconsumerqueue_1p1c{,.cpp} -std=c++14 -O3 -pthread -I/home/chuqq/temp/folly/folly_bin/include -L/home/chuqq/temp/folly/folly_bin/lib -lfolly
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include <folly/ProducerConsumerQueue.h>

#define RING_SIZE 1<<4

static struct rte_ring *r;

static __inline__ unsigned long long nstime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ((unsigned long long)ts.tv_sec)*1e9 + ts.tv_nsec;
}

folly::ProducerConsumerQueue<int> queue(RING_SIZE);
int count = 100000000;

void *enqueue_fun(void *data)
{
    int n = count;
    int i;
    int ret;
    void *p;

    for (i = 0; i < n; i++) {
        // p = (void*)(i+1);
        // ret = -1;
        // while (ret) {
        //     ret = rte_ring_sp_enqueue(r, p);
        // }
        while (!queue.write(i+1))
            ;
    }

    return NULL;
}

void *dequeue_func(void *data)
{
    int ret;
    int i = 0;
    int n = count;
    int p;
    int total_dequeue = 0;

    for (i = 0; i < n; i++) {
        // p = (void*)(i+1);
        // ret = -1;
        // while (ret) {
        //     ret = rte_ring_sc_dequeue(r, (void **)&p);
        //     total_dequeue++;
        // }
        while (!queue.read(p)) {
            total_dequeue++;
        }
        // total_dequeue++;
        // if (p != i+1) {
        //     printf("%d != %d\n", p, i);
        // }
    }

    printf("dequeue_func: total_dequeue=%d\n", total_dequeue);
    return NULL;
}


int main(int argc, char *argv[])
{
    int ret = 0;
    pthread_t pid1, pid2, pid3, pid4, pid5, pid6, pid7, pid8;
    pthread_attr_t pthread_attr;
    // int count = 100000000;
    int per_count = count;

    // r = rte_ring_create("test", RING_SIZE, 0);

    // if (r == NULL) {
    //     return -1;
    // }

    unsigned long long t1, t2;

    t1 = nstime();

    // producers
    pthread_create(&pid1, NULL, enqueue_fun, NULL);

    // consumers
    // dequeue_func((void *)per_count);
    pthread_create(&pid8, NULL, dequeue_func, NULL);

    pthread_join(pid1, NULL);
    pthread_join(pid8, NULL);


    t2 = nstime();
    printf("complete: count=%d, ns diff=%llu\n", count, t2-t1);

    // rte_ring_free(r);
    if (!queue.isEmpty()) {
        printf("queue is not empty\n");
    }

    return 0;
}
