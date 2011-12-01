
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
        task_map_t task_map_;
        advert     ad_;
        bool       work_;

      protected:
        saga_pm::master_worker::argvec_t task_quit (saga_pm::master_worker::argvec_t av);
        saga_pm::master_worker::argvec_t task_run  (saga_pm::master_worker::argvec_t av);

      public:
        worker (saga::url u);
        virtual ~worker (void);

        void  run           (void);
        state get_state     (void);
        void  register_task (std::string name, void * thisptr, void * taskptr);
    };
    ////////////////////////////////////////////////////////////////////

  } // namespace master_worker

} // namespace saga_pm

#endif // SAGA_PM_MASTER_WORKER_HPP

