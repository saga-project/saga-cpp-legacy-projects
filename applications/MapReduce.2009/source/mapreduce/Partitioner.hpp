//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_PARTITIONER_HPP_
#define MAPREDUCE_PARTITIONER_HPP_

#include <string>
#include <boost/functional/hash.hpp>

#include "factory_impl.hpp"

namespace mapreduce {

// Interface for partitioning intermediate keys.
class Partitioner {
 public:
  virtual ~Partitioner() {}
  virtual int GetPartition(const std::string& key, int num_partitions) = 0;
};

// Factory for partitioners.
typedef factory< ::mapreduce::Partitioner, std::string > PartitionerFactory;
typedef signatures<Partitioner()> partitioner_signatures;

// Macro for registering a partitioner in the framework.
#define REGISTER_PARTITIONER(name, id)   \
  REGISTER_CLASS( name, ::mapreduce::PartitionerFactory,  \
    #name, ::mapreduce::partitioner_signatures, id);


}   // namespace mapreduce

// Default partitioner implementation. Uses a simple hash function to do the
// partitioning.
class DefaultPartitioner : public mapreduce::Partitioner {
 public:
  int GetPartition(const std::string& key, int num_partitions) {
    return boost::hash_value(key) % num_partitions;
  }
};
REGISTER_PARTITIONER(DefaultPartitioner, 0);

#endif  // MAPREDUCE_PARTITIONER_HPP_
