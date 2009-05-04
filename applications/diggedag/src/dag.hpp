
#ifndef DIGGEDAG_DAG_HPP
#define DIGGEDAG_DAG_HPP

#include <map>

#include "enum.hpp"
#include "node.hpp"
#include "edge.hpp"
#include "scheduler.hpp"


namespace diggedag
{
  class dag 
  {
    private:
      std::map    <std::string, diggedag::node *> nodes_; // dag node names and instances
      std::vector <diggedag::edge *>              edges_; // dag edge instances

      diggedag::scheduler * scheduler_;
      state                 state_;

      // special nodes which act as anchor for input and output edges
      node                * input_;
      node                * output_;      

    protected:
      std::map    <std::string, diggedag::node *> get_nodes (void) { return nodes_; }
      std::vector <diggedag::edge *>              get_edges (void) { return edges_; }
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
      void  fire      (void);
      void  wait      (void);
      state get_state (void);
      void  set_state (state s);
      void  dump      (void);
      void  dump_node (std::string name);
      void  schedule  (void);
      void  prune     (void);

      void                  set_scheduler (std::string s);
      diggedag::scheduler * get_scheduler (void);
  };

} // namespace diggedag

#endif // DIGGEDAG_DAG_HPP

