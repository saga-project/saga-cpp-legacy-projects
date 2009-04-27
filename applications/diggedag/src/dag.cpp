

#include "dag.hpp"
#include "dag_impl.hpp"

namespace diggedag
{
  dag::dag (void) 
    : impl_ (new impl::dag (*this))
  { 
  }

  dag::dag (const dag & src) 
    : impl_ (src.get_impl ())
  { 
  }

  dag::~dag (void) 
  {
  }

  void dag::add_node (const std::string & name, 
                      node              & node)
  {
    node.set_name (name);
    impl_->add_node (name, node);
  }

  void dag::add_edge (edge & e, 
                      node & src, 
                      node & tgt)
  {
    impl_->add_edge (e, src, tgt);
  }

  // add edges to named nodes
  void dag::add_edge (edge              & e, 
                      const std::string & src, 
                      const std::string & tgt)
  {
    impl_->add_edge (e, src, tgt);
  }

  void dag::fire (void)
  {
    impl_->fire ();
  }

  void dag::wait (void)
  {
    impl_->wait ();
  }

  state dag::get_state (void)
  {
    return impl_->get_state ();
  }

  void dag::dump (void)
  {
    impl_->dump ();
  }

  void dag::schedule (void)
  {
    impl_->schedule ();
  }

  scheduler dag::get_scheduler (void)
  {
    return impl_->get_scheduler ();
  }

} // namespace diggedag

