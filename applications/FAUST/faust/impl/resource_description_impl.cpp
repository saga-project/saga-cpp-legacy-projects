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

#include <faust/faust/exception.hpp>
#include <faust/impl/resource_description_impl.hpp>

using namespace faust::impl;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
resource_description::resource_description(boost::shared_ptr <faust::detail::logwriter> log_sptr,
                                           saga::advert::entry & desc_adv)
: object(faust::object::ResourceDescription), log_sptr_(log_sptr), desc_adv_(desc_adv)
{
}

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
resource_description::resource_description(std::string XMLFileName)
: object(faust::object::ResourceDescription), xmlfilename_(XMLFileName)
{
}
