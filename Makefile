objects = rte_ring_main_1p1c rte_ring_main_2p2c rte_ring_main_4p4c rte_ring_main_3p1c

all: $(objects)
	echo "make all success"

%: %.c rte_ring.c
	gcc -g -O3 $^ -o $@ -lpthread

clean:
	rm -rf $(objects)

