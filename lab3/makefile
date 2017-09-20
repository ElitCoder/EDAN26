CC      := gcc
CFLAGS  := -Wall -Wextra
CFLAGS  += -D PARALLEL
CFLAGS  += -O3
LD_LIBS := -lpthread

all:	sort
	./sort

sort:	sort.o $(LD_LIBS)

sort.o:	sort.c

clean:
	rm -rf sort sort.o
