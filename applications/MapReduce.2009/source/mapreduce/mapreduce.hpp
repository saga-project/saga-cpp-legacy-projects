//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_MAPREDUCE_HPP_
#define MAPREDUCE_MAPREDUCE_HPP_

#include <saga/saga.hpp>

#include "version.hpp"
#include "protocol.hpp"

#include "Mapper.hpp"
#include "Partitioner.hpp"
#include "Reducer.hpp"

#include <boost/program_options.hpp>

#include "../utils/logging.hpp"

namespace mapreduce {

class MapReduceResult {
 public:
  bool finished;
};

// Static variable holding the parsed command line parameters.
extern boost::program_options::variables_map g_command_line_parameters;

// Initialize the framework. Must be called after processing command-line
// arguments.
void InitFramework(int argc, char** argv);

}

#endif  // MAPREDUCE_MAPREDUCE_HPP_
