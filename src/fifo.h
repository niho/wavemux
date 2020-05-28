#ifndef WAVEMUX_FIFO_H
#define WAVEMUX_FIFO_H

typedef struct FIFO {
    int capacity;
    int element_size;
    void *elements;
    int head;
    int tail;
} FIFO;

void    wavemux_fifo_init(FIFO *fifo, int capacity, int element_size);
void    wavemux_fifo_deinit(FIFO *fifo);

int     wavemux_fifo_isempty(FIFO *fifo);
int     wavemux_fifo_isfull(FIFO *fifo);
int     wavemux_fifo_get_num_elements(FIFO *fifo);

int     wavemux_fifo_push(FIFO *fifo, const void *element);
int     wavemux_fifo_pop(FIFO *fifo, void *element);

#endif
