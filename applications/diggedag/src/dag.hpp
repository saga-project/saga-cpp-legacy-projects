
#ifndef DIGGEDAG_DAG_HPP
#define DIGGEDAG_DAG_HPP

#include "util/shared_ptr.hpp"

#include "enum.hpp"
#include "node.hpp"
#include "edge.hpp"
#include "scheduler.hpp"


namespace diggedag
{
  namespace impl
  {
    class dag;
  }

  class dag 
  {
    protected:
      my_shared_ptr <impl::dag> impl_;
   // my_shared_ptr <impl::dag> get_impl (void) const { return impl_; } 

    private:
      bool  has_impl_;
      void  check_ (void) const;

    protected:
      void set_state (state s);
      friend class diggedag::impl::scheduler;


    public:
      dag (bool flag);
      dag (void);
      dag (const dag & src);
     ~dag (void);

      void add_node   (const std::string & name, 
                       node              & node);
      void add_edge   (edge & e, 
                       node & src, 
                       node & tgt);
      void add_edge   (edge              & e, 
                       const std::string & src, 
                       const std::string & tgt);
      void  fire      (void);
      void  wait      (void);
      state get_state (void);
      void  dump      (void);
      void  schedule  (void);

      scheduler get_scheduler (void);
  };

} // namespace diggedag

#endif // DIGGEDAG_DAG_HPP

