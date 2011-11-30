#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Distributed under the Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

SAGA_SRC        = $(wildcard *.cpp)
SAGA_BIN_OBJ    = $(SAGA_SRC:%.cpp=%.o)
SAGA_BIN        = $(SAGA_SRC:%.cpp=%)

include $(SAGA_LOCATION)/share/saga/make/saga.application.mk

SAGA_CXXFLAGS  += -I..
SAGA_LDFLAGS   += -L.. -lsaga_pm_master_worker

k: kill
l: list
r: run
s: stop_master stop_worker
c: cont_master cont_worker

kill:
	@ps -ef | grep mw_ | grep -v -e gvim -e grep -e saga-run | cut -c 8-16 | xargs -rt kill -TERM
	@ps -ef | grep mw_ | grep -v -e gvim -e grep -e saga-run | cut -c 8-16 | xargs -rt kill -KILL
	@rm -f /tmp/t
	@touch /tmp/t

list:
	@ps -ef | grep -e mw_master -e mw_worker | grep -v -e gvim -e grep -e saga-run || true

run:
	@make -C .. install 2>&1 >> /dev/null
	@make install       2>&1 >> /dev/null
	@./mw_master        2>&1 >> /tmp/t &

sm: stop_master
stop_master:
	@ps -ef | grep mw_master| grep -v -e gvim -e grep -e saga-run | cut -c 8-16 | xargs -rt kill -STOP

cm: cont_master
cont_master:
	@ps -ef | grep mw_master| grep -v -e gvim -e grep -e saga-run | cut -c 8-16 | xargs -rt kill -CONT

sw: stop_worker
stop_worker:
	@ps -ef | grep mw_worker| grep -v -e gvim -e grep -e saga-run | cut -c 8-16 | xargs -rt kill -STOP

cw: cont_worker
cont_worker:
	@ps -ef | grep mw_worker| grep -v -e gvim -e grep -e saga-run | cut -c 8-16 | xargs -rt kill -CONT

