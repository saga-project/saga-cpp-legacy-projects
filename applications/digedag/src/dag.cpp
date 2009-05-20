
#include "dag.hpp"

namespace digedag
{
  dag::dag (void)
    : state_ (Pending)
  { 
    scheduler_ = new digedag::scheduler ();

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

    // Nodes fire edges, edges fire nodes.  No matter which we delete
    // first, we are in trouble.  Thus, we need to *stop* them all, before we
    // start deleting

    scheduler_->stop ();

    // stop nodes
    {
      std::map <node_id_t, digedag::node *> :: iterator it;
      std::map <node_id_t, digedag::node *> :: iterator begin = nodes_.begin ();
      std::map <node_id_t, digedag::node *> :: iterator end   = nodes_.end ();

      for ( it = begin; it != end; it++ )
      {
        it->second->stop ();
      }
    }

    // stop edges
    {
      std::map <edge_id_t, edge_map_t> :: iterator it;
      std::map <edge_id_t, edge_map_t> :: iterator begin = edges_.begin ();
      std::map <edge_id_t, edge_map_t> :: iterator end   = edges_.end ();

      for ( it = begin; it != end; it++ )
      {
        for ( unsigned int i = 0; i < it->second.size (); i++ )
        {
          it->second[i]->stop ();
        }
      }
    }
    


    // ok, everything is stopped, now destroy
    
    // delete nodes
    {
      std::map <node_id_t, digedag::node *> :: iterator it;
      std::map <node_id_t, digedag::node *> :: iterator begin = nodes_.begin ();
      std::map <node_id_t, digedag::node *> :: iterator end   = nodes_.end ();

      for ( it = begin; it != end; it++ )
      {
        delete it->second;
      }
    }

    // delete edges
    {
      std::map <edge_id_t, edge_map_t> :: iterator it;
      std::map <edge_id_t, edge_map_t> :: iterator begin = edges_.begin ();
      std::map <edge_id_t, edge_map_t> :: iterator end   = edges_.end ();

      for ( it = begin; it != end; it++ )
      {
        for ( unsigned int i = 0; i < it->second.size (); i++ )
        {
          delete it->second[i];
        }
      }
    }


    // delete scheduler
    delete scheduler_;
  }


  void dag::add_node (const node_id_t & name, 
                      digedag::node  * node)
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


  void dag::add_edge (digedag::edge * e, 
                      digedag::node * src, 
                      digedag::node * tgt)
  {
    node * s = src;
    node * t = tgt;

    if ( src == NULL ) { s = input_ ; } 
    if ( tgt == NULL ) { t = output_; }

    s->add_edge_out (e);
    t->add_edge_in  (e);

    e->add_src_node (s);
    e->add_tgt_node (t);

    e->set_dag (this);

    edges_[edge_id_t (s->get_name (), t->get_name ())].push_back (e);

    // ### scheduler hook
    scheduler_->hook_edge_add (this, e);
  }


  // add edges to named nodes
  void dag::add_edge (digedag::edge  * e, 
                      const node_id_t & src, 
                      const node_id_t & tgt)
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
    std::map <node_id_t, digedag::node *> :: iterator it;
    std::map <node_id_t, digedag::node *> :: iterator begin = nodes_.begin ();
    std::map <node_id_t, digedag::node *> :: iterator end   = nodes_.end ();

    state_ = Pending;

    for ( it = begin; it != end; it++ )
    {
      it->second->reset ();
    }
  }


  void dag::dryrun (void)
  {
    if ( Pending != state_ )
      return;

    log (" dryun:  dag");

    std::map <node_id_t, digedag::node *> :: iterator it;
    std::map <node_id_t, digedag::node *> :: iterator begin = nodes_.begin ();
    std::map <node_id_t, digedag::node *> :: iterator end   = nodes_.end ();

    for ( it = begin; it != end; it++ )
    {
      if ( it->second->get_state () == Pending )
      {
        it->second->dryrun ();
      }
    }
  }


  void dag::fire (void)
  {
    log ("fire   dag  ");

    dump_node ("INPUT");
    dump_node ("OUTPUT");

    log (std::string ("state: ") + state_to_string (state_));

    if ( Incomplete != state_ &&
         Pending    != state_ )
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

    std::map <node_id_t, digedag::node *> :: iterator it;
    std::map <node_id_t, digedag::node *> :: iterator begin = nodes_.begin ();
    std::map <node_id_t, digedag::node *> :: iterator end   = nodes_.end ();

    for ( it = begin; it != end; it++ )
    {
      log (std::string ("       dag checks ") + it->second->get_name () 
           + ": " + state_to_string (it->second->get_state ()));
      if ( Pending == it->second->get_state () )
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


    {
      // count node states
      std::map <node_id_t, digedag::node *> :: const_iterator it;
      std::map <node_id_t, digedag::node *> :: const_iterator begin = nodes_.begin ();
      std::map <node_id_t, digedag::node *> :: const_iterator end   = nodes_.end ();

      int i = 0;
      for ( it = begin; it != end; it++ )
      {
        if ( ! (i++ % 10) )
        {
          log ();
        }

        state_total++;

        state s = it->second->get_state ();
        log (it->first + ":" + state_to_string (s) +  "\t", false);

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
    }
    log ();

    {
      // count edge states
      std::map <edge_id_t, edge_map_t> :: const_iterator it;
      std::map <edge_id_t, edge_map_t> :: const_iterator begin = edges_.begin ();
      std::map <edge_id_t, edge_map_t> :: const_iterator end   = edges_.end ();

      int cnt = 0;
      for ( it = begin; it != end; it++ )
      {
        for ( unsigned int i = 0; i < it->second.size (); i++ )
        {
          if ( ! (cnt++ % 5) )
          {
            // log ();
          }

          state_total++;

          state s = it->second[i]->get_state ();
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
      }
    }
    log ();


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
    log (" -  DAG    ----------------------------------\n");
    log (std::string (" state: ") + state_to_string (get_state ()));

    log (" -  NODES  ----------------------------------\n");
    {
      std::map <node_id_t, digedag::node *> :: const_iterator it;
      std::map <node_id_t, digedag::node *> :: const_iterator begin = nodes_.begin ();
      std::map <node_id_t, digedag::node *> :: const_iterator end   = nodes_.end ();

      for ( it = begin; it != end; it++ )
      {
        it->second->dump ();
      }
    }
    log (" -  NODES  ----------------------------------\n");

    log (" -  EDGES  ----------------------------------\n");
    {
      std::map <edge_id_t, edge_map_t> :: const_iterator it;
      std::map <edge_id_t, edge_map_t> :: const_iterator begin = edges_.begin ();
      std::map <edge_id_t, edge_map_t> :: const_iterator end   = edges_.end ();

      for ( it = begin; it != end; it++ )
      {
        for ( unsigned int i = 0; i < it->second.size (); i++ )
        {
          it->second[i]->dump ();
        }
      }
    }
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
    lock ();

    std::cout << msg;
    
    if ( eol )
    {
      std::cout << std::endl;
    }

    unlock ();
  }


  void dag::schedule (void)
  {
    // ### scheduler hook
    scheduler_->hook_dag_schedule (this);

    // FIXME: data transfers may end up to be redundant, and should be
    // pruned.
  }

  void dag::lock (void)
  {
    mtx_.lock ();
  }

  void dag::unlock (void)
  {
    mtx_.unlock ();
  }

  void dag::set_scheduler (std::string s)
  {
    scheduler_->set_scheduler (s);
  }

  digedag::scheduler * dag::get_scheduler (void)
  {
    return scheduler_;
  }
} // namespace digedag

