
#ifndef DIGEDAG_DAG_HPP
#define DIGEDAG_DAG_HPP

#include <map>

#include "util/scoped_lock.hpp"

#include "config.hpp"
#include "enum.hpp"
#include "node.hpp"
#include "edge.hpp"


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
  class scheduler;
  class dag : public boost::enable_shared_from_this <dag>
  {
    private:
      std::map <node_id_t, node_map_t> nodes_;     // dag node names and instances
      std::map <edge_id_t, edge_map_t> edges_;     // dag edge names and instances

      state                            state_;     // see get_state ()
      sp_t <scheduler>                 scheduler_;

      // special nodes which act as anchor for input and output edges
      sp_t <node>                      input_;
      sp_t <node>                      output_;      

      util::mutex                      mtx_;


    protected:
      // allow our friend, the sxheduler, full access to the dag data.
      // FIXME: we need to make sure that the scheduler is not changing these
      // data when we are operating on them...
      std::map <node_id_t, node_map_t> get_nodes (void) { return nodes_; }
      std::map <edge_id_t, edge_map_t> get_edges (void) { return edges_; }
      friend class scheduler;


    public:
      dag  (const std::string & scheduler_src = "");
      ~dag (void); 


      // create the dag
      void  add_node  (const std::string    & name, 
                       sp_t <node>            node);
      void  add_edge  (sp_t <edge>            e, 
                       sp_t <node>            src, 
                       sp_t <node>            tgt);
      void  add_edge  (sp_t <edge>            e, 
                       const std::string    & src, 
                       const std::string    & tgt);

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
  };

} // namespace digedag

#endif // DIGEDAG_DAG_HPP

