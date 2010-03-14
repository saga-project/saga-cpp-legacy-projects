/*
 #  Copyright (c) 2010 Katerina Stamou (kstamou@cct.lsu.edu)
 # 
 #  Distributed under the Boost Software License, Version 1.0. (See accompanying
 #  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/


#ifndef DIGEDAG_CHECKPOINT_HPP
#define DIGEDAG_CHECKPOINT_HPP

#include <saga/saga.hpp>

#include "config.hpp"
#include "dag.hpp"
#include "node.hpp"
#include "edge.hpp"
#include "enum.hpp"
#include "util/hash.hpp"
#include "util/scoped_lock.hpp"

#include <boost/dynamic_bitset.hpp>

namespace digedag
{
  class checkpoint_mgr 
  {
    private:
      boost::dynamic_bitset<> nodeset;
      boost::dynamic_bitset<> edgeset;
      bool                dagcomplete;

      struct checkpoint_conf_t
      {
        bool             compress;
        bool             verbose;
        bool		 hashlog;
        checkpoint_method method;
        std::string      filename;
        std::string	 filepath;
      } checkpoint_;

      void dumptofile        (void);
      void dumptoadvert      (void);
      void restorefromfile   (std::map <node_id_t, node_map_t> &dag_nodes, std::map <edge_id_t, edge_map_t> &dag_edges);
      void restorefromadvert (void);

    public:
      checkpoint_mgr  ();
      ~checkpoint_mgr (void);

      void set_file                (std::string daghash);
      std::string get_filename     (void);
      void set_filepath            (std::string fpath);
      void set_verbose             (bool flag);
      bool get_verbose             (void);
      void set_compress            (bool flag);
      bool get_compress            (void);
      void set_hashlog             (bool flag);
      bool get_hashlog             (void);
      void set_method              (checkpoint_method m);
      checkpoint_method get_method (void);

      void node_commit  (boost::shared_ptr <node> n, std::map <node_id_t, node_map_t> dag_nodes);
      void edge_commit  (boost::shared_ptr <edge> e, std::map <edge_id_t, edge_map_t> dag_edges);
      void dump         (std::map <node_id_t, node_map_t> dag_nodes, std::map <edge_id_t, edge_map_t> dag_edges);
      void restore      (std::map <node_id_t, node_map_t> &dag_nodes, std::map <edge_id_t, edge_map_t> &dag_edges);
      void dag_complete (void);
  };

} // namespace digedag

#endif // DIGEDAG_CHECKPOINT_HPP

