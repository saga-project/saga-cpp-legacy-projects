#  Copyright (c) 2005-2007 Andre Merzky (andre@merzky.net)
# 
#  Distributed under the Boost Software License, Version yes.0. (See accompanying
#  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

ifndef SAGA_MK_INC_CONF_SOCI
       SAGA_MK_INC_CONF_SOCI = yes

############################################################
# 
# SOCI
#

include /Users/luckow/workspace/saga-cpr-svn/adaptors/default/config//make.cfg

ifeq "$(SAGA_HAVE_SOCI)" "yes"

  SAGA_SOCI_LOCATION          = 
  SAGA_SOCI_INC_FLAGS         = 
  SAGA_SOCI_LIB_FLAGS         = 
  SAGA_SOCI_S_LIBS            = 

  SAGA_CPPINCS               += $(SAGA_SOCI_INC_FLAGS)
  SAGA_LDFLAGS_EXT           += $(SAGA_SOCI_LIB_FLAGS)
  SAGA_S_LIBS                += $(SAGA_SOCI_S_LIBS)

  ifdef SAGA_INCLUDE_INCLUDES

    ifeq "$(SAGA_HAVE_SOCI_SQLITE3)" "yes"
      include /Users/luckow/workspace/saga-cpr-svn/adaptors/default/config//saga.config.sqlite3.c.mk
    endif
    
    ifeq "$(SAGA_HAVE_SOCI_POSTGRESQL)" "yes"
      include /Users/luckow/workspace/saga-cpr-svn/adaptors/default/config//saga.config.postgresql.c.mk
    endif
    
    ifeq "$(SAGA_HAVE_SOCI_ORACLE)" "yes"
      include /Users/luckow/workspace/saga-cpr-svn/adaptors/default/config//saga.config.oracle.c.mk
    endif
    
    ifeq "$(SAGA_HAVE_SOCI_MYSQL)" "yes"
      include /Users/luckow/workspace/saga-cpr-svn/adaptors/default/config//saga.config.mysql.c.mk
    endif
    
    ifeq "$(SAGA_HAVE_SOCI_ODBC)" "yes"
      include /Users/luckow/workspace/saga-cpr-svn/adaptors/default/config//saga.config.odbc.c.mk
    endif
    
    ifeq "$(SAGA_HAVE_SOCI_FIREBIRD)" "yes"
      include /Users/luckow/workspace/saga-cpr-svn/adaptors/default/config//saga.config.firebird.c.mk
    endif

  endif # SAGA_INCLUDE_INCLUDES

endif # have soci


ifeq "$(SAGA_HAVE_INTERNAL_SOCI)" "yes"

  SAGA_EXT_SOCI               = /Users/luckow/workspace/saga-cpr-svn/adaptors/default/config//../external/soci/

  SAGA_SOCI_INC_FLAGS        += -I$(SAGA_EXT_SOCI)
  SAGA_SOCI_INC_FLAGS        += -I$(SAGA_EXT_SOCI)/core/

  SAGA_SOCI_LIB_FLAGS        += -L$(SAGA_EXT_SOCI)
  SAGA_SOCI_LIB_FLAGS        += $(SAGA_EXT_SOCI)/core/libsoci_core.$(SAGA_AEXT)


  ifeq "$(SAGA_HAVE_SOCI_SQLITE3)" "yes"

    SAGA_SOCI_INC_FLAGS      += -DSAGA_USE_SOCI_SQLITE3 
    SAGA_SOCI_INC_FLAGS      += -I$(SAGA_EXT_SOCI)/sqlite3/
    SAGA_SOCI_LIB_FLAGS      += $(SAGA_EXT_SOCI)/sqlite3/libsoci_sqlite3.$(SAGA_AEXT)
    SAGA_SOCI_S_LIBS         += $(SAGA_EXT_SOCI)/sqlite3/libsoci_sqlite3.$(SAGA_AEXT)

  endif


  ifeq "$(SAGA_HAVE_SOCI_POSTGRESQL)" "yes"

    SAGA_SOCI_INC_FLAGS      += -DSAGA_USE_SOCI_POSTGRESQL
    SAGA_SOCI_INC_FLAGS      += -I$(SAGA_EXT_SOCI)/postgresql/
    SAGA_SOCI_LIB_FLAGS      += $(SAGA_EXT_SOCI)/postgresql/libsoci_postgresql.$(SAGA_AEXT)
    SAGA_SOCI_S_LIBS         += $(SAGA_EXT_SOCI)/postgresql/libsoci_postgresql.$(SAGA_AEXT)

  endif

  SAGA_CPPINCS               += $(SAGA_SOCI_INC_FLAGS)
  SAGA_LDFLAGS_EXT           += $(SAGA_SOCI_LIB_FLAGS)
  SAGA_S_LIBS                += $(SAGA_SOCI_S_LIBS)


  ifdef SAGA_INCLUDE_INCLUDES

    ifeq "$(SAGA_HAVE_SOCI_SQLITE3)" "yes"
      include /Users/luckow/workspace/saga-cpr-svn/adaptors/default/config//saga.config.sqlite3.c.mk
    endif
    
    ifeq "$(SAGA_HAVE_SOCI_POSTGRESQL)" "yes"
      include /Users/luckow/workspace/saga-cpr-svn/adaptors/default/config//saga.config.postgresql.c.mk
    endif

  endif

endif # build soci internal
# 
###########################################################

endif # guard

