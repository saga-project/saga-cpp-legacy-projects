//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>

#include "../utils/defines.hpp"
#include "ConfigFileParser.hpp"
#include "../xmlParser/xmlParser.h"

using namespace AllPairs::Master;
using namespace std;

ConfigFileParser::ConfigFileParser() {
}

ConfigFileParser::ConfigFileParser(std::string cfgFilePath, AllPairs::LogWriter &log)
: cfgFilePath_(cfgFilePath), log_(&log) {
   parse_();
   //generateTestInstance_();
}

SessionDescription ConfigFileParser::getSessionDescription() {
   return sessionDesc_;
}

vector<BinaryDescription> ConfigFileParser::getExecutableList() {
   return binDescList_;
}

vector<FileDescription> ConfigFileParser::getFileListBase() {
   return fileDescListBase_;
}

vector<FileDescription> ConfigFileParser::getFileListFragment() {
   return fileDescListFragment_;
}

vector<HostDescription> ConfigFileParser::getTargetHostList() {
   return targetHostList_;
}

void ConfigFileParser::parse_(void) {
   std::string tmp("");
   try {
      XMLNode xMainNode=XMLNode::openFileHelper(cfgFilePath_.c_str(),"APDL");
      xMainNode = xMainNode.getChildNode("AllPairsSession");
      // parse the TaskFarmingSession node
      if( NULL != xMainNode.getAttribute("name") )		
         sessionDesc_.name = xMainNode.getAttribute("name");
      if( NULL != xMainNode.getAttribute("version") )
         sessionDesc_.version = xMainNode.getAttribute("version");
      if( NULL != xMainNode.getAttribute("user") )
         sessionDesc_.user = xMainNode.getAttribute("user");
      if( NULL != xMainNode.getAttribute("priority") )
         sessionDesc_.priority = xMainNode.getAttribute("priority");
      if( NULL != xMainNode.getAttribute("experimentID") )
         sessionDesc_.experimentID = xMainNode.getAttribute("experimentID");
      if( NULL != xMainNode.getAttribute("eventLevel") )
         sessionDesc_.eventLevel = xMainNode.getAttribute("eventLevel");
      // parse the OrchestratorDB section
      XMLNode xNode = xMainNode.getChildNode("OrchestratorDB").getChildNode("Host");
      if( NULL != xNode.getText())
         sessionDesc_.orchestrator = xNode.getText();
      else {
         std::string message("XML Parser: Orchestrator section not found");
         log_->write(message, LOGLEVEL_ERROR);
      }
      // parse the TargetHosts section
      xNode = xMainNode.getChildNode("TargetHosts");
      int n=xNode.nChildNode("Host");
      for(int i=0; i<n; ++i) {
         HostDescription hd; bool complete = true;
         XMLNode tmpNode = xNode.getChildNode("Host", i);
         if(NULL != tmpNode.getAttribute("arch"))
            hd.hostArch=tmpNode.getAttribute("arch"); else complete = false;
         if(NULL != tmpNode.getAttribute("OS"))
            hd.hostOS=tmpNode.getAttribute("OS"); else complete = false;
         if(NULL != tmpNode.getText())
            hd.rmURL = tmpNode.getText(); else complete = false;	
         if(complete)
            targetHostList_.push_back(hd);
         else {
            std::string message("XML Parser: Incomplete TargetHost/Host section found");
            log_->write(message, LOGLEVEL_ERROR);
         }
      }
      // parse the ApplicationBinaries section
      xNode = xMainNode.getChildNode("ApplicationBinaries");
      int m=xNode.nChildNode("BinaryImage");
      for(int i=0; i<m; ++i) {
         BinaryDescription bd; bool complete = true;
         XMLNode tmpNode = xNode.getChildNode("BinaryImage", i);
         if( NULL != tmpNode.getAttribute("arch") ) 
            bd.targetArch=tmpNode.getAttribute("arch"); else complete = false;
         if( NULL != tmpNode.getAttribute("OS") ) 
            bd.targetOS=tmpNode.getAttribute("OS"); else complete = false;
         if( NULL != tmpNode.getAttribute("extraArgs") ) 
            bd.extraArgs=tmpNode.getAttribute("extraArgs"); 
         if( NULL != tmpNode.getText())
            bd.URL = tmpNode.getText(); else complete = false;
         if(complete)
            binDescList_.push_back(bd);
         else {
            std::string message("XML Parser: Incomplete ApplicationBinaries/BinaryImage section found");
            log_->write(message, LOGLEVEL_ERROR);
         }
      }
      // parse the ApplicationFiles section
      xNode = xMainNode.getChildNode("ApplicationFiles").getChildNode("Bases");
      int k=xNode.nChildNode("File");
      for(int i=0; i<k; ++i) {
         FileDescription fd;
         bool complete = true;
         XMLNode tmpNode = xNode.getChildNode("File", i);
         if( NULL != tmpNode.getText() ) {
            fd.name = tmpNode.getText();
         }
         else complete = false;
         if(!complete) {
            std::string message("XML Parser: Incomplete Base files section found");
            log_->write(message, LOGLEVEL_ERROR);
            break;
         }
         else
            fileDescListBase_.push_back(fd);
      }
      xNode = xMainNode.getChildNode("ApplicationFiles").getChildNode("Fragments");
      k = xNode.nChildNode("File");
      for(int i=0; i<k; ++i) {
         FileDescription fd;
         bool complete = true;
         XMLNode tmpNode = xNode.getChildNode("File", i);
         if( NULL != tmpNode.getText() ) {
            fd.name = tmpNode.getText();
         }
         else complete = false;
         if(!complete) {
            std::string message("XML Parser: Incomplete ApplicationFile section found");
            log_->write(message, LOGLEVEL_ERROR);
            break;
         }
         else
            fileDescListFragment_.push_back(fd);
      }
   }
   catch(xmlParser::exception const &e) {
      std::string message("XML Parser FAILED:");
      message.append(e.what());
      log_->write(message, LOGLEVEL_FATAL);
      throw e; // propagate exception!
   }
}
