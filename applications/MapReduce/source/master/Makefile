#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Use, modification and distribution is subject to the Boost Software
#  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

SAGA_ROOT         = /home/michael/saga/trunk

SAGA_SRC          = $(wildcard *.cpp)
SAGA_ADD_BIN_OBJ  = $(SAGA_SRC:%.cpp=%.o)
SAGA_ADD_BIN_OBJ += ../xmlParser/xmlParser.o
SAGA_ADD_BIN_OBJ += ../utils/LogWriter.o
SAGA_BIN		      = main

SAGA_USE_BOOST_OPTIONS = "yes"

# include the application helper make file
include $(SAGA_ROOT)/make/saga.application.mk
