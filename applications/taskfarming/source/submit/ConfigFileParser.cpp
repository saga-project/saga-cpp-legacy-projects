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

#include <iostream>

#include "defines.hpp"
#include "ConfigFileParser.hpp"
#include "../xmlParser/xmlParser.h"

using namespace TaskFarmer::Frontend;

ConfigFileParser::ConfigFileParser()
{
}

ConfigFileParser::ConfigFileParser(std::string cfgFilePath, LogWriter &log)
: cfgFilePath_(cfgFilePath), log_(&log)
{
  parse_();
  //generateTestInstance_();
}

SessionDescription ConfigFileParser::getSessionDescription()
{
  return sessionDesc_;
}

vector<BinaryDescription> ConfigFileParser::getExecutableList()
{
  return binDescList_;
}

vector<ChunkDescription> ConfigFileParser::getChunkList()
{
  return chunkDescList_;
}

vector<HostDescription> ConfigFileParser::getTargetHostList()
{
  return targetHostList_;
}

void ConfigFileParser::parse_(void)
{
  std::string tmp("");
  
  try {
    XMLNode xMainNode=XMLNode::openFileHelper(cfgFilePath_.c_str(),"TFDL");
	xMainNode = xMainNode.getChildNode("TaskFarmingSession");
	
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
	  if( NULL != tmpNode.getAttribute("arch") )
		hd.hostArch=tmpNode.getAttribute("arch"); else complete = false;
	  if( NULL != tmpNode.getAttribute("OS") )
		hd.hostOS=tmpNode.getAttribute("OS"); else complete = false;
	  if( NULL != tmpNode.getAttribute("arch") )
		hd.rmQueue=tmpNode.getAttribute("queue");
	  if( NULL != tmpNode.getText())
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
	
	// parse the ApplicationChunks section
	xNode = xMainNode.getChildNode("ApplicationChunks");
	int k=xNode.nChildNode("Chunk");
	for(int i=0; i<k; ++i) 
	{
	  ChunkDescription cd; bool complete = true;
	  XMLNode tmpNode = xNode.getChildNode("Chunk", i);
	  if( NULL != tmpNode.getAttribute("invokeString") ) 
		cd.invokeString = tmpNode.getAttribute("invokeString"); else complete = false;
	  if( NULL != tmpNode.getAttribute("name") ) 
		cd.name = tmpNode.getAttribute("name"); else complete = false;
	  
	  if(complete) {
	    std::string message("XML Parser: Incomplete ApplicationChunks/Chunk section found");
		log_->write(message, LOGLEVEL_ERROR);
		break;
	  }
	  
	  // parse the File section within the chunks
	  int l=tmpNode.nChildNode("File");
	  for(int j=0; j<l; ++j) {
		XMLNode fileTmpNode = tmpNode.getChildNode("File", j);
		if( NULL != fileTmpNode.getText() )
		  cd.stageInFiles.push_back(fileTmpNode.getText());
	  }
	  chunkDescList_.push_back(cd);
	}
  }
  catch(xmlParser::exception const &e) 
  {
	std::string message("XML Parser FAILED:");
	message.append(e.what());
	log_->write(message, LOGLEVEL_FATAL);
	throw e; // propagate exception!
  }
}

