//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AP_GRAPH_HPP
#define AP_GRAPH_HPP

#define BOOST_NO_HASH
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace AllPairs {
   struct VertexProperty { std::string name; };
   struct EdgeProperty   { double weight;    };
   typedef boost::adjacency_list<boost::vecS,
                                 boost::vecS,
                                 boost::bidirectionalS,
                                 VertexProperty,
                                 EdgeProperty> Graph;
   typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
   typedef boost::graph_traits<Graph>::edge_descriptor Edge;
}

#endif //AP_GRAPH_HPP
