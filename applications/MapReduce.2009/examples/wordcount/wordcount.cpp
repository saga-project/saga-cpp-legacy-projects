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

//using namespace mapreduce;
int main(int argc, char** argv) {
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
  job.set_input_format("Text");
  FileInputFormat::AddInputPath(job, mapreduce::g_command_line_parameters["i"].as<std::string>());
  job.set_mapper_class("WordCountMap");
  job.set_reducer_class("WordCountReduce");
  job.set_output_format("SequenceFile");
  job.set_num_reduce_tasks(1);
  mapreduce::master::DistributedJobRunner job_runner(job);
  job_runner.Initialize(mapreduce::g_command_line_parameters["config"].as<std::string>());
  job_runner.Run(&result);
}
