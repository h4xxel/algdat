CFLAGS	+= -g -O0
LDFLAGS	+= -lrt

SRCFILES=$(wildcard *.c)
OBJFILES=$(SRCFILES:.c=.o)
.PHONY: all clean

all: $(OBJFILES)

clean:
	rm -f *.o

.c.o:
	$(CC) -c -o $@ $(CFLAGS) $<
	$(CC) -o $* $@ $(LDFLAGS)
