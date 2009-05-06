/*
 *  resource_description.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/22/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_RESOURCE_DESCRIPTION_HPP
#define FAUST_RESOURCE_DESCRIPTION_HPP

#include <boost/shared_ptr.hpp>

#include <saga/saga.hpp>

#include <faust/faust/object.hpp>
#include <faust/faust/defines.hpp>

namespace faust
{    
  // fwd. decl. implementation class // 
  ///@cond - exclude from Doxygen
  namespace agent {class app; }
  namespace impl { class resource_description; }
  ///@endcond - exclude from Doxygen
  
  namespace attributes 
  {
    namespace resource_description 
    {
      /*! \brief Unique identifier for this resource - usually the hostnameb
       *         <br>(example: <code>queenbee.loni.org</code>) */
      char const* const identifier              = "identifier";

      /*! \brief URL for the resource manager that executes the agent
       *         <br>(example: <code>gram://gg101.cct.lsu.edu/jobamanger-fork</code>) */
      char const* const faust_agent_submit_url  = "faust_agent_submit_url";

			/*! \brief Path to the faust_agent executable
       *         <br>(example: <code>/usr/local/faust/bin/faust_agent</code>) */
      char const* const faust_agent_binary_path = "faust_agent_binary_path";

			/*! \brief Path to the SAGA installation
       *         <br>(example: <code>/usr/local/saga-1.1/</code>) */
      char const* const saga_root_path          = "saga_root_path";

      /*! \brief (VECTOR) Environment variables to be set 
       *         <br>(example: <code>/usr/local/saga-1.1/</code>) */
      char const* const environment             = "environment";
      
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
       *         <br>(example: <code>qfree | awk '/workq/ {print $6}' | tr -d ,</code>)*/
      char const* const queue_nodes_free_cmd    = "queue_nodes_free_cmd";
    }
  }
  
  /*! \brief The %resource_description encapsulates all the attributes which define a 
   *         physical %resource_description. It has no methods of its own, but implements the 
   *         saga::attributes interface.
   *
   */
  class resource_description : public faust::object,
  public saga::detail::attribute<faust::resource_description>
  {
    
  private:

    friend struct saga::detail::attribute<faust::resource_description>;
    friend class faust::impl::resource_description;
    
    void setupAttributes();
    
  public:

		/// @cond - hide from Doxygen
    boost::shared_ptr <faust::impl::resource_description> get_impl (void) const;
    typedef faust::impl::object implementation_base_type;
		/// @endcond
		
    /*! \brief Creates a new %resource_description for a physical %resource.
     *
     */
    resource_description();
    
    /*! \brief Destroys this %object.
     *
     */
    ~resource_description();
		
  };
}

#endif /* FAUST_RESOURCE_DESCRIPTION_HPP */
