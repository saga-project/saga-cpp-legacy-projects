/*
 *  resource.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/22/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_RESOURCE_HPP
#define FAUST_RESOURCE_HPP

namespace faust
{    
	namespace attributes 
	{
		namespace resource {
			/*! \brief FAUST SPECIFIC: */
			char const* const contact     = "Contact";
			/*! \brief FAUST SPECIFIC: */
			char const* const workdir    = "Workdir";
		}
		
	}
	
	class resource // : public saga::job::resource
	{
	public:
		
		/*! \brief Creates a new %resource instance.
		 *
		 */
		resource();
		
		/*! \brief Creates a new %resource instance from an %resourceXML file 
		 *         stored at the location described by the URL parameter. 
		 *
		 */
		resource(std::string url);
		
		/*! \brief Destroys this %description instance.
		 *
		 */
		~resource();
		
	};
}





#endif /* FAUST_RESOURCE_HPP */