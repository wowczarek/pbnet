/*
 * Copyright (c) 2021, Wojciech Owczarek
 * All rights reserved.
 *
 * BSD 2-clause licence - see LICENSE in the top directory
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h> 

#include "tcp.h"

/* a trivial getaddrinfo() wrapper */
static int gai(const char* host, struct sockaddr *outaddr) {

    struct addrinfo hints, *out;
    int ret;

    out = NULL;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = PF_UNSPEC;

    ret = getaddrinfo(host, NULL, &hints, &out);

    if (ret != 0) return ret;

    if (out != NULL && (out->ai_family == AF_INET || out->ai_family == AF_INET6)) {
        memcpy(outaddr, out->ai_addr, sizeof(struct sockaddr));
    } else {
        ret = EAI_FAMILY;
    }

    freeaddrinfo(out);
    return ret;

}

int tcp_open(const char* host, const int port) {

    int fd = -1;
    int ret;

    struct sockaddr daddr; 
    struct sockaddr_in* din = (struct sockaddr_in*)&daddr; /* just a wrapper to get sin_ port */

    memset(&daddr, 0, sizeof(daddr));

    if(fd > -1) {
        close(fd);
    }

    if ((ret=gai(host, &daddr)) != 0) {
        fprintf(stderr, "Error: getaddrinfo(): %s\n", gai_strerror(ret));
        return -1;
    }

    if((fd = socket(daddr.sa_family, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "Error: could not create socket: %s \n", strerror(errno));
        return -1;
    } 

    din->sin_port = htons(port);

    if(connect(fd, &daddr, sizeof(daddr)) < 0)
    {
       fprintf(stderr, "Error: connect(): %s\n", strerror(errno));
       return -1;
    } 

    int flag = 1; setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));

    return fd;

}

ssize_t tcp_read(int fd, void* buf, size_t len) {
    int ret = recv(fd, buf, len, MSG_DONTWAIT);
    if((ret < 0) && (errno == EAGAIN || errno == EWOULDBLOCK)) return 0;
    return ret;
}

ssize_t tcp_write(int fd, void* buf, size_t len) {
    return send(fd, buf, len, 0);
}
