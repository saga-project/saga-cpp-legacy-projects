//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <saga/saga.hpp>

#include "../input_output.hpp"
#include "../io/SequenceFile.hpp"
#include "../Partitioner.hpp"

using namespace mapreduce;
using namespace mapreduce::io;
/*mapreduce::InputFormatFactory;
using mapreduce::InputFormatCreator;
using mapreduce::RawInputFormat;
using mapreduce::InputChunk;
using mapreduce::TypedInputFormat;
using mapreduce::RecordReader;*/

namespace mapreduce { namespace worker {

template <typename MapperT>
void TypedMapRunner<MapperT>::RunTask(JobDescription* job,
  ZeroCopyInputStream* input) {
  // Prepare input.
  boost::scoped_ptr<RawInputFormat> input_format(InputFormatFactory::get_by_key(
    job->get_input_format()));
  boost::scoped_ptr<InputChunk> chunk(input_format->CreateInputChunk(
    input));
  TypedInputFormat<typename MapperT::key_type, typename MapperT::value_type>
    typed_input(input_format.get());
  boost::scoped_ptr<RecordReader<typename MapperT::key_type, typename MapperT::value_type> >
    reader(typed_input.GetRecordReader(chunk.get()));
  // Prepare output.
  boost::scoped_ptr<RecordWriter<typename MapperT::out_key_type,
                                 typename MapperT::out_value_type> > writer;
  // See if we need to partition the output.
  boost::scoped_ptr<TaskDescription> map_task(new TaskDescription(job));
  if (job->get_num_reduce_tasks() > 0) {
    // Partitioned output.
    writer.reset(
      new MapPartitionedOutput<typename MapperT::out_key_type,
                               typename MapperT::out_value_type>(job));
  } else {
    RawOutputFormat* output_format = mapreduce::OutputFormatFactory::get_by_key(
      job->get_output_format());
    TypedOutputFormat<typename MapperT::out_key_type, typename MapperT::out_value_type>
      typed_output(output_format);
    // Direct output.
    writer.reset(typed_output.GetRecordWriter(map_task.get()));
  }
  // Run the mapper on this chunk.
  typename MapperT::Context context(reader.get(), writer.get());
  MapperT mapper;
  // TODO(miklos): exception handling?
  mapper.Run(&context);
  // Cleanup.
  reader->Close();
  writer->Close();
}

}   // namespace worker
}   // namespace mapreduce
