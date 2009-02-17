/*
 *  description.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/22/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_DESCRIPTION_HPP
#define FAUST_DESCRIPTION_HPP

#include <boost/shared_ptr.hpp>

#include <saga/saga.hpp>

#include <faust/faust/object.hpp>
#include <faust/faust/defines.hpp>

namespace faust
{    
  // fwd. decl. implementation class // 
  ///@cond - exclude from Doxygen
  namespace impl { class job_description; }
  ///@endcond - exclude from Doxygen
  
  namespace attributes 
  {
    namespace job_description {
    /*! \brief FAUST SPECIFIC: */
    char const* const desc01     = "desc01";
    /*! \brief FAUST SPECIFIC: */
    char const* const desc02    = "desc02";
    }
  }
  
  
  /*! \brief The %description encapsulates all the attributes which define a 
   *         %job. It has no methods of its own, but implements the 
   *         saga::attributes interface.
   *
   */
  class job_description : public faust::object,
                      public saga::detail::attribute<faust::job_description>
  {
    
  protected:
    /// @cond
    /** These methods are not within API scope */
    friend struct saga::detail::attribute<faust::job_description>;
    friend class faust::impl::job_description;
    /// @endcond
    
  private:
    
    boost::shared_ptr <faust::impl::job_description> get_impl (void) const;
    void setupAttributes();
    
  public:
    
    typedef faust::impl::object implementation_base_type;

    
    /*! \brief Creates a new %description instance.
     *
     */
    job_description();
    
    /*! \brief Creates a new %description instance from an XML file
     *
     */    
    job_description(std::string XMLFile);
    
    /*! \brief Destroys this %description instance.
     *
     */
    ~job_description();
  };
}

#endif /* FAUST_DESCRIPTION_HPP */
