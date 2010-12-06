jltest:		jltest.o
		gcc -o jltest jltest.o -L.. -ljoblist -L../../.. -lmtbl

jltest.o:	jltest.c
		gcc -I.. -c jltest.c

clean:
		rm -f jltest *.o
