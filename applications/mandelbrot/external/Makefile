#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Use, modification and distribution is subject to the Boost Software
#  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

SAGA_MB_ROOT  = ../

include $(SAGA_MB_ROOT)/config/make.cfg.base

ifeq "$(PNGWRITER_SOURCE)" "external"
 SAGA_SUBDIRS += libpngwriter
endif

ifeq "$(PNG_SOURCE)" "external"
 SAGA_SUBDIRS += libpng
endif

ifeq "$(FREETYPE_SOURCE)" "external"
 SAGA_SUBDIRS += libfreetype
endif


-include $(SAGA_LOCATION)/share/saga/make/saga.mk

# dir dependencies
ifeq "$(PNGWRITER_SOURCE)" "external"
 ifeq "$(PNG_SOURCE)" "external"
libpngwriter: libpng
 endif

 ifeq "$(FREETYPE_SOURCE)" "external"
libpngwriter: libfreetype
 endif
endif

