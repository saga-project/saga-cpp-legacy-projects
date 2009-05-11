/*
 *  resource_monitor_impl.cpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 02/13/09.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/faust/exception.hpp>
#include <faust/impl/resource_monitor_impl.hpp>

using namespace faust::impl;

////////////////////////////////////////////////////////////////////////////////
// 
resource_monitor::resource_monitor() : object(faust::object::ResourceMonitor)
{
  
}


////////////////////////////////////////////////////////////////////////////////
// 
resource_monitor::resource_monitor(saga::advert::entry & monitor_adv)

: object(faust::object::ResourceMonitor),  monitor_adv_(monitor_adv)
{

}


