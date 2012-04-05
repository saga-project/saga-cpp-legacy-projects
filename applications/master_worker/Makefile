#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Distributed under the Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

SAGA_SUBDIRS_DELAYED = example

SAGA_SRC     = $(wildcard *.cpp)
SAGA_LIB_OBJ = $(SAGA_SRC:%.cpp=%.o)
SAGA_LIB     = saga_pm_master_worker

include $(SAGA_LOCATION)/share/saga/make/saga.application.mk

all::
	@rm -f $(SAGA_LIBNAME).$(SAGA_VERSION)
	@ln -s $(SAGA_LIBNAME) $(SAGA_LIBNAME).$(SAGA_VERSION)

