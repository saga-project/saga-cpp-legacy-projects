//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AP_ASSIGNMENT_HPP
#define AP_ASSIGNMENT_HPP

namespace AllPairs {
  class Assignment {
   public:
     Assignment() {}
     Assignment(int from, int to) : 
        from_(from), to_(to), stringAvailable_(false) { }
     Assignment(std::string stringFrom, std::string stringTo) : 
        stringFrom_(stringFrom), stringTo_(stringTo), stringAvailable_(true) { }
     int getFrom(void) {
        return from_;
     }
     int getTo(void) {
        return to_;
     }
     std::string getStringFrom(void) {
        return stringFrom_;
     }
     std::string getStringTo(void) {
        return stringTo_;
     }
     bool stringAvailable(void) {
        return stringAvailable_;
     }
   private:
     int from_;
     int to_;
     std::string stringFrom_;
     std::string stringTo_;
     bool stringAvailable_;
  }; //Class Assignment

  class AssignmentChunk {
   public:
     AssignmentChunk() {}
     AssignmentChunk(std::vector<Assignment> &assignments, int id) : 
       assignments_(assignments) , id_(id) {
     }
     AssignmentChunk(int id) : id_(id) {
     }
     int getId(void) {
        return id_;
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
     std::vector<int> getFiles(void) {
        //Guess best location to describe this assignment chunk
        std::vector<int> files;
        std::vector<Assignment>::iterator assignmentsIt = assignments_.begin();
        std::vector<Assignment>::iterator assignmentsEnd = assignments_.end();
        while(assignmentsIt != assignmentsEnd) {
           int from = assignmentsIt->getFrom();
           int to   = assignmentsIt->getTo();
           if(std::find(files.begin(), files.end(), from) == files.end())
           {
              files.push_back(from);
           }
           if(std::find(files.begin(), files.end(), to) == files.end())
           {
              files.push_back(to);
           }
           ++assignmentsIt;
        }
        return files;
     }
   private:
     std::vector<Assignment> assignments_;
     int id_;
  }; //Class AssignmentChunk

  typedef std::vector<AssignmentChunk> assignmentChunksVector;

} //Namespace AllPairs
#endif // AP_ASSIGNMENT_HPP
