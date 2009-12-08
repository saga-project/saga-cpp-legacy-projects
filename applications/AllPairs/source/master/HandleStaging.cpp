//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "HandleStaging.hpp"

namespace AllPairs
{
 HandleStaging::HandleStaging(const saga::url &serverURL,
                              const std::vector<std::string> &hosts,
                              LogWriter *log)
    : serverURL_(serverURL), log_(log), hosts_(hosts)
 {
    for(std::vector<std::string>::iterator it = hosts_.begin(); it != hosts_.end(); ++it)
    {
       Vertex v = boost::add_vertex(networkGraph_);
       networkGraph_[v].name = *it;
    }
    if(hosts_.size() > 1)
    {
       assignStages_();
    }
 }

 Graph HandleStaging::getNetwork(void) {
    return networkGraph_;
 }

 HandleStaging::~HandleStaging()
 {
    std::cout << "before we close, what is our graph?" << std::endl;
    boost::graph_traits<Graph>::edge_iterator ei, ebegin, end;
    boost::graph_traits<Graph>::vertex_iterator vi, vbegin, vend;
    //Find vertex of currently reporting worker
    boost::tie(ebegin, end) = boost::edges(networkGraph_);
    std::cout << "edges" << std::endl;
    for(ei = ebegin; ei != end; ++ei)
    {
       std::cout << "(" << networkGraph_[boost::source(*ei, networkGraph_)].name << ", " <<
                           networkGraph_[boost::target(*ei, networkGraph_)].name << ") = " <<
                           networkGraph_[*ei].weight << std::endl;
    }
    boost::tie(vbegin, vend) = boost::vertices(networkGraph_);
    std::cout << "vertices" << std::endl;
    for(vi = vbegin; vi != vend; ++vi)
    {
       std::cout << networkGraph_[*vi].name << std::endl;
    }
 }

 void HandleStaging::assignStages_() 
 {
    std::string read;
    try {
       service_ = new saga::stream::server(serverURL_);
       while(finishedHosts_.size() != numWorkers_) {
          saga::stream::stream worker = service_->serve();
          worker.write(saga::buffer(MASTER_QUESTION_STATE, 6));
          read = network::read(worker);
          if(network::test(read, WORKER_STATE_IDLE)) {
             worker.write(saga::buffer(MASTER_QUESTION_LOCATION, 9));
             read = network::read(worker);
             if(find(finishedHosts_.begin(), finishedHosts_.end(), read) == finishedHosts_.end()) {
                std::string message("Begining staging for worker: ");
                message += read;
                log_->write(message, LOGLEVEL_INFO);
                //Not yet finished finding information about this hosts
                worker.write(saga::buffer(WORKER_COMMAND_STAGE, 5));
                network::expect(WORKER_RESPONSE_ACKNOLEDGE, network::read(worker));
                worker.write(saga::buffer(START_CHUNK, 5));
                network::expect(WORKER_RESPONSE_ACKNOLEDGE, network::read(worker));
                std::vector<std::string>::iterator it  = hosts_.begin();
                std::vector<std::string>::iterator end = hosts_.end();
                while(it != end) {
                   worker.write(saga::buffer(*it, it->size()));
                   network::expect(WORKER_RESPONSE_ACKNOLEDGE, network::read(worker));
                   ++it;
                }
                worker.write(saga::buffer(END_CHUNK, 3));
                network::expect(WORKER_RESPONSE_ACKNOLEDGE, network::read(worker));
             }
             else {
                //Got what we need, tell them to be idle
                worker.write(saga::buffer(MASTER_REQUEST_IDLE, 5));
                network::expect(WORKER_RESPONSE_ACKNOLEDGE, network::read(worker));
             }
          }
          else if(network::test(read, WORKER_STATE_DONE_STAGING)) {
             worker.write(saga::buffer(MASTER_QUESTION_LOCATION, 9));
             read = network::read(worker);
             std::string location(read);
             if(find(finishedHosts_.begin(), finishedHosts_.end(), read) == finishedHosts_.end()) {
                std::string message("Gathering results from staging with worker: ");
                message += location;
                log_->write(message, LOGLEVEL_INFO);
                Vertex dest, source;
                boost::graph_traits<Graph>::vertex_iterator vi, vbegin, vend;
                //Find vertex of currently reporting worker
                boost::tie(vbegin, vend) = boost::vertices(networkGraph_);
                for(vi = vbegin; vi != vend; ++vi)
                {
                   Vertex vertex = *vi;
                   if(networkGraph_[vertex].name == location) {
                      source = vertex;
                      break;
                   }
                }
                worker.write(saga::buffer(START_CHUNK, 5));
                network::expect(WORKER_RESPONSE_ACKNOLEDGE, network::read(worker));
                for(unsigned int i = 0; i < hosts_.size(); ++i)
                {
                   worker.write(saga::buffer(MASTER_QUESTION_RESULT, 7));
                   read = network::read(worker);
                   //Find vertex of currently in question host
                   boost::tie(vbegin, vend) = boost::vertices(networkGraph_);
                   if(hosts_[i] != location) 
                   {
                      for(boost::graph_traits<Graph>::vertex_iterator vi = vbegin; vi != vend; ++vi)
                      {
                         Vertex vertex = *vi;
                         if(networkGraph_[vertex].name == hosts_[i]) {
                            dest = vertex;
                            break;
                         }
                      }
                      Edge edge;
                      boost::tie(edge, boost::tuples::ignore) = boost::add_edge(source, dest, networkGraph_);
                      networkGraph_[edge].weight = boost::lexical_cast<double>(read);
                   }
                }
                worker.write(saga::buffer(END_CHUNK, 3));
                network::expect(WORKER_RESPONSE_ACKNOLEDGE, network::read(worker));
                //Not yet finished, add to finished list
                finishedHosts_.push_back(location);
                message = std::string("Gathered results from staging with worker: ");
                message += location;
                log_->write(message, LOGLEVEL_INFO);
                worker.write(saga::buffer(MASTER_REQUEST_IDLE, 5));
                network::expect(WORKER_RESPONSE_ACKNOLEDGE, network::read(worker));
             }
             else {
                //Got what we need already, tell them to be idle
                worker.write(saga::buffer(MASTER_REQUEST_IDLE, 5));
                network::expect(WORKER_RESPONSE_ACKNOLEDGE, network::read(worker));
             }
          }
       }
       delete service_;
    }
    catch (const network::networkException &e) {
       std::cerr << "Error: " << e.what() << std::endl;
       APPLICATION_ABORT;
    }
    catch (const saga::exception &e) {
       std::cerr << "Error: " << e.what() << std::endl;
       APPLICATION_ABORT;
    }
 }
} // namespace AllPairs
