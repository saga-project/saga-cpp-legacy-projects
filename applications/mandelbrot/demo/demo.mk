
ifndef SAGA_MB_TAG
 $(error SAGA_MB_TAG undefined)
endif

ifndef SAGA_MB_ROOT
 $(error SAGA_MB_ROOT undefined)
endif

SAGA_DEMO_HOME=$(SAGA_MB_ROOT)/demo-$(SAGA_MB_TAG)

include $(SAGA_MB_ROOT)/config/make.cfg.base

all: restart

clean: stop
	@rm -vf $(SAGA_DEMO_HOME)/demo.lock
	@rm -vf $(SAGA_DEMO_HOME)/demo.log
	@rm -vf $(SAGA_DEMO_HOME)/demo.stop

distclean: stop clean
	@rm -vrf $(SAGA_DEMO_HOME)/demo-*/
	@rm -vf  $(SAGA_DEMO_HOME)/today

restart: clean start

start:
	@echo "starting $(SAGA_MB_TAG) demo"
	@env  "SAGA_MB_TAG=$(SAGA_MB_TAG)" SAGA_MB_ROOT=$(SAGA_MB_ROOT) $(SAGA_MB_ROOT)/demo/run-demo.sh

stop:
	@-ps ef -C run-demo.sh | grep -e "SAGA_MB_TAG=$(SAGA_MB_TAG)" | cut -c 8-14 | xargs -rt kill

