CC=gcc
CFLAGS+=-std=gnu99 -Wall -I. -O3 -lrt

DEPS = serport.h tunif.h
OBJ1 = serport.o tunif.o pbnet.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
all: pbnet

pbnet: $(OBJ1)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -rf *.o *~ core pbnet
