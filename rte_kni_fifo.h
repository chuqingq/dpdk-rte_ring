
/*
 * Fifo struct mapped in a shared memory. It describes a circular buffer FIFO
 * Write and read should wrap around. Fifo is empty when write == read
 * Writing should never overwrite the read position
 */
struct rte_kni_fifo {
	volatile unsigned write;     /**< Next position to be written*/
	volatile unsigned read;      /**< Next position to be read */
	unsigned len;                /**< Circular buffer length */
	unsigned elem_size;          /**< Pointer size - for 32/64 bit OS */
	void *volatile buffer[];     /**< The buffer contains mbuf pointers */
};



/**
 * Initializes the kni fifo structure
 */
static void
kni_fifo_init(struct rte_kni_fifo *fifo, unsigned size)
{
	/* Ensure size is power of 2 */
	if (size & (size - 1)) {
		printf("KNI fifo size must be power of 2\n");
		exit(-1);
	}

	fifo->write = 0;
	fifo->read = 0;
	fifo->len = size;
	fifo->elem_size = sizeof(void *);
}

/**
 * Adds num elements into the fifo. Return the number actually written
 */
static inline unsigned
kni_fifo_put(struct rte_kni_fifo *fifo, void **data, unsigned num)
{
	unsigned i = 0;
	unsigned fifo_write = fifo->write;
	unsigned fifo_read = fifo->read;
	unsigned new_write = fifo_write;

	for (i = 0; i < num; i++) {
		new_write = (new_write + 1) & (fifo->len - 1);

		if (new_write == fifo_read)
			break;
		fifo->buffer[fifo_write] = data[i];
		fifo_write = new_write;
	}
	fifo->write = fifo_write;
	return i;
}

/**
 * Get up to num elements from the fifo. Return the number actully read
 */
static inline unsigned
kni_fifo_get(struct rte_kni_fifo *fifo, void **data, unsigned num)
{
	unsigned i = 0;
	unsigned new_read = fifo->read;
	unsigned fifo_write = fifo->write;
	for (i = 0; i < num; i++) {
		if (new_read == fifo_write)
			break;

		data[i] = fifo->buffer[new_read];
		new_read = (new_read + 1) & (fifo->len - 1);
	}
	fifo->read = new_read;
	return i;
}


