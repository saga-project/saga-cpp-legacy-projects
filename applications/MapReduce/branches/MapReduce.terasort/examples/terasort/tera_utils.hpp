#ifndef TERA_UTILS_HPP_
#define TERA_UTILS_HPP_

#include "mapreduce/input_output.hpp"

using mapreduce::TextInputFormat;
using mapreduce::SerializationHandler;

class TeraKeyPartitionGenerator {
 public:
  TeraKeyPartitionGenerator() {}
  // Reads some input chunks to generate the required number of samples from the
  // input keys.
  void SampleInput(JobDescription* job, long sample_size) {

     //find the number of chunks to it collect samples 
    boost::scoped_ptr<RawInputFormat> input_format(new TextInputFormat);
    std::vector<InputChunk*> chunks = input_format->GetChunks(*job);
    
    int num_samples = std::min(10, static_cast<int>(chunks.size()));
    long records_per_chunk = sample_size / num_samples; // collect only these number of keys per chunk
    int chunk_per_sample = chunks.size() / num_samples; // use only these  number of chunks
    long num_records = 0;
    std::string key_buffer; 
    // check for max sample size
    for (int i = 0; i < num_samples; ++i) {
      boost::scoped_ptr<RawRecordReader> reader(input_format->GetRecordReader(
          chunks[chunk_per_sample * i], job));//take  a chunk to collect keys
      while (reader->NextRecord()) {
        // Extract key from line of text.
        ZeroCopyInputStream* value = reader->current_value();
        key_buffer.clear(); //use a key_buffer to store the entire record but clear it before using for next record
        SerializationHandler<std::string>::Deserialize(value, &key_buffer);
        keys_.push_back(key_buffer.substr(0, 10));
        // if are at max records_per_chunk for this sample break and goto next chunk.
        if (num_records >= (i + 1)*records_per_chunk) {
          break;
        } 
        ++num_records; //incement records
      }
    }
  }
  // Sorts the sampled keys and creates num_partitions-1 split points
  std::vector<std::string> GetPartitions(int num_partitions) {
    
    int num_keys = static_cast<int>(keys_.size());
    if (num_keys < num_partitions) {
      throw new std::runtime_error("Requested more partitions than keys available");
    }
    //sort the keys 
    std::sort(keys_.begin(), keys_.end());
    // Get each nth sample to get partitioning keys.
    float samples_per_partition = static_cast<float>(num_keys) /
        static_cast<float>(num_partitions);
    std::vector<std::string> result(num_partitions - 1);
    for (int i = 1; i < num_partitions; ++i) {
      result[i - 1] = keys_[static_cast<int>(round(samples_per_partition * i))];
    }
    return result; //return the array
  }
  
// Writes the required number of partitions from the sampled 
  void WritePartitions(int num_partitions, saga::url path) {

    //declare writer using input_output.hpp 
    RecordWriter<std::string, std::string> writer(
        new SequenceFileRecordWriter(path));
    
    std::vector<std::string> partitions = GetPartitions(num_partitions);
    
    std::string null_value;
    // write these partitions into given path 
    for (int i = 0; i < static_cast<int>(partitions.size()); ++i) {
      writer.Write(partitions[i], null_value);
    }
    writer.Close();
  }
  // Reads partitions from the given SequenceFile.
  static std::vector<std::string> ReadPartitions(saga::url* path) {
    // declaration of reader is form  the input_output.hpp

    RecordReader<std::string, std::string> reader;
    boost::scoped_ptr<RawRecordReader> record_reader(
        new SequenceFileRecordReader(*path));
    reader.Initialize(record_reader.get());
    

    std::vector<std::string> partitions;
    while (reader.NextRecord()) {
      partitions.push_back(reader.current_key());
    }
    reader.Close();
   //return the patitiions 
   return partitions;
  }
 private:
  std::vector<std::string> keys_;
};

#endif /*TERA_UTILS_HPP_*/
