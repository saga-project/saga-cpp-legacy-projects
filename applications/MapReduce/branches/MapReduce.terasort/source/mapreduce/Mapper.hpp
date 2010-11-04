//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_MAPPER_HPP_
#define MAPREDUCE_MAPPER_HPP_

#include "input_output.hpp"
#include "factory_impl.hpp"
#include "worker/TypedMapRunner.hpp"

namespace mapreduce {

// Class defining the context in which a mapper is run.
template <typename KeyIn, typename ValueIn, typename KeyOut, typename ValueOut>
class MapperContext {
 public:
  // Read next record.
  bool NextRecord() {
    return reader_->NextRecord();
  }
  const KeyIn& current_key() { return reader_->current_key(); }
  const ValueIn& current_value() { return reader_->current_value(); }
  // Write data using the underlying writer.
  void Emit(KeyOut& key, ValueOut& value) {
    writer_->Write(key, value);
  }
  void Emit(const KeyOut key, const ValueOut value) {
    writer_->Write(key, value);
  }
 protected:
  MapperContext() {}
  void Initialize(RecordReader<KeyIn, ValueIn>* reader,
    RecordWriter<KeyOut, ValueOut>* writer) {
    reader_ = reader;
    writer_ = writer;
  }
  RecordReader<KeyIn, ValueIn>* reader_;
  RecordWriter<KeyOut, ValueOut>* writer_;
 private:
  DECLARE_LOGGER(Mapper);
};

// Mapper
template <typename KeyIn, typename ValueIn, typename KeyOut, typename ValueOut>
class Mapper {
 public:
 virtual ~Mapper() {}
  //typedef MapperContext<KeyIn, ValueIn, KeyOut, ValueOut> Context;
  class Context : public MapperContext<KeyIn, ValueIn, KeyOut, ValueOut>  {
   public:
    Context(RecordReader<KeyIn, ValueIn>* reader,
      RecordWriter<KeyOut, ValueOut>* writer) {
      Initialize(reader, writer);
    }
  };
  // Signature of mapper function.
  virtual void Map(const KeyIn& key, const ValueIn& value, Context* context) = 0; 
  // Default map function.
//  virtual void Map(const KeyIn key, const ValueIn& value, Context* context) {
//    context->Emit((KeyOut)key, (ValueOut)value);
//  }
  // Process the available records with this mapper.
  virtual void Run(Context* context) {
    while (context->NextRecord()) {
      Map(context->current_key(), context->current_value(), context);
    }
  }
  // Typedefs made publicly accessible.
  typedef KeyIn key_type;
  typedef ValueIn value_type;
  typedef KeyOut out_key_type;
  typedef ValueOut out_value_type;
};

// Executes a given mapper given the information present in the message.
// Must do deserialization of input chunk.
class MapRunner {
 public:
  virtual ~MapRunner() {}
  virtual void Execute(JobDescription* job, ZeroCopyInputStream* message) = 0;
};

// MapRunner factory.
typedef factory< ::mapreduce::MapRunner, std::string > MapRunnerFactory;
typedef signatures<MapRunner()> maprunner_signatures;

#define MAPPER_RUNNER_NAME(name)  name ## Runner__

// Macro for registering a Mapper in the framework.
#define REGISTER_MAPPER_CLASS(name, id)   \
  class name ## Runner__ : public ::mapreduce::MapRunner {  \
   public:  \
     void Execute(::mapreduce::JobDescription* job, ZeroCopyInputStream* server_message) {  \
       ::mapreduce::worker::TypedMapRunner<name> typed_runner; \
       typed_runner.RunTask(job, server_message); \
     }  \
  };  \
  REGISTER_CLASS( name ## Runner__, ::mapreduce::MapRunnerFactory, #name,   \
    ::mapreduce::maprunner_signatures, id);

}   // namespace mapreduce

#endif  // MAPREDUCE_MAPPER_HPP_
