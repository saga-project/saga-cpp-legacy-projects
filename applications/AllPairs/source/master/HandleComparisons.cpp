//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "HandleComparisons.hpp"

/*********************************************************
 * HandleComparisons tries to group together the proper files*
 * and assign them to a worker to reduce                 *
 * ******************************************************/
namespace AllPairs {
   // fileCount is the total number of files possibly outputted by
   // the map function (NUM_MAPS)
   HandleComparisons::HandleComparisons(Graph &networkGraph,
                                      assignmentChunksVector &assignments,
                                      std::map<int, std::vector<saga::url> > &files,
                                      saga::url serverURL, LogWriter *log)
    :  networkGraph_(networkGraph),
       assignments_(assignments),
       files_(files),
       serverURL_(serverURL), log_(log) {
      assignmentChunksVector::iterator end = assignments.end();
      for(assignmentChunksVector::iterator it = assignments.begin(); it != end; ++it) {
         unassigned_.push_back(it->getId());
      }
      try {
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
            std::string message("Established connection!");
            log_->write(message, LOGLEVEL_INFO);

            //Ask worker for state
            worker.write(saga::buffer(MASTER_QUESTION_STATE, 6));
            std::string read(network::read(worker));
            message.clear();
            log_->write(message, LOGLEVEL_INFO);

            if(network::test(read, WORKER_STATE_IDLE)) {
               if(finished_.size() == assignments_.size()) {
                  //Prevent unneccessary work assignments
                  //std::cout << "all assigned already, don't dish out, we only want done ones" << std::endl;
                  worker.write(saga::buffer(MASTER_REQUEST_IDLE, 5));
                  network::expect(WORKER_RESPONSE_ACKNOLEDGE, network::read(worker));
                  return;
               }
               //Ask worker for location, then try to assign closest available assignment chunk
               worker.write(saga::buffer(MASTER_QUESTION_LOCATION, 9));
               read = network::read(worker);
               
               message.clear();
               message = "Worker: " + read + " has state " + WORKER_STATE_IDLE;
               log_->write(message, LOGLEVEL_INFO);
               
               //std::cout << "Just got location from worker as :" << read << std::endl;
               
               AssignmentChunk chunk(getChunk_(read));
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
               std::cerr << "about to start!" << std::endl;
               while(it != end) {
                  std::string to, from;
                  if(it->stringAvailable() == true)
                  {
                      to   = it->getStringTo();
                      from = it->getStringFrom();
                  }
                  else
                  {
                      to   = files_[it->getTo()][0].get_string();
                      from = files_[it->getFrom()][0].get_string();
                  }
                  worker.write(saga::buffer(to, to.size()));
                  network::expect(network::read(worker), WORKER_RESPONSE_ACKNOLEDGE);
                  worker.write(saga::buffer(from, from.size()));
                  network::expect(network::read(worker), WORKER_RESPONSE_ACKNOLEDGE);
                  ++it;
                  std::cerr << "just gave: (" << to << ", " << from << ")" << std::endl;
               }
               std::cerr << "done" << std::endl;
               worker.write(saga::buffer(END_CHUNK, 3));
               network::expect(network::read(worker), WORKER_RESPONSE_ACKNOLEDGE);
               
               std::string message("Success: ");
               message += advert.get_string() + " is comparing chunk ";
               message += boost::lexical_cast<std::string>(currentChunkID);
               log_->write(message, LOGLEVEL_INFO);
               //If not in assigned, add it
               if(std::find(assigned_.begin(), assigned_.end(), currentChunkID) == assigned_.end()) {
                  assigned_.push_back(currentChunkID);
               }
               
               //If from unassigned, remove it
               std::vector<int>::iterator pos = std::find(unassigned_.begin(), unassigned_.end(), currentChunkID);
               if(pos != unassigned_.end()) {
                  unassigned_.erase(pos);
               }
               assigned = true;
               return;
            }
            else if(network::test(read, WORKER_STATE_DONE)) {
               message.clear();
               message = "Worker has state " + std::string(WORKER_STATE_DONE);
               log_->write(message, LOGLEVEL_INFO);
               
               worker.write(saga::buffer(MASTER_QUESTION_RESULT, 7));
               std::string result(network::read(worker));
               worker.write(saga::buffer(MASTER_REQUEST_IDLE, 5));
               
               std::string message("Worker finished chunk ");
               message += result;
               log_->write(message, LOGLEVEL_INFO);
               int resultInt = boost::lexical_cast<int>(result);
               
               //If in assigned, remove it
               std::vector<int>::iterator pos = std::find(assigned_.begin(), assigned_.end(), resultInt);
               if(pos != assigned_.end()) {
                  assigned_.erase(pos);
               }
               
               //Make sure not already inserted into finished list
               if(std::find(finished_.begin(), finished_.end(), resultInt) == finished_.end()) {
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
   AssignmentChunk HandleComparisons::getChunk_(const std::string &hostname) {
      //std::cout << "worker hostname: " << hostname << std::endl;
      if(unassigned_.size() > 0) {
         std::cout << "unassigned..." << std::endl;
         AssignmentChunk ac;
         bool found = false;
         boost::tie(ac, found) = AssignmentChunkCheck_(unassigned_, hostname);
         if(found == true) {
            std::cerr << "return from looking at locations of assignmentChunks..." << std::endl;
            return ac;
         }
         else {
            std::cerr << "Couldn't Find an exact location, just give any assignment out" << std::endl;
            std::cerr << "returned first one" << std::endl;
            return assignments_[unassigned_[0]];
         }
      }
      else if(assigned_.size() > 0) {
       //No more unassigned ones
       //Try to use locality information from graph
       AssignmentChunk ac;
       bool found = false;
       boost::tie(ac, found) = AssignmentChunkCheck_(assigned_, hostname);
       if(found == true)
       {
          return ac;
       }
       else
       {
          //Random so not to give out same assigned one every time
          return assignments_[assigned_[rand() % assigned_.size()]];
       }
    }
else if(finished_.size() > 0)
    {
       //No more assigned or unassigned assignments
       //No one should be asking!
       std::cerr << "Asking for assignments when only finished ones exists" << std::endl;
       //Give random
       return assignments_[finished_[rand() % finished_.size()]];
    }
    else
    {
       std::cerr << "Stop asking for chunks!" << std::endl;
       APPLICATION_ABORT;
    }
 }

 std::pair<AssignmentChunk, bool> HandleComparisons::AssignmentChunkCheck_(
       const std::vector<int> &set,
       const std::string &hostname)
 {
    double minimumDependency = -1;
    AssignmentChunk result;

    //Iterator over set, to determine available AssignmentChunks's dependency
    std::vector<int>::const_iterator end = set.end();
    for(std::vector<int>::const_iterator it  = set.begin(); it != end; ++it)
    {
       double tempMin = 0;
       AssignmentChunk tempResult;
       std::vector<Assignment>::iterator AEnd = assignments_[*it].getEnd();
       for(std::vector<Assignment>::iterator AIt  = assignments_[*it].getBegin();
           AIt != AEnd; ++AIt)
       {
          //Look through all known replicas, and find minimum dependency in graph
          double replicaMin = -1;
          std::string minFrom;
          std::string minTo;
          std::vector<saga::url>::iterator ReplicaIt  = files_[AIt->getFrom()].begin();
          std::vector<saga::url>::iterator ReplicaEnd = files_[AIt->getFrom()].end();
          while(ReplicaIt != ReplicaEnd)
          {
             //Lookup cost of each replica
             boost::graph_traits<Graph>::edge_iterator EIt, EBegin, Eend;
             boost::tie(EBegin, Eend) = boost::edges(networkGraph_);
             for(EIt = EBegin; EIt != Eend; ++EIt)
             {
                std::string source(networkGraph_[boost::source(*EIt, networkGraph_)].name);
                std::string target(networkGraph_[boost::target(*EIt, networkGraph_)].name);
                if(source == ReplicaIt->get_host() && target == hostname)
                {
                   double weight = networkGraph_[*EIt].weight;
                   if(weight < replicaMin || replicaMin == -1)
                   {
                      replicaMin = weight;
                      minFrom = ReplicaIt->get_string();
                   }
                }
             }
          }
          tempMin += replicaMin;
          replicaMin = -1;
          ReplicaIt  = files_[AIt->getTo()].begin();
          ReplicaEnd = files_[AIt->getTo()].end();
          while(ReplicaIt != ReplicaEnd)
          {
             //Lookup cost of each replica
             boost::graph_traits<Graph>::edge_iterator EIt, EBegin, Eend;
             boost::tie(EBegin, Eend) = boost::edges(networkGraph_);
             for(EIt = EBegin; EIt != Eend; ++EIt)
             {
                std::string source(networkGraph_[boost::source(*EIt, networkGraph_)].name);
                std::string target(networkGraph_[boost::target(*EIt, networkGraph_)].name);
                if(source == ReplicaIt->get_host() && target == hostname)
                {
                   double weight = networkGraph_[*EIt].weight;
                   if(weight < replicaMin || replicaMin == -1)
                   {
                      replicaMin = weight;
                      minTo = ReplicaIt->get_string();
                   }
                }
             }
             tempResult.push_back(Assignment(minFrom, minTo));
             tempMin += replicaMin;
          }
       }
       if(tempMin < minimumDependency || minimumDependency == -1)
       {
          result = tempResult;
          minimumDependency = tempMin;
       }
    }
    if(minimumDependency  == -1)
       return std::pair<AssignmentChunk, bool>(AssignmentChunk(), false);
    else
       return std::pair<AssignmentChunk, bool>(result, true);
 }

} // namespace AllPairs
