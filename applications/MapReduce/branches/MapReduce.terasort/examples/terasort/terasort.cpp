#include <sstream>
#include <string>

#include "mapreduce.hpp"
#include "master/DistributedJobRunner.hpp"

#include "tera_utils.hpp"

using std::stringstream;
using std::string;

using mapreduce::Mapper;
using mapreduce::JobDescription;

class TeraOutputRecordWriter : public RawRecordWriter {
 public:
  TeraOutputRecordWriter(saga::url& path) {
    writer_.reset(new SagaFileOutputStream(saga::filesystem::file(
        path, saga::filesystem::Write)));
  }
  ~TeraOutputRecordWriter() {}
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

// Output format for writing key/value pairs for TeraSort.
class TeraOutputFormat : public FileOutputFormat {
 public:
  RawRecordWriter* GetRecordWriter(TaskDescription* task) {
    // Default work path for task.
    saga::url default_url = FileOutputFormat::GetUrl(*task,
      FileOutputFormat::GetUniqueWorkFile(task));
    return new TeraOutputRecordWriter(default_url);
  }
};
class TeraPart : public Partitioner {
  public:
std::vector<std::string> partitions; 
 
void InitPart() {
//    saga::url partition_list = FileOutputFormat::GetUrl(*job, PARTITION_LIST);
      saga::url partition_list = "file://localhost//work/smaddi2/workerop/_partitions.lst";
     partitions = TeraKeyPartitionGenerator::ReadPartitions(
        &partition_list);
}


int GetPartition(const std::string& key, int num_partitions) {

        std::string deserialized_key;
        ArrayInputStream input_stream(mapreduce::string_as_array(&(const_cast<std::string&>(key))), key.size());
        mapreduce::SerializationHandler<std::string>::Deserialize(&input_stream, &deserialized_key);

    //saga::url partition_list = "file://localhost//work/smaddi2/workerop/_partitions.lst";
   // std::vector<std::string> partitions = TeraKeyPartitionGenerator::ReadPartitions(&partition_list);
     
    for (int i=0; i< partitions.size() ; i++)
      {
         std::string ps = partitions[i];
         if ( int(deserialized_key[0]) < int(ps[0]))
            return i;
      }
           return num_partitions-1;
}
};

REGISTER_PARTITIONER(TeraPart, 4);
REGISTER_OUTPUTFORMAT(TeraOutput, TeraOutputFormat);


// Mapper for extracting key/value from TeraSort input line.
#define TERASORT_KEY_LENGTH 10
class TeraSortMap : public Mapper<int, string, string, string> {
 public:
  void Map(const int& key, const string& value, Context* context) {
    // Get key/value.
    if (value.length() > 90)  
    context->Emit(value.substr(0,10),value.substr(11,90) );
  
}
};

REGISTER_MAPPER_CLASS(TeraSortMap, 1);

class TeraSortReduce : public Reducer<string, string, string, string> {
 public:
  void Reduce(const string& key, Iterator<string> &values, Context* context) {
    std::string val ;
   val.clear(); 
   while (values.Next()) {
        val += values.Current();
   }
 
    context->Emit(key," "+ val );
 
}
};
REGISTER_REDUCER_CLASS(TeraSortReduce, 1);

#define PARTITION_LIST  "_partition.lst"
class TeraPartitioner : public Partitioner {
 public:

  class TrieNode {
   public:
    TrieNode(int level) : level_(level) {}
    virtual int FindPartition(const std::string& key) = 0;
    int level() { return level_; }
   protected:
    int level_;
  };

  // Inner trie node holding 256 children for each possible next character.
  class InnerTrieNode : public TrieNode {
   public:
    InnerTrieNode(int level) : TrieNode(level) {}
    int FindPartition(const std::string& key) {
      if (key.length() <= level_) {
        return child[0]->FindPartition(key);
      } else {
        return child[(uint8_t)key.at(level_)]->FindPartition(key);
      }
    }
   friend class TeraPartitioner;
   boost::scoped_ptr<TrieNode> child[256];
  };

  // Leaf trie node holding all strings between lower and upper split key.
  class LeafTrieNode : public TrieNode {
   public:
    LeafTrieNode(int level, const std::vector<std::string>& splits, int lower, int upper) : TrieNode(level), splits_(splits), lower_(lower),
          upper_(upper) {}
    int FindPartition(const std::string& key) {
      for (int i = lower_; i < upper_; ++i) {

		if (splits_[i] > key) {
          return i;
        }
      }
      return upper_;
    }
   private:
    const std::vector<std::string>& splits_;
    int lower_;
    int upper_;
  };

  // Recursive function for building the trie for distributing keys evenly
  // across partitions.
  TrieNode* BuildTrie(const std::vector<std::string>& splits, int lower,

	int upper, const std::string& prefix, int max_depth) {
    int depth = prefix.size();
    if (depth >= max_depth || upper == lower) {
      return new LeafTrieNode(depth, splits, lower, upper);
    }
    // Create an inner trie node.
    InnerTrieNode* node = new InnerTrieNode(depth);
    std::string new_prefix(prefix);
    // Expand the prefix with one byte.
    new_prefix.push_back(1);
    int current_limit = lower;
    // Add a node for each possible next character.
    for (int c = 0; c < 255; ++c) {
      new_prefix[new_prefix.size()-1] = (char)(c + 1);
      // Find split right after currently tried prefix.
      lower = current_limit;
      while (splits[current_limit] < new_prefix && current_limit < upper) {
        ++current_limit;
      }
      new_prefix[new_prefix.size()-1] = (char)c;
      // Add new child.
      node->child[c].reset(BuildTrie(splits, lower, current_limit, new_prefix,
          max_depth));
    }
    // Assign rest of keys to last child.
    new_prefix[new_prefix.size()-1] = (char)127;
    node->child[255].reset(BuildTrie(splits, current_limit, upper, new_prefix,
        max_depth));
  }

  void InitPart() {
  //    saga::url partition_list = FileOutputFormat::GetUrl(*job, PARTITION_LIST);
        saga::url partition_list = "file://localhost//work/smaddi2/workerop/_partitions.lst";
        std::vector<std::string> partitions;
        partitions = TeraKeyPartitionGenerator::ReadPartitions(&partition_list);
        trie_root.reset(BuildTrie(partitions, 0, partitions.size(),std::string(""), 2));
  }
  // Partitioner implementation.
  int GetPartition(const std::string& key, int num_partitions) {
               std::string deserialized_key;
        ArrayInputStream input_stream(mapreduce::string_as_array(&(const_cast<std::string&>(key))), key.size());
        mapreduce::SerializationHandler<std::string>::Deserialize(&input_stream, &deserialized_key);
       return trie_root->FindPartition(deserialized_key.substr(1));
  }
       friend class TeraPartitioner;

boost::scoped_ptr<TrieNode> trie_root;

};


REGISTER_PARTITIONER(TeraPartitioner, 5);


int main(int argc, char** argv) {

    std::string input_loc;
    input_loc     = argv[1];

  // std::cerr << input_loc;

  try {
    // Will not continue execution if we are supposed to be a worker.
    // Otherwise jobs will be submitted.
    mapreduce::MapRunner* runner = mapreduce::MapRunnerFactory::get_by_key("TeraSortMap");
    assert(runner);
    mapreduce::ReduceRunner* runner2 = mapreduce::ReduceRunnerFactory::get_by_key("TeraSortReduce");
    assert(runner2);
    mapreduce::Partitioner* runner3 = mapreduce::PartitionerFactory::get_by_key("TeraPart");
    assert(runner3);

    mapreduce::InitFramework(argc, argv);
    MapReduceResult result;
    JobDescription job;
    // Specify input.
    job.set_input_format("Text"); 

     // try to get chunksize from env
    unsigned long int chunksize, tmpv;
    tmpv=64;
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

    FileInputFormat::AddInputPath(job, input_loc);
    job.set_mapper_class("TeraSortMap");
    job.set_reducer_class("TeraSortReduce");
    job.set_partitioner_class("TeraPartitioner");
    job.set_output_format("TeraOutput");
    job.set_num_reduce_tasks(2);
    TeraKeyPartitionGenerator partitioner;
    partitioner.SampleInput(&job, 2000);
    partitioner.WritePartitions(2, FileOutputFormat::GetUrl(job, "_partitions.lst") );
     
    std::cerr << "Done part";
    mapreduce::master::DistributedJobRunner job_runner(job);
    job_runner.Initialize(mapreduce::g_command_line_parameters["config"].as<std::string>());
    job_runner.Run(&result);
  } catch (const std::exception& e) {
    std::cerr << "std exception caught: " << e.what() << std::endl;
  } catch (const saga::exception& se) {
    std::cerr << "saga exception caught: " << se.what() << std::endl;
  }

}
