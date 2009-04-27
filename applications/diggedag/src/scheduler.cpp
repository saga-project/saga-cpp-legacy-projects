
#include "dag.hpp"
#include "scheduler.hpp"
#include "scheduler_impl.hpp"


namespace diggedag
{
  scheduler::scheduler (void)
    : impl_ (new impl::scheduler ())
  {
  }


  scheduler::scheduler (const scheduler & src)
    : impl_ (src.get_impl ())
  {
  }


  scheduler::~scheduler (void)
  {
  }

  void scheduler::hook_dag_create    (dag & d)                     { impl_-> hook_dag_create   (d);       }
  void scheduler::hook_dag_destroy   (dag & d)                     { impl_-> hook_dag_destroy  (d);       }
  void scheduler::hook_dag_schedule  (dag & d)                     { impl_-> hook_dag_schedule (d);       }
  void scheduler::hook_dag_run_pre   (dag & d)                     { impl_-> hook_dag_run_pre  (d);       }
  void scheduler::hook_dag_run_post  (dag & d)                     { impl_-> hook_dag_run_post (d);       }
  void scheduler::hook_dag_run_done  (dag & d)                     { impl_-> hook_dag_run_done (d);       }
  void scheduler::hook_dag_run_fail  (dag & d)                     { impl_-> hook_dag_run_fail (d);       }
  void scheduler::hook_dag_wait      (dag & d)                     { impl_-> hook_dag_wait     (d);       }

  void scheduler::hook_node_add      (dag & d, node & n)           { impl_-> hook_node_add     (d, n);    }
  void scheduler::hook_node_remove   (dag & d, node & n)           { impl_-> hook_node_remove  (d, n);    }
  void scheduler::hook_node_run_pre  (dag & d, node & n)           { impl_-> hook_node_run_pre (d, n);    }
  void scheduler::hook_node_run_done (dag & d, node & n)           { impl_-> hook_node_run_done(d, n);    }
  void scheduler::hook_node_run_fail (dag & d, node & n)           { impl_-> hook_node_run_fail(d, n);    }

  void scheduler::hook_edge_add      (dag & d, edge & e)           { impl_-> hook_edge_add     (d, e);    }
  void scheduler::hook_node_remove   (dag & d, edge & e)           { impl_-> hook_node_remove  (d, e);    }
  void scheduler::hook_edge_run_pre  (dag & d, edge & e)           { impl_-> hook_edge_run_pre (d, e);    }
  void scheduler::hook_edge_run_done (dag & d, edge & e)           { impl_-> hook_edge_run_done(d, e);    }
  void scheduler::hook_edge_run_fail (dag & d, edge & e)           { impl_-> hook_edge_run_fail(d, e);    }

} // namespace diggedag

