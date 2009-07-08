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
         std::string message("Established connection!");
         log_->write(message, LOGLEVEL_INFO);

         //Ask worker for state
         worker.write(saga::buffer(MASTER_QUESTION_STATE, 6));
         std::string read(network::read(worker));
         message.clear();
         log_->write(message, LOGLEVEL_INFO);

         if(network::test(read, WORKER_STATE_IDLE))
         {
            if(finished_.size() == assignments_.size())
            {
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
            if(std::find(assigned_.begin(), assigned_.end(), currentChunkID) == assigned_.end())
            {
               assigned_.push_back(currentChunkID);
            }

            //If from unassigned, remove it
            std::vector<int>::iterator pos = std::find(unassigned_.begin(), unassigned_.end(), currentChunkID);
            if(pos != unassigned_.end())
            {
               unassigned_.erase(pos);
            }
            assigned = true;
            return;
         }
         else if(network::test(read, WORKER_STATE_DONE))
         {
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
            if(pos != assigned_.end())
            {
               assigned_.erase(pos);
            }

            //Make sure not already inserted into finished list
            if(std::find(finished_.begin(), finished_.end(), resultInt) == finished_.end())
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
 AssignmentChunk HandleComparisons::getChunk_(const std::string &hostname) {
    //std::cout << "worker hostname: " << hostname << std::endl;
    if(unassigned_.size() > 0)
    {
       //std::cout << "unassigned..." << std::endl;
       AssignmentChunk ac;
       bool found = false;
       boost::tie(ac, found) = networkGraphCheck_(unassigned_, hostname);
       if(found == true)
       {
          //std::cout << "return from graph lowest latency unassigned: " << ac.getId() << std::endl;
          return ac;
       }
       else
       {
          boost::tie(ac, found) = AssignmentChunkCheck_(unassigned_, hostname);
          if(found == true)
          {
             //std::cout << "return from looking at locations of assignmentChunks..." << std::endl;
             return ac;
          }
          else
          {
             //Couldn't Find an exact location, just give any assignment out
             //std::cout << "returned first one" << std::endl;
             return assignments_[unassigned_[0]];
          }
       }
    }
    else if(assigned_.size() > 0)
    {
       //No more unassigned ones
       //Try to use locality information from graph
       AssignmentChunk ac;
       bool found = false;
       //std::cout << "assigned..." << std::endl;
       boost::tie(ac, found) = networkGraphCheck_(assigned_, hostname);
       if(found == true)
       {
          //std::cout << "return from graph lowest latency unassigned: " << ac.getId() << std::endl;
          return ac;
       }
       else
       {
          boost::tie(ac, found) = AssignmentChunkCheck_(assigned_, hostname);
          if(found == true)
          {
             //std::cout << "return from looking at locations of assignmentChunks..." << std::endl;
             return ac;
          }
          else
          {
             //Random so not to give out same assigned one every time
             //std::cout << "return random one" << std::endl;
             return assignments_[assigned_[rand() % assigned_.size()]];
          }
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
    std::vector<int>::const_iterator it  = set.begin();
    std::vector<int>::const_iterator end = set.end();
    while(it != end) {
       if(assignments_[*it].getLocation() == hostname) {
          //found exact match for host 
          return std::pair<AssignmentChunk, bool>(set[*it], true);
       }
       ++it;
    }
    return std::pair<AssignmentChunk, bool>(AssignmentChunk(), false);
 }


 std::pair<AssignmentChunk, bool> HandleComparisons::networkGraphCheck_(
       const std::vector<int> &set,
       const std::string &hostname)
 {
    AssignmentChunk closestAssignmentChunk;
    double distance = -1;
    boost::graph_traits<Graph>::edge_iterator ei, ebegin, end;
    boost::tie(ebegin, end) = boost::edges(networkGraph_);
    for(ei = ebegin; ei != end; ++ei)
    {
       std::string name(networkGraph_[boost::source(*ei, networkGraph_)].name);
       //Check to see if this edge's source is the hostname in question
       //std::cout << "checking to see if source of this edge(" << networkGraph_[boost::source(*ei, networkGraph_)].name << ") is equal";
       //std::cout << " to location of worker wanting work(" << hostname << ")" << std::endl;
       if(name == hostname)
       {
          double weight =  networkGraph_[*ei].weight;
          //std::cout << "It is!" << std::endl;
          //Found an edge to another host from the one in question
          if(weight < distance || distance < 0)
          {
             //Check to see if the target hostname is in the set
             //std::cout << "check to see if it is in the set" << std::endl;
             //std::cout << "target of edge: " << networkGraph_[boost::target(*ei, networkGraph_)].name << std::endl;
             std::vector<int>::const_iterator it  = set.begin();
             std::vector<int>::const_iterator end = set.end();
             while(it != end)
             {
                //std::cout << "iterator over set is: " << *it << std::endl;
                AssignmentChunk ac = assignments_[*it];
                //std::cout << "location of ac that this iterator gave us: " << ac.getLocation() << std::endl;
                if(ac.getLocation() == networkGraph_[boost::target(*ei, networkGraph_)].name)
                {
                   //We found an assignment in the set with the hostname of the target vertex
                   //meaning we know information about network traffic along this path
                   //we want the minimal assignment
                   //std::cout << "gotcha!" << std::endl;
                   if(distance == -1)
                   {
                      distance    = weight;
                      closestAssignmentChunk = ac;
                   }
                   else if(weight < distance)
                   {
                      distance    = weight;
                      closestAssignmentChunk = ac;
                   }
                   //Otherwise, this is not less than an already found weight describing the network
                }
                ++it;
             }
          }
       }
    }
    if(distance < 0)
    {
       //If a graph was not found, or the graph was not useful for this set and hostname
       return std::pair<AssignmentChunk, bool>(closestAssignmentChunk, false);
    }
    else
    {
       //Minimum assignment found
       return std::pair<AssignmentChunk, bool>(closestAssignmentChunk, true);
    }
 }
} // namespace AllPairs
