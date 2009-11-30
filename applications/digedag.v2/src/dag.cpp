
#include "dag.hpp"

namespace digedag
{
  dag::dag (const std::string & scheduler_src)
    : state_     (Pending),
      scheduler_ (new scheduler (this, scheduler_src)),
      input_     (new node ()),
      output_    (new node ())
  { 
    // add special nodes to dag already
    add_node ("INPUT",  input_);
    add_node ("OUTPUT", output_);

    // ### scheduler hook
    scheduler_->hook_dag_create ();
  }


  dag::~dag (void) 
  {
    // ### scheduler hook
    scheduler_->hook_dag_destroy ();

    // Nodes fire edges, edges fire nodes.  No matter which we delete
    // first, we are in trouble.  Thus, we need to *stop* them all, before we
    // start deleting

    scheduler_->stop ();

    // stop nodes
    {
      std::map <node_id_t, sp_t <node> > :: iterator it;
      std::map <node_id_t, sp_t <node> > :: iterator begin = nodes_.begin ();
      std::map <node_id_t, sp_t <node> > :: iterator end   = nodes_.end ();

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
    

    // ok, everything is stopped, and shared_ptr's will be destroyed here
  }


  void dag::add_node (const node_id_t & name, 
                      sp_t <node>       node)
  {
    if ( node == NULL )
    {
      std::cout << std::string ("NULL node: ") << name << std::endl;
      return;
    }

    nodes_[name] = node;

    node->set_name (name);

    // ### scheduler hook
    node->set_scheduler (scheduler_);
    scheduler_->hook_node_add (*(node.get ()));
  }


  void dag::add_edge (sp_t <edge> e, 
                      sp_t <node> src, 
                      sp_t <node> tgt)
  {
    sp_t <node> s = src;
    sp_t <node> t = tgt;

    if ( src == NULL ) { s = input_ ; } 
    if ( tgt == NULL ) { t = output_; }

    s->add_edge_out (e);
    t->add_edge_in  (e);

    e->add_src_node (s);
    e->add_tgt_node (t);

    edges_[edge_id_t (s->get_name (), t->get_name ())].push_back (e);

    // ### scheduler hook
    e->set_scheduler (scheduler_);
    scheduler_->hook_edge_add (*(e.get ()));
  }


  // add edges to named nodes
  void dag::add_edge (sp_t <edge>       e, 
                      const node_id_t & src, 
                      const node_id_t & tgt)
  {
    sp_t <node> n_src;
    sp_t <node> n_tgt;

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
    std::map <node_id_t, sp_t <node> > :: iterator it;
    std::map <node_id_t, sp_t <node> > :: iterator begin = nodes_.begin ();
    std::map <node_id_t, sp_t <node> > :: iterator end   = nodes_.end ();

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

    std::cout << " dryun:  dag" << std::endl;

    std::map <node_id_t, sp_t <node> > :: iterator it;
    std::map <node_id_t, sp_t <node> > :: iterator begin = nodes_.begin ();
    std::map <node_id_t, sp_t <node> > :: iterator end   = nodes_.end ();

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
    std::cout << "fire   dag  " << std::endl;

    // dump_node ("INPUT");
    // dump_node ("OUTPUT");

    std::cout << std::string ("state: ") << state_to_string (state_) << std::endl;

    if ( Incomplete != state_ &&
         Pending    != state_ )
      return;

    state_ = Running;

    // ### scheduler hook
    scheduler_->hook_dag_run_pre ();


    // search for nodes which have resolved inputs (no peding edges), and
    // fire them.  Whenever a node finishes, it fires it outgoing edges.
    // If those finish copying their data, they'll fire those nodes which
    // they are incoming edges of.  Of those nodes happen to have all
    // input edges resolved, the fire will indeed lead to an execution of
    // that node, etc.
    //
    // if no nodes can be fired, complain.  Graph is probably cyclic.
    bool cyclic = true;

    std::map <node_id_t, sp_t <node> > :: iterator it;
    std::map <node_id_t, sp_t <node> > :: iterator begin = nodes_.begin ();
    std::map <node_id_t, sp_t <node> > :: iterator end   = nodes_.end ();

    for ( it = begin; it != end; it++ )
    {
      // std::cout << std::string ("       dag checks ") << it->second->get_name () 
      //           << ": " << state_to_string (it->second->get_state ()) 
      //           << std::endl;

      if ( Pending == it->second->get_state () )
      {
        std::cout << std::string (" ===   dag fires node ") 
                  << it->second->get_name () << std::endl;
        it->second->fire ();
        cyclic = false;
      }
    }

    if ( cyclic )
    {
      state_ = Failed;

      // ### scheduler hook
      scheduler_->hook_dag_run_fail ();

      // dump ();

      throw "can't find pending nodes.  cyclic or empty graph?";
    }

    // ### scheduler hook
    scheduler_->hook_dag_run_post ();
  }


  void dag::wait (void)
  {
    // ### scheduler hook
    scheduler_->hook_dag_wait ();

    state s = get_state ();
    while ( s != Done   && 
            s != Failed )
    {
      std::cout << "dag    waiting..." << std::endl;
      ::sleep (1);
      s = get_state ();
    }

    std::cout << "dag state is final - exit wait" << std::endl;
  }


  state dag::get_state (void)
  {
    // these states are final - we can return immediately
    if ( Failed  == state_ ||
         Done    == state_ )
    {
      std::cout << "get_state on final state" << std::endl;
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
      std::map <node_id_t, sp_t <node> > :: const_iterator it;
      std::map <node_id_t, sp_t <node> > :: const_iterator begin = nodes_.begin ();
      std::map <node_id_t, sp_t <node> > :: const_iterator end   = nodes_.end ();

      int i = 0;
      for ( it = begin; it != end; it++ )
      {
        if ( ! (i++ % 5) )
        {
          std::cout << std::endl;
        }

        state_total++;

        state s = it->second->get_state ();
        std::cout << it->first << ":" << state_to_string (s) <<  "\t";

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
    std::cout << std::endl;

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
          if ( ! (cnt++ % 2) )
          {
            // std::cout << std::endl;
          }

          state_total++;

          state s = it->second[i]->get_state ();
          // std::cout << it->second[i]->get_name_s () 
          //           << ":" << state_to_string (s) <<  "\t", false << std::endl;

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
    // std::cout << std::endl;


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
      scheduler_->hook_dag_run_done ();
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
    return;

    std::cout << " -  DAG    ----------------------------------\n" << std::endl;
    std::cout << std::string (" state: ") << state_to_string (get_state ()) << std::endl;

    std::cout << " -  NODES  ----------------------------------\n" << std::endl;
    {
      std::map <node_id_t, sp_t <node> > :: const_iterator it;
      std::map <node_id_t, sp_t <node> > :: const_iterator begin = nodes_.begin ();
      std::map <node_id_t, sp_t <node> > :: const_iterator end   = nodes_.end ();

      for ( it = begin; it != end; it++ )
      {
        it->second->dump ();
      }
    }
    std::cout << " -  NODES  ----------------------------------\n" << std::endl;

    std::cout << " -  EDGES  ----------------------------------\n" << std::endl;
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
    std::cout << " -  DAG    ----------------------------------\n" << std::endl;
  }


  void dag::dump_node (std::string name)
  {
    if ( nodes_.find (name) != nodes_.end () )
    {
      nodes_[name]->dump (true);
    }
  }


  void dag::schedule (void)
  {
    // ### scheduler hook
    scheduler_->hook_dag_schedule ();

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

} // namespace digedag

