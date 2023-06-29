/*
 * Copyright (c) 2021, Wojciech Owczarek
 * All rights reserved.
 *
 * BSD 2-clause licence - see LICENSE in the top directory
 */

#ifndef __TCP_H_
#define __TCP_H_

int tcp_open(const char* host, const int port);
ssize_t tcp_read(int fd, void* buf, size_t len);
ssize_t tcp_write(int fd, void* buf, size_t len);
int tcp_check(int fd);

#endif /* __TCP_H_ */
