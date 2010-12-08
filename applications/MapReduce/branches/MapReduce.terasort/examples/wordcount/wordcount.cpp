#include <sstream>
#include <string>

#include "mapreduce.hpp"
#include "master/DistributedJobRunner.hpp"

using std::stringstream;
using std::string;

using mapreduce::Mapper;
using mapreduce::JobDescription;

/**
 *
 * Word count example mapper and reducer.
 *
 */

class WordOutputRecordWriter : public RawRecordWriter {
 public:
  WordOutputRecordWriter(saga::url& path) {
    writer_.reset(new SagaFileOutputStream(saga::filesystem::file(
        path, saga::filesystem::Write)));
  }
  ~WordOutputRecordWriter() {}
  // RawRecordWriter implementation. Extracts original strings from
  // serialized data directly (assumes their length is stored on 1 byte).
  void Write(const std::string& key, const std::string& value) {
    writer_->Write((key.c_str()+1), key.size()-1);
    writer_->Write((value.c_str()+1), value.size()-1);
    writer_->Write(static_cast<const void*>("\r\n"), 2);
  }
  void Close() {
    writer_.reset();
  }
 private:
  boost::scoped_ptr<SagaFileOutputStream> writer_;
};

// Output format for writing key/value pairs for WordSort.
class WordOutputFormat : public FileOutputFormat {
 public:
  RawRecordWriter* GetRecordWriter(TaskDescription* task) {
    // Default work path for task.
    saga::url default_url = FileOutputFormat::GetUrl(*task,
      FileOutputFormat::GetUniqueWorkFile(task));
    return new WordOutputRecordWriter(default_url);
  }
};
REGISTER_OUTPUTFORMAT(WordOutput, WordOutputFormat);

class WordCountMap : public Mapper<int, string, string, int> {
 public:
  void Map(const int& key, const string& value, Context* context) {
    stringstream stream(value);
    string word;
    while (stream.good()) {
      stream >> word;
      context->Emit(word, 1);
      word.clear();
    }
  }
};
REGISTER_MAPPER_CLASS(WordCountMap, 1);

class WordCountReduce : public Reducer<string, int, string, int> {
 public:
  void Reduce(const string& key, Iterator<int> &values, Context* context) {
    int sum = 0;
    while (values.Next()) {
      sum += values.Current();
    }
    context->Emit(key, sum);
  }
};
REGISTER_REDUCER_CLASS(WordCountReduce, 1);


int main(int argc, char** argv) {

  try {
    // Will not continue execution if we are supposed to be a worker.
    // Otherwise jobs will be submitted.
    // TODO(miklos): the below should be done by the registerer macro.
    mapreduce::MapRunner* runner = mapreduce::MapRunnerFactory::get_by_key("WordCountMap");
    assert(runner);
    mapreduce::ReduceRunner* runner2 = mapreduce::ReduceRunnerFactory::get_by_key("WordCountReduce");
    assert(runner2);
    mapreduce::InitFramework(argc, argv);

    MapReduceResult result;
    JobDescription job;
    // Specify input.
    job.set_input_format("Text");
    unsigned long int chunksize, tmpv;
    tmpv=256;
    char* env_chunksize = ::getenv ("MR_CHUNK_SIZE");
    if ( NULL != env_chunksize )
    {
      tmpv = ::atoi (env_chunksize);
    }
    chunksize = 1024 * 1024 * tmpv;
    std::stringstream ss;
    ss << chunksize;
    job.set_attribute("file.input.max_chunk_size", ss.str ());
    std::cout << "setting chunksize to " << ss.str () << std::endl;

    FileInputFormat::AddInputPath(job,
      "file://localhost//path/to/input");
    job.set_mapper_class("WordCountMap");
    job.set_reducer_class("WordCountReduce");
    job.set_output_format("WordOutput");
    job.set_num_reduce_tasks(2);
    mapreduce::master::DistributedJobRunner job_runner(job);
    job_runner.Initialize(
      mapreduce::g_command_line_parameters["config"].as<std::string>());
    job_runner.Run(&result);
  }catch ( const std::exception & e )
  { std::cerr << "std exception cought: " << e.what () << std::endl;
  } catch ( const saga::exception & se ){
    std::cerr << "saga exception cought: " << se.what () << std::endl;
  }
}
