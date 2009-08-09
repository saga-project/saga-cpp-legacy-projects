#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Use, modification and distribution is subject to the Boost Software
#  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

# use all cpp files from here
SAGA_SRC          = $(wildcard *.cpp)
SAGA_LIB_OBJ += $(SAGA_SRC:%.cpp=%.o)

SAGA_USE_BOOST_OPTIONS = "yes"

# include the application helper make file
include $(SAGA_LOCATION)/share/saga/make/saga.application.mk

BOOST_VERSION_CHECK = $(shell expr $(SAGA_BOOST_VERSION_MAJ) \<= 1 \& $(SAGA_BOOST_VERSION_MIN) \< 36)

ifneq "$(BOOST_VERSION_CHECK)" "0"
  SAGA_CPPINCS += -I../../external/boost/include
endif

SAGA_CPPINCS += -I../
