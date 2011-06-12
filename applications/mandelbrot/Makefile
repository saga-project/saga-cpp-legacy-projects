#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Use, modification and distribution is subject to the Boost Software
#  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

SAGA_MB_ROOT = ./

include $(SAGA_MB_ROOT)/config/make.cfg.base

SAGA_SUBDIRS     = config 

ifeq "$(MB_ENABLE_MASTER)" "yes"
 SAGA_SUBDIRS   += external master
endif

ifeq "$(MB_ENABLE_CLIENT)" "yes"
 SAGA_SUBDIRS   += client
endif


include $(SAGA_LOCATION)/share/saga/make/saga.mk

ifndef SAGA_IS_CLEANING
external: config
client:   config
master:   external
endif

