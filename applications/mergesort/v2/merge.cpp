#include <iostream>
#include "interface.hpp"

int main(int argc, char* argv[]){
   int x;
   std::vector<std::string> numbers;
   try{
      for(x=1;x<argc;x++){
         numbers.push_back(argv[x]);
//         std::cout << numbers[x-1] << std::endl;
      }
//      std::cout << numbers.max_size() << std::endl;
      std::cout << merge<int>(numbers,10000/*536870901*/) << std::endl;
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
