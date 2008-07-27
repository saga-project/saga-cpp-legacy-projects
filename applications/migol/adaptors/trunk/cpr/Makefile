#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Use, modification and distribution is subject to the Boost Software
#  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

SAGA_SRC              = $(wildcard *.cpp) 
SAGA_OBJ              = $(SAGA_SRC:%.cpp=%.o)

SAGA_ADAPTOR_NAME     = migol_cpr
SAGA_ADAPTOR_INI      = migol_cpr_adaptor.ini
SAGA_ADAPTOR_OBJ      = $(SAGA_OBJ)

SAGA_ADAPTOR_PACKAGES = cpr

JAVA_HOME=/usr/lib/jvm/java-1.5.0-sun/

# SAGA_DEBUG_MAKE       = yes

-include ../config/make.cfg
-include $(SAGA_MAKE_INCLUDE_ROOT)/saga.adaptor.mk

SAGA_CXXFLAGS    += -I. -I/System/Library/Frameworks/JavaVM.framework/Versions/1.5.0/Headers/ 
#SAGA_CXXFLAGS    += -I. -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux 
SAGA_CXXFLAGS    += -DPTHREADS -DWITH_NONAMESPACES -DWITH_OPENSSL -DWITH_COOKIES

SAGA_LDFLAGS     += -lssl -lcrypto -framework JavaVM
#SAGA_LDFLAGS     += -lssl -lcrypto -L${JAVA_HOME}/jre/lib/i386/client -ljvm
