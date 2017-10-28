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

    > ./main
    start enqueue, 5 producer threads, echo thread enqueue 10000000 numbers.
    start dequeue, 1 consumer thread, dequeue 50000000 numbers
    ticks diff: 13458364608
    ns diff: 3949222400
    dequeue total: 50000000, sum: -1431365568

