#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Use, modification and distribution is subject to the Boost Software
#  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

ifndef SAGA_LOCATION
 $(error SAGA_LOCATION not set)
endif

 include $(SAGA_LOCATION)/share/saga/make/saga.util.mk
-include $(SAGA_LOCATION)/share/saga/make/saga.python.mk

ifndef SAGA_MK_INC_PYTHON
 $(error Python make support not available at $(SAGA_LOCATION))
endif


HAVE_UUID      = $(shell python -c "import uuid      " > /dev/null 2>&1 && echo yes)
HAVE_REDIS     = $(shell python -c "import redis     " > /dev/null 2>&1 && echo yes)
HAVE_THREAPOOL = $(shell python -c "import threadpool" > /dev/null 2>&1 && echo yes)

check::
	@$(ECHO) "      checking   bigjob pre-requisites"
	@$(TEST) "yes" = "$(HAVE_UUID)"       || echo "      *** uuid modules not installed"
	@$(TEST) "yes" = "$(HAVE_REDIS)"      || echo "      *** redis modules not installed"
	@$(TEST) "yes" = "$(HAVE_THREADPOOL)" || echo "      *** threadpool modules not installed"

install:: check
	@$(ECHO) "      installing bigjob modules"
	@$(MKDIR)              $(SAGA_PY_MODULE_ROOT)/bigjob/
	@$(CP)    -r generic/* $(SAGA_PY_MODULE_ROOT)/bigjob/

