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

#define CONN_TIMEOUT 1
#define CONN_RETRIES 4

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
    int flags;

    struct sockaddr daddr; 
    struct sockaddr_in* din = (struct sockaddr_in*)&daddr; /* just a wrapper to get sin_ port */

    memset(&daddr, 0, sizeof(daddr));

    if(fd > -1) {
        close(fd);
    }

    if ((ret = gai(host, &daddr)) != 0) {
        fprintf(stderr, "Error: getaddrinfo(): %s\n", gai_strerror(ret));
        return -2;
    }

    if((fd = socket(daddr.sa_family, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "Error: could not create socket: %s \n", strerror(errno));
        return -1;
    } 

    din->sin_port = htons(port);

    /*
     * Now watch carefully. The following sequence of actions
     * is a shitshow on several levels, and I am fully aware.
     * Give me a decent way to react to connect() timeouts
     * and I will reconsider.
     */

    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        fprintf(stderr, "Error: fcntl(F_GETFL): %s\n", strerror(errno));
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        fprintf(stderr, "Error: fcntl(F_SETFL): %s\n", strerror(errno));
        return -1;

    }

    connect(fd, &daddr, sizeof(daddr));
    usleep(250000);
    errno = 0;
    for (int i = 0; i < CONN_RETRIES; i++) {
        ret = connect(fd, &daddr, sizeof(daddr));
        /* already connected - good */
        if (ret == 0) goto gotcon;
        if (ret == -1 && errno == EISCONN) goto gotcon;
        sleep(CONN_TIMEOUT);
    } 

   errno = ETIMEDOUT;
   close(fd);
   return -1;

gotcon:

    if (fcntl(fd, F_SETFL, flags & ~O_NONBLOCK) == -1) {
        fprintf(stderr, "Error: fcntl(F_SETFL): %s\n", strerror(errno));
        close(fd);
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

int tcp_check(int fd) {
    char frob;
    int ret = recv(fd, &frob, 1, MSG_PEEK | MSG_DONTWAIT);
    if ( (ret == -1) && !(errno == EINTR || errno == EWOULDBLOCK)) return -1;
    if (ret == 0) {
        return -1;
    }
    return 0;
}
