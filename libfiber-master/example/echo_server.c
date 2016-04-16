/*
 * Copyright (c) 2012-2015, Brian Watling and other contributors
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

//#include "fiber_manager.h"
#include "../include/fiber.h"
#include "../include/fiber_manager.h"

int start_server(const char* host, const char* port);
int volatile counter = 0;
fiber_mutex_t mutex;

// This function reads data from a socket and echos it back. Both read() and write()
// operations will block within the context of this function. The fiber runtime will
// intercept calls to read() or write and switch fibers if they will block. Basically,
// we write blocking code and libfiber makes it event driven.
void* client_function(void* param)
{
  //while(1)
  //printf("Hello from %d", *((int*)param));
    /*const int sock = (int)(intptr_t)param;
    char buffer[256];
    ssize_t num_read;
    while((num_read = read(sock, buffer, sizeof(buffer))) > 0) {
        if(num_read != write(sock, buffer, num_read)) {
            break;
        }
    }
    close(sock);
    return NULL;*/

    ++counter;

    pthread_t thread = pthread_self();
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    int j;
    int temp = 0;
    for(j = 0; j < 1000000; j++)
        temp++;

    int s = pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset);


    /*printf("Set returned by pthread_getaffinity_np() contained:\n");
    for (j = 0; j < CPU_SETSIZE; j++)
        if (CPU_ISSET(j, &cpuset))
            printf("CPU %d\n", j);

    printf("Hello Client 1\n");*/
    fiber_change(3);
    /*while(1){
      printf("Hello from %d\n", *((int*)param));
    }*/
    //sleep(5);
    //printf("Hello Client 2\n");
    pthread_t thread1 = pthread_self();
    CPU_ZERO(&cpuset);
    int s1 = pthread_getaffinity_np(thread1, sizeof(cpu_set_t), &cpuset);



    for(j = 0; j < 1000000; j++)
        temp++;

    /*printf("Set returned by pthread_getaffinity_np() contained:\n");
    for (j = 0; j < CPU_SETSIZE; j++) {
        //printf("in CPU\n");
        if (CPU_ISSET(j, &cpuset))
            printf("CPU %d\n", j);
    }

    printf("Value of temp is: %d\n", temp);*/

    return NULL;
}

// This is a simple echo server using fibers. The main() function opens a blocking
// listening socket. A new fiber is spawned for each client.
int main()
{


    //printf("\n Hello Client -2");
    fiber_manager_init(12);
    //printf("\n Hello Client -1");

    const char* host = "127.0.0.1";
    const char* port = "10000";

    // Open a listening socket. The socket will block while waiting for new connections.
    // Note that fiber_io.c is intercepting the call to socket(). Whenever server_socket
    // blocks we will switch fibers. If no fibers are available we'll wait for a new client
    // using epoll_wait (or equivalent). This is all done under the covers - the application
    // writer uses simple, blocking operations.
    /*const int server_socket = start_server(host, port);
    if(server_socket < 0) {
        printf("failed to create socket. errno: %d\n", errno);
        return errno;
    }*/
    //printf("\n Hello Client 0");
    int i;
    fiber_t *client_fiber[100];

    for(i = 0; i < 9; i++) {
        client_fiber[i] = fiber_create(10240, &client_function, NULL);
    }

    //fiber_do_real_sleep(10, 0);
    //printf("Global count %d\n", counter);
    for(i = 0; i < 9; i++) {
        printf("From main: %d join successful\n",i);
        fiber_join(client_fiber[i], NULL);
    }

    printf("In main fiber now\n");
    // Block until a new client appears. Spawn a new fiber for each client.
    int sock;
    /*
    while((sock = accept(server_socket, NULL, NULL)) >= 0) {
        fiber_t* client_fiber = fiber_create(10240, &client_function, (void*)(intptr_t)sock);
        fiber_detach(client_fiber);
    }
    */
    return 0;
}

// This function creates a listening socket. It's not particularly important to
// this example. The main point is that the socketwill block because we don't enable
// non-blocking mode.
int start_server(const char* host, const char* port)
{
    struct addrinfo hints, *res;
    printf("Server starting.........\n");
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;// use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;// fill in my IP for me
    getaddrinfo(host, port, &hints, &res);

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sockfd < 0) {
        return -1;
    }
    if(bind(sockfd, res->ai_addr, res->ai_addrlen)) {
        close(sockfd);
        return -1;
    }
    if(listen(sockfd, 100)) {
        close(sockfd);
        return -1;
    }
    return sockfd;
}

