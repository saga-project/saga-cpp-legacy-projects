//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_INPUT_OUTPUT_HPP_
#define MAPREDUCE_INPUT_OUTPUT_HPP_

#include <saga/saga.hpp>
#include "serialization.hpp"
#include "job.hpp"
#include "factory_impl.hpp"
#include "io/raw_format.hpp"

namespace mapreduce {

// Helper function for consuming data from input and writing it into a string.
inline static void CopyStreamIntoString(ZeroCopyInputStream* input, std::string& output) {
  const void* buffer;
  int size;
  int total_size = 0;
  // Copy stream's content into output.
  while (input->Next(&buffer, &size)) {
    output.append(reinterpret_cast<const char*>(buffer), size);
    total_size += size;
  }
}

// Class representing one piece of input data.
class InputChunk {
 public:
  virtual ~InputChunk() {}
  virtual int GetLength() const = 0;
  virtual const std::vector<saga::url>& GetLocations() = 0;
};

// Class for reading bytes representing serialized objects.
class RawRecordReader {
 public:
  virtual ~RawRecordReader() {}
  // Initialize record reader with the specified chunk.
  virtual void Initialize(InputChunk* input_chunk) = 0;
  // Retrieve next record.
  virtual bool NextRecord() = 0;
  // Get inputstream for current key.
  virtual ZeroCopyInputStream* current_key() = 0;
  // Get immutable buffer for current key.
  virtual void get_key_buffer(const uint8** buffer, int* size) {
    // Default implementation.
    CopyStreamIntoString(current_key(), default_key_buffer_);
    *buffer = reinterpret_cast<const uint8*>(string_as_array(&default_key_buffer_));
    *size = default_key_buffer_.size();
  }
  // Get inputstream for current value.
  virtual ZeroCopyInputStream* current_value() = 0;
  // Close the record reader.
  virtual void Close() = 0;
 private:
  std::string default_key_buffer_;
};

// Class for reading typed input using a RawRecordReader.
template <typename KeyT, typename ValueT>
class RecordReader {
 public:
  RecordReader() {}
  // Initialize record reader with the raw record reader.
  void Initialize(RawRecordReader* raw_reader) { raw_reader_ = raw_reader; }
  // Retrieve next record.
  bool NextRecord() {
    bool success = raw_reader_->NextRecord();
    if (success) {
      // Deserialize key and value.
      SerializationHandler<KeyT>::Deserialize(raw_reader_->current_key(),
        &current_key_);
      SerializationHandler<ValueT>::Deserialize(raw_reader_->current_value(),
        &current_value_);
    }
    return success;
  }
  // Close the record reader.
  void Close() { raw_reader_->Close(); }
  const KeyT& current_key() const { return current_key_; }
  const ValueT& current_value() const { return current_value_; }
 private:
  KeyT current_key_;
  ValueT current_value_;
  RawRecordReader* raw_reader_;
};

// Class for writing key/value pairs represented by blocks of bytes.
class RawRecordWriter {
 public:
  virtual ~RawRecordWriter() {};
  // Output the given key/value pair.
  virtual void Write(const std::string& key, const std::string& value) = 0;
  // Close the record writer.
  virtual void Close() = 0;
};

// RecordWriter capable of outputting typed key/value pairs.
template <typename KeyT, typename ValueT>
class RecordWriter {
 public:
  RecordWriter(RawRecordWriter* writer) {
    Initialize(writer);
  }
  virtual ~RecordWriter() {}
  virtual void Initialize(RawRecordWriter* writer) { raw_writer_ = writer; }
  virtual void Write(const KeyT& key, const ValueT& value) {
    key_buffer_.clear();
    value_buffer_.clear();
    // Serialize the key/value pair.
    StringOutputStream key_out(&key_buffer_);
    StringOutputStream value_out(&value_buffer_);
    SerializationHandler<KeyT>::Serialize(&key, &key_out);
    SerializationHandler<ValueT>::Serialize(&value, &value_out);
    // Write them using the RawRecordWriter.
    raw_writer_->Write(key_buffer_, value_buffer_);
  }
  // Close the record writer.
  virtual void Close() {
    raw_writer_->Close();
  }
 protected:
  RecordWriter() {}

  std::string key_buffer_;
  std::string value_buffer_;
  RawRecordWriter* raw_writer_;
};

// Typed version of a RawInputFormat which handles the deserialization of
// objects from the underlying data stream by delegating it to a RecordReader.
template <typename KeyT, typename ValueT>
class TypedInputFormat {
 public:
  TypedInputFormat(RawInputFormat* raw_input_format)
    : raw_input_format_(raw_input_format) {}
  // Deserialize the InputChunk needed for this TypedInputFormat from the message.
  InputChunk* CreateInputChunk(ZeroCopyInputStream* buffer) {
    return raw_input_format_->CreateInputChunk(buffer);
  }
  // Chunk the input data specified in the job's descriptor.
  std::vector<InputChunk*> GetChunks(const JobDescription& job) {
    return raw_input_format_->GetChunks(job);
  }
  // Create a record reader to be used by the Mapper/Combiner/Reducer function.
  RecordReader<KeyT, ValueT>* GetRecordReader(InputChunk* chunk) {
    RecordReader<KeyT, ValueT>* new_reader = new RecordReader<KeyT, ValueT>();
    new_reader->Initialize(raw_input_format_->GetRecordReader(chunk, NULL));
    return new_reader;
  }
 private:
  RawInputFormat* raw_input_format_;
};

// InputFormat factory.
typedef factory< ::mapreduce::RawInputFormat, std::string > InputFormatFactory;
typedef signatures<RawInputFormat()> inputformat_signatures;

// Macro for registering an InputFormat in the framework.
#define REGISTER_INPUTFORMAT(name, clazz, id)   \
  REGISTER_CLASS( clazz, ::mapreduce::InputFormatFactory,  \
    #name, ::mapreduce::inputformat_signatures, id);


// Typed version of a RawOutputFormat which handles the serialization of
// objects given from the caller by delegating it to a RecordWriter.
template <typename KeyT, typename ValueT>
class TypedOutputFormat {
 public:
  TypedOutputFormat(RawOutputFormat* output_format)
    : output_format_(output_format) {}
  RecordWriter<KeyT, ValueT>* GetRecordWriter(TaskDescription* task) {
    RawRecordWriter* raw_writer = output_format_->GetRecordWriter(task);
    return new RecordWriter<KeyT, ValueT>(raw_writer);
  }
 private:
  RawOutputFormat* output_format_;
};

// OutputFormat factory.
/*typedef factory< ::mapreduce::RawOutputFormat, std::string > OutputFormatFactory;
typedef signatures<RawOutputFormat()> outputformat_signatures;

// Macro for registering an OutputFormat in the framework.
#define REGISTER_OUTPUTFORMAT(name, clazz, id)   \
  REGISTER_CLASS( clazz, ::mapreduce::OutputFormatFactory,  \
    #name, ::mapreduce::outputformat_signatures, id);
*/
#define REGISTER_OUTPUTFORMAT(name, clazz)  \
  static RegisterInFactory< ::mapreduce::RawOutputFormat, clazz > clazz ## RegistererClass__ (#name)

typedef ::factory_ns::GenericFactory< ::mapreduce::RawOutputFormat, std::string > OutputFormatFactory;

}   // namespace mapreduce

// Path list separator used by file-based input/output formats.
#define PATH_LIST_SEPARATOR ";"

// Include built-in input/output formats.
#include "input/TextInputFormat.hpp"
#include "output/SequenceFileOutputFormat.hpp"

#endif  // MAPREDUCE_INPUT_OUTPUT_HPP_
