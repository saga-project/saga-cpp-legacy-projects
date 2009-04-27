/*
 *  job_dag.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 12/10/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_IMPL_JOB_DAG_HPP
#define FAUST_IMPL_JOB_DAG_HPP

#include <boost/graph/graph_traits.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/visitors.hpp>

#include <faust/faust/job.hpp>
#include <faust/faust/job_group.hpp>
#include <faust/faust/dependency.hpp>


namespace faust { namespace detail {
  
  struct DAG_vertex_properties {
    std::string name;
    faust::job job;
  };
  
  struct DAG_edge_properties {
    std::string name;
    faust::dependency dep_type;
  };
  
  typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, 
    DAG_vertex_properties, DAG_edge_properties> DAG;
  
  typedef boost::graph_traits<DAG>::vertex_descriptor DAGVertex;
  typedef boost::graph_traits<DAG>::edge_descriptor DAGEdge;

  class job_dag{ 
    
  private:
    DAG dag_;
    
  public:
    job_dag();
    ~job_dag();
    
    
  };
  
} }

#endif /* FAUST_IMPL_JOB_DAG_HPP */

