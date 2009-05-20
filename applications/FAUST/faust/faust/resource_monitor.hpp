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

#include <faust/faust/exception.hpp>
#include <faust/faust/object.hpp>
#include <faust/faust/defines.hpp>

#include <faust/impl/resource_monitor_impl.hpp>

namespace faust
{    
  // fwd. decl. implementation class // 
  ///@cond - exclude from Doxygen
  namespace agent {class app; namespace monitor { class monitor_group; class monitor; }}
  namespace impl { class resource_monitor; class resource; }
  ///@endcond - exclude from Doxygen
  
  namespace attributes 
  {
    namespace resource_monitor 
    {
      /*! \brief  Directory identifier. */
      char const* const dir_id              = "dir_id"; 
      
      /*! \brief  Directory path.*/
      char const* const dir_path            = "dir_path";

      /*! \brief  Last update of the directory informations.*/
      char const* const dir_last_update     = "dir_last_update";
      
      /*! \brief  Total space available on the storage device (in kB). */
      char const* const dir_dev_space_total = "dir_dev_space_total";
      
      /*! \brief  Space currently used on the storage device (in kB).*/
      char const* const dir_dev_space_used  = "dir_dev_space_used";
      
      /*! \brief  Total availalbe quota space in this directory (in kB). */
      char const* const dir_quota_total     = "dir_quota_total";
      
      /*! \brief   Quota space currently used in this directory (in kB)*/
      char const* const dir_quota_used      = "dir_quota_used";
      
      /*! \brief  Queue ideentifier. */
      char const* const queue_id            = "queue_id";
      
      /*! \brief  Queue name.*/
      char const* const queue_name          = "queue_name";
      
      /*! \brief  Last update of the queue informations.*/
      char const* const queue_last_update   = "queue_last_update";
      
      /*! \brief  Number of total nodes assigned to this queue.*/
      char const* const queue_nodes_total   = "queue_nodes_total";
      
      /*! \brief  Number of nodes that are currently queued.*/
      char const* const queue_nodes_queued  = "queue_nodes_queued";
      
      /*! \brief  Number of nodes that are currently busy.*/
      char const* const queue_nodes_busy    = "queue_nodes_busy";
      
      /*! \brief  Number of nodes that are currently down.*/
      char const* const queue_nodes_down    = "queue_nodes_down";
      
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
    friend class faust::agent::monitor::monitor;
    friend class faust::agent::monitor::monitor_group;
    
    boost::shared_ptr <faust::impl::resource_monitor> get_impl (void) const;
    
    void setupAttributes();
    resource_monitor() ;
    
  public:
		
		/// @cond - hide from Doxygen
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
    
    /*! \brief Returns the value of the vector %attribute identified by 'key'.
     *
     */
    std::vector<std::string> get_vector_attribute(std::string key) const
    {
      return saga::detail::attribute<faust::resource_monitor> ::get_vector_attribute(key);
    }
    
    /*! \brief Returns a list of all defined attribute key.
     *
     */
    std::vector<std::string> list_attributes() const;     
    
  };
}

#endif /* FAUST_RESOURCE_MONITOR_HPP */
