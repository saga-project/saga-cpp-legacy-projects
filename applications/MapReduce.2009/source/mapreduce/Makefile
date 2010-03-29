#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Use, modification and distribution is subject to the Boost Software
#  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)


SAGA_SUBDIRS      = input output io master worker

SAGA_HDR          = $(wildcard *.hpp)
SAGA_SRC          = $(wildcard *.cpp)

SAGA_LIB_OBJ     += $(SAGA_SRC:%.cpp=%.o)

SAGA_LIB_OBJ     += $(wildcard io/*.o)
SAGA_LIB_OBJ     += $(wildcard input/*.o)
SAGA_LIB_OBJ     += $(wildcard output/*.o)
SAGA_LIB_OBJ     += $(wildcard master/*.o)
SAGA_LIB_OBJ     += $(wildcard worker/*.o)

SAGA_LIB_OBJ     += $(wildcard ../xmlParser/xmlParser.o)
SAGA_LIB_OBJ     += $(wildcard ../utils/LogWriter.o)
SAGA_LIB_OBJ     += $(wildcard ../../external/boost-log/libs/log/src/*.o)

# Why aren't these added by the above wildcard?!
# SAGA_LIB_OBJ     += input/TextInputFormat.o
# SAGA_LIB_OBJ     += worker/SystemInfo.o
# SAGA_LIB_OBJ     += master/DistributedJobRunner.o
# SAGA_LIB_OBJ     += worker/WorkerThread.o
# SAGA_LIB_OBJ     += io/SequenceFile.o

SAGA_LIB          = saga_mapreduce

SAGA_USE_BOOST_OPTIONS = "yes"

# include the application helper make file
include $(SAGA_LOCATION)/share/saga/make/saga.application.mk

SAGA_LDFLAGS     += -lprotobuf

SAGA_CPPINCS  += -I./ 
SAGA_CPPINCS  += -I../ 
SAGA_CPPINCS  += -I../../external/factory/
SAGA_CPPINCS  += -I../../external/boost-log/
SAGA_LDFLAGS  += -L../../external/boost-log/log/libs/src/ -lboost_log

lib:: $(SAGA_SUBDIRS)

