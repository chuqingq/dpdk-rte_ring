// rte_ring多进程共享内存版本
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/shm.h>

#include "rte_ring.h"

#define RING_SIZE 4

const int count = 100000000;
#define ENTRY_NUM 4
#define BUF_SIZE 10
#define SHM_KEY 1200

typedef struct {
    uint8_t *buf_base;
    uint8_t buf[ENTRY_NUM*BUF_SIZE];
    struct rte_ring r;
} mpmc_t;

mpmc_t *mpmc;

void *enqueue_fun(void *not_used)
{
    int ret;
    long p; // index buf的槽位
    for (int i = 0; i < count; i++) {
        ret = -1;
        while (ret) {
            ret = rte_ring_mc_dequeue(&mpmc->r, (void **)&p);
        }

        // 写入数据
        mpmc->buf[p] = (uint8_t)i;
        printf("produce: %ld, %d\n", p, mpmc->buf[p]);

        ret = -1;
        while (ret) {
            ret = rte_ring_mp_enqueue(&mpmc->r, (void *)p);
        }
    }

    return NULL;
}

void *dequeue_func(void *not_used)
{
    int ret;
    long p;

    for (int i = 0; i < count; i++) {
        ret = -1;
        while (ret) {
            ret = rte_ring_mc_dequeue(&mpmc->r, (void **)&p);
        }

        // 读取数据并处理
        printf("dequeue: %ld, %d\n", p, mpmc->buf[p]);

        ret = -1;
        while (ret) {
            ret = rte_ring_mp_enqueue(&mpmc->r, (void *)p);
        }
    }

    return NULL;
}


int main_p()
{
    int ret;

    // 改为shm
    int shmid = shmget((key_t)SHM_KEY, sizeof(*mpmc)+4*sizeof(void *), 0666|IPC_CREAT);
	if(shmid == -1)
	{
		// fprintf(stderr, "shmget failed\n");
        perror("shmget failed111");
		exit(-1);
	}
	//将共享内存连接到当前进程的地址空间
	void *shm = shmat(shmid, (void*)0, 0);
	if(shm == (void*)-1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(-1);
	}
    mpmc = (mpmc_t *)shm;


    struct rte_ring *r = rte_ring_init(&mpmc->r, "test", RING_SIZE, 0);
    uint8_t *buffer = mpmc->buf;
    mpmc->buf_base = mpmc->buf;


    if (r == NULL) {
        return -1;
    }

    // 先把4个空缓冲区都放进去
    rte_ring_mp_enqueue(r, (void*)0);
    rte_ring_mp_enqueue(r, (void*)1);
    rte_ring_mp_enqueue(r, (void*)2);
    ret = rte_ring_mp_enqueue(r, (void*)3);
    if (ret != 0) {
        perror("enqueue err");
    }

    // unsigned long long t1, t2;

    // t1 = nstime();

    // producers
    // pthread_create(&pid1, NULL, enqueue_fun, NULL);
    enqueue_fun(NULL);
    // pthread_create(&pid3, NULL, enqueue_fun, (void *)per_count);
    // pthread_create(&pid4, NULL, enqueue_fun, (void *)per_count);

    // consumers
    // pthread_create(&pid5, NULL, dequeue_func, NULL);
    // pthread_create(&pid6, NULL, dequeue_func, NULL);
    // pthread_create(&pid7, NULL, dequeue_func, NULL);
    // pthread_create(&pid8, NULL, dequeue_func, (void *)per_count);

    // pthread_join(pid1, NULL);
    // pthread_join(pid2, NULL);
    // pthread_join(pid3, NULL);
    // pthread_join(pid4, NULL);
    // pthread_join(pid5, NULL);
    // pthread_join(pid6, NULL);
    // pthread_join(pid7, NULL);
    // pthread_join(pid8, NULL);

    // t2 = nstime();
    // printf("complete: count=%d, ns diff=%llu\n", count, t2-t1);

    // rte_ring_free(r);
    // free(mpmc);

    return 0;
}

int main_c()
{
    int ret = 0;
    // pthread_t pid1, pid2, pid3, pid4, pid5, pid6, pid7, pid8;
    // pthread_attr_t pthread_attr;
    // int count = 100000000;
    // int per_count = count/2;

    // r = rte_ring_create("test", RING_SIZE, 0);

    // mpmc = (mpmc_t *)malloc(sizeof(*mpmc)+count*sizeof(void *));

    // 改为shm
    int shmid = shmget((key_t)SHM_KEY, sizeof(*mpmc)+4*sizeof(void *), 0666|IPC_CREAT);
	if(shmid == -1)
	{
		// fprintf(stderr, "shmget failed\n");
        perror("shmget failed111");
		exit(-1);
	}
	//将共享内存连接到当前进程的地址空间
	void *shm = shmat(shmid, (void*)0, 0);
	if(shm == (void*)-1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(-1);
	}
    mpmc = (mpmc_t *)shm;


    // struct rte_ring *r = rte_ring_init(&mpmc->r, "test", RING_SIZE, 0);
    // uint8_t *buffer = mpmc->buf;
    printf("buf: %p\n", mpmc->buf);
    struct rte_ring *r = &mpmc->r;

    if (r == NULL) {
        return -1;
    }

    // 先把4个缓冲区都放进去
    // rte_ring_mp_enqueue(r, &buffer[0]);
    // rte_ring_mp_enqueue(r, &buffer[1]);
    // rte_ring_mp_enqueue(r, &buffer[2]);
    // rte_ring_mp_enqueue(r, &buffer[3]);

    // unsigned long long t1, t2;

    // t1 = nstime();

    // producers
    // pthread_create(&pid1, NULL, enqueue_fun, NULL);
    // pthread_create(&pid3, NULL, enqueue_fun, (void *)per_count);
    // pthread_create(&pid4, NULL, enqueue_fun, (void *)per_count);

    // consumers
    // pthread_create(&pid5, NULL, dequeue_func, NULL);
    dequeue_func(NULL);
    // pthread_create(&pid6, NULL, dequeue_func, NULL);
    // pthread_create(&pid7, NULL, dequeue_func, NULL);
    // pthread_create(&pid8, NULL, dequeue_func, (void *)per_count);

    // pthread_join(pid1, NULL);
    // pthread_join(pid2, NULL);
    // pthread_join(pid3, NULL);
    // pthread_join(pid4, NULL);
    // pthread_join(pid5, NULL);
    // pthread_join(pid6, NULL);
    // pthread_join(pid7, NULL);
    // pthread_join(pid8, NULL);

    // t2 = nstime();
    // printf("complete: count=%d, ns diff=%llu\n", count, t2-t1);

    // rte_ring_free(r);
    // free(mpmc);

    return 0;
}

int main() {
    return main_p();
    // return main_c();
}
