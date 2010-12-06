CC = gcc -g -I . -I../mtbl -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

libjoblist.a:	joblist.c
		$(CC) -c joblist.c
		ar rv libjoblist.a joblist.o

install:
		cp libjoblist.a ../..
		cp joblist.h ../../include

clean:		
		rm -f libjoblist.a *.o *.a core
