//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <saga/saga.hpp>

#include "input/SequenceFileInputFormat.hpp"
#include "Reducer.hpp"
#include "input_output.hpp"

using namespace mapreduce;
using namespace mapreduce::io;

namespace mapreduce { namespace worker {

/*class RawRecordReaderIterator : public RawKeyValueIterator {
 public:
  RawRecordReaderIterator(RawRecordReader* reader) : reader_(reader) {}
  // RawKeyValueIterator interface implementation.
  bool Next() {
    return reader_->Next();
  }
  ZeroCopyInputStream* current_key() {
    return reader_->current_key();
  }
  ZeroCopyInputStream* current_value() {
    return reader_->current_value();
  }
  void Close() {
    reader_->Close();
  }
};*/

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
    LOG_DEBUG << "No input files specified. Quitting";
    return;
  }
  // Create reader for files.
  // MergeSortingRawReader(input_files, comparator)
  LOG_DEBUG << "create read";
  boost::scoped_ptr<RawRecordReader> reader(new SequenceFileRecordReader(
    saga::url(input_files.back()), 0));
/*  {
      RecordReader<std::string, int> reader_;
  reader_.Initialize(reader.get());
  while (reader_.NextRecord()) {
    std::cerr <<reader_.current_key() << " " << reader_.current_value() << std::endl;
  }
  reader_.Close();
    
  }*/
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
  LOG_DEBUG << "run";
  reducer.Run(&context);
  // Cleanup.
  reader->Close();
  writer->Close();
}

}   // namespace worker
}   // namespace mapreduce
