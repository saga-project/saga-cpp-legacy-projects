#include <iostream>
#include "type.hpp"
#include "interface.hpp"

int main(int argc, char* argv[]){
   int x;
   std::vector<std::string>::size_type y;
   std::vector<std::string> chunks;
   try{
      for(x=1;x<argc;x++){
         chunks.push_back(std::string(argv[x]));
      }
      chunks=chunker<int>(chunks,100);//10485760);
      for(y=0;y<chunks.size();y++){
         std::cout << chunks[y] << std::endl;
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
