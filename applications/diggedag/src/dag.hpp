
#ifndef DIGGEDAG_DAG_HPP
#define DIGGEDAG_DAG_HPP

#include <map>

#include "util/scoped_lock.hpp"

#include "enum.hpp"
#include "node.hpp"
#include "edge.hpp"
#include "scheduler.hpp"


namespace diggedag
{
  class dag 
  {
    private:
      std::map <node_id_t, node_map_t> nodes_;  // dag node names and instances
      std::map <edge_id_t, edge_map_t> edges_;  // dag edge names and instances

      diggedag::scheduler * scheduler_;
      state                 state_;

      // special nodes which act as anchor for input and output edges
      node                * input_;
      node                * output_;      

      util::mutex           mtx_;


    protected:
      std::map <node_id_t, node_map_t> get_nodes (void) { return nodes_; }
      std::map <edge_id_t, edge_map_t> get_edges (void) { return edges_; }
      friend class scheduler;


    public:
      dag  (void);
      ~dag (void); 

      void  add_node  (const std::string & name, 
                       diggedag::node    * node);
      void  add_edge  (diggedag::edge    * e, 
                       diggedag::node    * src, 
                       diggedag::node    * tgt);
      void  add_edge  (diggedag::edge    * e, 
                       const std::string & src, 
                       const std::string & tgt);
      void  dryrun    (void);
      void  reset     (void);
      void  fire      (void);
      void  wait      (void);
      state get_state (void);
      void  set_state (state s);
      void  dump      (void);
      void  dump_node (std::string name);
      void  schedule  (void);
      void  lock      (void);
      void  unlock    (void);
      void  log       (std::string msg = "", 
                       bool        eol = true);

      void                  set_scheduler (std::string s);
      diggedag::scheduler * get_scheduler (void);
  };

} // namespace diggedag

#endif // DIGGEDAG_DAG_HPP

