# benchmark result

* 原版：count=10000, time=6.1ms
* 去掉malloc，直接赋值整数：count=10000, time=2.1ms

在chuqq-lubuntu-home上验证：

* count=10000000
    * ticks diff: 22420985796
    * ns diff: 6579135488
* -O3
    * ticks diff: 14132481306
    * ns diff: 4146670848
    * 4427956224/50000000=82 平均每个操作花82ns


* RING_SIZE从1<<24改成1<<4，count仍然是1000w

```
> ./main
start enqueue, 5 producer threads, echo thread enqueue 10000000 numbers.
start dequeue, 1 consumer thread, dequeue 50000000 numbers
ticks diff: 13458364608
ns diff: 3949222400
dequeue total: 50000000, sum: -1431365568
```

# throughput

`rte_ring`的size都是16
* `rte_ring`, count=1e8, 1p1c
    * complete: count=100000000, ns diff=1295983104
* `kni_fifo`, count=1e8, 1p1c
    * complete: count=100000000, ns diff=1900617216
* `folly.UMPMCBlockingQueue`, count=1e8, 4p4c
    * count: 100000000, elapsed: 7534004338 ns
* `rte_ring`, count=1e8, 2p2c
    * complete: count=100000000, ns diff=8329000704
* `rte_ring`, count=1e8, 5p1c
    * complete: count=100000000, ns diff=7801386240
* `aring`, count=1e8, 1p1c
    * Complete: count: 100000000, ns diff: 8928640512
* `rte_ring`, count=1e8, 4p4c
    * complete: count=100000000, ns diff=11837379072
* `golang.channel`, count=1e8, 4p4c
    * 2017/11/05 17:48:22 count: 100000000, time diff: 9.213674116s
* `golang.channel`, count=1e8, 2p2c
    * 2017/11/05 17:48:47 count: 100000000, time diff: 7.282469757s
* `golang.channel`, count=1e8, 1p1c
    * 2017/11/05 17:49:00 count: 100000000, time diff: 5.829984881s

# delay

在高性能服务器（E9000）上验证：
* 在EAL环境下，`rte_ring`的1p1c大约768ns
* 在非EAL环境下（已提取），`rte_ring`的1p1c大约6~7us
也就是10倍的关系，可见EAL环境可能做了一些特殊处理，或者提取时用的普通malloc性能较差。

