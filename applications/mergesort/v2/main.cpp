#include <vector>
#include <iostream>
#include <saga/saga.hpp>

int main(int argc, char* argv[]){
//   saga::session s;
   std::vector<std::string> files_to_chunk;  //Can only be 2 now
   try{
      files_to_chunk.push_back(argv[1]);
      saga::job::service js(saga::url("any://localhost"));
      std::stringstream mergesort_job;
      saga::job::ostream in0, in1;
      saga::job::istream out0, err0, out1, err1;
      mergesort_job << "/home/michael/saga/trunk/examples/uses/mergesort/v2/chunker " << files_to_chunk[0];
      std::cout << mergesort_job.str() << std::endl;
      saga::job::job j0;//,j1;
//      j0 = js.run_job(mergesort_job.str(),"any:://localhost",in0,out0,err0);
   }      
   catch (saga::exception const &e) {
     std::cerr << "Error: " << e.what() << std::endl;
     return -1;
   }
   catch (std::exception const &e){
      std::cerr << "Error:  " << e.what() << std::endl;
      return -2;
   }
   return 0;
}
