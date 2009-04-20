
#ifndef DIGGEDAG_NODE_HPP
#define DIGGEDAG_NODE_HPP

#include "util/shared_ptr.hpp"
#include "node_impl.hpp"

namespace diggedag
{
  class edge;
  class node 
  {
    private:
      util::shared_ptr <impl::node> impl_;
      util::shared_ptr <impl::node> get_impl (void) const { return impl_; } 

      bool has_impl_;

    public:
      node (void)
        : has_impl_ (false)
      {
      }

      node (const diggedag::node_description & nd)
        : impl_  (new impl::node (nd))
        , has_impl_ (true)
      {
      }

      explicit node (const std::string cmd)
        : impl_ (new impl::node (cmd))
        , has_impl_ (true)
      {
      }

      node (const node & src)
        : impl_ (src.get_impl ())
        , has_impl_ (true)
      {
      }

      ~node (void)
      {
      }

      void check_ (void) const
      {
        if ( ! has_impl_ )
        {
          throw "Cannot call methods on an empty node";
        }
      }

      void add_edge_in (const edge & in)
      {
        check_ ();
        impl_->add_edge_in (in);
      }

      void set_name (const std::string & name)
      {
        check_ ();
        impl_->set_name (name);
      }

      void add_edge_out (const edge & out)
      {
        check_ ();
        impl_->add_edge_out (out);
      }

      void fire (void)
      {
        check_ ();
        impl_->fire ();
      }

      std::string get_name (void) const
      {
        check_ ();
        return impl_->get_name ();
      }

      diggedag::node_description get_description (void) const
      {
        check_ ();
        return impl_->get_description ();
      }

      diggedag::state get_state (void) const
      {
        check_ ();
        return impl_->get_state ();
      }
  };

} // namespace diggedag

#endif // DIGGEDAG_NODE_HPP

