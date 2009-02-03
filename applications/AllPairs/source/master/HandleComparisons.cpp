#include "HandleComparisons.hpp"
#include <boost/lexical_cast.hpp>
#include "../utils/defines.hpp"

/*********************************************************
 * HandleComparisons tries to group together the proper files*
 * and assign them to a worker to reduce                 *
 * ******************************************************/
namespace AllPairs
{
 // fileCount is the total number of files possibly outputted by
 // the map function (NUM_MAPS)
 HandleComparisons::HandleComparisons(std::vector<saga::url> &fragmentFiles, LogWriter *log)
    :  fragmentFiles_(fragmentFiles), log_(log)
 {
    unassigned_ = std::vector<saga::url>(fragmentFiles);
    saga::url url("tcp://localhost:8000");
    try
    {
       service_ = saga::stream::server(url);
    }
    catch(saga::exception const& e) {
       std::cerr << "saga::exception caught: " << e.what() << std::endl;
    }
 }
/*********************************************************
 * assignReduces is the only public function that tries  *
 * to assign reduce files to idle workers                *
 * ******************************************************/
 void HandleComparisons::assignWork()
 {
    while(finished_.size() < fragmentFiles_.size()) {
       saga::url fragmentFile(get_file_());
       issue_command_(fragmentFile);  // Try to assign the chunk to someone
    }
 }
/*********************************************************
 * issue_command takes the grouped files and tries to    *
 * assign them to an idel worker.  If a worker is done,  *
 * the results are recorded                              *
 * ******************************************************/
 void HandleComparisons::issue_command_(saga::url fragmentFile) {
   int mode = saga::advert::ReadWrite;
   bool assigned = false; //Describes status of current file
   while(assigned == false) {
      try {
         saga::stream::stream worker = service_.serve();
         std::string message("Established connection to ");
         message += worker.get_url().get_string();
         log_->write(message, LOGLEVEL_INFO);

         //Ask worker for state
         worker.write(saga::buffer(MASTER_QUESTION_STATE));
         char buff[255];
         saga::ssize_t read_bytes = worker.read(saga::buffer(buff));
         std::string state(buff);
         if(state == WORKER_STATE_IDLE)
         {
            //Worker is idle
            message.clear();
            message = "Issuing worker ";
            message += worker.get_url().get_string();
            message += " to map " + fragmentFile.get_string();
            log_->write(message, LOGLEVEL_INFO); 

            //Ask where their advert is
            worker.write(saga::buffer(MASTER_QUESTION_ADVERT));
            char buff[255];
            saga::ssize_t read_bytes = worker.read(saga::buffer(buff));
            saga::url advert = saga::url(std::string(buff));

            //Write chunk to worker
            saga::advert::directory possibleWorker(advert, mode);
            saga::advert::entry adv(possibleWorker.open(saga::url("./fragmentFile"), mode | saga::advert::Create));
            adv.store_string<saga::task_base::Sync>(fragmentFile.get_string());

            //Tell worker about data
            worker.write(saga::buffer(WORKER_COMMAND_COMPARE));
            read_bytes = worker.read(saga::buffer(buff));
            if(std::string(buff) != WORKER_RESPONSE_ACKNOLEDGE)
            {
               std::cerr << "Worker did not accept chunk!" << std::endl;
               break;
            }

            std::string message("Assigned worker");
            message += possibleWorker.get_url().get_path() + " to compare fragment: " + fragmentFile.get_string();
            log_->write(message, LOGLEVEL_INFO);
            assigned_.push_back(fragmentFile);

            //If from unassigned, remove it
            std::vector<saga::url>::iterator end = unassigned_.end();
            std::vector<saga::url>::iterator unassigned_fileIT = unassigned_.end();
            bool found = false;
            for(std::vector<saga::url>::iterator unassigned_IT = unassigned_.begin();
                unassigned_IT != end;
                ++unassigned_IT)
            {
               if(fragmentFile == *unassigned_IT)
               {
                  found = true;
                  break;
               }
            }
            if(found == true)
            {
               unassigned_.erase(unassigned_fileIT);
            }
            assigned = true;
         }
         else if(state == WORKER_STATE_DONE)
         {
            worker.write(saga::buffer(MASTER_QUESTION_RESULT));
            char buff[255];
            saga::ssize_t read_bytes = worker.read(saga::buffer(buff));
            saga::url result = saga::url(std::string(buff));

            std::string message("Worker ");
            message += worker.get_url().get_string() + " finished fragment " + result.get_string();
            log_->write(message, LOGLEVEL_INFO);

            //If in assigned, remove it
            std::vector<saga::url>::iterator end = assigned_.end();
            std::vector<saga::url>::iterator assigned_fileIT = assigned_.end();
            bool found = false;
            for(std::vector<saga::url>::iterator assigned_IT = assigned_.begin();
                assigned_IT != end;
                ++assigned_IT)
            {
               if(result == *assigned_IT)
               {
                  found = true;
                  break;
               }
            }
            if(found == true)
            {
               assigned_.erase(assigned_fileIT);
            }

            //Make sure not already inserted into finished list
            end = finished_.end();
            std::vector<saga::url>::iterator finished_fileIT = finished_.end();
            found = false;
            for(std::vector<saga::url>::iterator finished_IT = finished_.begin();
                finished_IT != end;
                ++finished_IT)
            {
               if(result == *finished_IT)
               {
                  found = true;
                  break;
               }
            }
            if(found == false)
            {
               finished_.push_back(result);
            }
         }
       }
       catch(saga::exception const & e) {
          std::string message(e.what());
          //log->write(message, LOGLEVEL_ERROR);
       }
    }
 }
 saga::url HandleComparisons::get_file_() {
    if(unassigned_.size() > 0)
    {
       //Return anything on this list
       return unassigned_[0];
    }
    else if(assigned_.size() > 0)
    {
       //No more unassigned ones
       //Return anything on this list
       return assigned_[rand() % assigned_.size()];
    }
    else
    {
       //No more assigned ones
       //Just give a finished one from finished
       return finished_[rand() % finished_.size()];
    }
 }
} // namespace AllPairs
