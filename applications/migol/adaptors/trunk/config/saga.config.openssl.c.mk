#  Copyright (c) 2005-2007 Andre Merzky (andre@merzky.net)
# 
#  Distributed under the Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

ifndef SAGA_MK_INC_CONF_OPENSSL
       SAGA_MK_INC_CONF_OPENSSL = yes

############################################################
# 
# OPENSSL
#
include /Users/luckow/workspace/saga-cpr-svn/adaptors/default/config//make.cfg

ifeq "$(SAGA_HAVE_OPENSSL)" "yes"
  SAGA_OPENSSL_LOCATION  = /usr
  SAGA_OPENSSL_INC_FLAGS = -I/usr/include/
  SAGA_OPENSSL_LIB_FLAGS = -L/usr/lib/ -lssl
  SAGA_OPENSSL_S_LIBS    = 

  SAGA_CPPINCS          += $(SAGA_OPENSSL_INC_FLAGS)
  SAGA_LDFLAGS_EXT      += $(SAGA_OPENSSL_LIB_FLAGS)
  SAGA_S_LIBS           += $(SAGA_OPENSSL_S_LIBS)
endif
# 
###########################################################

endif # guard

