
#ifndef DIGGEDAG_NODE_HPP
#define DIGGEDAG_NODE_HPP

#include "node_impl.hpp"

namespace diggedag
{
  class node 
  {
    public:
      class description : public saga::job::description 
      {
        // FIXME: find a sensible and short-named way to predef attribs, in the
        // diggedag namespace
        //
        // namespace attributes
        // {
        //   char const * const description_executable          = "Executable";
        //   char const * const description_arguments           = "Arguments";
        //   char const * const description_environment         = "Environment";
        //   char const * const description_working_directory   = "WorkingDirectory";
        //   char const * const description_input               = "Input";
        //   char const * const description_output              = "Output";
        //   char const * const description_error               = "Error";
        //   char const * const description_candidate_hosts     = "CandidateHosts";
        //   char const * const description_job_project         = "JobProject";
        //   char const * const description_spmd_variation      = "SPMDVariation";
        //   char const * const description_total_cpu_count     = "TotalCPUCount";
        //   char const * const description_number_of_processes = "NumberOfProcesses";
        //   char const * const description_processes_per_host  = "ProcessesPerHost";
        //   char const * const description_threads_per_process = "ThreadsPerProcess";
        // }
      };

    private:
      util::shared_ptr <impl::node> impl_;
      util::shared_ptr <impl::node> get_impl (void) const { return impl_; } 

      std::string name_;

    public:
      node (const saga::job::description & jd, 
            const std::string              name)
        : impl_ (new impl::node (jd, name)), 
          name_ (name)
      {
      }

      node (const node & src)
        : impl_ (src.get_impl ()),
          name_ (src.get_name ())
      {
      }

      ~node (void)
      {
      }

      void add_edge_in (const edge & in)
      {
        impl_->add_edge_in (in);
      }

      void add_edge_out (const edge & out)
      {
        impl_->add_edge_out (out);
      }

      void fire (void)
      {
        impl_->fire ();
      }

      std::string get_name (void) const
      {
        return impl_->get_name ();
      }

      diggedag::state get_state (void) const
      {
        return impl_->get_state ();
      }
  };

} // namespace diggedag

#endif // DIGGEDAG_NODE_HPP

