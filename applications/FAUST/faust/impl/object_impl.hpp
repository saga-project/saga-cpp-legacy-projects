/*
 *  object_impl.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 12/16/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_IMPL_OBJECT_HPP
#define FAUST_IMPL_OBJECT_HPP

#include <saga/saga.hpp>

#include <boost/noncopyable.hpp>

#include <faust/faust/object.hpp>
#include <faust/impl/logwriter.hpp>

namespace faust 
{
  namespace impl 
  {
    //////////////////////////////////////////////////////////////////////////
    //
    class object : private boost::noncopyable, 
    public boost::enable_shared_from_this <object>
    {    
      
    private:
      
      faust::object::type type_;
      std::string uuid_;
      
      static bool faust_initialized_;
      static void initialize_faust();
      
      static boost::shared_ptr <faust::detail::logwriter> log_sptr_;
      
    protected:
      
      static std::string faust_root_namesapce_;
      
    public:
      
      object (faust::object::type type);
      virtual ~object (void) { /*delete log_;*/ }
      
      faust::object::type get_type() const;
      std::string get_uuid() {return uuid_;}
      
      virtual saga::impl::attribute* get_attributes();
      virtual saga::impl::attribute const* get_attributes() const;
      
      boost::shared_ptr <faust::detail::logwriter> get_log() { return log_sptr_; }
      
    };
    //
    //////////////////////////////////////////////////////////////////////////
  }
}

#endif /* FAUST_IMPL_OBJECT_HPP */

