#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h> 

#include "thread.h"
#include "stream.h"

#define PORT 8080


static void *wavemux_connection_main(void *arg) {
    wavemux_thread_t *thr = (wavemux_thread_t *)arg;
    wavemux_stream_init(thr);
    wavemux_thread_pool_remove(thr);
    wavemux_thread_deinit(thr);
    return NULL;
}

int main(void) {
    int server_fd, new_socket; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address);  
    char *hello = "Hello from server"; 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    }

    printf("Listening on port %d...\n", PORT);

    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                         (socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        }
        printf("New connection from %s:%d\n", inet_ntoa(address.sin_addr), address.sin_port);
        wavemux_thread_init(wavemux_connection_main, new_socket);
    }

    send(new_socket , hello , strlen(hello) , 0 ); 
    printf("Hello message sent\n"); 

    return 0;
}

