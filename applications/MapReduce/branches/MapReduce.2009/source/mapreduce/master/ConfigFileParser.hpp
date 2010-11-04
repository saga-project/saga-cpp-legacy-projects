//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MR_FRONTEND_CFGFILEPARSER_HPP
#define MR_FRONTEND_CFGFILEPARSER_HPP

#include <string>
#include <vector>
#include "../../utils/LogWriter.hpp"

namespace mapreduce {
   namespace master {
      // Describes a session
      struct SessionDescription {
         std::string name;
         std::string version;
         std::string orchestrator;
         std::string user;
         std::string priority;
         std::string experimentID;
         std::string eventLevel;
      };
      
      // Describes an executable 
      struct BinaryDescription {
         std::string URL;
         std::string targetOS;
         std::string targetArch;
         std::string extraArgs;
      };
  
      // Describes an executable 
      struct HostDescription {
         std::string rmURL;
         std::string hostOS;
         std::string hostArch;
      };
      
      // Describes a data file
      struct FileDescription {
         std::string name;
      };
   
      // Parses a Task Farming config file 
      class ConfigFileParser {
        private:
         std::string                    cfgFilePath_;
         SessionDescription             sessionDesc_;
         std::vector<HostDescription>   targetHostList_;
         std::vector<BinaryDescription> binDescList_;
         std::vector<FileDescription>   fileDescList_;
         std::string                    outputBase_;
         std::string                    masterAddress_;
         mapreduce::LogWriter          *log_;
        

        public:
         ConfigFileParser();
         ConfigFileParser(std::string cfgFilePath, mapreduce::LogWriter &log);
         void parse_(void);
        
         SessionDescription             getSessionDescription(void);
         std::vector<BinaryDescription> getExecutableList(void);
         std::vector<HostDescription>   getTargetHostList(void);
         std::vector<FileDescription>   getFileList(void);
         std::string                    getOutputBase(void);
         std::string                    getMasterAddress();
        
      };
   } //namespace master
} //namespace mapreduce

#endif //MR_FRONTEND_CFGFILEPARSER_HPP
