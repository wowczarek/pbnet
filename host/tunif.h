/*
 * Copyright (c) 2021, Wojciech Owczarek
 * All rights reserved.
 *
 * BSD 2-clause licence - see LICENSE in the top directory
 */

#ifndef __TUNIF_H_
#define __TUNIF_H_

#include <stdint.h>

int tunif_open(const char *addr, const uint8_t mask, const int queuelen);
int tunif_shutdown(int fd);

#endif /* __TUNIF_H_ */