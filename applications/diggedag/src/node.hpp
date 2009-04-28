
#ifndef DIGGEDAG_NODE_HPP
#define DIGGEDAG_NODE_HPP

#include "util/shared_ptr.hpp"

#include "enum.hpp"
#include "node_description.hpp"


namespace diggedag
{
  namespace impl 
  {
    class dag;
    class node;
  }

  class edge;
  class dag;
  class node 
  {
    protected:
      my_shared_ptr <impl::node> impl_;
   // my_shared_ptr <impl::node> get_impl (void) const { return impl_; } 

    private:
      bool  has_impl_;
      void  check_ (void) const;


      protected:
        void set_dag (dag & d);
        friend class diggedag::impl::dag;
  

    public:
      node (void);
      node (const node_description & nd);
      node (const node             & src);
      explicit 
      node (const std::string cmd);
     ~node (void);

      void             add_edge_in     (const edge & in);
      void             set_name        (const std::string & name);
      void             add_edge_out    (const edge & out);
      void             fire            (void);
      std::string      get_name        (void) const;
      node_description get_description (void) const;
      state            get_state       (void) const;
  };

} // namespace diggedag

#endif // DIGGEDAG_NODE_HPP

