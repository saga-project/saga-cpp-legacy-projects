
#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Distributed under the Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

SAGA_SUBDIRS      = util

SAGA_SRC          = $(wildcard *.cpp)
SAGA_ADD_LIB_OBJ  = $(SAGA_SRC:%.cpp=%.o)
SAGA_ADD_LIB_OBJ += util/thread.o util/split.o
SAGA_LIB          = diggedag

include $(SAGA_LOCATION)/share/saga/make/saga.application.mk

SAGA_CXXFLAGS    += -Wall # -ansi -std=c++98

SAGA_CPPINCS     += -I./
SAGA_CPPINCS     += -I../external/tinyxml++/
SAGA_LDFLAGS     += -L../external/tinyxml++/ -lsaga_tinyxml++

