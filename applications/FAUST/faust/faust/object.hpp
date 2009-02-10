/*
 *  object.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/29/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
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
  
  // fwd. decl. implementation class // 
  ///@cond - exclude from Doxygen
  namespace impl { class object ; }
  ///@endcond - exclude from Doxygen
  
  /////////////////////////////////////////////////////////////////////////////
  //
  class object 
  {    
      
  protected:
    
    typedef boost::shared_ptr<faust::impl::object> impl_ptr;
    impl_ptr impl_;
    
    boost::shared_ptr <faust::impl::object> get_impl (void) const
    { 
      return impl_;
    }
    
  public:
    
    enum type 
    {
      Unknown     =   -1,
      Service     =    1,
      Job         =    2,
      JobGroup    =    4,
      Description =    5,
      Resource    =    6
    };
    
    explicit object() {};
    explicit object(faust::impl::object *obj, faust::object::type tp);
    explicit object (boost::shared_ptr<faust::impl::object> init, faust::object::type tp);
  
    faust::object::type get_type() const;
        
  private:
    
    faust::object::type type_;
    
  };
  
  std::string get_object_type_name(faust::object t);

}

#endif /* FAUST_OBJECT_HPP */