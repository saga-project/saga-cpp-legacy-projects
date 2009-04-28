
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
  class node;
  class edge;
  class scheduler 
  {
    protected:
      my_shared_ptr <impl::scheduler> impl_;
      // my_shared_ptr <impl::scheduler> get_impl (void) const { return impl_; } 


    public:
      scheduler  (void);
      scheduler  (const scheduler & src);
      ~scheduler (void);

      void hook_dag_create    (dag & d);
      void hook_dag_destroy   (dag & d);
      void hook_dag_schedule  (dag & d);
      void hook_dag_run_pre   (dag & d);
      void hook_dag_run_post  (dag & d);
      void hook_dag_run_done  (dag & d);
      void hook_dag_run_fail  (dag & d);
      void hook_dag_wait      (dag & d);
      
      void hook_node_add      (dag & d, node & n);
      void hook_node_remove   (dag & d, node & n);
      void hook_node_run_pre  (dag & d, node & n);
      void hook_node_run_done (dag & d, node & n);
      void hook_node_run_fail (dag & d, node & n);
   
      void hook_edge_add      (dag & d, edge & e);
      void hook_node_remove   (dag & d, edge & e);
      void hook_edge_run_pre  (dag & d, edge & e);
      void hook_edge_run_done (dag & d, edge & e);
      void hook_edge_run_fail (dag & d, edge & e);
  };

} // namespace diggedag

#endif // DIGGEDAG_SCHEDULER_HPP

