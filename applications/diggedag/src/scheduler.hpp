
#ifndef DIGGEDAG_SCHEDULER_HPP
#define DIGGEDAG_SCHEDULER_HPP

#include "util/shared_ptr.hpp"


namespace diggedag
{
  namespace impl
  {
    class scheduler;
  }

  class dag;
  class scheduler 
  {
    private:
      boost::shared_ptr <impl::scheduler> impl_;
      boost::shared_ptr <impl::scheduler> get_impl (void) const { return impl_; } 


    public:
      scheduler  (void);
      scheduler  (const scheduler & src);
      ~scheduler (void);

      void run   (diggedag::dag & d);

      void hook_node_add      (dag & d, node & n);
      void hook_node_add_in   (dag & d, node & n, edge & e);
      void hook_node_add_out  (dag & d, node & n, edge & e);
      void hook_node_del_in   (dag & d, node & n, edge & e);
      void hook_node_del_out  (dag & d, node & n, edge & e);
      void hook_node_run_pre  (dag & d, node & n);
      void hook_node_run_done (dag & d, node & n);
      void hook_node_run_fail (dag & d, node & n);
      void hook_node_remove   (dag & d, node & n);

      void hook_edge_add      (dag & d, edge & n);
      void hook_edge_add_src  (dag & d, edge & n, node & e);
      void hook_edge_add_tgt  (dag & d, edge & n, node & e);
      void hook_edge_del_src  (dag & d, edge & n, node & e);
      void hook_edge_del_tgt  (dag & d, edge & n, node & e);
      void hook_edge_run_pre  (dag & d, edge & n);
      void hook_edge_run_done (dag & d, edge & n);
      void hook_edge_run_fail (dag & d, edge & n);
      void hook_node_remove   (dag & d, edge & n);
  };

} // namespace diggedag

#endif // DIGGEDAG_SCHEDULER_HPP

