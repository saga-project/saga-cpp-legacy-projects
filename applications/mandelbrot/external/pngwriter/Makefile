#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Distributed under the Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

SAGA_SRC     = $(wildcard *.cpp)
SAGA_LIB_OBJ = $(SAGA_SRC:%.cpp=%.o)
SAGA_ARC     = pngwriter

FONTS        = $(wildcard *.ttf)

-include ../../config/make.cfg
include $(SAGA_LOCATION)/share/saga/make/saga.mk


install::
	@$(ECHO) "      installing   fonts"
	@$(INSTALL) $(FONTS) share/pngwriter/fonts/

