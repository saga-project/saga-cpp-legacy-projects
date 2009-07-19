#include "RunComparison.hpp"
#include "../utils/defines.hpp"

namespace AllPairs {

/*********************************************************
 * RunComparison constructor sets up the state and other info   *
 * for the worker.                                       *
 * ******************************************************/
   RunComparison::RunComparison(saga::stream::stream server, LogWriter *log) :
      server_(server) , log_(log)
   {
      char buff[255];

      saga::ssize_t read_bytes = server_.read(saga::buffer(buff));
      std::string question(buff, read_bytes);
      if(question == START_CHUNK)
      {
         server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
         memset(buff, 0, 255);
         read_bytes = server_.read(saga::buffer(buff));
         std::string communication(buff, read_bytes);
         //The first try is always ID
         chunkID_ = boost::lexical_cast<int>(communication);
         std::cout << "Got ID: " << communication << std::endl;
         server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));

         while(1)
         {
            std::string communication;

            memset(buff, 0, 255);
            read_bytes = server_.read(saga::buffer(buff));
            communication = std::string(buff, read_bytes);
            if(communication == END_CHUNK)
            {
               break;
            }
            std::string url1(communication);
            server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
            
            memset(buff, 0, 255);
            read_bytes = server_.read(saga::buffer(buff));
            communication = std::string(buff, read_bytes);
            std::string url2(communication);
            server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));

            std::cout << "got (" << url1 << ", " << url2 << ")" << std::endl;
            chunk_.push_back(std::pair<std::string, std::string>(url1, url2));
         }
         //Ack end chunk
         server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
         chunk_IT = chunk_.begin();
      }
      else
      {
         std::cerr << "Misbehaving master?" << std::endl;
         //FIXME proper handling here
     }

   }
/*********************************************************
 * ~RunComparison destructor returns the state of the worker to *
 * what is expected by the master after a mapping is done*
 * ******************************************************/
   RunComparison::~RunComparison() {
   }

/*********************************************************
 * getFile retreives the chunk that was posted for this  *
 * worker to use from the advert database.               *
 * ******************************************************/
   bool RunComparison::hasAssignment() {
      if(chunk_IT == chunk_.end()) {
         return false;
      }
      return true;
   }

   assignment RunComparison::getAssignment() {
      assignment asn(*chunk_IT);
      chunk_IT++;
      return asn;
   }

   assignmentChunk RunComparison::getAssignmentChunk() {
      return chunk_;
   }

   int RunComparison::getChunkID() {
      return chunkID_;
   }
} // namespace AllPairs 

