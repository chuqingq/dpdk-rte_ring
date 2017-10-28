objects = rte_ring_main.o rte_ring.o
CC = gcc

all: $(objects)
	$(CC) -g -O3 -o main $(objects) -lpthread

$(objects): %.o: %.c
	$(CC) -c $(CFLAGS) -O3 $< -o $@

clean:
	rm -f main *.o
