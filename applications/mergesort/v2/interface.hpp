#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <saga/saga.hpp>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>
#include "type.hpp" //can't be here

std::vector<std::string> chunker(std::vector<std::string> file_arg, size_t block_size);
std::vector<std::string> sorter(std::vector<std::string> chunks_arg, std::string delim);
std::string merge(std::vector<std::string> list_to_merge);

template <class T>
std::vector<std::string> chunker(std::vector<std::string> file_arg, size_t block_size){
   std::vector<std::string> chunk_filenames_list;
   typename std::vector<std::string>::const_iterator fileIterator;
   for(fileIterator=file_arg.begin();fileIterator!=file_arg.end();fileIterator++){
      int chunk_number=0;
      saga::filesystem::file f(saga::url(*fileIterator), saga::filesystem::Read);
      boost::iostreams::stream <saga_file_device> in (*fileIterator);
      while(in.good()){
         size_t size_total=0;
         T elem;
         in >> elem;
         if(in.fail()) break;
         std::string chunk_name(*fileIterator);
         std::stringstream convert;
         convert << chunk_number;
         chunk_name.append(".chunk");
         chunk_name.append(convert.str());
         saga::filesystem::file fout (saga::url(chunk_name), saga::filesystem::Write | saga::filesystem::Create); 
         boost::iostreams::stream <saga_file_device> out (chunk_name); 
         out << " ";
         out << elem;
         size_total+=sizeof(elem);
         size_total+=sizeof(" ");
         while(size_total<block_size){
//            std::cout << "Success:  " << size_total << " < " << block_size << std::endl;
            T elem;
            in >> elem;
            if(in.fail()) break;
            out << " "; //ask how to make this more generic
            out << elem;
            size_total+=sizeof(elem);
            size_total+=sizeof(" ");
         }
         fout.close();
         chunk_number++;
         chunk_filenames_list.push_back(chunk_name);
      }
   }
   return chunk_filenames_list;
}

template <class T>
std::vector<std::string> sorter(std::vector<std::string> chunks_arg,std::string delim){
   typename std::vector<std::string>::const_iterator fileIterator;
   std::vector<std::string> sorted_filenames_list;
   for(fileIterator=chunks_arg.begin();fileIterator!=chunks_arg.end();fileIterator++){
      T elem;
      saga::filesystem::file f(saga::url(*fileIterator), saga::filesystem::Read);
      boost::iostreams::stream <saga_file_device> in (*fileIterator);
      if(!in.good()) continue;
      std::vector<T> values;
      std::string sorted_chunk_name(*fileIterator);
      sorted_chunk_name.append(".sorted");
      saga::filesystem::file fout (saga::url(sorted_chunk_name), saga::filesystem::Write | saga::filesystem::Create);
      boost::iostreams::stream <saga_file_device> out (sorted_chunk_name);
      while(in >> elem){
         values.push_back(elem);
      }
      std::sort(values.begin(),values.end());
      for(typename std::vector<T>::size_type x=0;x<values.size();x++){
         out << " ";
         out << values[x];
      }
      sorted_filenames_list.push_back(sorted_chunk_name);
   }
   return sorted_filenames_list;
}

template <class T>
std::vector<std::string> private_merge(std::vector<std::string> filename_list, size_t buf_size){
   static int counter=0;
   std::vector<std::string> retval;
   if(filename_list.size()%2!=0) retval.push_back(filename_list.back());
   for(unsigned int x = 1;x<filename_list.size();x+=2){
      saga::filesystem::file f1(saga::url(filename_list[x-1]), saga::filesystem::Read);
      boost::iostreams::stream <saga_file_device> in1 (f1);
      //Open file1 & associate stream
      saga::filesystem::file f2(saga::url(filename_list[x]), saga::filesystem::Read);
      boost::iostreams::stream <saga_file_device> in2 (f2);
      //Open file2 & associate stream
      std::string filename("file.merged");
      std::stringstream convert;
      convert << counter;
      filename.append(convert.str());
      saga::filesystem::file fout (saga::url(filename), saga::filesystem::Write | saga::filesystem::Create);
      boost::iostreams::stream <saga_file_device> out (filename);
      //Create file where all merges to
      std::vector<T> leftover1;
      std::vector<T> leftover2;
      //A variable to catch what was greater than the greatest stored in list2
      while(1) {
         bool l1success = false;
         bool l2success = false;
         int end = -1;
         std::vector<T> list1(leftover1),list2(leftover2);
         leftover1.clear();
         leftover2.clear();
         //Create 2 vectors
         while(list1.size() < buf_size) {
            T elem;
            if(!(in1 >> elem)) {
               l1success = true;
               break;
            }
            list1.push_back(elem);
         }
         //Read as much as possible and note if got all
         while(list2.size() < buf_size) {
            T elem;
            if(!(in2 >> elem)){
               l2success = true;
               break;
            }
            if(elem > list1.back() && end!=-1) {
               end = list2.size();
            }
            list2.push_back(elem);
         }
         if(end == -1){
            end = list2.size();
         }
         //Read as much as possible and not when became greater than the largest in list1
         std::vector<T> output;
         while(1) {
            T min;
            if(list1.front() < list2.front()){ 
               min = list1.front();
               list1.erase(list1.begin());
            }
            else {
               min = list2.front();
               list2.erase(list2.begin());
            }
            output.push_back(min);
            if(list1.size()==0 || list2.size()==0) {
               leftover1 = list1;
               leftover2 = list2;
               //CREATE LEFTOVERS
               break;
            }
         }
         //merge as until greater than last element in list1
         for(unsigned int y=0;y<output.size();y++){
            out << " ";
            out << output[y];
         }
         //write the information to a file
         //Pull out leftover data for next iteration
         if(l1success && l2success) {
            for(unsigned int y=0;y<leftover1.size();y++){
               out << " ";
               out << leftover1[y];
           }
            for(unsigned int y=0;y<leftover2.size();y++){
               out << " ";
               out << leftover2[y];
           }
            break;
         }
         //if both are successfull then be happy!
      }
      retval.push_back(filename);
      counter++;
      //Add to list of newly merged files
      //Update how many files are in the merged list
   }
   return retval;
}

template <class T> //The list of files you want to merge, buffer_size is the largest vector/array/list you can have
std::string merge(std::vector<std::string> list_to_merge,size_t buf_siz){
   int x=0;
   while(list_to_merge.size()>1){
      list_to_merge = private_merge<T>(list_to_merge,buf_siz);
      x++;
   }
   return list_to_merge[0];
}

#endif

