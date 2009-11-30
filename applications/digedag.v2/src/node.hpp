
#ifndef DIGEDAG_NODE_HPP
#define DIGEDAG_NODE_HPP

#include <vector>

#include <saga/saga.hpp>

#include "util/scoped_lock.hpp"

#include "config.hpp"
#include "enum.hpp"
#include "dag.hpp"
#include "node.hpp"
#include "edge.hpp"
#include "scheduler.hpp"
#include "node_description.hpp"


namespace digedag
{
  class node : public boost::enable_shared_from_this <node>
  {
    private:
      node_description           nd_;       // node application to run

      std::vector <sp_t <edge> > edge_in_;  // input  data
      std::vector <sp_t <edge> > edge_out_; // output data

      std::string                rm_;
      std::string                cmd_;
      std::string                pwd_;
      std::string                host_;
      std::string                path_;
      std::string                name_;     // instance name
      state                      state_;    // instance state

      sp_t <scheduler>           scheduler_;

      bool                       is_void_;  // void node?

      util::mutex                mtx_;


    public:
      node  (node_description & nd, 
             std::string                  name = "");
      node  (std::string                  cmd,
             std::string                  name = "");
      node  (void); //  a void node does nothing, and just fires its edges.
      ~node (void);

      void             set_name        (std::string name);
      void             add_edge_in     (sp_t <edge> e);
      void             add_edge_out    (sp_t <edge> e);

      void             dryrun          (void);
      void             reset           (void);
      void             fire            (void);
      void             stop            (void);
      void             dump            (bool deep = false);
      void             work            (void);
      std::string      get_name        (void) const;
      node_description get_description (void) const;
      void             set_state       (state s);
      state            get_state       (void);
      void             set_pwd         (std::string pwd);
      void             set_rm          (std::string rm);
      void             set_host        (std::string host);
      void             set_path        (std::string path);

      void             set_scheduler   (sp_t <scheduler> s);
  };

} // namespace digedag

#endif // DIGEDAG_NODE_HPP

