#include <iostream>
#include <string>
#include <vector>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <saga/saga.hpp>

std::vector<int> parse(char *data){
   using boost::lexical_cast;
   using boost::bad_lexical_cast;
   std::vector<int> retval;
   std::string str(data);
   typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
   boost::char_separator<char> sep(" \n");
   tokenizer tokens(str, sep);
   try{
      for (tokenizer::iterator tok_iter = tokens.begin();tok_iter != tokens.end(); ++tok_iter){
         retval.push_back(lexical_cast<int>(*tok_iter));
      }
   }
   catch(bad_lexical_cast &){
      std::cerr << "Error" << std::endl;
   }
   return retval;
}

int main(int argc,char **argv){
   unsigned int size_to_read,offset;
   char *inbuff;
   std::vector<int> numbers;
   if(argc<4){
      std::cout << "\nUsage: " << argv[0] << " filename [offset amount] [size to read]\n" << std::endl;
      return -1;
   }
   offset=atoi(argv[2]);
   size_to_read = atoi(argv[3]);
   inbuff = new char [size_to_read+1];
   saga::size_t readbytes = 0;
   try{
      saga::filesystem::file f (saga::url(argv[1]), saga::filesystem::Read);
      f.seek(offset,saga::filesystem::Start);
      readbytes = f.read (saga::buffer(inbuff,size_to_read));
      numbers=parse(inbuff);
      std::sort(numbers.begin(),numbers.end());
      for(unsigned int x=0;x<numbers.size();x++){
         if(x==numbers.size()-1){
            std::cout << numbers[x];
            break;
         }
         std::cout << numbers[x] << " ";
      }
   }
   catch (saga::exception const &e) {
      std::cerr << "Couldn't read the file: " << e.what() << std::endl;
      return -1; 
   }
   catch (std::exception const &e){
      std::cerr << "Error:  " << e.what() << std::endl;
      return -2;
   }
   return 0; /*check return values if error*/
}
