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

#ifndef TF_FRONTEND_CFGFILEPARSER_HPP
#define TF_FRONTEND_CFGFILEPARSER_HPP

#include <string>
#include <vector>

#include "LogWriter.hpp"

using namespace std;

namespace TaskFarmer 
{
  namespace Frontend 
  {
	/*! Describes a session
	 *
	 */
	typedef struct SessionDescription {
	  string name;
	  string version;
	  string orchestrator;
	  string user;
	  string priority;
	  string experimentID;
	  string eventLevel;
	};
	
    /*! Describes an executable 
     *
     */
    typedef struct BinaryDescription {
      string URL;
      string targetOS;
      string targetArch;
      string extraArgs;
    };

    /*! Describes an executable 
     *
     */
    typedef struct HostDescription {
      string rmURL;
      string rmQueue;
      string hostOS;
      string hostArch;
    };
    
    /*! Describes a data chunk 
     *
     */
    typedef struct ChunkDescription {
      vector<string> stageInFiles;
      string         invokeString;
	  string		 name;
    };
 
    /*! Parses a Task Farming config file 
     *
     */
    class ConfigFileParser
    {      
    public:
	  ConfigFileParser();
      ConfigFileParser(std::string cfgFilePath, LogWriter &log);
      
	  SessionDescription		getSessionDescription(void);
      vector<BinaryDescription> getExecutableList(void);
      vector<HostDescription>   getTargetHostList(void);
      vector<ChunkDescription>  getChunkList(void);
      
    private:
	  std::string cfgFilePath_;
	  LogWriter*  log_;
	
	  SessionDescription		sessionDesc_;
      vector<HostDescription>   targetHostList_;
      vector<BinaryDescription> binDescList_;
      vector<ChunkDescription>  chunkDescList_;
      
	  void parse_(void);
    };
  } //namespace Frontend
} //namespace TaskFarmer

#endif //TF_FRONTEND_CFGFILEPARSER_HPP

