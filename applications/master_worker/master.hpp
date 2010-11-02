
#ifndef SAGA_PM_MASTER_WORKER_MASTER_HPP
#define SAGA_PM_MASTER_WORKER_MASTER_HPP

#include "master_worker.hpp"
#include "w.hpp"

namespace saga_pm
{
  namespace master_worker
  {

    ////////////////////////////////////////////////////////////////////
    class master
    {
      private:
        saga::job::service       js_;
        unsigned int             nworker_;
        std::string              worker_exe_;
        std::vector <w>          worker_;


      protected:
        int      worker_run  (std::string task, 
                              argvec_t    args);
        argvec_t worker_wait (int         tid);


      public:
        master (unsigned int nworker, 
                std::string  worker_exe);
        ~master (void);

        void shutdown (void);
    };
    ////////////////////////////////////////////////////////////////////

  } // namespace master_worker

} // namespace saga_pm

#endif // SAGA_PM_MASTER_WORKER_MASTER_HPP

