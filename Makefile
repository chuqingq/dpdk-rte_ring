objects = rte_ring_main_1p1c rte_ring_main_2p2c rte_ring_main_4p4c rte_ring_main_5p1c rte_kni_fifo_main_1p1c

all: $(objects)
	@echo "make all success"

%: %.c rte_ring.c
	gcc -O3 $^ -o $@ -lpthread

clean:
	rm -rf $(objects)

