//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "HandleComparisons.hpp"

/*********************************************************
 * HandleComparisons tries to group together the proper files*
 * and assign them to a worker to reduce                 *
 * ******************************************************/
namespace AllPairs
{
 // fileCount is the total number of files possibly outputted by
 // the map function (NUM_MAPS)
 HandleComparisons::HandleComparisons(Graph &networkGraph, assignmentChunksVector &assignments, saga::url serverURL, LogWriter *log)
    :  networkGraph_(networkGraph), assignments_(assignments), serverURL_(serverURL), log_(log)
 {
    assignmentChunksVector::iterator end = assignments.end();
    for(assignmentChunksVector::iterator it = assignments.begin();
        it != end; ++it)
    {
       unassigned_.push_back(it->getId());
    }
    try
    {
       service_ = new saga::stream::server(serverURL_);
    }
    catch(saga::exception const& e) {
       std::cerr << "saga::exception caught: " << e.what() << std::endl;
    }
 }

 HandleComparisons::~HandleComparisons()
 {
    service_->close();
    delete service_;
 }
/*********************************************************
 * assignReduces is the only public function that tries  *
 * to assign reduce files to idle workers                *
 * ******************************************************/
 void HandleComparisons::assignWork()
 {
    while(finished_.size() < assignments_.size()) {
       issueCommand_();  // Try to assign the chunk to someone
    }
 }
/*********************************************************
 * issue_command takes the grouped files and tries to    *
 * assign them to an idel worker.  If a worker is done,  *
 * the results are recorded                              *
 * ******************************************************/
 void HandleComparisons::issueCommand_() {
   bool assigned = false; //Describes status of current file
   while(assigned == false) {
      try {
         saga::stream::stream worker = service_->serve();
         std::string message("Established connection to ");
         message += worker.get_url().get_string();
         log_->write(message, LOGLEVEL_INFO);

         //Ask worker for state
         worker.write(saga::buffer(MASTER_QUESTION_STATE, 6));
         std::string read(network::read(worker));
         message.clear();
         message = "Worker: " + worker.get_url().get_string() + " has state " + read;
         log_->write(message, LOGLEVEL_INFO);

         if(network::test(read, WORKER_STATE_IDLE))
         {
            if(finished_.size() == assignments_.size())
            {
               //Prevent unneccessary work assignments
               worker.write(saga::buffer(MASTER_REQUEST_IDLE, 5));
               network::expect(WORKER_RESPONSE_ACKNOLEDGE, network::read(worker));
            }
            //Ask worker for location, then try to assign closest available assignment chunk
            worker.write(saga::buffer(MASTER_QUESTION_LOCATION, 9));
            read = network::read(worker);

            AssignmentChunk chunk(getChunk_(saga::url(read)));
            int currentChunkID = chunk.getId();
            //Worker is idle
            message.clear();
            message = "Attempting to issue worker ";
            message += worker.get_url().get_string();
            message += " to handle Assignment Chunk ";
            message += boost::lexical_cast<std::string>(currentChunkID);
            message += " ...";
            log_->write(message, LOGLEVEL_INFO); 

            //Ask where their advert is
            worker.write(saga::buffer(MASTER_QUESTION_ADVERT, 7));
            saga::url advert(network::read(worker));

            message.clear();
            message += worker.get_url().get_string();
            message += " <==> " + advert.get_string();
            message += " ... ";
            log_->write(message, LOGLEVEL_INFO); 

            //Tell worker about data
            worker.write(saga::buffer(WORKER_COMMAND_COMPARE, 7));
            network::expect(network::read(worker), WORKER_RESPONSE_ACKNOLEDGE);
            //Write chunk to worker
            std::vector<Assignment>::iterator it  = chunk.getBegin();
            std::vector<Assignment>::iterator end = chunk.getEnd();
            worker.write(saga::buffer(START_CHUNK, 5));

            network::expect(network::read(worker), WORKER_RESPONSE_ACKNOLEDGE);

            //Give the id first
            std::string stringID = boost::lexical_cast<std::string>(currentChunkID);
            worker.write(saga::buffer(stringID, stringID.size()));
            network::expect(network::read(worker), WORKER_RESPONSE_ACKNOLEDGE);

            while(it != end)
            {
               std::string to   = it->getTo();
               std::string from = it->getFrom();
               worker.write(saga::buffer(to, to.size()));
               network::expect(network::read(worker), WORKER_RESPONSE_ACKNOLEDGE);
               worker.write(saga::buffer(from, from.size()));
               network::expect(network::read(worker), WORKER_RESPONSE_ACKNOLEDGE);
               ++it;
            }
            worker.write(saga::buffer(END_CHUNK, 3));
            network::expect(network::read(worker), WORKER_RESPONSE_ACKNOLEDGE);

            std::string message("Success: ");
            message += advert.get_string() + " is comparing chunk ";
            message += boost::lexical_cast<std::string>(currentChunkID);
            log_->write(message, LOGLEVEL_INFO);
            //If not in assigned, add it
            std::vector<int>::iterator aend = assigned_.end();
            std::vector<int>::iterator assigned_IT = assigned_.begin();
            bool found = false;
            while(assigned_IT != aend)
            {
               if(currentChunkID == (*assigned_IT))
               {
                  found = true;
                  break;
               }
                ++assigned_IT;
            }
            if(found == false)
            {
               assigned_.push_back(currentChunkID);
            }

            //If from unassigned, remove it
            aend = unassigned_.end();
            std::vector<int>::iterator unassigned_IT = unassigned_.begin();
            while(unassigned_IT != aend)
            {
               if(currentChunkID == (*unassigned_IT))
               {
                  unassigned_.erase(unassigned_IT);
                  break;
               }
                ++unassigned_IT;
            }
            assigned = true;
            return;
         }
         else if(network::test(read, WORKER_STATE_DONE))
         {
            worker.write(saga::buffer(MASTER_QUESTION_RESULT, 7));
            std::string result(network::read(worker));
            worker.write(saga::buffer(MASTER_REQUEST_IDLE, 5));

            std::string message("Worker ");
            message += worker.get_url().get_string() + " finished chunk ";
            message += result;
            log_->write(message, LOGLEVEL_INFO);

            //If in assigned, remove it
            std::vector<int>::iterator end = assigned_.end();
            int resultInt = boost::lexical_cast<int>(result);
            for(std::vector<int>::iterator assigned_IT = assigned_.begin();
                assigned_IT != end;
                ++assigned_IT)
            {
               if(resultInt == *assigned_IT)
               {
                  assigned_.erase(assigned_IT);
                  break;
               }
            }

            //Make sure not already inserted into finished list
            end = finished_.end();
            std::vector<int>::iterator finished_fileIT = finished_.end();
            bool found = false;
            for(std::vector<int>::iterator finished_IT = finished_.begin();
                finished_IT != end;
                ++finished_IT)
            {
               if(resultInt == *finished_IT)
               {
                  found = true;
                  break;
               }
            }
            if(found == false)
            {
               finished_.push_back(resultInt);
            }
         }
       }
       catch(saga::exception const & e) {
          std::string message(e.what());
          log_->write(message, LOGLEVEL_ERROR);
       }
    }
 }
 AssignmentChunk HandleComparisons::getChunk_(const saga::url location) {
    std::string hostname = location.get_host();
    if(unassigned_.size() > 0)
    {
       //Return element on list whose should correspond to a close data set
       std::vector<int>::iterator it  = unassigned_.begin();
       std::vector<int>::iterator end = unassigned_.end();
       while(it != end) {
          if(assignments_[unassigned_[*it]].getLocation() == location) {
             std::cerr << "FOUND EXACT MATCH FOR LOCATION: " << location;
             std::cerr << " to assignmentchunk with location: " << assignments_[unassigned_[*it]].getLocation() << std::endl;
             return assignments_[unassigned_[*it]];
          }
          ++it;
       }
       //Couldn't Find an exact location, just give one out.
       return assignments_[unassigned_[0]];
    }
    else if(assigned_.size() > 0)
    {
       //No more unassigned ones
       //These will be redundant, but possibly improve runtime.
       std::vector<int>::iterator it  = assigned_.begin();
       std::vector<int>::iterator end = assigned_.end();
       while(it != end) {
          if(assignments_[assigned_[*it]].getLocation() == location) {
             return assignments_[assigned_[*it]];
          }
          ++it;
       }
       //Couldn't find exact location, just give one out
       //Random so not to give out same assigned one every time
       return assignments_[assigned_[rand() % assigned_.size()]];
    }
    else if(finished_.size() > 0)
    {
       //No more assigned ones
       //No one should be asking!
       std::vector<int>::iterator it  = finished_.begin();
       std::vector<int>::iterator end = finished_.end();
       while(it != end) {
          if(assignments_[finished_[*it]].getLocation() == location) {
             return assignments_[finished_[*it]];
          }
          ++it;
       }
       return assignments_[finished_[rand() % finished_.size()]];
    }
    else
    {
       std::cerr << "Stop asking for chunks!" << std::endl;
       APPLICATION_ABORT;
    }
 }
} // namespace AllPairs
