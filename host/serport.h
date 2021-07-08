/*
 * Copyright (c) 2021, Wojciech Owczarek
 * All rights reserved.
 *
 * BSD 2-clause licence - see LICENSE in the top directory
 */

#ifndef __SERPORT_H_
#define __SERPORT_H_

#define SP_NONE 0
#define SP_CANONICAL 1
#define SP_PARITY_N       0
#define SP_PARITY_NONE    0
#define SP_PARITY_EVEN    1
#define SP_PARITY_Y       SP_PARITY_EVEN
#define SP_PARITY_ODD     2

#define SP_FLOWCONTROL_NONE    0
#define SP_FLOWCONTROL_XONXOFF 1
#define SP_FLOWCONTROL_SW      SP_FLOWCONTROL_XONXOFF
#define SP_FLOWCONTROL_RTSCTS  2
#define SP_FLOWCONTROL_HW      SP_FLOWCONTROL_RTSCTS

#define SP_NOFLUSH 0
#define SP_FLUSH   1

typedef struct {
   unsigned int baudrate;
   unsigned int databits;
   unsigned int parity;
   unsigned int stopbits;
   unsigned int flowcontrol;
   unsigned int minchars;
   unsigned int timeout;
   unsigned int canonical;
} sp_params;

int sp_open(const char* devpath, const sp_params *params, const unsigned int flags);
int sp_nread(int fd);

#endif /* __SERPORT_H_ */
