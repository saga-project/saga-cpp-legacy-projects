
#ifndef DIGGEDAG_NODE_HPP
#define DIGGEDAG_NODE_HPP

#include <vector>

#include <saga/saga.hpp>

#include "util/thread.hpp"
#include "util/scoped_lock.hpp"

#include "enum.hpp"
#include "dag.hpp"
#include "node.hpp"
#include "edge.hpp"
#include "scheduler.hpp"
#include "node_description.hpp"


namespace diggedag
{
  class node : public diggedag::util::thread
  {
    private:
      diggedag::node_description     nd_;       // node application to run

      std::vector <diggedag::edge *> edge_in_;  // input  data
      std::vector <diggedag::edge *> edge_out_; // output data

      std::string                    pwd_;
      std::string                    host_;
      std::string                    name_;     // instance name
      diggedag::state                state_;    // instance state

      diggedag::dag                * dag_;
      diggedag::scheduler          * scheduler_;

      bool                           is_void_;  // void node?

      util::mutex                    mtx_;



    public:
      node  (diggedag::node_description & nd, 
             std::string                  name = "");
      node  (std::string                  cmd,
             std::string                  name = "");
      node  (void); //  a void node does nothing, and just fires its edges.
      ~node (void);

      void            set_name        (std::string      name);
      void            add_edge_in     (diggedag::edge * e);
      void            add_edge_out    (diggedag::edge * e);

      void            dryrun          (void);
      void            fire            (void);
      void            stop            (void);
      void            dump            (bool deep = false);
      void            thread_work     (void);
      std::string     get_name        (void) const;
      diggedag::node_description 
                      get_description (void) const;
      diggedag::state get_state       (void);
      void            set_pwd         (std::string pwd);
      void            set_host        (std::string host);

      void            set_dag         (diggedag::dag  * d);
  };

} // namespace diggedag

#endif // DIGGEDAG_NODE_HPP

