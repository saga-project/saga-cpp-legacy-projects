#  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
# 
#  Distributed under the Boost Software License, Version 1.0. (See accompanying 
#  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

SAGA_MAKE_INCLUDE_ROOT = $(SAGA_LOCATION)/share/saga/make/

SAGA_SRC   += attribute_set.cpp
SAGA_SRC   += attribute_values_view.cpp
SAGA_SRC   += slim_string.cpp
SAGA_SRC   += code_conversion.cpp
SAGA_SRC   += core.cpp
SAGA_SRC   += stream_provider.cpp
SAGA_SRC   += severity_level.cpp
SAGA_SRC   += global_logger_storage.cpp
SAGA_SRC   += named_scope.cpp
SAGA_SRC   += timer.cpp
SAGA_SRC   += sink.cpp
SAGA_SRC   += text_ostream_backend.cpp
SAGA_SRC   += thread_specific.cpp
SAGA_SRC   += parser_utils.cpp
SAGA_SRC   += init_from_stream.cpp
SAGA_SRC   += filter_parser.cpp
SAGA_SRC   += formatter_parser.cpp

ifdef SAGA_WINDOWS
  SAGA_SRC += simple_event_log.mc
  SAGA_SRC += event_log_backend.cpp
  SAGA_SRC += nt6_event_log_backend.cpp
  SAGA_SRC += debug_output_backend.cpp
else
  SAGA_SRC += syslog_backend.cpp
endif

SAGA_OBJ   = $(SAGA_SRC:%.cpp=%.o)


include $(SAGA_MAKE_INCLUDE_ROOT)/saga.mk

ifndef SAGA_WINDOWS
  SAGA_CXXFLAGS += -I../../../
  SAGA_CXXFLAGS += -DBOOST_LOG_DLL
  SAGA_CXXFLAGS += -DBOOST_LOG_USE_NATIVE_SYSLOG=1
endif

event_log_backend.cpp: simple_event_log.mc

