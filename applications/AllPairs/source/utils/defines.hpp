//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AP_DEFINES_HPP
#define AP_DEFINES_HPP

#define APPLICATION_ABORT       throw std::exception();

#define ATTR_LAST_SEEN             "LAST_SEEN"
#define ATTR_STARTUP_TIME          "STARTUP_TIME"
                                   
#define ATTR_HOST_NAME             "HOST_NAME"
#define ATTR_HOST_TYPE             "HOST_TYPE"
#define ATTR_HOST_LOAD_AVG         "HOST_LOAD_AVG"
                                   
#define ATTR_CPU_COUNT             "CPU_COUNT"
#define ATTR_CPU_TYPE              "CPU_TYPE"
#define ATTR_CPU_SUBTYPE           "CPU_SUBTYPE"
#define ATTR_CPU_FREQ              "CPU_FREQ"
#define ATTR_MEM_TOTAL             "MEM_TOTAL"
                                   
#define ATTR_EXE_ARCH              "EXE_ARCH"
#define ATTR_EXE_LOCATION          "EXE_LOCATION"

#define MASTER_QUESTION_STATE      "STATE?"
#define MASTER_QUESTION_ADVERT     "ADVERT?"
#define MASTER_QUESTION_RESULT     "RESULT?"

#define WORKER_RESPONSE_ACKNOLEDGE "ACKNOLEDGE"
#define WORKER_RESPONSE_REJECT     "REJECT"

#define ADVERT_DIR_WORKERS         "WORKERS"
#define ADVERT_DIR_BINARIES        "BINARIES"
#define ADVERT_DIR_RESULTS         "RESULTS"
#define ADVERT_DIR_BASE_FILES      "BASE-FILES"
#define ADVERT_DIR_FRAGMENT_FILES  "FRAGMENT_FILES"
#define ADVERT_ENTRY_SERVER        "SERVER"

#define WORKER_COMMAND_COMPARE     "COMPARE"    // Copy given file to another host
#define WORKER_COMMAND_QUIT        "QUIT"       // Quit the worker

#define WORKER_STATE_IDLE          "IDLE"       // Ready to Map or Reduce
#define WORKER_STATE_DONE          "DONE"       // Done with assigned task
#define WORKER_STATE_FAIL          "FAIL"       // Assigned task failed 
#define WORKER_STATE_COMPARING     "COMPARING"  // Done with assigned task

#define LOGLEVEL_FATAL             0x2   
#define LOGLEVEL_ERROR             0x4  
#define LOGLEVEL_WARNING           0x8   
#define LOGLEVEL_INFO              0x16   
#define LOGLEVEL_DEBUG             0x32  

#endif // AP_DEFINES_HPP

