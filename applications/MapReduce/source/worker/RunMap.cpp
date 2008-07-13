#include "RunMap.hpp"

namespace MapReduce {

/*********************************************************
 * RunMap constructor sets up the state and other info   *
 * for the worker.                                       *
 * ******************************************************/
   RunMap::RunMap(saga::advert::directory workerDir,
                 saga::advert::directory chunksDir,
                 saga::advert::directory intermediateDir) {
         chunksDir_       = chunksDir;
         intermediateDir_ = intermediateDir;
         workerDir_       = workerDir;
      try {
         workerDir_.set_attribute("COMMAND", "");
         workerDir_.set_attribute("STATE", WORKER_STATE_MAPPING);
      }
      catch(saga::exception const & e) {
         throw;
      }
   }

/*********************************************************
 * ~RunMap destructor returns the state of the worker to *
 * what is expected by the master after a mapping is done*
 * ******************************************************/
   RunMap::~RunMap() {
      int mode = saga::advert::ReadWrite | saga::advert::Create;
      try {
         for(int count = 0; count < NUM_MAPS; count++) {
            saga::advert::entry adv = intermediateDir_.open(saga::url("mapFile-"+boost::lexical_cast<std::string>(count)), mode);
            std::string filestring("/tmp/mapFile-" + boost::lexical_cast<std::string>(count));
            saga::filesystem::file f(saga::url(filestring), saga::filesystem::ReadWrite);
            adv.store_string(f.get_url().get_string());
         }
         workerDir_.set_attribute("STATE", WORKER_STATE_DONE);
         workerDir_.set_attribute("COMMAND", "");
      }
      catch(saga::exception const & e) {
         throw;
      }
      sleep(5);
   }

/*********************************************************
 * getFile retreives the chunk that was posted for this  *
 * worker to use from the advert database.               *
 * ******************************************************/
   std::string RunMap::getFile() {
      try {
         saga::advert::entry adv(chunksDir_.open(saga::url("./chunk"), saga::advert::ReadWrite));
         return adv.retrieve_string();
      }
      catch(saga::exception const & e) {
         throw;
      }
   }
} // namespace MapReduce

