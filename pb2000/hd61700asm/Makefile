CC=gcc
CFLAGS+=-std=c99 -I. -O3 -D__DLP_INLINE
BINNAME=hd61700

DEPS = $(BINNAME).h
OBJ1 = $(BINNAME).o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(BINNAME)

$(BINNAME): $(OBJ1)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -rf *.o *~ $(BINNAME)
