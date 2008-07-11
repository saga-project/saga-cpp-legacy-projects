
#ifndef WORKLOAD_HPP
#define WORKLOAD_HPP

#include "logger.hpp"

#include <string>

#include <stdlib.h>

#include <iostream>
#include <fstream>


// this class represents the work to be performed.  work() has to be called
// until test() returns true, indicating that all work is done.
//
// Our specific instance just increases the counter until it reaches the value
// of the first command line argument.
//
// the c'tor args assume two entries
//   arg 1: string name - uniquely identifies this workload instance
//   arg 2: int    max  - maximum number of iterations to perform

class workload 
{
  private:
    logger & log_;

    int cnt_;            // data to work on
    int max_;            // break condition

    std::fstream f_in_;  // input  file
    std::fstream f_out_; // output file

    std::fstream f_log_;   // log file

  public:
    workload (logger & log);
   ~workload (void);

    void set_input  (std::string in);   // register input  data file with worker
    void set_output (std::string out);  // register output data file with worker

    void work (void);  // perform a piece of work 
    bool test (void);  // check if there is more to be done
};

#endif // WORKLOAD_HPP

