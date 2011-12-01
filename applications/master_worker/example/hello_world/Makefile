
SAGA_MW_ROOT    = ../../

SAGA_SRC        = $(wildcard *.cpp)
SAGA_BIN_OBJ    = $(SAGA_SRC:%.cpp=%.o)
SAGA_BIN        = $(SAGA_SRC:%.cpp=%)

SAGA_MW_MASTER  = mw_master
SAGA_MW_WORKER  = mw_worker

include $(SAGA_LOCATION)/share/saga/make/saga.application.mk
include $(SAGA_MW_ROOT)/saga.master_worker.mk

