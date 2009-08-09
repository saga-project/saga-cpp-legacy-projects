#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Use, modification and distribution is subject to the Boost Software
#  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

SAGA_CPPINCS     += -I../../external/factory -I../../source/mapreduce -I../../source/

SAGA_SRC          = $(wildcard *.cpp)
SAGA_ADD_BIN_OBJ  = $(SAGA_SRC:%.cpp=%.o)

SAGA_BIN		      = wordcount

SAGA_USE_BOOST_OPTIONS = "yes"

# include the application helper make file
include $(SAGA_LOCATION)/share/saga/make/saga.application.mk

SAGA_LDFLAGS     += -lsaga_engine -lprotobuf -lboost_thread-gcc42-mt -lboost_log-gcc42-mt -L../../source/mapreduce -lsaga_mapreduce
