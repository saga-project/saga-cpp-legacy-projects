
ifndef SAGA_MW_ROOT
	$(error need SAGA_MW_ROOT defined)
endif

ifndef SAGA_MW_WORKER
	$(error need SAGA_MW_WORKER defined)
endif

ifndef SAGA_MW_MASTER
	$(error need SAGA_MW_MASTER defined)
endif


SAGA_CXXFLAGS  += -I$(SAGA_MW_ROOT)
SAGA_LDFLAGS   += -L$(SAGA_MW_ROOT) -lsaga_pm_master_worker


k:  kill
l:  list
r:  run
s:  stop_master stop_worker
c:  cont_master cont_worker
sm: stop_master
cm: cont_master
sw: stop_worker
cw: cont_worker


kill:
	@ps -ef | grep -e $(SAGA_MW_MASTER) -e $(SAGA_MW_WORKER) | grep -v -e gvim -e grep -e saga-run | cut -c 8-16 | xargs -rt kill -TERM
	@ps -ef | grep -e $(SAGA_MW_MASTER) -e $(SAGA_MW_WORKER) | grep -v -e gvim -e grep -e saga-run | cut -c 8-16 | xargs -rt kill -KILL
	@rm -f /tmp/t
	@touch /tmp/t

list:
	@ps -ef | grep -e $(SAGA_MW_MASTER) -e $(SAGA_MW_WORKER)  | grep -v -e gvim -e grep -e saga-run || true

run:
	@make -C .. install  2>&1 >> /dev/null
	@make install        2>&1 >> /dev/null
	@./$(SAGA_MW_MASTER) 2>&1 >> /tmp/t &

stop_master:
	@ps -ef | grep $(SAGA_MW_MASTER)| grep -v -e gvim -e grep -e saga-run | cut -c 8-16 | xargs -rt kill -STOP

cont_master:
	@ps -ef | grep $(SAGA_MW_MASTER)| grep -v -e gvim -e grep -e saga-run | cut -c 8-16 | xargs -rt kill -CONT

stop_worker:
	@ps -ef | grep $(SAGA_MW_WORKER) | grep -v -e gvim -e grep -e saga-run | cut -c 8-16 | xargs -rt kill -STOP

cont_worker:
	@ps -ef | grep $(SAGA_MW_WORKER) | grep -v -e gvim -e grep -e saga-run | cut -c 8-16 | xargs -rt kill -CONT

