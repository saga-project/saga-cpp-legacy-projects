
#include "node.hpp"
#include "node_impl.hpp"

namespace diggedag
{
  node::node (void)
    : has_impl_ (false)
  {
  }

  node::node (const diggedag::node_description & nd)
    : impl_  (new impl::node (nd))
    , has_impl_ (true)
  {
    impl_->set_node (*this);
  }

  node::node (const std::string cmd)
    : impl_ (new impl::node (cmd))
    , has_impl_ (true)
  {
    impl_->set_node (*this);
  }

  node::node (const node & src)
    : impl_ (src.impl_)
    , has_impl_ (true)
  {
  }

  node::~node (void)
  {
  }

  void node::check_ (void) const
  {
    if ( ! has_impl_ )
    {
      throw "Cannot call methods on an empty node";
    }
  }

  void node::add_edge_in (const edge & in)
  {
    check_ ();
    impl_->add_edge_in (in);
  }

  void node::set_name (const std::string & name)
  {
    check_ ();
    impl_->set_name (name);
  }

  void node::add_edge_out (const edge & out)
  {
    check_ ();
    impl_->add_edge_out (out);
  }

  void node::fire (void)
  {
    check_ ();
    impl_->fire ();
  }

  std::string node::get_name (void) const
  {
    check_ ();
    return impl_->get_name ();
  }

  diggedag::node_description node::get_description (void) const
  {
    check_ ();
    return impl_->get_description ();
  }

  diggedag::state node::get_state (void) const
  {
    check_ ();
    return impl_->get_state ();
  }

  void node::set_dag (dag & d)
  {
    check_ ();
    impl_->set_dag (d);
  }

} // namespace diggedag

