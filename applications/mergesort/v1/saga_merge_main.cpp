#include <iostream>
#include <string>
#include <saga/saga.hpp>
#include <vector>

int merge_and_print(std::vector<saga::job::istream> &list);
//Doesn't work on empty files yet
//Need to change so you can split a file n ways
//Should work on most files especially the example ones (input)
int main(int argc, char* argv[]){
   saga::job::ostream in0, in1;
   saga::job::istream out0, err0, out1, err1;
   std::stringstream mergesort_job;
   std::vector<saga::url> list;
   saga::filesystem::file first;
   unsigned int readbytes = 0, file_size=0;
   char middle[20];
   if(argc<2){
      std::cerr << "\nUsage: " << argv[0] << " filename \n" << std::endl;
      return -1;
   }
   try{
      saga::url lurl = saga::url(argv[1]);
      lurl.set_scheme("any");
      lurl.set_host("localhost");
      saga::logical_file::logical_file lf(lurl, saga::filesystem::Read);
      list = lf.list_locations();
      saga::filesystem::file first(list[0],saga::filesystem::Read); 
      file_size=first.get_size();
      if(file_size==0){
         std::cerr << "File empty!" << std::endl;
         return 1;
      }
      first.seek(file_size/2,saga::filesystem::Start);
      first.read(saga::buffer(middle,20)); 
      while(middle[readbytes]!=' ' && middle[readbytes]!='\n' && middle[readbytes]!='\t' && readbytes < sizeof(middle)){
            readbytes++;
      }
      saga::job::service js(saga::url("any://localhost"));
      //job::job j0,j1;
      mergesort_job << "/home/michael/algo/mergesort/saga_merge/saga_mergesort " << list[0].get_url() << " " << 0 << " " << (file_size/2)+readbytes;
      /*j0 =*/ js.run_job("any:://localhost",mergesort_job.str(),in0,out0,err0);
      mergesort_job.str(std::string());
      mergesort_job << "/home/michael/algo/mergesort/saga_merge/saga_mergesort "<<  list[0].get_url() << " " << " " << (file_size/2)+readbytes << " " << (file_size/2)-readbytes;
      /*j1 =*/ js.run_job("any://localhost",mergesort_job.str(),in1,out1,err1);
      std::vector<saga::job::istream> output;
      output.push_back(out0);
      output.push_back(out1);
      merge_and_print(output);
   }
   catch (saga::exception const &e) {
     std::cerr << "Error: " << e.what() << std::endl;
     return -1;
   }
   catch (std::exception const &e){
      std::cerr << "Error:  " << e.what() << std::endl;
      return -2;
   }
//    j0.wait(); still flakey in linux
//    j1.wait();
//    std::cout << j1.get_state();
//check return value of j g.get_attribute() then can report from this
   return 0;
}

// Create a vector of streams, then loop through until all are done
int merge_and_print(std::vector<saga::job::istream> &list){
   int min=0;
   int position=-1;
   int x=0;
   int dist=0;
   std::vector<int> top, rank;
   std::vector<saga::job::istream>::iterator spot;
   for(spot = list.begin();spot!=list.end();spot++,x++){
      if(!spot->eof() && !spot->fail()){
         int num;
         *spot >> num;
         top.push_back(num);
         rank.push_back(x);
      }
      else{
         list.erase(spot);
      }
   }
   if(!top.empty()){
      min = top.front();
      position = rank.front();
   }
   else{
      std::cerr << "Empty List!" << std::endl;
      return -1;
   }
   for(std::vector<int>::size_type c=1;c<top.size();c++){
      if(top[c] < min){
         min = top[c];
         position = rank[c];
      }
   }
   std::cout << min << " ";
   std::vector<int>::iterator remove = top.begin() + position;
   top.erase(remove);
   remove = rank.begin() + position;
   rank.erase(remove);
   while(!list.empty()){
      if(!list[position].eof() && !list[position].fail()){
         int num;
         list[position] >> num;
         top.push_back(num);
         rank.push_back(position);
      }
      else{
         std::vector<saga::job::istream>::iterator remove = list.begin() + position;
         list.erase(remove);
      }
      if(!top.empty()){
         min = top.front();
         position = rank.front();
         dist = 0;
      }
      for(std::vector<int>::size_type c=0;c<top.size();c++){
         if(top[c] < min){
            min = top[c];
            position = rank[c];
            dist = c;
         }
      }
      std::cout << min << " ";
      std::vector<int>::iterator remove = top.begin() + dist;
      if(list.empty()){
         std::sort(top.begin(),top.end());
         for(unsigned int loop=0;loop<top.size();loop++){
            std::cout << top[loop] << " ";
         }
         std::cout << std::endl << "Complete!" << std::endl;
         return 1;
      }
      top.erase(remove);
      remove = rank.begin() + dist;
      rank.erase(remove);
   }
   return 1;
}
