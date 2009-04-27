
#ifndef DIGGEDAG_SCHEDULER_IMPL_HPP
#define DIGGEDAG_SCHEDULER_IMPL_HPP

#include <vector>

#include <saga/saga.hpp>

#include "util/thread.hpp"


namespace diggedag
{
  class dag;
  namespace impl
  {
    class scheduler 
    {
      private:
        // diggedag::dag dag_;
        // bool          have_dag_;

      public:
         scheduler (void);
        ~scheduler (void);

        void run   (diggedag::dag & d);
    };

  } // namespace impl

} // namespace diggedag

#endif // DIGGEDAG_SCHEDULER_IMPL_HPP

