
CC = gcc
CFLAGS = -O2
LIBS= -llikwid -lncurses

SANITIZE = false

ifeq ($(SANITIZE), true)
	LIBS := -lasan $(LIBS)
	CFLAGS := -g -fsanitize=address -fsanitize=leak $(CFLAGS)
endif

all: likwid-perftop


likwid-perftop: likwid-perftop.c
	$(CC) $(CFLAGS) likwid-perftop.c -o likwid-perftop $(LIBS)

clean:
	rm -f likwid-perftop

.PHONY: clean
