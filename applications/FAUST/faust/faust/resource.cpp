/*
 *  resource.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 02/13/098.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/faust/resource.hpp>
#include <faust/impl/resource_impl.hpp>

using namespace faust;

////////////////////////////////////////////////////////////////////////////////
//
boost::shared_ptr <faust::impl::resource> resource::get_impl (void) const
{ 
  typedef faust::object base_type;
  return boost::static_pointer_cast <faust::impl::resource> (
                                                        this->base_type::get_impl ());
}

////////////////////////////////////////////////////////////////////////////////
//
resource::resource(std::string resource_id) 
: faust::object (new faust::impl::resource(resource_id), object::Resource)
{
}

////////////////////////////////////////////////////////////////////////////////
//
resource::resource(faust::resource_description resource_desc, bool persistent) 
: faust::object (new faust::impl::resource(resource_desc, persistent), 
                 object::Resource)
{
}

////////////////////////////////////////////////////////////////////////////////
//
resource::~resource() 
{
}

////////////////////////////////////////////////////////////////////////////////
//
faust::resource_description resource::get_description()
{
  return get_impl()->get_description();
}

////////////////////////////////////////////////////////////////////////////////
//
faust::resource_monitor resource::get_monitor()
{
	return get_impl()->get_monitor();
}

////////////////////////////////////////////////////////////////////////////////
//
bool resource::is_persistent(void) 
{
  return get_impl()->is_persistent();
}

////////////////////////////////////////////////////////////////////////////////
//
void resource::set_persistent(bool yesno)
{
  get_impl()->set_persistent(yesno);
}
