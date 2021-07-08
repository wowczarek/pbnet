/*
 * Copyright (c) 2021, Wojciech Owczarek
 * All rights reserved.
 *
 * BSD 2-clause licence - see LICENSE in the top directory
 */

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/if_tun.h>

#include "tunif.h"

#define BASE_TUN_DEV "/dev/net/tun"


int tunif_open(const char *saddr, const uint8_t mask, const int queuelen) {

    const char* basedev = BASE_TUN_DEV;
    int fd, res, sfd;
    uint32_t bitmask;
    struct ifreq ifr;
    struct sockaddr_in* addr = (struct sockaddr_in*)&ifr.ifr_addr;

    /* open tun dev */
    fd = open(basedev, O_RDWR | O_NONBLOCK);

    if(fd == -1) {
        return fd;
    }

    /* set up (new) tun interface */
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    res = ioctl(fd, TUNSETIFF, &ifr);
    if(res == -1) goto die;

    fprintf(stderr, "got %s... ", ifr.ifr_name);

    /* open a helper socket for SIOXXX */
    sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if(sfd == -1) goto die;

    /* parse and set IP address */
    ifr.ifr_addr.sa_family = AF_INET;
    res = inet_pton(AF_INET, saddr, &addr->sin_addr);
    if(res != 1) goto die;
    res = ioctl(sfd, SIOCSIFADDR, &ifr);
    if(res == -1) goto die;

    /* set net mask, /32 and up = 31 */
    bitmask = htonl(~ (0xffffffff >> ((mask > 31) ? 31 : mask)));
    addr->sin_addr.s_addr = bitmask;
    res = ioctl(sfd, SIOCSIFNETMASK, &ifr);
    if(res == -1) goto die;

    /* set UP, RUNNING flags */
    res = ioctl(sfd, SIOCGIFFLAGS, &ifr);
    if(res == -1) goto die;
    ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
    res = ioctl(sfd, SIOCSIFFLAGS, &ifr);
    if(res == -1) goto die;

    /* set desired TX queue length */
    ifr.ifr_qlen = queuelen;
    res = ioctl(sfd, SIOCSIFTXQLEN, &ifr);
    if(res == -1) goto die;

    /* dispose of the helper socket */
    close(sfd);

    return fd;

die:

    fprintf(stderr, "tunif error!\n");
    close(fd);
    return -1;

}

int tunif_shutdown(int fd) {
    close(fd);
    return 0;
}
