CC = gcc
CFLAGS = -O0 -g -pthread
LDFLAGS = -O0 -g -pthread

all: examen

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $<

examen: examen.o
	$(CC) $(LDFLAGS) -o $@ $<

.PHONY: clean all


clean:
	-rm examen examen.o
