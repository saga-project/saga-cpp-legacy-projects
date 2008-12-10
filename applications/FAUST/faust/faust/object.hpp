/*
 *  object.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 11/29/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_OBJECT_HPP
#define FAUST_OBJECT_HPP

// include boost helper
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>


namespace faust {
  
  /////////////////////////////////////////////////////////////////////////////
  //
  class object 
  {    
  public:
    
    enum type {
      Unknown    =   -1,
      Service    =    1,
      Job        =    2,
      JobGroup   =    4
    };
    object() {}
    object(faust::object::type);
  
    ~object();
    
    faust::object::type get_type() const;
    
  private:
    faust::object::type type_;

  };
  
  std::string get_object_type_name(faust::object t);

}

#endif /* FAUST_OBJECT_HPP */