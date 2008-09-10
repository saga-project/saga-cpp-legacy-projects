#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Use, modification and distribution is subject to the Boost Software
#  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

SAGA_SRC         = $(wildcard *.cpp)
SAGA_BIN         = dayinlife

SAGA_ADD_BIN_OBJ = $(SAGA_SRC:%.cpp=%.o)

UID              = $(shell id -u)
LOG              = ./dayinlife.log

ITER             = 60
NAME             = dayinlife

include $(SAGA_LOCATION)/share/saga/make/saga.application.mk

clean:: tmpclean

.PHONY: tmpclean
tmpclean::
	@$(ECHO) "      clean tmps"
	@$(RM)   saga_advert.db saga_replica.db input.dat output.dat $(LOG)

run:: tmpclean $(SAGA_BIN)
	@$(RM) $(LOG)
	 $(SAGA_PWD)/$(SAGA_BIN) $(NAME) $(ITER)
	@$(ECHO) "- LOG -"
	@tail -f $(LOG)
	@$(ECHO) "-----------"
  
