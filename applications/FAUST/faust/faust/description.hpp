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
    char const* const input_data_volume     = "InputDataVolume";
    /*! \brief FAUST SPECIFIC: */
    char const* const output_data_volume    = "OutputDataVolume";
    
    
    /*! \brief Same as in saga::job::attributes */
    char const* const executable            = "Executable";
    /*! \brief Same as in saga::job::attributes */
    char const* const arguments             = "Arguments";
    /*! \brief Same as in saga::job::attributes */
    char const* const environment           = "Environment";
    /*! \brief Same as in saga::job::attributes */
    char const* const working_directory     = "WorkingDirectory";
    /*! \brief Same as in saga::job::attributes */
    char const* const file_transfer         = "FileTransfer";
    /*! \brief Same as in saga::job::attributes */
    char const* const cleanup               = "Cleanup";
    /*! \brief Same as in saga::job::attributes */
    char const* const job_start_time        = "JobStartTime";
    /*! \brief Same as in saga::job::attributes */
    char const* const total_cpu_time        = "TotalCPUTime";
    /*! \brief Same as in saga::job::attributes */
    char const* const wall_time_limit       = "WallTimeLimit";
    /*! \brief Same as in saga::job::attributes */
    char const* const total_physical_memory = "TotalPhysicalMemory";
    /*! \brief Same as in saga::job::attributes */
    char const* const cpu_architecture      = "CPUArchitecture";
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
    
  public:
    
    /*! \brief Creates a new %description instance.
     *
     */
    description();
    
    /*! \brief Destroys this %description instance.
     *
     */
    ~description();
    
    /*! \brief Sets the value for the attribute identified by key.
     *
     */
    //void set_attribute(std::string key, std::string value);
    
    /*! \brief Returns the value of the attribute identified by key.
     *
     */
    //std::string get_attribute(std::string key);
    
  };
}

#endif /* FAUST_DESCRIPTION_HPP */
