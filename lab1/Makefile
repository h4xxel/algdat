CFLAGS	+= -g -O0
LDFLAGS	+= -ldarnit -lrt

SRCFILES=$(wildcard *.c)
OBJFILES=$(SRCFILES:.c=.o)
.PHONY: all clean

all: $(OBJFILES)

clean:
	rm -f *.o

.c.o:
	$(CC) -c -o $@ $(CFLAGS) $<
	$(CC) -o $* $@ $(LDFLAGS)
