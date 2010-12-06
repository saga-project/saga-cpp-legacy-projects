# Filename: Makefile.SunOS

# Version  Developer        Date     Change
# -------  ---------------  -------  -----------------------
# 1.0      John Good        13Feb08  Original SunOS Makefile

.SUFFIXES:
.SUFFIXES: .c .o

CC     =	gcc -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
CFLAGS =	-g -I. -I../../lib/include -I../../Montage
LIBS   =	-L../../lib -lwcs -lcfitsio -lcoord -lsocket -lnsl -lm

.c.o:
		$(CC) $(CFLAGS)  -c  $*.c

mTruncate:	mTruncate.o
		$(CC) -o mTruncate mTruncate.o \
		../../Montage/checkHdr.o   \
		../../Montage/checkWCS.o   \
		$(LIBS)

install:
		cp mTruncate ../../bin

clean:
		rm -f mTruncate *.o
