//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AP_CHUNKER_HPP
#define AP_CHUNKER_HPP

#include <saga/saga.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>
#include "type.hpp"

namespace AllPairs {
   template <typename T>
   std::vector<std::string> chunker(std::vector<std::string> file_arg, std::size_t block_size = 16777216){
      std::vector<std::string> chunk_filenames_list;
      typename std::vector<std::string>::const_iterator fileIterator;
      for(fileIterator=file_arg.begin();fileIterator!=file_arg.end();fileIterator++){
         int chunk_number=0;
         boost::iostreams::stream <saga_file_device> in (*fileIterator);
         while(in.good()){
            std::size_t size_total=0; T elem;
            in >> elem;
            if(in.fail()) break;
            std::string chunk_name(*fileIterator);
            std::stringstream convert;
            convert << chunk_number;
            chunk_name.append(".chunk");
            chunk_name.append(convert.str());
            boost::iostreams::stream <saga_file_device> out (chunk_name); 
            out << " ";
            out << elem;
            try
            {
              size_total+=boost::lexical_cast<std::string>(elem).size() + 1;
              while(size_total<block_size){
                 T elem;
                 in >> elem;
                 if(in.fail()) break;
                 out << " ";
                 out << elem;
                 size_total+=boost::lexical_cast<std::string>(elem).size() + 1;
              }
            }
            catch (boost::bad_lexical_cast &)
            {
                std::cerr << "Bad Lexical Cast Failure!" << std::endl;
            }
            chunk_number++;
            chunk_filenames_list.push_back(chunk_name);
         }
      }
      return chunk_filenames_list;
   }
}

#endif // AP_CHUNKER_HPP

