
#ifndef DIGGEDAG_NODE_IMPL_HPP
#define DIGGEDAG_NODE_IMPL_HPP

#include <vector>

#include <saga/saga.hpp>

#include "edge.hpp"

namespace diggedag
{
  namespace impl
  {
    class node : public diggedag::util::uid, 
                 public diggedag::util::thread
    {
      private:
        saga::job::description       jd_;       // node application to run

        std::vector <diggedag::edge> edge_in_;  // input  data
        std::vector <diggedag::edge> edge_out_; // output data

        std::string                  name_;     // instance name
        diggedag::state              state_;    // instance state

      public:
        node (const saga::job::description & jd, 
              const std::string name)
          : jd_    (jd)
          , name_  (name)
          , state_ (diggedag::Pending)
        {
          // std::cout << "create node " << uid_get () << std::endl;
        }

        ~node (void)
        {
          // std::cout << "delete node " << uid_get () << std::endl;
        }

        void add_edge_in (const diggedag::edge & e)
        {
          // std::cout << "edge-i node " << name_ 
          //           << " : "  << e.get_src () 
          //           << " -> " << e.get_tgt () << std::endl;

          edge_in_.push_back (e);
        }

        void add_edge_out (const diggedag::edge & e)
        {
          // std::cout << "edge-o node " << name_ 
          //           << " : "  << e.get_src () 
          //           << " -> " << e.get_tgt () << std::endl;
 
          edge_out_.push_back (e);
        }

        // ensure the node application is run.  Before doing that, the input data
        // edges need to be Ready, to ensure that input data are available for the
        // application.  If they are not ready, fire has no effect.
        void fire (void)
        {
          std::cout << "fire   node " << name_ << std::endl;

          // check if all input data are ready
          for ( unsigned int i = 0; i < edge_in_.size (); i++ )
          {
            if ( Ready != edge_in_[i].get_state () )
            {
              std::cout << "       node " << name_ << " canceled" << std::endl;
              return;
            }
            else
            {
              std::cout << "       node " << name_ << " : edge " 
                << edge_in_[i].get_src () << "->" 
                << edge_in_[i].get_tgt () << " is ready" << std::endl;
            }
          }

          // check if node was started before (!Pending).  
          // If not, mark that we start the work (Running)
          {
            util::scoped_lock l = thread_scoped_lock ();

            if ( Pending != state_ )
              return;

            // we have work to do...
            state_ = Running;
          }

          // all input edges are Ready, i.e. all input data are available.  We
          // can thus really execute the node application.
          //
          // So: run the application, in extra thread
          thread_run ();
        }


        // thread_work is called when the node is fired, and all prerequesites
        // are fullfilled, i.e. all input data are available.  First we execute
        // the node's application, then we fire all outgoing edges, to get data
        // staged out.
        void thread_work (void)
        {
          // TODO: run the saga job for the job description here

          // FIXME: for now, we simply fake work by sleeping for some amount of
          // time
          ::sleep (1);
          // std::cout << " ===== after  sleep " << uid_get () << std::endl;


          // get data staged out, e.g. fire outgoing edges
          for ( unsigned int i = 0; i < edge_out_.size (); i++ )
          {
            // std::cout << "       node " << name_ << " fires edge "
            //   << edge_out_[i].get_src () << "->" 
            //   << edge_out_[i].get_tgt () << std::endl;
            edge_out_[i].fire ();
          }


          // when all is done, we can update the state
          thread_lock ();
          state_ = Ready;
          thread_unlock ();
        }

        std::string node::get_name (void) const
        {
          return name_;
        }

        diggedag::state get_state (void)
        {
          // check if all input data are ready
          for ( unsigned int i = 0; i < edge_in_.size (); i++ )
          {
            if ( Ready != edge_in_[i].get_state () )
            {
              return Incomplete;
            }
          }

          return state_;
        }
    };

  } // namespace impl

} // namespace diggedag

#endif // DIGGEDAG_NODE_IMPL_HPP

