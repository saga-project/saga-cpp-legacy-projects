//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_INPUT_FILEINPUTFORMAT_HPP_
#define MAPREDUCE_INPUT_FILEINPUTFORMAT_HPP_

#include <boost/algorithm/string.hpp>
#include "../input_output.hpp"

#define JOB_ATTRIBUTE_FILE_INPUTPATH  "mapreduce.file.input_path"
#define JOB_INPUT_PATH_SEPARATOR  ";"

namespace mapreduce {

// InputChunk for file-based input formats.
class FileInputChunk : public InputChunk {
 public:
  friend class SerializationHandler<FileInputChunk>;
  FileInputChunk() {}
  FileInputChunk(const saga::url& path, int start_offset, int length,
    const std::vector<saga::url>& hosts)
  : path_(path), start_offset_(start_offset), length_(length), hosts_(hosts) {}
  // Accessors.
  int start_offset() const { return start_offset_; }
  const saga::url& path() const { return path_; }
  // InputChunk implementation.
  int GetLength() const { return length_; }
  const std::vector<saga::url>& GetLocations() {
    return hosts_;
  }
 private:
  saga::url path_;
  int start_offset_;
  int length_;
  std::vector<saga::url> hosts_;
};

// Serialization support for FileInputChunk.
template<>
class SerializationHandler<FileInputChunk> {
 public:
  static void Serialize(const FileInputChunk* chunk, ZeroCopyOutputStream* output) {
    CodedOutputStream coded_output(output);
    const std::string& url = chunk->path().get_url();
    coded_output.WriteVarint32(url.length());
    coded_output.WriteRaw(url.data(), url.length());
    coded_output.WriteVarint32(chunk->start_offset());
    coded_output.WriteVarint32(chunk->GetLength());
    // TODO: hosts
  }
  static void Deserialize(ZeroCopyInputStream* input, FileInputChunk* chunk) {
    CodedInputStream coded_input(input);
    uint32 url_length;
    coded_input.ReadVarint32(&url_length);
    std::string url;
    coded_input.ReadString(&url, url_length);
    chunk->path_.set_url(url);
    coded_input.ReadVarint32(reinterpret_cast<uint32*>(&chunk->start_offset_));
    coded_input.ReadVarint32(reinterpret_cast<uint32*>(&chunk->length_));
    // TODO: hosts
  }
};

// Base of all file-based input formats. Implements a general method for
// chunking.
class FileInputFormat : public RawInputFormat {
 public:
  static void AddInputPath(JobDescription& job, const std::string& path) {
    std::string current_paths(job.get_attribute(JOB_ATTRIBUTE_FILE_INPUTPATH));
    if (!current_paths.empty()) {
      current_paths += ";";
    }
    current_paths += path;
    // Replace attribute value.
    job.set_attribute(JOB_ATTRIBUTE_FILE_INPUTPATH, current_paths);
  }
  //
  // RawInputFormat implementation.
  //
  void SerializeInputChunk(InputChunk* chunk, ZeroCopyOutputStream* buffer) {
    FileInputChunk* input_chunk = dynamic_cast<FileInputChunk*>(chunk);
    assert(input_chunk);
    SerializationHandler<FileInputChunk>::Serialize(input_chunk, buffer);
  }
  InputChunk* CreateInputChunk(ZeroCopyInputStream* input) {
    // Deserialize FileInputChunk from buffer.
    FileInputChunk* input_chunk = new FileInputChunk();
    SerializationHandler<FileInputChunk>::Deserialize(input, input_chunk);
    return input_chunk;
  }
  // Chunk the input data specified in the job's descriptor.
  std::vector<InputChunk*> GetChunks(const JobDescription& job) {
    std::vector<InputChunk*> result;
    // List input paths for this job.
    std::vector<std::string> paths;
    ListInputPaths(job, paths);
    // Get files to be processed on each path.
    std::vector<saga::url> file_list;
    for (std::vector<std::string>::const_iterator path_it = paths.begin();
      path_it != paths.end(); ++path_it) {
      saga::url url(*path_it);
      // Check if this path denotes a directory.
      bool is_directory = false;
      try {
        saga::filesystem::directory directory(url, saga::filesystem::Read);
        // It is a directory.
        is_directory = true;
        // List files in this directory.
        std::vector<saga::url> entries = directory.list();
        for (std::vector<saga::url>::const_iterator entry_it = entries.begin();
          entry_it != entries.end(); ++entry_it) {
          if (!directory.is_dir(*entry_it) && !directory.is_link(*entry_it)) {
            file_list.push_back(saga::url(directory.get_url().get_string() +
                entry_it->get_url()));
          }
        }
      } catch (const saga::exception& e) {
      }
      if (!is_directory) {
        file_list.push_back(url);
      }
    }
    // Generate chunks for each file.
    std::vector<saga::url>::const_iterator file_it = file_list.begin();
    while (file_it != file_list.end()) {
      // Get replicas for this file.
      // FIXME(miklos): this should be done at the block level.
      //saga::replica::logical_file logical_file(url);
      //const std::vector<saga::url>& locations = logical_file.list_locations();
      std::vector<saga::url> locations;
      // Create chunks for this file.
      ChunkFile(job, *file_it, locations, result);
      ++file_it;
    }
    return result;
  }
 private:
  // Retrieve the input paths from the job's description. 
  void ListInputPaths(const JobDescription& job,
    std::vector<std::string>& result) {
    const std::string& paths = job.get_attribute(JOB_ATTRIBUTE_FILE_INPUTPATH);
    // Tokenize path list.
    ::boost::split(result, paths, ::boost::is_any_of(";"));
  }
  // Split the given file into chunks.
  void ChunkFile(const JobDescription& job, const saga::url& url,
    const std::vector<saga::url>& locations, std::vector<InputChunk*>& result) {
    // Take maximum chunk size into consideration.
    int max_chunk_size = job.get_attribute("file.input.max_chunk_size",
      64*1024*1024);
    saga::filesystem::file file(url);
    int offset = 0;
    int remaining_size = file.get_size();
    while (remaining_size > 0) {
      int chunk_size = std::min(max_chunk_size, remaining_size);
      result.push_back(new FileInputChunk(url, offset, chunk_size, locations));
      offset += chunk_size;
      remaining_size -= chunk_size;
    }
  }
};

}   // namespace mapreduce

#endif  // MAPREDUCE_INPUT_FILEINPUTFORMAT_HPP_
