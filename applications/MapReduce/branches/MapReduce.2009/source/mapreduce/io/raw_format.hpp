//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_IO_RAW_FORMAT_HPP_
#define MAPREDUCE_IO_RAW_FORMAT_HPP_

#include "../job.hpp"
#include "../serialization.hpp"

namespace mapreduce {

// Forward declarations.
class InputChunk;
class RawRecordReader;
class RawRecordWriter;

// Interface for handling custom input formats dealing with blocks of bytes
// representing serialized objects. 
class RawInputFormat {
 public:
  virtual ~RawInputFormat() {}
  // Serialize the InputChunk.
  virtual void SerializeInputChunk(InputChunk* chunk, ZeroCopyOutputStream* buffer) = 0;
  // Deserialize the InputChunk needed for this InputFormat from the message.
  virtual InputChunk* CreateInputChunk(ZeroCopyInputStream* buffer) = 0;
  // Chunk the input data specified in the job's descriptor.
  virtual std::vector<InputChunk*> GetChunks(const JobDescription& job) = 0;
  // Create a record reader to be used by the Mapper/Combiner/Reducer function.
  virtual RawRecordReader* GetRecordReader(InputChunk* chunk, JobDescription* job) = 0;
};

// Interface for handling custom output formats dealing with blocks of bytes
// possibly representing serialized objects.
class RawOutputFormat {
 public:
  virtual ~RawOutputFormat() {}
  virtual RawRecordWriter* GetRecordWriter(TaskDescription* job) = 0;
};

}   // namespace mapreduce

#endif  // MAPREDUCE_IO_RAW_FORMAT_HPP_
