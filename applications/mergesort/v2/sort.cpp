
#include <iostream>
#include "interface.hpp"

int main(int argc, char* argv[]){
   int x;
   std::vector<std::string> to_be_sorted;
   std::vector<std::string> sorted;
   try{
      for(x=1;x<argc;x++){
         to_be_sorted.push_back(std::string(argv[x]));
      }
      sorted=sorter<int>(to_be_sorted, " \n");
      for(std::vector<std::string>::size_type x=0;x<sorted.size();x++){
         std::cout << sorted[x] << std::endl;
      }
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
