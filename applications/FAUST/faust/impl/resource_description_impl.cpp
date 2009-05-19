/*
 *  resource_description_impl.cpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/30/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <iostream>
#include <fstream>

#include <faust/faust/exception.hpp>
#include <faust/impl/resource_description_impl.hpp>

using namespace faust::impl;

//////////////////////////////////////////////////////////////////////////
//
namespace {
  
  inline void tokenize(const std::string& str,
                       std::vector<std::string>& tokens,
                       const std::string& delimiters = " ")
  {
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);
    
    while (std::string::npos != pos || std::string::npos != lastPos)
    {
      // Found a token, add it to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, lastPos);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
resource_description::resource_description()
: object(faust::object::ResourceDescription)
{
  
}


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
resource_description::resource_description(saga::advert::entry & desc_adv)
: object(faust::object::ResourceDescription), desc_adv_(desc_adv)
{
  
}


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
resource_description::resource_description(std::string filename)
: object(faust::object::ResourceDescription)
{
  
}

////////////////////////////////////////////////////////////////////////////////
//
void resource_description::read_from_file(std::string filename)
{
  using namespace std;
  
  typedef std::map<std::string, std::vector<std::string> > vector_map_t;
  typedef std::map<std::string, std::vector<std::string> >::iterator vector_map_it_t;
  
  typedef std::map<std::string, std::string> scalar_map_t;
  typedef std::map<std::string, std::string>::iterator scalar_map_it_t;
  
  SAGA_OSSTREAM strm;
  strm << "Reading resource description from file: " << filename << " ";
  
  ifstream infile;
  infile.open(filename.c_str(), ios::in);
  
  if(!infile.is_open())
  {
    LOG_WRITE_FAILED_AND_THROW_2(get_log(), strm, "Could not open file.", faust::NoSuccess);
  }
  else
  {
    try
    {
      string line;
      vector_map_t vector_map;
      scalar_map_t scalar_map;
      
      while(! infile.eof() )
      {
        getline(infile, line);
        
        vector<string> tokens;
        tokenize(line, tokens, "\t");
        
        if(tokens.size() != 4) continue;
        
        else
        {
          if(tokens.at(0) == "V") 
          {
            vector_map_it_t it = vector_map.find(tokens.at(2));
            if(it == vector_map.end())
            {
              std::vector<std::string> val;
              val.push_back(tokens.at(3));
              vector_map.insert( pair<std::string, std::vector<std::string> >(tokens.at(2), val) );
            }
            else
            {
              ((*it).second).push_back(tokens.at(3));
            }
          }
          else if(tokens.at(0) == "S")
          {
            std::string val = tokens.at(3);
            scalar_map.insert( pair<std::string, std::string>(tokens.at(2), val) );
          }
          else
          {
            continue;
          }
        }
      } // while
      
      vector_map_it_t vec_it;
      for(vec_it = vector_map.begin(); vec_it != vector_map.end(); ++vec_it)
      {
        attributes_.set_vector_attribute((*vec_it).first, (*vec_it).second);
      }
      
      scalar_map_it_t scal_it;
      for(scal_it = scalar_map.begin(); scal_it != scalar_map.end(); ++ scal_it)
      {
        attributes_.set_attribute((*scal_it).first, (*scal_it).second);
      }
      LOG_WRITE_SUCCESS_2(get_log(), strm);
    }
    catch(saga::exception const & e)
    {
      LOG_WRITE_FAILED_AND_THROW_2(get_log(), strm, e.what(), faust::NoSuccess);
    }
  }
}  

////////////////////////////////////////////////////////////////////////////////
//
void resource_description::write_to_file(std::string filename)
{
  using namespace std;
  
  SAGA_OSSTREAM strm;
  strm << "Writing resource description to file: " << filename << " ";
  
  ofstream outfile;
  outfile.open(filename.c_str(), ios::out | ios::trunc);
  
  if(outfile.is_open())
  {
    /* file format: type(S/V) \t index(0..n) \t key \t value */
    
    std::vector<std::string> attribs = attributes_.list_attributes();
    std::vector<std::string>::const_iterator it;
    
    for(it = attribs.begin(); it != attribs.end(); ++it)
    {
      if((*it) == "utime" || (*it) == "ctime" || (*it) == "persistent")
        continue;
      
      if(attributes_.attribute_is_vector(*it)) 
      {
        unsigned int va_index = 0;
        std::vector<std::string> va = attributes_.get_vector_attribute(*it);
        std::vector<std::string>::const_iterator va_it;
        
        for(va_it = va.begin(); va_it != va.end(); ++va_it)
        {
          outfile << "V" << "\t" << va_index << "\t" 
          << (*it) << "\t" << (*va_it) << std::endl; 
          ++va_index;
        }
      }
      else 
      {
        outfile << "S" << "\t" << "0" << "\t" 
        << (*it) << "\t" << attributes_.get_attribute(*it) << std::endl;
      }
    }
    LOG_WRITE_SUCCESS_2(get_log(), strm);
  }
  else
  {
    LOG_WRITE_FAILED_AND_THROW_2(get_log(), strm, "Could not open file.", faust::NoSuccess);
  }
}
