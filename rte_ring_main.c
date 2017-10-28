#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "rte_ring.h"

#define RING_SIZE 16<<20

typedef struct cc_queue_node {
    int data;
} cc_queue_node_t;

static struct rte_ring *r;

typedef unsigned long long ticks;

static __inline__ ticks getticks(void)
{
    u_int32_t a, d;

    asm volatile("rdtsc" : "=a" (a), "=d" (d));
    return (((ticks)a) | (((ticks)d) << 32));
}

static unsigned long long nstime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ((unsigned long long)ts.tv_sec)*1e9 + ts.tv_nsec;
}


void *enqueue_fun(void *data)
{
    int n = (int)data;
    int i = 0;
    int ret;
    cc_queue_node_t *p;

    for (; i < n; i++) {
        p = (cc_queue_node_t *)malloc(sizeof(cc_queue_node_t));
        p->data = i;
        ret = rte_ring_mp_enqueue(r, p);
        if (ret != 0) {
            printf("enqueue failed: %d\n", i);
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
    cc_queue_node_t *p;
    ticks t1, t2, diff;
    unsigned long long ns1, ns2;
    //return;

    ns1 = nstime();
    t1 = getticks();
    while (1) {
        p = NULL;
        ret = rte_ring_sc_dequeue(r, (void **)&p);
        if (ret != 0) {
            //do something
        }
        if (p != NULL) {
            i++;
            sum += p->data;
            free(p);
            if (i == n) {
                break;
            }
        }
    }

    t2 = getticks();
    ns2 = nstime();
    diff = t2 - t1;
    printf("ticks diff: %llu\n", diff);
    printf("ns diff: %llu\n", ns2-ns1);
    printf("dequeue total: %d, sum: %d\n", i, sum);

    return NULL;
}


int main(int argc, char *argv[])
{
    int ret = 0;
    pthread_t pid1, pid2, pid3, pid4, pid5, pid6;
    pthread_attr_t pthread_attr;
    int count = 10000;

    r = rte_ring_create("test", RING_SIZE, 0);

    if (r == NULL) {
        return -1;
    }

    printf("start enqueue, 5 producer threads, echo thread enqueue %d numbers.\n", count);

    pthread_attr_init(&pthread_attr);
    if ((ret = pthread_create(&pid1, &pthread_attr, enqueue_fun, (void *)count)) == 0) {
        pthread_detach(pid1);
    }

    if ((ret = pthread_create(&pid2, &pthread_attr, enqueue_fun, (void *)count)) == 0) {
        pthread_detach(pid2);
    }

    if ((ret = pthread_create(&pid3, &pthread_attr, enqueue_fun, (void *)count)) == 0) {
        pthread_detach(pid3);
    }
    
    if ((ret = pthread_create(&pid4, &pthread_attr, enqueue_fun, (void *)count)) == 0) {
        pthread_detach(pid4);
    }

    if ((ret = pthread_create(&pid5, &pthread_attr, enqueue_fun, (void *)count)) == 0) {
        pthread_detach(pid5);
    }

    printf("start dequeue, 1 consumer thread, dequeue %d numbers\n", 5*count);

    if ((ret = pthread_create(&pid6, &pthread_attr, dequeue_func, (void *)(5*count))) == 0) {
        //pthread_detach(pid6);
    }
    
    pthread_join(pid6, NULL);

    rte_ring_free(r);

    return 0;
}
