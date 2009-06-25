
#ifndef DIGEDAG_DAG_HPP
#define DIGEDAG_DAG_HPP

#include <map>

#include "util/scoped_lock.hpp"

#include "enum.hpp"
#include "node.hpp"
#include "edge.hpp"
#include "scheduler.hpp"


namespace digedag
{
  // the 'dag' class represents a directed acyclic graph.  Its data model is
  // extremely simple: it maintains a list of nodes and edges.  The
  // depenedencies between them are stored implicitely: each edge knows its
  // source and target node, and each nodes knows about its incoming and
  // outgoing edges.  Whe firing (i.e. running) a dag, those nodes with
  // satisfied depencies are started, which upon completion activate their
  // outgoing edges, which activate their target nodes, etc etc.
  //
  // A dag also has a scheduler, which can traverse the dag, and change
  // attributes of edges and nodes, such as its assigment to a specific
  // resource.
  class dag 
  {
    private:
      std::map <node_id_t, node_map_t> nodes_;  // dag node names and instances
      std::map <edge_id_t, edge_map_t> edges_;  // dag edge names and instances

      digedag::scheduler * scheduler_;
      state                 state_;             // see get_state ()

      // special nodes which act as anchor for input and output edges
      node                * input_;
      node                * output_;      

      util::mutex           mtx_;

      saga::session         session_;


    protected:
      // allow our friend, the sxheduler, full access to the dag data.
      // FIXME: we need to make sure that the scheduler is not changing these
      // data when we are operating on them...
      std::map <node_id_t, node_map_t> get_nodes (void) { return nodes_; }
      std::map <edge_id_t, edge_map_t> get_edges (void) { return edges_; }
      friend class scheduler;


    public:
      dag  (void);
      ~dag (void); 


      // create the dag
      void  add_node  (const std::string & name, 
                       digedag::node    * node);
      void  add_edge  (digedag::edge    * e, 
                       digedag::node    * src, 
                       digedag::node    * tgt);
      void  add_edge  (digedag::edge    * e, 
                       const std::string & src, 
                       const std::string & tgt);

      // operations on a dag
      void  dryrun    (void);
      void  reset     (void);
      void  fire      (void);
      void  wait      (void);
      state get_state (void);
      void  set_state (state s);
      void  schedule  (void);

      // inspection
      void  dump      (void);
      void  dump_node (std::string name);

      // other tools
      void  lock      (void);
      void  unlock    (void);
      void  log       (std::string msg = "", 
                       bool        eol = true);

      void                  set_scheduler (std::string s);
      digedag::scheduler * get_scheduler (void);
  };

} // namespace digedag

#endif // DIGEDAG_DAG_HPP

