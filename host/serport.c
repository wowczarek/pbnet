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
#include <termios.h>

#include "serport.h"

static speed_t get_speed(unsigned int baudrate) {

#define BAUD_CASE(rate) case rate: return B##rate;

   switch(baudrate) {

      BAUD_CASE(0);
      BAUD_CASE(50);
      BAUD_CASE(75);
      BAUD_CASE(110);
      BAUD_CASE(134);
      BAUD_CASE(150);
      BAUD_CASE(200);
      BAUD_CASE(300);
      BAUD_CASE(600);
      BAUD_CASE(1200);
      BAUD_CASE(1800);
      BAUD_CASE(2400);
      BAUD_CASE(4800);
      BAUD_CASE(9600);
      BAUD_CASE(19200);
      BAUD_CASE(38400);
      BAUD_CASE(57600);
      BAUD_CASE(115200);
      BAUD_CASE(230400);
      BAUD_CASE(460800);
      BAUD_CASE(500000);
      BAUD_CASE(576000);
      BAUD_CASE(921600);
      BAUD_CASE(1000000);
      BAUD_CASE(1152000);
      BAUD_CASE(1500000);
      BAUD_CASE(2000000);
      BAUD_CASE(2500000);
      BAUD_CASE(3000000);
      BAUD_CASE(3500000);
      BAUD_CASE(4000000);
   }

return __MAX_BAUD + 1;

#undef BAUD_CASE

}

static tcflag_t get_csize(const unsigned int size) {

   switch(size) {
      case 5: return CS5;
      case 6: return CS6;
      case 7: return CS7;
      case 8: return CS8;
   }

   return 0;

}

static int sp_setparams(int fd, const sp_params *params, const unsigned int flush) {

   struct termios attrs;
   memset(&attrs, 0, sizeof(attrs));

   speed_t speed = get_speed(params->baudrate);

   /* parameter check */

   if(speed > __MAX_BAUD) {
      fprintf(stderr, "Error: Unknown / incorrect baud rate: %d\n", params->baudrate);
      return -1;
   }

   if(params->databits < 5 || params->databits > 8) {
      fprintf(stderr, "Error: Incorrect data bits: %d, expected 5...8\n", params->databits);
      return -1;
   }

   if(params->parity > 2) {
      fprintf(stderr, "Error: Incorrect parity: %d, expected 0 (SP_PARITY_N / SP_PARITY_NONE), 1 (SP_PARITY_Y / SP_PARITY_EVEN) or 2 (SP_PARITY_ODD)\n", params->parity);
      return -1;

   }

   if(params->stopbits < 1 || params->stopbits > 2) {
      fprintf(stderr, "Error: Incorrect stop bits: %d, expected 1 or 2\n", params->stopbits);
      return -1;
   }

   if(params->flowcontrol > 2) {
      fprintf(stderr, "Error: Incorrect flow control option: %d, expected 0 (SP_FLOWCONTROL_NONE), 1 (SP_FLOWCONTROL_XONXOFF / SP_FLOWCONTROL_SW) or 2 (SP_FLOWCONTROL_RTSCTS / SP_FLOWCONTROL_HW)\n", params->flowcontrol);
      return -1;
   }

   if(tcgetattr(fd, &attrs) != 0) {
      fprintf(stderr, "Error: Could not get serial port attributes: %s\n", strerror(errno));
      return -1;
   }

   /* set speed */
   cfsetispeed(&attrs, speed);
   cfsetospeed(&attrs, speed);

   /* set data bits */
   attrs.c_cflag |= get_csize(params->databits);

   /* set parity */
   if(params->parity == SP_PARITY_N) {
      attrs.c_cflag &= ~PARENB;
   }
   if(params->parity == SP_PARITY_Y) {
      attrs.c_cflag |= PARENB;
   }
   if(params->parity == SP_PARITY_ODD) {
      attrs.c_cflag |= PARENB | PARODD;
   }

   /* set flow control */
   attrs.c_cflag &= ~CRTSCTS;
   attrs.c_iflag &= ~(IXON | IXOFF | IXANY);

   if(params->flowcontrol == SP_FLOWCONTROL_RTSCTS) {
      attrs.c_cflag |= CRTSCTS;
   }

   if(params->flowcontrol == SP_FLOWCONTROL_XONXOFF) {
      attrs.c_iflag |= IXON | IXOFF | IXANY;
   }

   /* stop bits */
   attrs.c_cflag &= ~CSTOPB;

   if(params->stopbits == 2) {
     attrs.c_cflag |= CSTOPB;
   }

   /* timeouts */
   attrs.c_cc[VMIN] =  params->minchars;
   attrs.c_cc[VTIME] = params->timeout; /* tenths of a second here, not milliseconds as one might expect */

   /* the rest */
   attrs.c_cflag |=  CREAD | CLOCAL; /* enable receiver, ignore modem control lines */
   attrs.c_iflag &= ~( IGNBRK | ICRNL | INPCK | ISTRIP ); /* ignore break sequence, no cr/nl swap, no parity check, no 7-bit stripping */
   attrs.c_oflag &= ~OPOST;          /* disable post-processing */
   attrs.c_lflag  =  SP_NONE;        /* disable all local flags: canonical mode, echos, signals */

   /* canonical mode? */
   if(params->canonical) {
      attrs.c_lflag |= ICANON;
   }

   /* flush port */
   if(flush == SP_FLUSH) {
      tcflush(fd, TCIOFLUSH);
   }

   /* set attrs */
   if(tcsetattr(fd, TCSANOW, &attrs) != 0) {
      fprintf(stderr, "Error: Could not set serial port attributes: %s\n", strerror(errno));
      return -1;
   }

   return 0;

}

int sp_open(const char* devpath, const sp_params *params, const unsigned int flags) {

   int fd = -1;

   if(strlen(devpath) == 0) {
      fprintf(stderr, "Error: No serial device path given\n");
      return -1;
   }

   fd = open(devpath, O_RDWR | O_NOCTTY | O_NDELAY | flags);

   if(fd == -1) {
      fprintf(stderr, "Error: Could not open device %s: %s\n", devpath, strerror(errno));
      return -1;
   }

   if(sp_setparams(fd, params, SP_FLUSH) != 0) {
      close(fd);
      return -1;
   }

   return fd;

}

/* get number of bytes available for reading */
int sp_nread(int fd) {

    int nr = 0;
    int res;

    res = ioctl(fd, FIONREAD, &nr);

    if(res == -1) return res;

    return nr;

}
