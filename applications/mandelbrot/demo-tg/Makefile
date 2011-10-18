
SAGA_MB_ROOT  = ../

include $(SAGA_MB_ROOT)/config/make.cfg.base

all:

clean:
	-ps -ef | grep run_demo.sh | grep -v -e grep -e vi | cut -c 8-14 | xargs -r kill
	rm -f  demo.lock
	rm -f  demo.log
	rm -f  demo.stop

distclean: clean
	rm -rf demo-*/
	rm -f  today

restart: clean
	./run_demo.sh

