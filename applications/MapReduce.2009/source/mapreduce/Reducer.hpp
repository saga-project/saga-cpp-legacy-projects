//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_REDUCER_H_
#define MAPREDUCE_REDUCER_H_

#include "serialization.hpp"
#include "factory_impl.hpp"
#include "job.hpp"
#include "input_output.hpp"
#include "utils/logging.hpp"

namespace mapreduce {

// Interface for the comparison of blocks of bytes representing serialized
// objects.
template <class T>
class RawComparator {
 public:
  virtual ~RawComparator() {}
  // Compare two byte buffers.
  virtual int Compare(const uint8* data1, int size1, const uint8* data2,
    int size2) = 0;
};

template <class T>
class RawBytesComparator : public RawComparator<T> {
 public:
  int Compare(const uint8* data1, int size1, const uint8* data2, int size2);
};

// Default comparator class which deserializes the keys from the given buffers
// and compares them using comparison operators.
template <class T>
class DefaultComparator {
 public:
  int Compare(const uint8* data1, int size1, const uint8* data2, int size2) {
    ArrayInputStream ais1(data1, size1);
    ArrayInputStream ais2(data2, size2);
    T key1, key2;
    SerializationHandler<T>::Deserialize(&ais1, &key1);
    SerializationHandler<T>::Deserialize(&ais2, &key2);
    if (key1 < key2) {
      return -1;
    } else if (key1 > key2) {
      return 1;
    } else {
      return 0;
    }
  }
};

// Simple iterator interface for enumerating values for a given key.
template <typename T>
class Iterator {
 public:
  // Obtain next object in the iterator.
  virtual bool Next() = 0;
  // Retrieve reference to current object in the iterator.
  virtual T& Current() = 0;
};

// Class defining the context in which a mapper is run.
template <typename KeyIn, typename ValueIn, typename KeyOut, typename ValueOut>
class ReducerContext : public Iterator<ValueIn> {
 public:
  // Get next key.
  bool NextKey();
  const KeyIn& current_key() { return key_; }
  const ValueIn& current_value() { return value_; }
  // Write data using the underlying writer.
  void Emit(KeyOut& key, ValueOut& value) {
    LOG_DEBUG << "Emit (" << key << ", " << value << ")";
    writer_->Write(key, value);
  }
  void Emit(const KeyOut key, const ValueOut value) {
    LOG_DEBUG << "Emit (" << key << ", " << value << ")";
    writer_->Write(key, value);
  }
  Iterator<ValueIn>& GetCurrentValues() {
    iterator_begin_ = true;
    stop_ = false;
    return *this;
  }
  // Iterator implementation.
  bool Next();
  ValueIn& Current() { return value_; }
 protected:
  ReducerContext() {}
  void Initialize(RawRecordReader* input, RawComparator<KeyIn>* comparator,
    RecordWriter<KeyOut, ValueOut>* writer);

  RawRecordReader* input_;
  RawComparator<KeyIn>* comparator_;
  RecordWriter<KeyOut, ValueOut>* writer_;
 private:
  // Get next key/value pair.
  bool NextPair();

  DECLARE_LOGGER(ReducerContext);
  KeyIn key_;   // Current deserialized key in context.
  ValueIn value_;   // Current deserialized value in context.
  std::string key_buffer_;
  std::string previous_key_;
  std::string value_buffer_;
  bool iterator_begin_;   // Iterate through values for the same key.
  bool stop_;   // Whether end of key chain has been reached by iterator.
  bool exhausted_;  // Whether input has been consumed.
  bool same_as_previous_key_;   // Is the next key identical to the current one?
};

//
// Reducer class definition.
//
template <typename KeyIn, typename ValueIn, typename KeyOut, typename ValueOut,
  class Comparator = DefaultComparator<KeyOut> >
class Reducer {
 public:
  // Context specific to this reducer instantiation.
  class Context : public ReducerContext<KeyIn, ValueIn, KeyOut, ValueOut>  {
   public:
    Context(RawRecordReader* input, RawComparator<KeyIn>* comparator,
      RecordWriter<KeyOut, ValueOut>* writer) {
      Initialize(input, comparator, writer);
    }
  };
  // Signature for Reducer function.
  virtual void Reduce(const KeyIn& key, Iterator<ValueIn>& values, Context* context) = 0;
  // Process the available data with this reducer.
  virtual void Run(Context* context) {
    // Iterate through all keys in this context.
    while (context->NextKey()) {
      LOG_DEBUG << "Next key is " << context->current_key();
      Reduce(context->current_key(), context->GetCurrentValues(), context);
    }
  }
  // Typedefs made publicly accessible.
  typedef KeyIn key_type;
  typedef ValueIn value_type;
  typedef KeyOut out_key_type;
  typedef ValueOut out_value_type;
  typedef Comparator comparator_type;
 private:
  DECLARE_LOGGER(Reducer);
};

// Executes a given mapper given the information present in the message.
// Must do deserialization of input chunk.
class ReduceRunner {
 public:
  virtual ~ReduceRunner() {}
  virtual void Execute(TaskDescription* job, saga::advert::directory& input_dir) = 0;
};

}   // namespace mapreduce

#include "worker/TypedReduceRunner.hpp"

// Implementation.
#include "Reducer_impl.hpp"

namespace mapreduce {

// ReduceRunner factory.
typedef factory< ::mapreduce::ReduceRunner, std::string > ReduceRunnerFactory;
typedef signatures<ReduceRunner()> reducerunner_signatures;

// Macro for registering a Reducer in the framework.
#define REGISTER_REDUCER_CLASS(name, id)   \
  class name ## Runner__ : public ::mapreduce::ReduceRunner {  \
   public:  \
     void Execute(::mapreduce::TaskDescription* task, saga::advert::directory& input_dir) {  \
       ::mapreduce::worker::TypedReduceRunner<name> typed_runner; \
       typed_runner.RunTask(task, input_dir); \
     }  \
  };  \
  REGISTER_CLASS( name ## Runner__, ::mapreduce::ReduceRunnerFactory, #name,   \
    ::mapreduce::reducerunner_signatures, id);

}   // namespace mapreduce

#endif  // MAPREDUCE_REDUCER_H_
