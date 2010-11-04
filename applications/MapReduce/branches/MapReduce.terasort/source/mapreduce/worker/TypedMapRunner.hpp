//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_WORKER_TYPEDMAPRUNNER_HPP_
#define MAPREDUCE_WORKER_TYPEDMAPRUNNER_HPP_

#include <map>
#include <deque>
#include "job.hpp"
#include "serialization.hpp"
#include "Partitioner.hpp"
#include "utils/logging.hpp"

using mapreduce::Partitioner;

namespace mapreduce { namespace worker {

// Class handling the actual execution of a map task.
template <typename MapperT>
class TypedMapRunner {
 public:
  void RunTask(JobDescription* job, ZeroCopyInputStream* input);
};

// Class for outputting partitioned intermediate data. Buffers the output
// key/value pairs in a std::map using operator< as the comparator.
// Note: as such, an operator< needs to be specified for custom key types,
// otherwise compilation of MapPartitionedOutput will fail.
template <typename KeyT, typename ValueT>
class MapPartitionedOutput : public RecordWriter<KeyT, ValueT> {
 public:
  MapPartitionedOutput(JobDescription* job) : job_(job) {
    Initialize();
}

  // RecordWriter implementation.
  void Write(const KeyT& key, const ValueT& value) {
    key_buffer_.clear();
    // Serialize the key/value pair.
    StringOutputStream key_out(&key_buffer_);
    SerializationHandler<KeyT>::Serialize(&key, &key_out);
    // Get partition for this key.
    int partition = partitioner_->GetPartition(key_buffer_, num_partitions_);
     // Sanity check.
    if (partition < 0 || partition >= num_partitions_) {
      throw saga::bad_parameter("Wrong partition returned by partitioner");
    }
    // Choose intermediate buffer and save value.
    IntermediateBufferT* buffer = buffers_[partition];
    (*buffer)[key].push_back(value);
  }
  void Close() {
    // Flush buffers and close all writers.
    for (int i = 0; i < num_partitions_; ++i) {
      FlushIntermediateBuffer(buffers_[i], writers_[i]);
      writers_[i]->Close();
      delete writers_[i];
      delete tasks_[i];
      delete buffers_[i];
    }
  }

  typedef std::map< KeyT, std::deque<ValueT> > IntermediateBufferT;

 private:
  // Get partitioner class from configuration and instantiate it.
  void SetupPartitioner() {
    num_partitions_ = job_->get_num_reduce_tasks();
    const std::string& partitioner_class = job_->get_partitioner_class();
    if (partitioner_class.empty()) {
      // Use default partitioner.
      partitioner_.reset(new DefaultPartitioner());
    } else {
      Partitioner* use_partitioner = PartitionerFactory::get_by_key(
        partitioner_class);
      if (use_partitioner != NULL) {
        partitioner_.reset(use_partitioner);
        partitioner_->InitPart(); 
    } else {
        throw saga::bad_parameter("Unknown partitioner class specified");
      }
    }
  }

  // Get partitioner, and create RecordWriters for each of the partitions. 
  void Initialize() {
    SetupPartitioner();
    // Create intermediate output directory.
    // ...
    // Create tasks and writers.
    writers_.reset(new RawRecordWriter *[num_partitions_]);
    buffers_.reset(new IntermediateBufferT *[num_partitions_]);
    tasks_.reset(new TaskDescription *[num_partitions_]);
    for (int partition = 0; partition < num_partitions_; ++partition) {
      buffers_[partition] = new IntermediateBufferT();
      TaskDescription* task = new TaskDescription(job_);
      task->set_id(partition);
      tasks_[partition] = task;
      //output_path = FileOutputFormat::GetUniqueWorkFile(task);
      // Later this will be done by a Committer which will atomically rename the final output
      // to the filename below (possibly after mergesorting spills of intermediate data).
      std::string output_path(FileOutputFormat::GetOutputPath(*job_));
      output_path.append(FileOutputFormat::GetOutputPartitionName(job_,"map", job_->get_attribute("runner.chunk_id"), partition));
      LOG_DEBUG << "\nOutput path for partition " << partition << ": "
                << output_path;
      try {
        saga::url output_url(FileOutputFormat::GetUrl(*job_, output_path));
        writers_[partition] = new SequenceFileRecordWriter(output_url);
      } catch (saga::exception const& e) {
        LOG_ERROR << "Failed to create map writer for "
                  << FileOutputFormat::GetUrl(*job_, output_path).get_string()
                  << "; reason: " << e.what();
        throw e;
      }
    }
  }

  // Write out the sorted contents of the intermediate buffer using the
  // specified writer.
  void FlushIntermediateBuffer(IntermediateBufferT* buffer,
    RawRecordWriter* writer) {
    typename IntermediateBufferT::const_iterator it = buffer->begin();
    while (it != buffer->end()) {
      typename std::deque<ValueT>::const_iterator value_it =
        it->second.begin();
      while (value_it != it->second.end()) {
        key_buffer_.clear();
        value_buffer_.clear();
        // Serialize the key/value pair.
        StringOutputStream key_out(&key_buffer_);
        StringOutputStream value_out(&value_buffer_);
        SerializationHandler<KeyT>::Serialize(&(it->first), &key_out);
        SerializationHandler<ValueT>::Serialize(&(*value_it), &value_out);
        writer->Write(key_buffer_, value_buffer_);
        ++value_it;
      }
      ++it;
    }
  }

  DECLARE_LOGGER(TypedMapRunner);
  // Buffers for current key/value.
  std::string key_buffer_;
  std::string value_buffer_;
  // Currently processed job.
  JobDescription* job_;
  // Used partitioner.
  boost::scoped_ptr<Partitioner> partitioner_;
  int num_partitions_;
  // Buffers for intermediate values for each partition.
  boost::scoped_array<IntermediateBufferT*> buffers_;
  boost::scoped_array<TaskDescription*> tasks_;
  // Writers for each partition.
  boost::scoped_array<RawRecordWriter*> writers_;
};

}   // namespace worker
}   // namespace mapreduce

// Implementation.
#include "TypedMapRunner_impl.hpp"

#endif  // MAPREDUCE_WORKER_TYPEDMAPRUNNER_HPP_
