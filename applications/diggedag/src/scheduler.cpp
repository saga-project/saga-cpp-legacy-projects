
#include "dag.hpp"
#include "scheduler.hpp"
#include "scheduler_impl.hpp"


namespace diggedag
{
  scheduler::scheduler (void)
    : impl_ (new impl::scheduler ())
  {
  }


  scheduler::scheduler (const scheduler & src)
    : impl_ (src.get_impl ())
  {
  }


  scheduler::~scheduler (void)
  {
  }


  void scheduler::run (diggedag::dag & d)
  {
    impl_->run (d);
  }

} // namespace diggedag

