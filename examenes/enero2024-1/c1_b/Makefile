SRC = $(wildcard *.c)
TARGETS_SRC = $(wildcard c1_*.c)
TARGETS = $(TARGETS_SRC:%.c=%)
CC = gcc
CFLAGS = -pthread -g $(CPPFLAGS)

LDFLAGS = -pthread
LIBS =

all: $(TARGETS)
	ls -t
	head Makefile
	cat test_c1b.txt

# Pattern rule to build executables from .c files
%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LIBS)
