
#ifndef DIGEDAG_NODE_HPP
#define DIGEDAG_NODE_HPP

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


namespace digedag
{
  class node : public digedag::util::thread
  {
    private:
      digedag::node_description     nd_;       // node application to run

      std::vector <digedag::edge *> edge_in_;  // input  data
      std::vector <digedag::edge *> edge_out_; // output data

      std::string                    rm_;
      std::string                    cmd_;
      std::string                    pwd_;
      std::string                    host_;
      std::string                    path_;
      std::string                    name_;     // instance name
      digedag::state                state_;    // instance state

      digedag::dag                * dag_;
      digedag::scheduler          * scheduler_;

      bool                           is_void_;  // void node?

      util::mutex                    mtx_;

      saga::session                  session_;



    public:
      node  (digedag::node_description & nd, 
             std::string                  name = "");
      node  (std::string                  cmd,
             std::string                  name = "");
      node  (void); //  a void node does nothing, and just fires its edges.
      ~node (void);

      void            set_name        (std::string      name);
      void            add_edge_in     (digedag::edge * e);
      void            add_edge_out    (digedag::edge * e);

      void            dryrun          (void);
      void            reset           (void);
      void            fire            (void);
      void            stop            (void);
      void            dump            (bool deep = false);
      void            thread_work     (void);
      std::string     get_name        (void) const;
      digedag::node_description 
                      get_description (void) const;
      void            set_state       (state s);
      digedag::state get_state       (void);
      void            set_pwd         (std::string pwd);
      void            set_rm          (std::string rm);
      void            set_host        (std::string host);
      void            set_path        (std::string path);

      void            set_dag         (saga::session  s, 
                                       digedag::dag * d);
  };

} // namespace digedag

#endif // DIGEDAG_NODE_HPP

