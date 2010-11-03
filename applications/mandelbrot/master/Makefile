#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Use, modification and distribution is subject to the Boost Software
#  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

SAGA_SRC         = $(wildcard *.cpp)
SAGA_ADD_BIN_OBJ = $(SAGA_SRC:%.cpp=%.o)
SAGA_BIN         = mandelbrot_master

SAGA_SUBDIRS     = pngwriter

include $(SAGA_LOCATION)/share/saga/make/saga.application.mk
include pngwriter/pngwriter.mk

run:: $(SAGA_BIN)
	$(SAGA_PWD)/$(SAGA_BIN)
  
SAGA_CXXFLAGS += -I/usr/X11/include
SAGA_LDFLAGS  += -L/usr/X11/lib   -lX11


