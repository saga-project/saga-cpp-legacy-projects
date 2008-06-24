#  Copyright (c) 2005-2007 Andre Merzky (andre@merzky.net)
# 
#  Distributed under the Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

ifndef SAGA_MK_INC_CONF_XMLRPC
       SAGA_MK_INC_CONF_XMLRPC = yes

###########################################################
# 
# XML-RPC
#
include /Users/luckow/workspace/saga-cpr-svn/adaptors/default/config//make.cfg

ifeq "$(SAGA_HAVE_XMLRPC)" "yes"
  SAGA_XMLRPC_LOCATION  = 
  SAGA_XMLRPC_INC_FLAGS = 
  SAGA_XMLRPC_LIB_FLAGS = 
  SAGA_XMLRPC_S_LIBS    = 

  SAGA_CPPINCS         += $(SAGA_XMLRPC_INC_FLAGS)
  SAGA_S_LIBS          += $(SAGA_XMLRPC_S_LIBS)

  ifdef SAGA_INCLUDE_INCLUDES

    ifeq "$(SAGA_HAVE_OPENSSL)" "yes"
      include /Users/luckow/workspace/saga-cpr-svn/adaptors/default/config//saga.config.openssl.c.mk
    endif
    
  endif # SAGA_INCLUDE_INCLUDES

endif
# 
###########################################################

endif # guard

