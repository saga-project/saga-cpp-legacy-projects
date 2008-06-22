CC=gcc
#CFLAGS=-I./ -I./cnet -g -DDOCACHE=0 -DSDSC
CFLAGS=-I./ -I./cnet -g -DHTTPCACHE=1 -DDOCACHE=0
#CFLAGS=-I./ -I./cnet -g -DDOCACHE=0
LDFLAGS=-L./ -L./cnet -lbqp -lcnet

SUBDIRS=cnet
OBJS=bqp_data.o bqp_tools.o bqp_vgrads_api.o
LIBS=libbqp.a
TARGS=varq bqp varq_range test_bqp_vgrads bqp_precog bqp_api_example bqp3dplot varq_reserve

ALLOBJS=$(OBJS) $(LIBS) $(TARGS)

all: subs $(ALLOBJS)

% : %.c 
	$(CC) -c $(CFLAGS) $(@).c -o $(@).o
	$(CC) $(LDFLAGS) $(@).o -o $@ $(LDFLAGS)

subs: 
	for i in $(SUBDIRS); do cd $$i && $(MAKE); cd ..; done

libbqp.a: 
	ar -rs libbqp.a ${OBJS}

clean:
	for i in $(SUBDIRS); do cd $$i && $(MAKE) clean; cd ..; done
	rm -f *.a *.o *~* *#* $(TARGS)

