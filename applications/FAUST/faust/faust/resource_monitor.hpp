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
  namespace impl { class resource_monitor; class resource; }
  ///@endcond - exclude from Doxygen
  
  namespace attributes 
  {
    namespace resource_monitor 
    {
      /*! \brief Your own identifier for this directory
       *         <br>(example: <code>mydir1</code>) */
      char const* const dir_id                  = "dir_id"; 
      
      /*! \brief Path to the directory
       *         <br>(example: <code>/scratch/</code>) */
      char const* const dir_path                = "dir_path";
      
      /*! \brief Command to retrieve the amount of total space on this device (in kB)
       *         <br>(example: <code>df . | awk '/\// {print $2}'</code>) */
      char const* const dir_dev_space_total_cmd = "dir_dev_space_total_cmd";
      
      /*! \brief Command to retrieve the amount of used space on this device (in kB)
       *         <br>(example: <code>df . | awk '/\// {print $3}'</code>) */
      char const* const dir_dev_space_used_cmd  = "dir_dev_space_used_cmd";
      
      /*! \brief Command to retrieve the amount of total quota in this directory (in kB)
       *         <br>(example: <code>quota | awk '/home/ {print $4}'</code>) */
      char const* const dir_quota_total_cmd     = "dir_quota_total_cmd";
      
      /*! \brief Command to retrieve the amount of used quota in this directory (in kB)
       *         <br>(example: <code>quota | awk '/home/ {print $2}'</code>) */
      char const* const dir_quota_used_cmd      = "dir_quota_used_cmd";
      
      /*! \brief Your own identifier for this queue
       *         <br>(example: <code>queue1</code>) */
      char const* const queue_id                = "queue_id";
      
      /*! \brief Name of the queue
       *         <br>(example: <code>workq</code>) */
      char const* const queue_name              = "queue_name";
      
      /*! \brief Command to retrieve the total number of nodes for this queue
       *         <br>(example: <code>qfree | awk '/workq/ {print $4}' | tr -d ,</code>)*/
      char const* const queue_nodes_total_cmd   = "queue_nodes_total_cmd";
      
      /*! \brief Command to retrieve the number of currently free nodes for this queue
       *         <br>(example: <code>qfree | awk '/workq/ {print $6}</code>' | tr -d ,)*/
      char const* const queue_nodes_free_cmd    = "queue_nodes_free_cmd";
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
		
		// service impl. class needs to be friend to call private c'tor 
		friend class faust::impl::resource;
    
  protected:
    /// @cond
    /** These methods are not within API scope */
    friend struct saga::detail::attribute<faust::resource_monitor>;
    friend class faust::impl::resource_monitor;
    /// @endcond
		resource_monitor(); // No default constructor
    
  private:
    
    boost::shared_ptr <faust::impl::resource_monitor> get_impl (void) const;
    void setupAttributes();

  public:
    typedef faust::impl::object implementation_base_type;

    ~resource_monitor();
    
  };
}

#endif /* FAUST_RESOURCE_MONITOR_HPP */
