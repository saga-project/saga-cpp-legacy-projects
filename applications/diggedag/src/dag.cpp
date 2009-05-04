
#include "dag.hpp"

namespace diggedag
{
  dag::dag (void)
    : state_ (Pending)
  { 
    // std::cout << "create dag " << std::endl;
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
      std::cout << "NULL node: " << name << std::endl;
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
    std::cout << "fire   dag  " << std::endl;

    // dump_node ("INPUT");
    // dump_node ("OUTPUT");

    if ( Pending != state_ )
    {
      return;
    }

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
        std::cout << "       dag fires node " << (*it).second->get_name () << std::endl;
        (*it).second->fire ();
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
      std::cout << "dag    waiting..." << std::endl;
      ::sleep (1);
      s = get_state ();
    }

    std::cout << "dag state is final - exit wait\n";
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
        // std::cout << std::endl;
      }

      state_total++;

      state s = (*it).second->get_state ();
      // std::cout << (*it).first << ":" << state_to_string (s) <<  "\t" << std::flush;

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
    // std::cout << std::endl;

    for ( unsigned int i = 0; i < edges_.size (); i++ )
    {
      if ( ! (i % 10) )
      {
        // std::cout << std::endl;
      }

      state_total++;

      state s = edges_[i]->get_state ();
      // std::cout << "edge[" << i << "] :" << state_to_string (s) <<  "\t" << std::flush;

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

    std::cout << " -  DAG    ----------------------------------\n";
    std::cout << " state: " << state_to_string (get_state ()) << " \n";

    std::cout << " -  NODES  ----------------------------------\n";
    for ( it = begin; it != end; it++ )
    {
      (*it).second->dump ();
    }
    std::cout << " -  NODES  ----------------------------------\n";

    std::cout << " -  EDGES  ----------------------------------\n";
    for ( unsigned int i = 0; i < edges_.size (); i++ )
    {
      edges_[i]->dump ();
    }
    std::cout << " -  EDGES  ----------------------------------\n";
    std::cout << " -  DAG    ----------------------------------\n";
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
    std::cout << " ### dag::schedule" << std::endl;

    // ### scheduler hook
    scheduler_->hook_dag_schedule (this);

    // clean up
    prune ();
  }


  // prune removes edges which exist twice.  Depending on the DAG, that may be
  // correct or not, but let's do that _significant_ optimization, and deal with
  // special cases as they arrive.
  //
  // don't do that for nodes: most likely, nodes are expected to run multiple
  // times...
  void dag::prune (void)
  {
    std::cout << " ### pruning graph" << std::endl;
    std::vector <diggedag::edge *> new_edges;

    for ( unsigned int i = 0; i < edges_.size (); i++ )
    {
      bool unique = true;

      for ( unsigned int j = i + 1; j < edges_.size (); j++ )
      {
        if ( *edges_[i] == *edges_[j] )
        {
          unique = false;
          break;
        }
      }

      if ( unique )
      {
        new_edges.push_back (edges_[i]);
      }
    }

    edges_ = new_edges;
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

