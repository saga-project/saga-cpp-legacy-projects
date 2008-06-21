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

#include "version.hpp"
#include "defines.hpp"
#include "SubmitApp.hpp"

using namespace saga;
using namespace TaskFarmer::Frontend;

///////////////////////////////////////////////////////////////////////////////
//
SubmitApp::SubmitApp(std::string configFilePath)
{
  // create new LogWriter instance that writes to stdout
  log = new TaskFarmer::LogWriter(std::string(TF_FRONTEND_EXE_NAME), 
                                  std::cout);
								  
  cfgFileParser_ = ConfigFileParser(configFilePath, *log);
  database_ = cfgFileParser_.getSessionDescription().orchestrator;
  
  // generate a startup timestamp 
  time(&startupTime_);
  
  // create a UUID for this agent
  //uuid_ = uuid().string();
  uuid_ = "DUMMY-UUID";
  
  /*std::cout << std::string(TF_FRONTEND_EXE_NAME) << " " 
            << TF_FRONTEND_VERSION_FULL << " - creating new session: " << std::endl;
			  
  std::cout << "\tOrchestratorDB      = " << database_ << std::endl
			<< "\tNew session UUID    = " << uuid_ << std::endl
			<< "\tSession description = " << configFilePath << std::endl;*/
  
}

///////////////////////////////////////////////////////////////////////////////
//
SubmitApp::~SubmitApp()
{
  delete log;
}

///////////////////////////////////////////////////////////////////////////////
//
void SubmitApp::run()
{
  // register with the db
  registerWithDB_();
  
  // create a new session
  createNewSession_();
  
  // add binaries to the Orchestartor DB
  populateBinariesList_();
  
  // add work chunks to the Orchestartor DB
  populateChunkList_();
  
  // spawn agents
  spawnAgents_();
  
  log->write("All done - exiting normally :)", LOGLEVEL_INFO);
}

///////////////////////////////////////////////////////////////////////////////
//
void SubmitApp::registerWithDB_()
{ 
  std::string message("Connecting to Orchestrator (");
  message += (database_) + ")... ";
  
  int mode = advert::ReadWrite;  
  std::string advertKey("advert://");
  advertKey += database_ + "//" ;
  try {
    advert::directory(advertKey, mode);
  }
  catch(saga::exception const & e) {
    message += "FAILED ("; message += e.what(); message += ")";
    log->write(message, LOGLEVEL_FATAL);
	APPLICATION_ABORT
  }
  message += "SUCCESS"; log->write(message, LOGLEVEL_INFO);
}

///////////////////////////////////////////////////////////////////////////////
//
void SubmitApp::createNewSession_()
{ 
  std::string message("Creating a new session (");
  message += (uuid_) + ")... ";

  int mode = advert::ReadWrite | advert::Create;  
  std::string advertKey("advert://");
  advertKey += database_ + "//" + uuid_ + "/";
  try {
    sessionBaseDir_ = advert::directory(advertKey, mode);
	sessionBaseDir_.set_attribute("name",    cfgFileParser_.getSessionDescription().name);
	sessionBaseDir_.set_attribute("user",    cfgFileParser_.getSessionDescription().user);
	sessionBaseDir_.set_attribute("version", cfgFileParser_.getSessionDescription().version);
	
    agentsDir_   = sessionBaseDir_.open_dir(ADVERT_DIR_AGENTS, mode);
    binariesDir_ = sessionBaseDir_.open_dir(ADVERT_DIR_BINARIES, mode);
    chunksDir_   = sessionBaseDir_.open_dir(ADVERT_DIR_CHUNKS, mode);
  }
  catch(saga::exception const & e) {
    message += "FAILED ("; message += e.what(); message += ")";
    log->write(message, LOGLEVEL_FATAL);
	APPLICATION_ABORT
  }
  message += "SUCCESS"; log->write(message, LOGLEVEL_INFO);
}

///////////////////////////////////////////////////////////////////////////////
//
void SubmitApp::populateBinariesList_(void)
{
  unsigned int successCounter = 0;
  vector<BinaryDescription> binaryList = cfgFileParser_.getExecutableList();
  vector<BinaryDescription>::const_iterator binaryListIT = binaryList.begin();
  while(binaryListIT != binaryList.end())
  {
    std::string message("Adding new binary for "+ (*binaryListIT).targetOS + "/" 
                        + (*binaryListIT).targetArch + " to session... ");
    try {
      int mode = advert::ReadWrite | advert::Create;
      advert::entry adv = binariesDir_.open((*binaryListIT).targetOS+"_"+(*binaryListIT).targetArch, mode);
      adv.set_attribute(ATTR_EXE_ARCH,      (*binaryListIT).targetArch);
      adv.set_attribute(ATTR_EXE_LOCATION,  (*binaryListIT).URL);
      //adv.set_attribute(ATTR_EXE_LOCATION,  (*binaryListIT).extraArgs);
      message += "SUCCESS"; log->write(message, LOGLEVEL_INFO);
      ++successCounter;
    }
    catch(saga::exception const & e) {
      message += "FAILED ("; message += e.what(); message += ")";
      log->write(message, LOGLEVEL_ERROR);
    }
    ++binaryListIT;
  }
  if(successCounter == 0) {
    log->write("No binaries defined for this session. ABORTING", LOGLEVEL_FATAL);
    APPLICATION_ABORT;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
void SubmitApp::populateChunkList_(void)
{
  unsigned int successCounter = 0;
  vector<ChunkDescription> chunkList = cfgFileParser_.getChunkList();
  vector<ChunkDescription>::const_iterator chunkListIT = chunkList.begin();
  while(chunkListIT != chunkList.end())
  {
    // create a UUID for this chunk
    string chunkUUID = uuid().string();
    string message("Adding new chunk with UUID "+ chunkUUID + " to session...");
    try {
      int mode = advert::ReadWrite | advert::Create;
      advert::entry adv = chunksDir_.open(chunkUUID, mode);
  
      // iterate over stage-in files
      std::string inFileList("");
      vector<string>::const_iterator inFilesIT = (*chunkListIT).stageInFiles.begin();
      while(inFilesIT != (*chunkListIT).stageInFiles.end()) 
      {
        inFileList += (*inFilesIT) + ";";
        ++inFilesIT;
      }

      adv.set_attribute(ATTR_CHUNK_IN_FILES, inFileList);
      message += "SUCCESS"; log->write(message, LOGLEVEL_INFO);
      ++successCounter;
    }
    catch(saga::exception const & e) {
      message += "FAILED ("; message += e.what(); message += ")";
      log->write(message, LOGLEVEL_ERROR);
    }    
    ++chunkListIT;
  }
  if(successCounter == 0) {
    log->write("No chunks defined for this session. ABORTING", LOGLEVEL_FATAL);
    APPLICATION_ABORT;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
void SubmitApp::spawnAgents_(void)
{
  unsigned int successCounter = 0;
  vector<HostDescription>  hostList = cfgFileParser_.getTargetHostList();
  vector<HostDescription>::const_iterator hostListIT = hostList.begin();
  while(hostListIT != hostList.end()) 
  {
    std::string message("Launching agent on host " + (*hostListIT).rmURL + "... ");
    try {
      /* create a job description */
      job::description jd;
      jd.set_attribute(job::attributes::description_executable, 
                       "./tf_agent/agent");
      jd.set_attribute(job::attributes::description_candidatehosts, 
                       (*hostListIT).rmURL);
      jd.set_attribute(job::attributes::description_queue, 
                       (*hostListIT).rmQueue);
      
      job::service js((*hostListIT).rmURL);
      job::job agentJob= js.create_job(jd);
      agentJob.run();
      
      message += "SUCCESS"; log->write(message, LOGLEVEL_INFO);
      ++successCounter;
    }
    catch(saga::exception const & e) {
      message += "FAILED ("; message += e.what(); message += ")";
      log->write(message, LOGLEVEL_ERROR);
    }
    ++hostListIT;
  }
  
  if(successCounter == 0) {
    log->write("Couldn't launch any agents. ABORTING", LOGLEVEL_FATAL);
    APPLICATION_ABORT
  }
}
