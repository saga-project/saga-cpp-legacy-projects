
#ifndef DIGGEDAG_DAG_IMPL_HPP
#define DIGGEDAG_DAG_IMPL_HPP

#include <saga/saga.hpp>

#include "node.hpp"


namespace diggedag
{
  namespace impl
  {
    class dag : public diggedag::util::uid
    {
      private:
        std::vector <diggedag::node> nodes_; // dag nodes
        state state_;


      public:
        dag (void) 
          : state_ (Pending)
        { 
          // std::cout << "create dag  " << uid_get () << std::endl;
        }

        ~dag (void) 
        {
          // std::cout << "delete dag  " << uid_get () << std::endl;
        }

        void add_node (diggedag::node & n)
        {
          nodes_.push_back (n);
        }

        void add_edge (diggedag::edge & e, 
                       diggedag::node & src, 
                       diggedag::node & tgt)
        {
          // src need to know what edge to fire after completion
          src.add_edge_out (e);

          // edge needs to know what node to fire after completion
          tgt.add_edge_in  (e);
          e.add_node       (tgt);
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
          for ( unsigned int i = 0; i < nodes_.size (); i++ )
          {
            if ( Pending == nodes_[i].get_state () )
            {
              std::cout << "       dag fires node " << nodes_[i].get_name () << std::endl;
              nodes_[i].fire ();
            }
          }

          // TODO complete.  Need async.
        }

        state get_state (void)
        {
          for ( unsigned int i = 0; i < nodes_.size (); i++ )
          {
            if ( Ready != nodes_[i].get_state () )
            {
              // std::cout << "node " << nodes_[i].get_name () << " is ! ready" << std::endl;
              state_ = Running;
              return state_;
            }
          }

          // std::cout << "nodes are ready" << std::endl;

          // if all are Ready, the DAG is done.
          state_ = Ready;
          return state_;
        }
    };
  } // namespace impl

} // namespace diggedag

#endif // DIGGEDAG_DAG_IMPL_HPP

