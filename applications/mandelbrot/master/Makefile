#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Use, modification and distribution is subject to the Boost Software
#  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

HAVE_PNG = yes
HAVE_X11 = no


SAGA_SRC        += job_starter.cpp
SAGA_SRC        += mandelbrot.cpp
SAGA_SRC        += mandelbrot_master.cpp

ifeq "$(HAVE_PNG)" "yes"
 FLAGS          += -DHAVE_PNG
 SAGA_SRC       += output_png.cpp
 SAGA_SUBDIRS    = pngwriter
endif

ifeq "$(HAVE_X11)" "yes"
 FLAGS          += -DHAVE_X11
 SAGA_SRC       += output_x11.cpp
endif

SAGA_ADD_BIN_OBJ = $(SAGA_SRC:%.cpp=%.o)
SAGA_BIN         = mandelbrot_master


include $(SAGA_LOCATION)/share/saga/make/saga.application.mk


run:: $(SAGA_BIN)
	$(SAGA_PWD)/$(SAGA_BIN)
  
SAGA_CXXFLAGS += $(FLAGS)

ifeq "$(HAVE_PNG)" "yes"
 include pngwriter/pngwriter.mk
endif

ifeq "$(HAVE_X11)" "yes"
 SAGA_CXXFLAGS += -I/usr/X11/include
 SAGA_LDFLAGS  += -L/usr/X11/lib   -lX11
endif


