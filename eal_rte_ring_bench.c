#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/queue.h>

#include <rte_memory.h>
#include <rte_launch.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_debug.h>

#include <rte_ring.h>

static __inline__ uint64_t nstime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ((uint64_t)ts.tv_sec)*1e9 + ts.tv_nsec;
}

struct rte_ring *ring;

static int
lcore_hello(__attribute__((unused)) void *arg)
{
        unsigned lcore_id;
        lcore_id = rte_lcore_id();
        printf("hello from core %u\n", lcore_id);
        return 0;
}

static int
lcore_enqueue(uint64_t obj)
{
        int ret = rte_ring_sp_enqueue(ring, (void *)obj);
        if (ret != 0)
                printf("rte_ring_sp_enqueue failed.\n");
        return ret;
}
static int
lcore_dequeue(__attribute__((unused)) void *arg)
{
        void *out;
        while(1)
        {
                int ret = rte_ring_sc_dequeue(ring, (void **)&out);
                if (ret == 0)
                {
                        printf("tooken: %luns\n", nstime() - (uint64_t)out);
                }
        }
        return 0;
}

int
main(int argc, char **argv)
{
        int ret;
        unsigned lcore_id;

        ret = rte_eal_init(argc, argv);
        if (ret < 0)
                rte_panic("Cannot init EAL\n");

        ring = rte_ring_create("TEST", 1024, rte_socket_id(), 0);
        if (ring == NULL)
                rte_panic("ring create failed.\n");

        /* call lcore_hello() on every slave lcore */
        RTE_LCORE_FOREACH_SLAVE(lcore_id) {
                rte_eal_remote_launch(lcore_dequeue, NULL, lcore_id);
        }

        /* call it on master lcore too */
        uint64_t start = nstime();
        lcore_enqueue(start);

        rte_eal_mp_wait_lcore();
        return 0;
}
