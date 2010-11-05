
#ifndef SAGA_PM_MASTER_WORKER_WORKER_HPP
#define SAGA_PM_MASTER_WORKER_WORKER_HPP

#include "util.hpp"
#include "advert.hpp"

namespace saga_pm
{
  namespace master_worker
  {

    ////////////////////////////////////////////////////////////////////
    class worker
    {
      private:
        call_map_t call_map_;
        advert          ad_;

      protected:
        void init (call_map_t call_map);

        saga_pm::master_worker::argvec_t call_quit (saga_pm::master_worker::argvec_t av);

      public:
        worker (void);
        virtual ~worker (void);

        void run (void);

    };
    ////////////////////////////////////////////////////////////////////

  } // namespace master_worker

} // namespace saga_pm

#endif // SAGA_PM_MASTER_WORKER_HPP

