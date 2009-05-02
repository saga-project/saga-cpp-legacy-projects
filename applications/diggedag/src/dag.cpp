
#include "dag.hpp"

namespace diggedag
{
  dag::dag (void)
    : state_ (Pending)
  { 
    // std::cout << "create dag " << std::endl;
    scheduler_ = new diggedag::scheduler ();

    // ### scheduler hook
    scheduler_->hook_dag_create (this);
  }


  dag::~dag (void) 
  {
    // ### scheduler hook
    scheduler_->hook_dag_destroy (this);

    std::map <std::string, diggedag::node *> :: iterator it;
    std::map <std::string, diggedag::node *> :: iterator begin = nodes_.begin ();
    std::map <std::string, diggedag::node *> :: iterator end   = nodes_.end ();


    // Nodes fire edges, edges fire nodes.  No matter which we delete
    // first, we are in trouble.  Thus, we need to *stop* them all, before we
    // start deleting
    {
      for ( it = begin; it != end; it++ )
      {
        (*it).second->stop ();
      }

      for ( unsigned int i = 0; i < edges_.size (); i++ )
      {
        edges_[i]->stop ();
      }
    }


    // ok, everything is stopped, now destroy
    {
      for ( it = begin; it != end; it++ )
      {
        delete (*it).second;
      }

      for ( unsigned int i = 0; i < edges_.size (); i++ )
      {
        delete edges_[i];
      }
    }

    // delete scheduler, nodes and edges
    delete scheduler_;

    std::cout << "deleted  scheduler " << std::endl;
  }


  void dag::add_node (const std::string & name, 
                      diggedag::node    * node)
  {
    nodes_[name] = node;

    node->set_dag  (this);
    node->set_name (name);

    // ### scheduler hook
    scheduler_->hook_node_add (this, node);
  }


  void dag::add_edge (diggedag::edge * e, 
                      diggedag::node * src, 
                      diggedag::node * tgt)
  {
    // src need to know what edge to fire after completion
    src->add_edge_out (e);

    // edge needs to know what node to fire after completion
    tgt->add_edge_in  (e);

    e->add_src_node   (src);
    e->add_tgt_node   (tgt);

    edges_.push_back (e);

    e->set_dag (this);

    // ### scheduler hook
    scheduler_->hook_edge_add (this, e);
  }


  // add edges to named nodes
  void dag::add_edge (diggedag::edge    * e, 
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


  void dag::dryrun (void)
  {
    std::cout << " dryun:  dag" << std::endl;

    std::map <std::string, diggedag::node *> :: iterator it;
    std::map <std::string, diggedag::node *> :: iterator begin = nodes_.begin ();
    std::map <std::string, diggedag::node *> :: iterator end   = nodes_.end ();

    std::cout << "         dag : " << std::endl;

    for ( it = begin; it != end; it++ )
    {
      std::cout << "             fire " << (*it).first << std::endl;
      (*it).second->dryrun ();
    }
  }


  void dag::fire (void)
  {
    if ( Pending != state_ )
    {
      return;
    }


    state_ = Running;

    std::cout << "fire   dag  " << std::endl;


    // ### scheduler hook
    scheduler_->hook_dag_run_pre (this);


    // search for nodes which have resolved inputs (no peding edges), and
    // fire them.  Whenever a node finishes, it fires it outgoing edges.
    // If those finish copying their data, they'll fire those nodes which
    // they are incoming edges of.  Of those nodes happen to have all
    // input edges resolved, the fire will indeed lead to an execution of
    // that node, etc.
    //
    // if no nodes can be fired, complain.  Graph is probably cyclic.
    bool cyclic = true;

    std::map <std::string, diggedag::node *> :: iterator it;
    std::map <std::string, diggedag::node *> :: iterator begin = nodes_.begin ();
    std::map <std::string, diggedag::node *> :: iterator end   = nodes_.end ();

    for ( it = begin; it != end; it++ )
    {
      if ( Pending == (*it).second->get_state () )
      {
        // std::cout << "       dag fires node " << (*it).second->get_name () << std::endl;
        (*it).second->fire ();
        cyclic = false;
      }
    }

    if ( cyclic )
    {
      state_ = Failed;

      // ### scheduler hook
      scheduler_->hook_dag_run_fail (this);

      throw "can't find pending nodes.  cyclic or empty graph?";
    }

    // ### scheduler hook
    scheduler_->hook_dag_run_post (this);
  }


  void dag::wait (void)
  {
    // ### scheduler hook
    scheduler_->hook_dag_wait (this);

    state s = get_state ();
    while ( s != Ready  && 
            s != Failed )
    {
      std::cout << "dag    waiting..." << std::endl;
      ::sleep (1);
      s = get_state ();
    }

    std::cout << "dag state is final - exit wait\n";
  }


  // FIXME: we should also check edge states!
  state dag::get_state (void)
  {
    // these states are final - we can return immediately
    if ( Failed  == state_ ||
         Ready   == state_ )
    {
      std::cout << "state is final" << std::endl;
      return state_;
    }

    int state_incomplete = 0;
    int state_stopped    = 0;
    int state_pending    = 0;
    int state_running    = 0;
    int state_ready      = 0;
    int state_failed     = 0;
    int state_total      = 0;


    // count states
    std::map <std::string, diggedag::node *> :: const_iterator it;
    std::map <std::string, diggedag::node *> :: const_iterator begin = nodes_.begin ();
    std::map <std::string, diggedag::node *> :: const_iterator end   = nodes_.end ();

    for ( it = begin; it != end; it++ )
    {
      state_total++;

      state s = (*it).second->get_state ();
      std::cout << (*it).first << ":" << state_to_string (s) <<  "\t" << std::flush;

      switch ( s )
      {
        case Incomplete:
          state_incomplete++;
          break;
        case Stopped:
          state_stopped++;
          break;
        case Pending:
          state_pending++;
          break;
        case Running:
          state_running++;
          break;
        case Ready:
          state_ready++;
          break;
        case Failed:
          state_failed++;
          break;
      }
    }

    // if any job failed, dag is considered to have failed
    if ( state_failed > 0 )
    {
      state_ = Failed;
    }

    // one job being stopped, incomplete, pending or Running defines the dag's state
    else if ( state_stopped > 0 )
    {
      state_ = Stopped;
    }
    else if ( state_incomplete > 0 )
    {
      state_ = Incomplete;
    }
    else if ( state_pending > 0 )
    {
      state_ = Pending;
    }
    else if ( state_running > 0 )
    {
      state_ = Running;
    }

    // if all states are Ready, dag is ready
    else if ( state_ready == state_total )
    {
      state_ = Ready;

      // ### scheduler hook
      scheduler_->hook_dag_run_done (this);
    }
    else
    {
      // cannot happen (tm)
      throw "inconsistent dag state";
    }
    
    std::cout << "dag    state is " << state_to_string (state_) << std::endl;

    return state_;
  }

  void dag::set_state (state s)
  {
    state_ = s;
  }

  void dag::dump (void)
  {
    std::map <std::string, diggedag::node *> :: const_iterator it;
    std::map <std::string, diggedag::node *> :: const_iterator begin = nodes_.begin ();
    std::map <std::string, diggedag::node *> :: const_iterator end   = nodes_.end ();


    for ( it = begin; it != end; it++ )
    {
      std::cout << " --------------------------------------------\n";
      std::cout << " NODE " << (*it).second->get_name () << std::endl;

      diggedag::node_description nd = (*it).second->get_description ();

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
      std::cout << edges_[i]->get_src_node ()->get_name () 
        << " ---->\t "  
        << edges_[i]->get_tgt_node ()->get_name () 
        << std::endl;
    }
    std::cout << " -  EDGES  ----------------------------------\n";
  }


  void dag::schedule (void)
  {
    // ### scheduler hook
    scheduler_->hook_dag_schedule (this);
  }

  void dag::set_scheduler (std::string s)
  {
    scheduler_->set_scheduler (s);
  }

  diggedag::scheduler * dag::get_scheduler (void)
  {
    return scheduler_;
  }
} // namespace diggedag

