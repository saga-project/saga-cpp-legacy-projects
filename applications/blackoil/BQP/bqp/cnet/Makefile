CC=gcc
CFLAGS=-I./ -Wall -g -D_REENTRANT
LDFLAGS=-L./
OBJS=cnet.o

all: $(OBJS)
	ar -rs libcnet.a $(OBJS)
	ranlib libcnet.a

clean:
	rm -f *.o *.a *~*
