#include "RunReduce.hpp"
#include "../utils/defines.hpp"
#include "../utils/merger.hpp"
#include "../utils/parseMapLine.hpp"

namespace MapReduce {

/*********************************************************
 * RunReduce constructor sets the state of the worker and*
 * retreives input files from the advert database posted *
 * by the master.                                        *
 * ******************************************************/
   RunReduce::RunReduce(saga::advert::directory workerDir, saga::advert::directory reduceInputDir,
                        std::string outputPrefix) {
      int mode = saga::advert::ReadWrite;
      workerDir_      = workerDir;
      reduceInputDir_ = reduceInputDir;
      outputPrefix_   = outputPrefix;
      try {
          workerDir_.set_attribute("STATE", WORKER_STATE_REDUCING);
          std::vector<saga::url> entries = reduceInputDir_.list("*");
          std::vector<saga::url>::const_iterator entriesIT = entries.begin();
          while(entriesIT != entries.end()) {
             saga::advert::entry adv(*entriesIT, mode);
             files_.push_back(adv.retrieve_string());
             entriesIT++;
          }
      }
      catch(saga::exception const & e) {
         throw;
      }
   }

/*********************************************************
 * ~RunReduce destructor returns the state of the worker *
 * to what is expected by the master after a reduce is   *
 * finished.                                             *
 * ******************************************************/
   RunReduce::~RunReduce() {
      try {
         saga::url fileurl(outputPrefix_ + "/mapFile-reduced");
         saga::advert::entry adv(workerDir_.open(saga::url("./output"), saga::advert::ReadWrite | saga::advert::Create));
         adv.store_string(fileurl.get_string());
         workerDir_.set_attribute("COMMAND", "");
         workerDir_.set_attribute("STATE", WORKER_STATE_DONE_REDUCE);
      }
      catch(saga::exception const& e) {
         throw;
      }
   }

/*********************************************************
 * getLines returns a representation of each line from   *
 * the input files as a map<string, vector<string> > to  *
 * be passed to the user defined reduce function.        *
 * ******************************************************/
   void RunReduce::getLines(unorderedMap &keyValues) {
      std::vector<std::string> lines;
      std::vector<std::string>::const_iterator linesIT;
      merger<std::string>(lines, files_);

      for(linesIT = lines.begin();linesIT!=lines.end();linesIT++) {
         std::string key(getKey(*linesIT));
         std::vector<std::string> values;
         parseMapLine(values, *linesIT);

         if(keyValues.find(key) == keyValues.end()) {
            //Not in structure
            strVectorPtr initialValue(new std::vector<std::string>(values));
            keyValues[key] = initialValue;
         }
         else {
            //Contained in structure
            std::vector<std::string>::const_iterator valuesIT = values.begin();
            std::vector<std::string>::const_iterator end = values.end();
            while(valuesIT != end) {
               keyValues[key]->push_back(*valuesIT);
               ++valuesIT;
            }
         }
      }
  }
} // namespace MapReduce

