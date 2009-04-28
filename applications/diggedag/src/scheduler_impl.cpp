
#include <vector>

#include <saga/saga.hpp>

#include "util/split.hpp"
#include "util/scoped_lock.hpp"

#include "scheduler_impl.hpp"


namespace diggedag
{
  namespace impl
  {
    scheduler::scheduler (void)
    {
      std::cout << "scheduler ctor" << std::endl;
    }

    scheduler::~scheduler (void)
    {
      std::cout << "scheduler dtor" << std::endl;
    }


    void scheduler::hook_dag_create (diggedag::dag  & d)                     
    {
      my_scoped_lock sl (mtx_);
      std::cout << "scheduler hook_dag_create" << std::endl;
    }
   
   
    void scheduler::hook_dag_destroy (diggedag::dag & d)                     
    {
      my_scoped_lock sl (mtx_);
      std::cout << "scheduler hook_dag_destroy" << std::endl;
    }
   
   
    void scheduler::hook_dag_schedule (diggedag::dag & d)                     
    {
      my_scoped_lock sl (mtx_);
      std::cout << "scheduler hook_dag_schedule" << std::endl;
    }
   
   
    void scheduler::hook_dag_run_pre (diggedag::dag & d)                     
    {
      my_scoped_lock sl (mtx_);
      std::cout << "scheduler hook_dag_run_pre" << std::endl;
    }
   
   
    void scheduler::hook_dag_run_post (diggedag::dag & d)                     
    {
      my_scoped_lock sl (mtx_);
      std::cout << "scheduler hook_dag_run_post" << std::endl;
    }
   
   
    void scheduler::hook_dag_run_done (diggedag::dag & d)                     
    {
      my_scoped_lock sl (mtx_);
      std::cout << "scheduler hook_dag_run_done" << std::endl;
    }
   
   
    void scheduler::hook_dag_run_fail (diggedag::dag & d)                     
    {
      my_scoped_lock sl (mtx_);
      std::cout << "scheduler hook_dag_run_fail" << std::endl;
    }
   
   
    void scheduler::hook_dag_wait (diggedag::dag & d)                     
    {
      my_scoped_lock sl (mtx_);
      std::cout << "scheduler hook_dag_wait" << std::endl;
    }
   
   
   
    void scheduler::hook_node_add (diggedag::dag  & d,
                                   diggedag::node & n)           
    {
      my_scoped_lock sl (mtx_);
      std::cout << "scheduler hook_node_add" << std::endl;
    }
   
   
    void scheduler::hook_node_remove (diggedag::dag  & d,
                                      diggedag::node & n)           
    {
      my_scoped_lock sl (mtx_);
      std::cout << "scheduler hook_node_remove" << std::endl;
    }
   
   
    void scheduler::hook_node_run_pre (diggedag::dag  & d,
                                       diggedag::node & n)           
    {
      my_scoped_lock sl (mtx_);
      std::cout << "scheduler hook_node_run_pre" << std::endl;
    }
   
   
    void scheduler::hook_node_run_done (diggedag::dag  & d,
                                        diggedag::node & n)           
    {
      my_scoped_lock sl (mtx_);
      std::cout << "scheduler hook_node_run_done" << std::endl;
    }
   
   
    void scheduler::hook_node_run_fail (diggedag::dag  & d,
                                        diggedag::node & n)           
    {
      my_scoped_lock sl (mtx_);
      std::cout << "scheduler hook_node_run_fail" << std::endl;

      // tell the dag that a node failed
      // FIXME
      /// d.set_state (Failed);
    }
   
   
   
    void scheduler::hook_edge_add (diggedag::dag  & d,
                                   diggedag::edge & e)           
    {
      my_scoped_lock sl (mtx_);
      std::cout << "scheduler hook_edge_add" << std::endl;
    }
   
   
    void scheduler::hook_node_remove (diggedag::dag  & d,
                                      diggedag::edge & e)           
    {
      my_scoped_lock sl (mtx_);
      std::cout << "scheduler hook_node_remove" << std::endl;
    }
   
   
    void scheduler::hook_edge_run_pre (diggedag::dag  & d,
                                       diggedag::edge & e)           
    {
      my_scoped_lock sl (mtx_);
      std::cout << "scheduler hook_edge_run_pre" << std::endl;
    }
   
   
    void scheduler::hook_edge_run_done (diggedag::dag  & d,
                                        diggedag::edge & e)           
    {
      my_scoped_lock sl (mtx_);
      std::cout << "scheduler hook_edge_run_done" << std::endl;
    }
   
   
    void scheduler::hook_edge_run_fail (diggedag::dag  & d,
                                        diggedag::edge & e)           
    {
      my_scoped_lock sl (mtx_);
      std::cout << "scheduler hook_edge_run_fail" << std::endl;
    }


  } // namespace impl

} // namespace diggedag


