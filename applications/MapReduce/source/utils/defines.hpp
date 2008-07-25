//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MR_DEFINES_HPP
#define MR_DEFINES_HPP

#define APPLICATION_ABORT       throw std::exception();

#define ATTR_LAST_SEEN          "LAST_SEEN"
#define ATTR_STARTUP_TIME       "STARTUP_TIME"
                                
#define ATTR_HOST_NAME          "HOST_NAME"
#define ATTR_HOST_TYPE          "HOST_TYPE"
#define ATTR_HOST_LOAD_AVG      "HOST_LOAD_AVG"
                                
#define ATTR_CPU_COUNT          "CPU_COUNT"
#define ATTR_CPU_TYPE           "CPU_TYPE"
#define ATTR_CPU_SUBTYPE        "CPU_SUBTYPE"
#define ATTR_CPU_FREQ           "CPU_FREQ"
#define ATTR_MEM_TOTAL          "MEM_TOTAL"
                                
#define ATTR_EXE_ARCH           "EXE_ARCH"
#define ATTR_EXE_LOCATION       "EXE_LOCATION"
                                
#define ATTR_IN_FILES           "IN_FILES"
#define ATTR_OUT_FILES          "OUT_FILES"
#define ATTR_CHUNK_INVOKE_STR   "CHUNK_INVOKE_STR"
                                
#define ADVERT_DIR_WORKERS      "WORKERS"
#define ADVERT_DIR_BINARIES     "BINARIES"
#define ADVERT_DIR_CHUNKS       "CHUNKS"
#define ADVERT_DIR_INTERMEDIATE "INTERMEDIATE"
#define ADVERT_DIR_REDUCE_INPUT "REDUCE_INPUT"

#define WORKER_COMMAND_PAUSE    "PAUSE"   // Pause the current job
#define WORKER_COMMAND_RESUME   "RESUME"  // Resume the current job
#define WORKER_COMMAND_DISCARD  "DISCARD" // Discard the current job
#define WORKER_COMMAND_QUIT     "QUIT"    // Cleanup and quit the agent
#define WORKER_COMMAND_MAP      "MAP"     // Run a map on file
#define WORKER_COMMAND_REDUCE   "REDUCE"  // Run a reduce on files 

#define WORKER_STATE_IDLE       "IDLE"     // Ready to Map or Reduce
#define WORKER_STATE_DONE       "DONE"     // Done with assigned task
#define WORKER_STATE_FAIL       "FAIL"     // Assigned task failed 
#define WORKER_STATE_MAPPING    "MAPPING"  // Currently Mapping
#define WORKER_STATE_REDUCING   "REDUCING" // Currently Reducing

#define NUM_MAPS 3
#define MAX_INTERMEDIATE_SIZE 1000

#define LOGLEVEL_FATAL     0x2   
#define LOGLEVEL_ERROR     0x4  
#define LOGLEVEL_WARNING   0x8   
#define LOGLEVEL_INFO      0x16   
#define LOGLEVEL_DEBUG     0x32  

#endif // MR_DEFINES_HPP

