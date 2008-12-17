/*
 *  object.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 12/16/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_IMPL_OBJECT_HPP
#define FAUST_IMPL_OBJECT_HPP

#include <boost/noncopyable.hpp>

#include <faust/faust/object.hpp>

namespace faust {
  
  namespace impl {
    
    ////////////////////////////////////////////////////////////////////////////
    //
    class object : private boost::noncopyable, 
                   public boost::enable_shared_from_this <object>
      {    
        
      private:
        
        faust::object::type type_;
        
      public:
        
        object (faust::object::type type);
        virtual ~object (void) {}
        
        faust::object::type get_type() const;
        
      };
  }
  
}

#endif /* FAUST_IMPL_OBJECT_HPP */