
#ifndef DIGGEDAG_NODE_HPP
#define DIGGEDAG_NODE_HPP

#include "node_impl.hpp"

namespace diggedag
{
  class node 
  {
    private:
      util::shared_ptr <impl::node> impl_;
      util::shared_ptr <impl::node> get_impl (void) const { return impl_; } 

      std::string name_;

    public:
      node (const std::string                  name, 
            const diggedag::node_description & nd)
            
        : impl_ (new impl::node (name, nd)), 
          name_ (name)
      {
      }

      node (const std::string name, 
            const std::string cmd)
            
        : impl_ (new impl::node (name, cmd)), 
          name_ (name)
      {
      }

      node (const node & src)
        : impl_ (src.get_impl ()),
          name_ (src.get_name ())
      {
      }

      ~node (void)
      {
      }

      void add_edge_in (const edge & in)
      {
        impl_->add_edge_in (in);
      }

      void add_edge_out (const edge & out)
      {
        impl_->add_edge_out (out);
      }

      void fire (void)
      {
        impl_->fire ();
      }

      std::string get_name (void) const
      {
        return impl_->get_name ();
      }

      diggedag::state get_state (void) const
      {
        return impl_->get_state ();
      }
  };

} // namespace diggedag

#endif // DIGGEDAG_NODE_HPP

