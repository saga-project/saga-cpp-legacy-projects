
#ifndef SAGA_PM_MASTER_WORKER_MASTER_HPP
#define SAGA_PM_MASTER_WORKER_MASTER_HPP

#include "util.hpp"
#include "advert.hpp"

namespace saga_pm
{
  namespace master_worker
  {

    ////////////////////////////////////////////////////////////////////
    class master
    {
      private:
        unsigned int              nworker_;
        std::string               worker_exe_;
        std::vector <std::string> worker_args_;
        std::vector <advert>      worker_ads_;
        std::string               master_ad_;
        saga::advert::directory   ad_;


      protected:
        int      worker_run  (std::string task, 
                              argvec_t    args);
        argvec_t worker_wait (int         tid);


      public:
        master (unsigned int              nworker, 
                std::string               worker_exe, 
                std::vector <std::string> worker_args);
        ~master (void);

        void shutdown (void);
    };
    ////////////////////////////////////////////////////////////////////

  } // namespace master_worker

} // namespace saga_pm

#endif // SAGA_PM_MASTER_WORKER_MASTER_HPP

