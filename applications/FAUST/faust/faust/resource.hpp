/*
 *  resource.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 02/13/09.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_RESOURCE_HPP
#define FAUST_RESOURCE_HPP

#include <boost/shared_ptr.hpp>

#include <faust/faust/object.hpp>
#include <faust/faust/exports.hpp>

#include <faust/faust/resource_monitor.hpp>
#include <faust/faust/resource_description.hpp>

namespace faust
{
  ///@cond - exclude from Doxygen
  // fwd. decl. implementation class 
  namespace impl { class resource; } 
  ///@endcond - exclude from Doxygen

    class  resource : public faust::object
    {
    private:
			resource() {}; // No default constructor
			boost::shared_ptr <faust::impl::resource> get_impl (void) const;
			
    public:
			/*! \brief Creates a new %resource %object for a pysical %resource
			 *         described by a %resource_description.
			 */
      explicit resource(faust::resource_description RD);
						
			///@cond - exclude from Doxygen
      ~resource();
			///@endcond - exclude from Doxygen
      
			/*! \brief Returns the resource_description object that was used to 
			 *         construct this %resource instance.
			 */ 
      faust::resource_description get_description();

			/*! \brief Returns the resource_monitor object that contains informations
			 *         about the physical %resource this %resource instance is 
			 *	   		 associated with.
			 */ 
			faust::resource_monitor get_monitor();      
    };
}

#endif /* FAUST_RESOURCE_HPP */
