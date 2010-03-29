#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Use, modification and distribution is subject to the Boost Software
#  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

SAGA_SRC = $(wildcard *.cpp)
SAGA_HDR = $(wildcard *.hpp)
SAGA_OBJ = $(SAGA_SRC:%.cpp=%.o)

include $(SAGA_LOCATION)/share/saga/make/saga.mk

SAGA_CPPINCS += -I../ 
SAGA_CPPINCS += -I../../../external/factory
SAGA_CPPINCS += -I../../../external/boost-log/

