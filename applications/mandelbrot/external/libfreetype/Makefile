#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Use, modification and distribution is subject to the Boost Software
#  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

SAGA_MB_ROOT  = ../../

include $(SAGA_MB_ROOT)/config/make.cfg.base

SAGA_SRC         += unix/ftsystem.c
SAGA_SRC         += base/ftdebug.c
SAGA_SRC         += base/ftinit.c
SAGA_SRC         += base/ftbase.c
SAGA_SRC         += base/ftbbox.c
SAGA_SRC         += base/ftbdf.c
SAGA_SRC         += base/ftbitmap.c
SAGA_SRC         += base/ftcid.c
SAGA_SRC         += base/ftfstype.c
SAGA_SRC         += base/ftgasp.c
SAGA_SRC         += base/ftglyph.c
SAGA_SRC         += base/ftgxval.c
SAGA_SRC         += base/ftlcdfil.c
SAGA_SRC         += base/ftmm.c
SAGA_SRC         += base/ftotval.c
SAGA_SRC         += base/ftpatent.c
SAGA_SRC         += base/ftpfr.c
SAGA_SRC         += base/ftstroke.c
SAGA_SRC         += base/ftsynth.c
SAGA_SRC         += base/fttype1.c
SAGA_SRC         += base/ftwinfnt.c
SAGA_SRC         += base/ftxf86.c
SAGA_SRC         += truetype/truetype.c
SAGA_SRC         += type1/type1.c
SAGA_SRC         += cff/cff.c
SAGA_SRC         += cid/type1cid.c
SAGA_SRC         += pfr/pfr.c
SAGA_SRC         += type42/type42.c
SAGA_SRC         += winfonts/winfnt.c
SAGA_SRC         += pcf/pcf.c
SAGA_SRC         += bdf/bdf.c
SAGA_SRC         += sfnt/sfnt.c
SAGA_SRC         += autofit/autofit.c
SAGA_SRC         += pshinter/pshinter.c
SAGA_SRC         += raster/raster.c
SAGA_SRC         += smooth/smooth.c
SAGA_SRC         += cache/ftcache.c
SAGA_SRC         += gzip/ftgzip.c
SAGA_SRC         += lzw/ftlzw.c
SAGA_SRC         += psaux/psaux.c
SAGA_SRC         += psnames/psmodule.c
SAGA_ADD_LIB_OBJC = $(SAGA_SRC:%.c=%.o)
SAGA_ARC          = freetype

include $(SAGA_LOCATION)/share/saga/make/saga.mk

SAGA_CCFLAGS     += -DFT_CONFIG_CONFIG_H="<freetype/config/ftconfig.h>" -DFT2_BUILD_LIBRARY -DFT_CONFIG_MODULES_H="<freetype/config/ftmodule.h>" -Iinclude/

