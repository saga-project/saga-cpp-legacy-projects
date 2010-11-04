#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Use, modification and distribution is subject to the Boost Software
#  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)


SAGA_SUBDIRS      = xmlParser utils worker master

SAGA_SRC          = $(wildcard *.cpp)

# use all cpp files from here
SAGA_ADD_BIN_OBJ  = $(SAGA_SRC:%.cpp=%.o)

SAGA_USE_BOOST_OPTIONS = "yes"

# include the application helper make file
include $(SAGA_LOCATION)/share/saga/make/saga.application.mk

