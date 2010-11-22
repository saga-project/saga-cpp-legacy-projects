#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Use, modification and distribution is subject to the Boost Software
#  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

HAVE_PNG      = no
HAVE_X11      = no
HAVE_FREETYPE = no


SAGA_SRC        += job_starter.cpp
SAGA_SRC        += mandelbrot.cpp
SAGA_SRC        += mandelbrot_master.cpp

ifeq "$(HAVE_PNG)" "yes"
 FLAGS          += -DHAVE_PNG
 SAGA_SRC       += output_png.cpp
 SAGA_SUBDIRS    = pngwriter
else
 FLAGS          += -DHAVE_PNG
 SAGA_CXXFLAGS  += -I../external/libpng/
 SAGA_LDFLAGS   += ../external/libpng/libpng.a
 SAGA_SRC       += output_png.cpp
 SAGA_SUBDIRS    = pngwriter
endif

ifeq "$(HAVE_FREETYPE)" "no"
 FLAGS          += -DHAVE_FREETYPE
 SAGA_CXXFLAGS  += -I../external/libfreetype/include/
 SAGA_LDFLAGS   += ../external/libfreetype/libfreetype.a
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

include pngwriter/pngwriter.mk

ifeq "$(HAVE_X11)" "yes"
 SAGA_CXXFLAGS += -I/usr/X11/include
 SAGA_LDFLAGS  += -L/usr/X11/lib   -lX11
endif


