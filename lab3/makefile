CC=gcc
CFLAGS=-Wall
CFLAGS+=-DPARALLEL

all:	sort
	./sort

sort:	sort.o

sort.o:	sort.c

clean:
	rm -rf sort sort.o
