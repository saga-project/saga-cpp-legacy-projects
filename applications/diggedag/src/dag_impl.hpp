
#ifndef DIGGEDAG_DAG_IMPL_HPP
#define DIGGEDAG_DAG_IMPL_HPP

#include <map>

#include <saga/saga.hpp>

#include "edge.hpp"
#include "node.hpp"


namespace diggedag
{
  namespace impl
  {
    class dag 
    {
      private:
        std::map    <std::string, diggedag::node> nodes_; // dag node names and instances
        std::vector <diggedag::edge>              edges_; // dag edge instances
        state state_;


      public:
        dag (void) 
          : state_ (Pending)
        { 
          // std::cout << "create dag " << std::endl;
        }

        ~dag (void) 
        {
          // std::cout << "delete dag " << std::endl;
        }


        void add_node (const std::string & name, 
                       diggedag::node    & node)
        {
          nodes_[name] = node;
        }

        void add_edge (diggedag::edge & e, 
                       diggedag::node & src, 
                       diggedag::node & tgt)
        {
          // src need to know what edge to fire after completion
          src.add_edge_out (e);

          // edge needs to know what node to fire after completion
          tgt.add_edge_in  (e);

          e.add_src_node   (src);
          e.add_tgt_node   (tgt);

          edges_.push_back (e);
        }

        
        // add edges to named nodes
        void add_edge (diggedag::edge    & e, 
                       const std::string & src, 
                       const std::string & tgt)
        {
          if ( nodes_.find (src) == nodes_.end () ||
               nodes_.find (tgt) == nodes_.end () )
          {
            std::cerr << " cannot add edge between " << src 
                      << " and " << tgt << std::endl;
            throw "No such node";
          }
          add_edge (e, nodes_[src], nodes_[tgt]);
        }


        void fire (void)
        {
          std::cout << "fire   dag  " << std::endl;

          state_ = Running;

          // search for nodes which have resolved inputs (no peding edges), and
          // fire them.  Whenever a node finishes, it fires it outgoing edges.
          // If those finish copying their data, they'll fire those nodes which
          // they are incoming edges of.  Of those nodes happen to have all
          // input edges resolved, the fire will indeed lead to an execution of
          // that node, etc.
          std::map <std::string, diggedag::node> :: iterator it;
          std::map <std::string, diggedag::node> :: iterator begin = nodes_.begin ();
          std::map <std::string, diggedag::node> :: iterator end   = nodes_.end ();

          for ( it = begin; it != end; it++ )
          {
            if ( Pending == (*it).second.get_state () )
            {
              std::cout << "       dag fires node " << (*it).second.get_name () << std::endl;
              (*it).second.fire ();
            }
          }

          // TODO complete.  Need async.
        }

        state get_state (void)
        {
          std::map <std::string, diggedag::node> :: const_iterator it;
          std::map <std::string, diggedag::node> :: const_iterator begin = nodes_.begin ();
          std::map <std::string, diggedag::node> :: const_iterator end   = nodes_.end ();

          for ( it = begin; it != end; it++ )
          {
            if ( Ready != (*it).second.get_state () )
            {
              // std::cout << "node " << (*it).second.get_name () << " is ! ready" << std::endl;
              state_ = Running;
              return state_;
            }
          }

          // std::cout << "nodes are ready" << std::endl;

          // if all are Ready, the DAG is done.
          state_ = Ready;
          return state_;
        }

        void dump (void)
        {
          std::map <std::string, diggedag::node> :: const_iterator it;
          std::map <std::string, diggedag::node> :: const_iterator begin = nodes_.begin ();
          std::map <std::string, diggedag::node> :: const_iterator end   = nodes_.end ();

          for ( it = begin; it != end; it++ )
          {
            std::cout << " --------------------------------------------\n";
            std::cout << " NODE " << (*it).second.get_name () << std::endl;
           
            diggedag::node_description nd = (*it).second.get_description ();

            std::vector <std::string> attribs = nd.list_attributes ();

            for ( unsigned int a = 0; a < attribs.size (); a++ )
            {
              std::cout << "      " << attribs[a] << ": ";

              if ( nd.attribute_is_vector (attribs[a]) )
              {
                std::vector <std::string> vals = nd.get_vector_attribute (attribs[a]);

                for ( unsigned int v = 0; v < vals.size (); v++ )
                {
                  std::cout << vals[v] << " ";
                }
              }
              else
              {
                std::cout << nd.get_attribute(attribs[a]);
              }
              std::cout << std::endl;
            }

            std::cout << " --------------------------------------------\n";
          }

          std::cout << " -  EDGES  ----------------------------------\n";
          for ( unsigned int i = 0; i < edges_.size (); i++ )
          {
            std::cout << edges_[i].get_src_node ().get_name () 
                      << " ---->\t "  
                      << edges_[i].get_tgt_node ().get_name () 
                      << std::endl;
          }
          std::cout << " -  EDGES  ----------------------------------\n";
        }
    
    };

  } // namespace impl

} // namespace diggedag

#endif // DIGGEDAG_DAG_IMPL_HPP

