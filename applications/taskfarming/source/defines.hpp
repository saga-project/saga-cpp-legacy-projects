// The MIT License
//
// Copyright (c) 2007 Ole Weidner (oweidner@cct.lsu.edu)

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef TF_AGENT_DEFINES_HPP
#define TF_AGENT_DEFINES_HPP

#define APPLICATION_ABORT	  throw std::exception();

#define ATTR_LAST_SEEN        "LAST_SEEN"
#define ATTR_STARTUP_TIME     "STARTUP_TIME"

#define ATTR_HOST_NAME        "HOST_NAME"
#define ATTR_HOST_TYPE        "HOST_TYPE"
#define ATTR_HOST_LOAD_AVG    "HOST_LOAD_AVG"

#define ATTR_CPU_COUNT        "CPU_COUNT"
#define ATTR_CPU_TYPE         "CPU_TYPE"
#define ATTR_CPU_SUBTYPE      "CPU_SUBTYPE"
#define ATTR_CPU_FREQ         "CPU_FREQ"
#define ATTR_MEM_TOTAL        "MEM_TOTAL"

#define ATTR_EXE_ARCH         "EXE_ARCH"
#define ATTR_EXE_LOCATION     "EXE_LOCATION"

#define ATTR_CHUNK_IN_FILES   "CHUNK_IN_FILES"
#define ATTR_CHUNK_OUT_FILES  "CHUNK_OUT_FILES"
#define ATTR_CHUNK_INVOKE_STR "CHUNK_INVOKE_STR"

#define ADVERT_DIR_AGENTS     "AGENTS"
#define ADVERT_DIR_BINARIES   "BINARIES"
#define ADVERT_DIR_CHUNKS     "CHUNKS"

#define AGENT_COMMAND_PAUSE    0x2    // Pause the current job
#define AGENT_COMMAND_RESUME   0x4    // Resume the current job
#define AGENT_COMMAND_DISCARD  0x8   // Discard the current job
#define AGENT_COMMAND_QUIT     0x16   // Cleanup and quit the agent

#define LOGLEVEL_FATAL     0x2   
#define LOGLEVEL_ERROR     0x4  
#define LOGLEVEL_WARNING   0x8   
#define LOGLEVEL_INFO      0x16   
#define LOGLEVEL_DEBUG     0x32  


#endif

