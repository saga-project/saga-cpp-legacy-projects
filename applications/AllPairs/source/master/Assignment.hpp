//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AP_ASSIGNMENT_HPP
#define AP_ASSIGNMENT_HPP

namespace AllPairs {
  class Assignment {
   public:
     Assignment(std::string from, std::string to) : from_(from), to_(to) { 
        //Just make location one of the hostnames of the two files
        location_ = saga::url(from_).get_host();
     }
     Assignment(std::string from, std::string to, std::string location) :
        from_(from), to_(to), location_(location) { }
     std::string getFrom(void) {
        return from_;
     }
     std::string getTo(void) {
        return to_;
     }
     std::string getLocation(void) {
        return location_;
     }
     void setLocation(std::string location) {
        location_ = location;
     }
   private:
     std::string from_;
     std::string to_;
     std::string location_;
  }; //Class Assignment

  class AssignmentChunk {
   public:
     AssignmentChunk(std::vector<Assignment> &assignments, int id) : 
       assignments_(assignments) , id_(id) {
        location_ = std::string("");
     }
     AssignmentChunk(int id) : id_(id) {
        location_ = std::string("");
     }
     int getId(void) {
        return id_;
     }
     std::string getLocation(void) {
        return location_;
     }
     void push_back(Assignment assignment) {
        assignments_.push_back(assignment);
     }
     std::vector<Assignment>::iterator getBegin(void) {
        return assignments_.begin();
     }
     std::vector<Assignment>::iterator getEnd(void) {
        return assignments_.end();
     }
     Assignment get(int index) {
        return assignments_[index];
     }
     unsigned int size(void) {
        return assignments_.size();
     }
     void guessLocation(void) {
        //Guess best location to describe this assignment chunk
        std::map<std::string, int> counter;
        std::map<std::string, int>::iterator counterEnd = counter.end();
        std::vector<Assignment>::iterator assignmentsIt = assignments_.begin();
        std::vector<Assignment>::iterator assignmentsEnd = assignments_.end();
        while(assignmentsIt != assignmentsEnd) {
           if(counter.find(assignmentsIt->getLocation()) != counterEnd) {
              counter[assignmentsIt->getLocation()]++;
           }
           else {
              counter.insert(std::pair<std::string, int>(assignmentsIt->getLocation(), 1));
           }
           ++assignmentsIt;
        }
        //Now find which location occurs the most
        std::map<std::string, int>::iterator counterIt = counter.begin();
        int maximumVal = 0;
        std::string maximumLoc = std::string("");
        while(counterIt != counterEnd) {
           if(counterIt->second > maximumVal) {
              maximumVal = counterIt->second;
              maximumLoc = counterIt->first;
           }
           counterIt++;
        }
        location_ = maximumLoc;
     }
   private:
     std::vector<Assignment> assignments_;
     int id_;
     std::string location_;
  }; //Class AssignmentChunk

  typedef std::vector<AssignmentChunk> assignmentChunksVector;

} //Namespace AllPairs
#endif // AP_ASSIGNMENT_HPP
