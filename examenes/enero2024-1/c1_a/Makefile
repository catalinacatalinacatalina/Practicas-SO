SRC = $(wildcard *.c)
TARGETS_SRC = $(wildcard c1_*.c)
TARGETS = $(TARGETS_SRC:%.c=%)
CC = gcc
CFLAGS = -pthread -g $(CPPFLAGS)
LDFLAGS = -pthread
LIBS =

all: $(TARGETS)

%: %.c
    $(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LIBS)

test:
    ls -t
    head Makefile
    cat test_c1b.txt