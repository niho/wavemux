#include <stdlib.h>
#include <string.h>

#include "fifo.h"
#include "atomic.h"


void wavemux_fifo_init(FIFO *fifo, int capacity, int element_size) {
    memset(fifo, 0, sizeof(FIFO));
    fifo->capacity = capacity + 1;
    fifo->element_size = element_size;
    fifo->elements = malloc(fifo->capacity * element_size);
}

void wavemux_fifo_deinit(FIFO *fifo) {
    if (fifo->elements) {
        free(fifo->elements);
    }
    memset(fifo, 0, sizeof(FIFO));
}

static int increment(int idx, int capacity)
{
    return (idx + 1) % capacity;
}

int wavemux_fifo_is_empty(FIFO* fifo) {
    return wavemux_atomic_load(&fifo->head) == wavemux_atomic_load(&fifo->tail);
}

int wavemux_fifo_is_full(FIFO* fifo) {
    const int nextTail = increment(wavemux_atomic_load(&fifo->tail), fifo->capacity);
    return nextTail == wavemux_atomic_load(&fifo->head);
}

int wavemux_fifo_get_num_elements(FIFO* fifo) {
    const int currentTail = wavemux_atomic_load(&fifo->tail);
    const int currentHead = wavemux_atomic_load(&fifo->head);
    
    const int d = currentTail - currentHead;
    return d < 0 ? d + fifo->capacity : d;
}

int wavemux_fifo_push(FIFO* fifo, const void* element)
{    
    int currentTail = wavemux_atomic_load(&fifo->tail);
    const int nextTail = increment(currentTail, fifo->capacity);
    if(nextTail != wavemux_atomic_load(&fifo->head))
    {
        memcpy(&(((unsigned char*)fifo->elements)[currentTail * fifo->element_size]), element, fifo->element_size);
        wavemux_atomic_store(nextTail, &fifo->tail);
        return 1;
    }
    return 0;
}

int wavemux_fifo_pop(FIFO* fifo, void* element)
{
    const int currentHead = wavemux_atomic_load(&fifo->head);
    if(currentHead == wavemux_atomic_load(&fifo->tail))
    {
        return 0; // empty queue
    }
    
    memcpy(element, &fifo->elements[currentHead * fifo->element_size], fifo->element_size);
    wavemux_atomic_store(increment(currentHead, fifo->capacity), &fifo->head);
    return 1;
}

