CC		= gcc

#CFLAGS		= -Wall -pedantic -std=c99 -m32 -g
#CFLAGS		= -m64 -g -Wall -pedantic -Werror -std=c99
#CFLAGS		= -O3 -maltivec -Wall -pedantic -std=c99
#CFLAGS		= -g -Wall -pedantic -std=c99 -D_GNU_SOURCE
CFLAGS      = -O3 -Wall -pedantic -std=c99 -D_GNU_SOURCE

OBJS		= main.o list.o error.o random.o set.o dataflow.o

OUT		= live

#LDFLAGS		= -lpthread -static
LDFLAGS         = -lpthread

#S=100000
#V=100000
#U=4
#A=1000
#T=4
#P=0

S=20000
V=20000
U=4
A=200
T=4
P=0

all: $(OBJS)	
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(OUT)
	#valgrind --tool=helgrind ./$(OUT) $(S) $(V) $(U) $(A) $(T) $(P) $(L)
	#valgrind ./$(OUT) $(S) $(V) $(U) $(A) $(T) $(P)
	./$(OUT) $(S) $(V) $(U) $(A) $(T) $(P)
	#valgrind --tool=helgrind ./$(OUT)

clean:
	rm -f $(OUT) $(OBJS) cfg.dot
