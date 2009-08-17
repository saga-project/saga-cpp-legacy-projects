//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <saga/saga.hpp>

#include "input/SequenceFileInputFormat.hpp"
#include "Reducer.hpp"
#include "input_output.hpp"
#include "MergingRawRecordReader.hpp"

using namespace mapreduce;
using namespace mapreduce::io;

namespace mapreduce { namespace worker {

template <typename ReducerT>
void TypedReduceRunner<ReducerT>::RunTask(TaskDescription* task,
  saga::advert::directory& input_dir) {
  // Prepare input.
  // Get input files for this partition.
  std::vector<std::string> input_files;
  LOG_DEBUG << "Reading input files from " << input_dir.get_url().get_string();
  try {
    std::vector<saga::url> entries = input_dir.list("*");
    std::vector<saga::url>::const_iterator entries_it = entries.begin();
    int mode = saga::advert::ReadWrite;
    while(entries_it != entries.end()) {
      saga::advert::entry adv(input_dir.open(*entries_it, mode));
      input_files.push_back(adv.retrieve_string());
      LOG_DEBUG << "Added reduce input " << adv.retrieve_string();
      ++entries_it;
    }
  } catch(saga::exception const & e) {
      throw;
  }
  if (input_files.size() == 0) {
    LOG_DEBUG << "No input files specified. Quitting.";
    return;
  }
  // Create reader for files.
  boost::scoped_ptr<RawRecordReader> reader;
  if (input_files.size() == 1) {
    LOG_DEBUG << "Creating single reader";
    // Special case when no merging is needed.
    reader.reset(new SequenceFileRecordReader(saga::url(
      input_files.back()), 0));
  } else {
    LOG_DEBUG << "Creating merging reader";
    // Instantiate readers for each intermediate input part.
    std::vector<RawRecordReader*> readers;
    std::vector<std::string>::const_iterator input_file = input_files.begin();
    while (input_file != input_files.end()) {
      readers.push_back(new SequenceFileRecordReader(
        FileOutputFormat::GetUrl(*task, *input_file), 0));
      ++input_file;
    }
    // Merge input files while reading them.
    reader.reset(new MergingRawRecordReader<typename ReducerT::out_key_type,
      typename ReducerT::comparator_type>(readers));
  }
  // Prepare output.
  boost::scoped_ptr<RecordWriter<typename ReducerT::out_key_type,
                                 typename ReducerT::out_value_type> > writer;
  RawOutputFormat* output_format = mapreduce::OutputFormatFactory::get_by_key(
    task->get_output_format());
  TypedOutputFormat<typename ReducerT::out_key_type, typename ReducerT::out_value_type>
    typed_output(output_format);
  // Direct output.
  writer.reset(typed_output.GetRecordWriter(task));
  // Create comparator.
  RawComparator<typename ReducerT::key_type>* comparator =
    new RawBytesComparator<typename ReducerT::key_type>();
  // Run the reducer on this partition.
  typename ReducerT::Context context(reader.get(), comparator, writer.get());
  ReducerT reducer;
  reducer.Run(&context);
  // Cleanup.
  reader->Close();
  writer->Close();
}

}   // namespace worker
}   // namespace mapreduce
