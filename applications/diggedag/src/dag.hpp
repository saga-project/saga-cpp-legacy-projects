
#ifndef DIGGEDAG_DAG_HPP
#define DIGGEDAG_DAG_HPP

#include "dag_impl.hpp"


namespace diggedag
{
  class dag 
  {
    private:
      util::shared_ptr <impl::dag> impl_;
      util::shared_ptr <impl::dag> get_impl (void) const { return impl_; } 


    public:
      dag (void) 
        : impl_ (new impl::dag ())
      { 
      }

      dag (const dag & src) 
        : impl_ (src.get_impl ())
      { 
      }

      ~dag (void) 
      {
      }

      void add_node (node & n)
      {
        impl_->add_node (n);
      }

      void add_edge (edge & e, node & src, node & tgt)
      {
        impl_->add_edge (e, src, tgt);
      }

      void fire (void)
      {
        impl_->fire ();
      }

      state get_state (void)
      {
        return impl_->get_state ();
      }

  };

} // namespace diggedag

#endif // DIGGEDAG_DAG_HPP

