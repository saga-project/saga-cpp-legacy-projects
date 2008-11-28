//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>

#include "../utils/defines.hpp"
#include "ConfigFileParser.hpp"
#include "../xmlParser/xmlParser.h"

using namespace MapReduce::Master;

ConfigFileParser::ConfigFileParser()
{
}

ConfigFileParser::ConfigFileParser(std::string cfgFilePath)
: cfgFilePath_(cfgFilePath)
{
}

SessionDescription ConfigFileParser::getSessionDescription()
{
  return sessionDesc_;
}

std::vector<BinaryDescription> ConfigFileParser::getExecutableList()
{
  return binDescList_;
}

std::vector<FileDescription> ConfigFileParser::getFileList()
{
  return fileDescList_;
}

std::vector<HostDescription> ConfigFileParser::getTargetHostList()
{
  return targetHostList_;
}

std::string ConfigFileParser::getOutputPrefix()
{
  return outputPrefix_;
}

void ConfigFileParser::parse(void)
{
  std::string tmp("");
  
  XMLNode xMainNode=XMLNode::openFileHelper(cfgFilePath_.c_str(),"MRDL");
  xMainNode = xMainNode.getChildNode("MapReduceSession");
  
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
     std::cerr << "[Error] " << message << std::endl;
     throw xmlParser::exception(message);
  }
    
  // parse the TargetHosts section
  xNode = xMainNode.getChildNode("TargetHosts");
  int n=xNode.nChildNode("Host");
  for(int i=0; i<n; ++i) {
    HostDescription hd; bool complete = true;
    XMLNode tmpNode = xNode.getChildNode("Host", i);
    if( NULL != tmpNode.getAttribute("arch") )
      hd.hostArch=tmpNode.getAttribute("arch"); else complete = false;
    if( NULL != tmpNode.getAttribute("OS") )
      hd.hostOS=tmpNode.getAttribute("OS"); else complete = false;
    if( NULL != tmpNode.getText())
      hd.rmURL = tmpNode.getText(); else complete = false;	

    if(complete)
      targetHostList_.push_back(hd);
    else {
       std::string message("XML Parser: Incomplete TargetHost/Host section found");
       std::cerr << "[Error] " << message << std::endl;
       throw xmlParser::exception(message);
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
      std::cerr << "[Error] " << message << std::endl;
      throw xmlParser::exception(message);
    }
  }

  xNode = xMainNode.getChildNode("OutputPrefix");
  if( NULL != xNode.getText() ) {
    outputPrefix_ = xNode.getText();
  }
  else {
     std::string message("XML Parser: Incomplete OutputPrefix section found");
     std::cerr << "[Error] " << message << std::endl;
     throw xmlParser::exception(message);
  }

  // parse the ApplicationFiles section
  xNode = xMainNode.getChildNode("ApplicationFiles");
  int k=xNode.nChildNode("File");
  for(int i=0; i<k; ++i) 
  {
    FileDescription fd;
    bool complete = true;
    XMLNode tmpNode = xNode.getChildNode("File", i);
    if( NULL != tmpNode.getText() ) {
       fd.name = tmpNode.getText();
    }
    else complete = false;
    if(!complete) {
       std::string message("XML Parser: Incomplete ApplicationFile section found");
       std::cerr << "[Error] " << message << std::endl;
       throw xmlParser::exception(message);
    }
    else
       fileDescList_.push_back(fd);
  }
}

