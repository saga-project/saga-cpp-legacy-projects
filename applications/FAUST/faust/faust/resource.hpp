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
  
  /*! \brief Class description
   *
   */
  class  resource : public faust::object
  {
  private:
    resource() {}; // No default constructor
    boost::shared_ptr <faust::impl::resource> get_impl (void) const;
    
  public:
    /*! \brief Creates a new %resource %object representing a pysical %resource
     *         described by the %resource_description (RD) argument.
     */
    explicit resource(faust::resource_description resource_desc, bool persistent=true);
    
    /*! \brief Creates a new %resource %object that reconnects to an existing
     *         %resource in the database identified by resource_id argument.
     */
    explicit resource(std::string resource_id);
    
    /*! \brief Shuts down the faust agent associated with this %resource and 
		 *         destroys this %object.
     *
     */
    ~resource();
    
    /*! \brief Returns the resource_description %object that was used to 
     *         construct this %resource instance.
     */ 
    faust::resource_description get_description();
    
    /*! \brief Returns the resource_monitor %object that contains informations
     *         about the physical %resource this %resource %object is 
     *	   		 associated with.
     */ 
    faust::resource_monitor get_monitor();   
    
    bool is_persistent(void);
    
    void set_persistent(bool yesno);
  };
}

#endif /* FAUST_RESOURCE_HPP */
