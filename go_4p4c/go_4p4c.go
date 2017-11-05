package main

import (
	"log"
	"sync"
	"time"
)

var (
	threadNum     = 1 // goroutines of producers/consumers
	count     int = 1e8
	percount      = count / threadNum
	ch            = make(chan int, 16)
	wg        sync.WaitGroup
)

func main() {
	t1 := time.Now()

	for i := 0; i < threadNum; i++ {
		go producer()
	}

	wg.Add(threadNum)
	for i := 0; i < threadNum; i++ {
		go consumer()
	}

	wg.Wait()
	t2 := time.Now()

	log.Printf("count: %v, time diff: %v\n", count, t2.Sub(t1))
}

func producer() {
	for i := 0; i < percount; i++ {
		ch <- i
	}
}

func consumer() {
	for i := 0; i < percount; i++ {
		<-ch
	}
	wg.Done()
}
