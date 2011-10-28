
SAGA_MB_ROOT  = ../

include $(SAGA_MB_ROOT)/config/make.cfg.base

ifndef SAGA_MB_TAG
	$(error SAGA_MB_TAG undefined)
endif

all: restart

clean: stop
	@rm -vf demo.lock
	@rm -vf demo.log
	@rm -vf demo.stop

distclean: stop clean
	@rm -vrf demo-*/
	@rm -vf  today

restart: clean start

start:
	@echo "starting $(SAGA_MB_TAG) demo"
	@env  "TAG=[$(SAGA_MB_TAG)]" $(SAGA_MB_ROOT)/demo/run-demo.sh

stop:
	@-ps ef -C run-demo.sh | grep -e "TAG=[$(SAGA_MB_TAG)]" | cut -c 8-14 | xargs -rt kill

