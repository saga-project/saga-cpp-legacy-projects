/*
 *  serialize.hpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 05/07/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_IMPL_DETAIL_SERIALIZE_HPP
#define FAUST_IMPL_DETAIL_SERIALIZE_HPP

#include <iostream>
#include <fstream>

#include <saga/saga.hpp>

#include <faust/impl/logwriter.hpp>
#include <faust/faust/exception.hpp>

namespace faust { namespace impl { namespace detail {
  
  //////////////////////////////////////////////////////////////////////////////
  //
  template<class T> void writeAttributesToFile(T obj, std::string filename)//,
                                             //  boost::shared_ptr <faust::detail::logwriter> log_sptr_)
  {
    using namespace std;
    
    SAGA_OSSTREAM strm;
    strm << "Writing attributes to file: " << filename << " ";
    
    ofstream outfile;
    outfile.open(filename, ios::out | ios::trunc);
    
    if(outfile.is_open())
    {
      /* file format: type(S/V) \t index(0..n) \t key \t value */
      
      std::vector<std::string> attribs = obj.list_attributes();
      std::vector<std::string>::const_iterator it;
      
      for(it = attribs.begin(); it != attribs.end(); ++it)
      {
        if((*it) == "utime" || (*it) == "ctime" || (*it) == "persistent")
          continue;
        
        if(obj.attribute_is_vector(*it)) 
        {
          unsigned int va_index = 0;
          std::vector<std::string> va = obj.get_vector_attribute(*it);
          std::vector<std::string>::const_iterator va_it;
          
          for(va_it = va.begin(); va_it != va.end(); ++it)
          {
            outfile << "V" << "\t" << va_index << "\t" 
            << (*it) << "\t" << (*va_it) << std::endl; 
          }
        }
        else 
        {
          outfile << "S" << "\t" << "0" << "\t" 
          << (*it) << "\t" << obj.get_attribute(*it) << std::endl;
        }
      }
      //LOG_WRITE_SUCCESS(strm);
    }
    else
    {
      //LOG_WRITE_FAILED_AND_THROW(strm, "Could not open file.", faust::NoSuccess);
    }
    
  }
  
  //////////////////////////////////////////////////////////////////////////////
  //
  template<class T> void readAttributesFromFile(T obj, std::string filename,
                                                boost::shared_ptr <faust::detail::logwriter> lw)
  {
    
  }
  
  //////////////////////////////////////////////////////////////////////////////
  //
  template<class T> void writeAttributesToDB(T obj, std::string name, saga::advert::entry ae,
                                             boost::shared_ptr <faust::detail::logwriter> log_sptr_, std::string key="")
  {
    SAGA_OSSTREAM strm;
    strm << "Writing " << name << " attributes to database. " ;
    
    try 
    {
      if( key.empty() )
      {
        std::vector<std::string> attribs = obj.list_attributes();
        std::vector<std::string>::const_iterator it;
        for(it = attribs.begin(); it != attribs.end(); ++it)
        {
          if((*it) == "utime" || (*it) == "ctime" || (*it) == "persistent")
            continue;
          
          if(obj.attribute_is_vector(*it)) {
            ae.set_vector_attribute((*it), obj.get_vector_attribute((*it)));
          }
          else {
            ae.set_attribute((*it), obj.get_attribute((*it)));
          }
        }
      }
      else
      {
        if(obj.attribute_is_vector(key))
        {
          ae.set_vector_attribute(key, obj.get_vector_attribute(key));
        }
        else
        {
          ae.set_attribute(key, obj.get_attribute(key));
        }
      }
      LOG_WRITE_SUCCESS(strm);
    }
    catch(saga::exception const & e) 
    {
      LOG_WRITE_FAILED_AND_THROW(strm, e.what(), faust::NoSuccess);
    }    
  };
  
  //////////////////////////////////////////////////////////////////////////////
  //
  template<class T> void readAttributesFromDB(T obj, std::string name, saga::advert::entry ae,
                                              boost::shared_ptr <faust::detail::logwriter> log_sptr_, std::string key="")
  {
    SAGA_OSSTREAM strm;
    strm << "Reading " << name << " attributes from database. ";
    
    try 
    {
      if( key.empty() )
      {
        std::vector<std::string> attribs = ae.list_attributes();
        std::vector<std::string>::const_iterator it;
        for(it = attribs.begin(); it != attribs.end(); ++it)
        {
          if((*it) == "utime" || (*it) == "ctime" || (*it) == "persistent")
            continue;
          
          if(ae.attribute_is_vector(*it)) {
            obj.set_vector_attribute((*it), ae.get_vector_attribute((*it)));
          }
          else {
            obj.set_attribute((*it), ae.get_attribute((*it)));
          }
        }
      }
      else
      {
        if(ae.attribute_is_vector(key))
        {
          obj.set_vector_attribute(key, ae.get_vector_attribute(key));
        }
        else
        {
          obj.set_attribute(key, ae.get_attribute(key));
        }
      }
      LOG_WRITE_SUCCESS(strm);
    }
    catch(saga::exception const & e) 
    {
      LOG_WRITE_FAILED_AND_THROW(strm, e.what(), faust::NoSuccess);
    }  
  };
  
}}}; // namespace faust::impl::detail

#endif
