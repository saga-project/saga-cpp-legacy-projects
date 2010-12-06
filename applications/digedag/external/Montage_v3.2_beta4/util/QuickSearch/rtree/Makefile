CC = gcc

OBJECTS = card.o \
	index.o \
	mfmalloc.o \
	node.o \
	rect.o \
	split_q.o 

all: librtree.a test

librtree.a: $(OBJECTS)
	ar rcv librtree.a $(OBJECTS)
	ranlib librtree.a

test: test.o librtree.a
	$(CC) $(LDFLAGS) -o $@ test.o librtree.a -lm

.c.o:
	$(CC) $(CFLAGS) -Wall -c $<

clean:
	rm -rf test ./*.o ./*.a
