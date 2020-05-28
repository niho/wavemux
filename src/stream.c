#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include <ogg/ogg.h>
#include "thread.h"
#include "stream.h"

#define BUF_LEN 8192


void wavemux_stream_write(wavemux_thread_t *thr, ogg_page *og) {
    if (write(thr->socket, og->header, og->header_len) == -1) {
        perror("socket write failed");
    }
    if (write(thr->socket, og->body, og->body_len) == -1) {
        perror("socket write failed");
    }
}

void wavemux_stream_read(ogg_sync_state *oy, int socket) {
    char *buffer;
    size_t num_bytes;
    ogg_page og;
 
    while(1) {
        if (ogg_sync_pageout(oy, &og) == 1) {
            printf("(%d) ogg_sync_pageout %ld %ld\n", socket, og.header_len, og.body_len);
            wavemux_thread_pool_each((wavemux_thread_iter_t)wavemux_stream_write, &og);
        }

        buffer = ogg_sync_buffer(oy, BUF_LEN);
        if (buffer == NULL) {
            printf("ogg_sync_buffer error\n");
            return;
        }

        num_bytes = read(socket, buffer, BUF_LEN);

        printf("(%d) read num bytes: %zu\n", socket, num_bytes);
        
        if (num_bytes <= 0) {
            break;
        }

        if (ogg_sync_wrote(oy, num_bytes) == -1) {
            printf("(%d) ogg_sync_wrote buffer overflow\n", socket);
        }
    }

    printf("(%d) connection closed\n", socket);
}

void wavemux_stream_init(wavemux_thread_t *thr) { 
    ogg_sync_state oy;
    ogg_sync_init(&oy);

    wavemux_stream_read(&oy, thr->socket);

    ogg_sync_clear(&oy);
}

