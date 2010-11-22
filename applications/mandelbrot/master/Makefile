#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Use, modification and distribution is subject to the Boost Software
#  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

SAGA_MB_ROOT  = ../

include $(SAGA_MB_ROOT)/config/make.cfg.base

SAGA_SRC        += job_starter.cpp
SAGA_SRC        += mandelbrot.cpp
SAGA_SRC        += mandelbrot_master.cpp
SAGA_SRC        += output_png.cpp

ifeq "$(HAVE_X11)" "yes"
 SAGA_SRC       += output_x11.cpp
endif

SAGA_ADD_BIN_OBJ = $(SAGA_SRC:%.cpp=%.o)
SAGA_BIN         = mandelbrot_master


include $(SAGA_LOCATION)/share/saga/make/saga.application.mk

run:: $(SAGA_BIN)
	$(SAGA_PWD)/$(SAGA_BIN)
  
