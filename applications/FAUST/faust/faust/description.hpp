/*
 *  description.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
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
  namespace impl { class description; }
  ///@endcond - exclude from Doxygen
  
  namespace attributes 
  {
    namespace description {
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
  class description : public faust::object,
                      public saga::detail::attribute<faust::description>
  {
    
  protected:
    /// @cond
    /** These methods are not within API scope */
    friend struct saga::detail::attribute<faust::description>;
    friend class faust::impl::description;
    /// @endcond
    
  private:
    
    boost::shared_ptr <faust::impl::description> get_impl (void) const;
    void setupAttributes();
    
  public:
    
    /*! \brief Creates a new %description instance.
     *
     */
    description();
    
    /*! \brief Creates a new %description instance from an XML file
     *
     */    
    description(std::string XMLFile);
    
    /*! \brief Destroys this %description instance.
     *
     */
    ~description();
  };
}

#endif /* FAUST_DESCRIPTION_HPP */
