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

# HAVE_UUID       = $(shell env PYTHONPATH=$(SAGA_LOCATION)/$(SAGA_PYTHON_PKG_PATH):$(PYTHONPATH) python -c "import uuid      " && echo yes)
# HAVE_REDIS      = $(shell env PYTHONPATH=$(SAGA_LOCATION)/$(SAGA_PYTHON_PKG_PATH):$(PYTHONPATH) python -c "import redis     " && echo yes)
# HAVE_THREADPOOL = $(shell env PYTHONPATH=$(SAGA_LOCATION)/$(SAGA_PYTHON_PKG_PATH):$(PYTHONPATH) python -c "import threadpool" && echo yes)
# 
# check::
# 	$(ECHO) "      checking   bigjob pre-requisites"
# 	$(TEST) "yes" = "$(HAVE_UUID)"       || ( echo "      *** uuid modules not installed"       && false )
# 	$(TEST) "yes" = "$(HAVE_REDIS)"      || ( echo "      *** redis modules not installed"      && false )
# 	$(TEST) "yes" = "$(HAVE_THREADPOOL)" || ( echo "      *** threadpool modules not installed" && false )
# 
# install:: check
# 	$(ECHO) "      installing bigjob modules"
# 	$(MKDIR)              $(SAGA_PY_MODULE_ROOT)/bigjob/
# 	$(CP)    -r generic/* $(SAGA_PY_MODULE_ROOT)/bigjob/

HAVE_SETUP      = $(shell env PYTHONPATH=$(SAGA_LOCATION)/$(SAGA_PYTHON_PKG_PATH):$(PYTHONPATH) python -c "from setuptools import setup, find_packages" 2>&1 && echo yes)

install-check::
	@$(ECHO) "      checking   bigjob installation pre-requisites"
	 $(TEST) "yes" = '$(HAVE_SETUP)'      || echo "     installing missing python setuptools"
	 $(TEST) "yes" = '$(HAVE_SETUP)'      || (cd ext/setuptools-0.6c11/ ; env PYTHONPATH=$(SAGA_LOCATION)/$(SAGA_PYTHON_PKG_PATH) $(SAGA_PYTHON) setup.py install --prefix=$(SAGA_LOCATION) )

install:: install-check
	@$(ECHO) "      installing bigjob modules"
	 cd generic ; env PYTHONPATH=$(SAGA_LOCATION)/$(SAGA_PYTHON_PKG_PATH) $(SAGA_LOCATION)/bin/saga-run.sh $(SAGA_PYTHON) setup.py install --prefix=$(SAGA_LOCATION)

