#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Use, modification and distribution is subject to the Boost Software
#  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

SAGA_SRC         = $(wildcard *.cpp)
SAGA_BIN         = dayinlife

SAGA_ADD_BIN_OBJ = $(SAGA_SRC:%.cpp=%.o)

UID              = $(shell id -u)
LOG              = dayinlife.log

include $(SAGA_LOCATION)/share/saga/make/saga.application.mk

clean:: tmpclean

.PHONY: tmpclean
tmpclean::
	@$(ECHO) "      clean tmps"
	@$(RM)   saga_advert.db saga_replica.db counter.out counter.in $(LOG) exception.log

run:: tmpclean $(SAGA_BIN)
	@$(RM) $(LOG)
	-$(SAGA_PWD)/$(SAGA_BIN) localhost localhost 11
	-@tail -f $(LOG)
	@$(ECHO) "counter: "
	@$(CAT)  counter.out 
	@$(ECHO) ""
  
