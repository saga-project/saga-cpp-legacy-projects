/*
 *  resource.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/22/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_RESOURCE_HPP
#define FAUST_RESOURCE_HPP

#include <boost/shared_ptr.hpp>

#include <saga/saga.hpp>

#include <faust/faust/object.hpp>
#include <faust/faust/defines.hpp>

namespace faust
{    
  // fwd. decl. implementation class // 
  ///@cond - exclude from Doxygen
  namespace impl { class resource; }
  ///@endcond - exclude from Doxygen
  
  namespace attributes 
  {
    namespace resource {
      /*! \brief FAUST SPECIFIC: */
      char const* const desc01     = "desc01";
      char const* const desc02     = "desc02";
    }
  }
  
  /*! \brief The %description encapsulates all the attributes which define a 
   *         %job. It has no methods of its own, but implements the 
   *         saga::attributes interface.
   *
   */
  class resource : public faust::object,
  public saga::detail::attribute<faust::resource>
  {
    
  protected:
    /// @cond
    /** These methods are not within API scope */
    friend struct saga::detail::attribute<faust::resource>;
    friend class faust::impl::resource;
    /// @endcond
    
  private:
    
    boost::shared_ptr <faust::impl::resource> get_impl (void) const;
    
  public:
    
    /*! \brief Creates a new %description instance.
     *
     */
    resource();
    
    /*! \brief Destroys this %description instance.
     *
     */
    ~resource();
    
  };
}

#endif /* FAUST_DESCRIPTION_HPP */
