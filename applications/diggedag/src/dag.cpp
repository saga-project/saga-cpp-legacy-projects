
#include "dag.hpp"

namespace diggedag
{
  dag::dag (void)
    : state_ (Pending)
  { 
    scheduler_ = new diggedag::scheduler ();

    // create special nodes
    input_  = new node ();
    output_ = new node ();

    add_node ("INPUT",  input_);
    add_node ("OUTPUT", output_);

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
  }


  void dag::add_node (const std::string & name, 
                      diggedag::node    * node)
  {
    if ( node == NULL )
    {
      log (std::string ("NULL node: ") + name);
      return;
    }

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
    if ( src != NULL )
    {
      src->add_edge_out (e);
      e->add_src_node   (src);
    }
    else
    {
      input_->add_edge_out (e);
      e->add_src_node (input_);
    }

    // edge needs to know what node to fire after completion
    if ( tgt != NULL )
    {
      tgt->add_edge_in  (e);
      e->add_tgt_node   (tgt);
    }
    else
    {
      output_->add_edge_in  (e);
      e->add_tgt_node (output_);
    }

    e->set_dag (this);

    edges_.push_back (e);

    // ### scheduler hook
    scheduler_->hook_edge_add (this, e);
  }


  // add edges to named nodes
  void dag::add_edge (diggedag::edge    * e, 
                      const std::string & src, 
                      const std::string & tgt)
  {
    node * n_src = NULL;
    node * n_tgt = NULL;

    if ( nodes_.find (src) != nodes_.end () )
    {
      n_src = nodes_[src];
    }

    if ( nodes_.find (tgt) != nodes_.end () )
    {
      n_tgt = nodes_[tgt];
    }

    add_edge (e, n_src, n_tgt);
  }


  void dag::reset (void)
  {
    std::map <std::string, diggedag::node *> :: iterator it;
    std::map <std::string, diggedag::node *> :: iterator begin = nodes_.begin ();
    std::map <std::string, diggedag::node *> :: iterator end   = nodes_.end ();

    state_ = Pending;

    for ( it = begin; it != end; it++ )
    {
      (*it).second->reset ();
    }
  }


  void dag::dryrun (void)
  {
    if ( Pending != state_ )
      return;

    log (" dryun:  dag");

    std::map <std::string, diggedag::node *> :: iterator it;
    std::map <std::string, diggedag::node *> :: iterator begin = nodes_.begin ();
    std::map <std::string, diggedag::node *> :: iterator end   = nodes_.end ();

    for ( it = begin; it != end; it++ )
    {
      if ( it->second->get_state () == Pending )
      {
        (*it).second->dryrun ();
      }
    }
  }


  void dag::fire (void)
  {
    log ("fire   dag  ");

    // dump_node ("INPUT");
    // dump_node ("OUTPUT");

    if ( Pending != state_ )
      return;

    state_ = Running;

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
        log (std::string ("       dag fires node ") + it->second->get_name ());
        it->second->fire ();
        cyclic = false;
      }
    }

    if ( cyclic )
    {
      state_ = Failed;

      // ### scheduler hook
      scheduler_->hook_dag_run_fail (this);

      dump ();

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
    while ( s != Done   && 
            s != Failed )
    {
      log ("dag    waiting...");
      ::sleep (1);
      s = get_state ();
    }

    log ("dag state is final - exit wait");
  }


  state dag::get_state (void)
  {
    // these states are final - we can return immediately
    if ( Failed  == state_ ||
         Done    == state_ )
    {
      return state_;
    }

    int state_incomplete = 0;
    int state_stopped    = 0;
    int state_pending    = 0;
    int state_running    = 0;
    int state_done       = 0;
    int state_failed     = 0;
    int state_total      = 0;


    // count node states
    std::map <std::string, diggedag::node *> :: const_iterator it;
    std::map <std::string, diggedag::node *> :: const_iterator begin = nodes_.begin ();
    std::map <std::string, diggedag::node *> :: const_iterator end   = nodes_.end ();

    int i = 0;
    for ( it = begin; it != end; it++ )
    {
      if ( ! (i++ % 10) )
      {
        // log ();
      }

      state_total++;

      state s = (*it).second->get_state ();
      // log (it->first + ":" + state_to_string (s) +  "\t", false);

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
        case Done:
          state_done++;
          break;
        case Failed:
          state_failed++;
          break;
      }
    }
    log ();

    for ( unsigned int i = 0; i < edges_.size (); i++ )
    {
      state_total++;

      state s = edges_[i]->get_state ();

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
        case Done:
          state_done++;
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

    // if all states are Done, dag is ready
    else if ( state_done == state_total )
    {
      state_ = Done;

      // ### scheduler hook
      scheduler_->hook_dag_run_done (this);
    }
    else
    {
      // cannot happen (tm)
      throw "inconsistent dag state";
    }
    
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

    log (" -  DAG    ----------------------------------\n");
    log (std::string (" state: ") + state_to_string (get_state ()));

    log (" -  NODES  ----------------------------------\n");
    for ( it = begin; it != end; it++ )
    {
      (*it).second->dump ();
    }
    log (" -  NODES  ----------------------------------\n");

    log (" -  EDGES  ----------------------------------\n");
    for ( unsigned int i = 0; i < edges_.size (); i++ )
    {
      edges_[i]->dump ();
    }
    log (" -  EDGES  ----------------------------------\n");
    log (" -  DAG    ----------------------------------\n");
  }


  void dag::dump_node (std::string name)
  {
    if ( nodes_.find (name) != nodes_.end () )
    {
      nodes_[name]->dump (true);
    }
  }


  void dag::log (std::string msg, bool eol)
  {
    mtx_.lock ();

    std::cout << msg;
    
    if ( eol )
    {
      std::cout << std::endl;
    }

    mtx_.unlock ();
  }


  void dag::schedule (void)
  {
    // ### scheduler hook
    scheduler_->hook_dag_schedule (this);

    // FIXME: data transfers may end up to be redundant, and ch=should be
    // pruned.
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

