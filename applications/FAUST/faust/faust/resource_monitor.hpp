/*
 *  resource_monitor.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 02/13/09.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_RESOURCE_MONITOR_HPP
#define FAUST_RESOURCE_MONITOR_HPP

#include <boost/shared_ptr.hpp>

#include <saga/saga.hpp>

#include <faust/faust/object.hpp>
#include <faust/faust/defines.hpp>

namespace faust
{    
  // fwd. decl. implementation class // 
  ///@cond - exclude from Doxygen
  namespace agent {class app; }
  namespace impl { class resource_monitor; class resource; }
  ///@endcond - exclude from Doxygen
  
  namespace attributes 
  {
    namespace resource_monitor 
    {
      /*! \brief  */
      char const* const dir_id              = "dir_id"; 
      
      /*! \brief  */
      char const* const dir_path            = "dir_path";
      
      /*! \brief  */
      char const* const dir_dev_space_total = "dir_dev_space_total";
      
      /*! \brief  */
      char const* const dir_dev_space_used  = "dir_dev_space_used";
      
      /*! \brief  */
      char const* const dir_dev_space_free  = "dir_dev_space_free";
      
      /*! \brief  */
      char const* const dir_quota_total     = "dir_quota_total";
      
      /*! \brief  */
      char const* const dir_quota_used      = "dir_quota_used";

      /*! \brief  */
      char const* const dir_quota_free      = "dir_quota_free";
      
      /*! \brief  */
      char const* const queue_id            = "queue_id";
      
      /*! \brief  */
      char const* const queue_name          = "queue_name";
      
      /*! \brief  */
      char const* const queue_nodes_total   = "queue_nodes_total";

      /*! \brief  */
      char const* const queue_nodes_used    = "queue_nodes_used";
      
      /*! \brief  */
      char const* const queue_nodes_free    = "queue_nodes_free";
    }
  }
  
  /*! \brief The %resource_monitor encapsulates all the attributes which define a 
   *         physical %resource_monitor. It has no methods of its own, but implements the 
   *         saga::attributes interface.
   *
   */
  class resource_monitor : public faust::object,
  public saga::detail::attribute<faust::resource_monitor>
  {

  private: 

    friend struct saga::detail::attribute<faust::resource_monitor>;
    friend class faust::impl::resource_monitor;
		friend class faust::impl::resource;
    friend class faust::agent::app;
    
    void setupAttributes();
    resource_monitor() {} ;

  public:
		
		/// @cond - hide from Doxygen
    boost::shared_ptr <faust::impl::resource_monitor> get_impl (void) const;
    typedef faust::impl::object implementation_base_type;
		/// @endcond

		/*! \brief D'TOR: Properly destroys this %object instance. 
     *
     */
    ~resource_monitor();
    
    /*! \brief Returns TRUE if the %attribute identified by 'key' exists. FALSE otherwise.
     *
     */
    bool attribute_exists(std::string key) const 
    {
      return saga::detail::attribute<faust::resource_monitor> ::attribute_exists(key); 
    }
    
    /*! \brief Returns TRUE if the %attribute identified by 'key' is a vector %attribute. FALSE otherwise. 
     *
     */
    bool attribute_is_vector(std::string key) const 
    {
      return saga::detail::attribute<faust::resource_monitor> ::attribute_is_vector(key); 
    }
    
    /*! \brief Returns the value of the scalar %attribute identified by 'key'.
     *
     */
    std::string get_attribute(std::string key) const
    {
      return saga::detail::attribute<faust::resource_monitor> ::get_attribute(key);
    }
    
    /// @cond 
    void set_attribute(std::string key, std::string value) 
    {
      // THROW EXCEPTION
    }
    /// @endcond
    
    /*! \brief Returns the value of the vector %attribute identified by 'key'.
     *
     */
    std::vector<std::string> get_vector_attribute(std::string key) const
    {
      return saga::detail::attribute<faust::resource_monitor> ::get_vector_attribute(key);
    }
    
    /// @cond
    void set_vector_attribute(std::string key, std::vector<std::string> value) 
    {
      // THROW EXCEPTION
    }
    /// @endcond
    
    /*! \brief Returns a list of all defined attribute key.
     *
     */
    //std::vector<std::string> list_attributes() const 
    //{
    //  return saga::detail::attribute<faust::resource_monitor> ::list_attributes(); 
    //}
    
    
  };
}

#endif /* FAUST_RESOURCE_MONITOR_HPP */
