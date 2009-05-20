
SAGA_SRC         = $(wildcard *.cpp)
SAGA_OBJ         = $(SAGA_SRC:%.cpp=%.o)
SAGA_BIN         = $(SAGA_SRC:%.cpp=%)

include $(SAGA_LOCATION)/share/saga/make/saga.application.mk

SAGA_CXXFLAGS   += -Wall -ansi -std=c++98

SAGA_CPPINCS    += -I../src/
SAGA_LDFLAGS    += -L../src/ -ldigedag

SAGA_CPPINCS    += -I../external/tinyxml++/
SAGA_LDFLAGS    += -L../external/tinyxml++/ -lsaga_tinyxml++

