#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Use, modification and distribution is subject to the Boost Software
#  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

SAGA_SRC          = $(wildcard *.cpp)
SAGA_BIN          = perf_matrix

# use all cpp files from here
SAGA_ADD_BIN_OBJ  = $(SAGA_SRC:%.cpp=%.o)

# also, the subdirs contain some sources
SAGA_ADD_BIN_SRC += xmlParser/xmlParser.cpp 
SAGA_ADD_BIN_SRC += LogWriter.cpp 
SAGA_ADD_BIN_SRC += $(wildcard agent/*.cpp)

# so, we can now derive the objects from the sources
SAGA_ADD_BIN_OBJ  = $(SAGA_SRC:%.cpp=%.o)
SAGA_ADD_BIN_OBJ  = $(SAGA_ADD_BIN_SRC:%.cpp=%.o)

SAGA_USE_BOOST_OPTIONS = "yes"

include $(SAGA_LOCATION)/share/saga/make/saga.application.mk
