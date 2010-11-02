
#ifndef SAGA_PM_MASTER_WORKER_ADVERT_HPP
#define SAGA_PM_MASTER_WORKER_ADVERT_HPP

#include "master_worker.hpp"

namespace saga_pm
{
  namespace master_worker
  {

    ////////////////////////////////////////////////////////////////////
    //
    // worker adverts are rooted in the dir specified by the master, and are layed out 
    // as follows:
    //
    //   id     : worker id        (int)          // serial, assigned by master, written only once by worker
    //   state  : worker state     (enum)         // see above - != job state!
    //   task   : work to do       (string)       // only valid on state == Assigned
    //   par_in : input parameters (vec <string>) // only valid on state == Assigned
    //   par_out: input parameters (vec <string>) // only valid on state == Done
    //   error  : exception        (string)       // only valid on state == Failed
    //
    class advert
    {
      private:
        std::string         id_;
        saga::url           url_;
        saga::advert::entry ad_;

      public:
        advert  (void);
        advert  (saga::url      u, 
                 bool           create = false);
        advert  (const advert & orig);
        ~advert (void);

        std::string               get_id      (void                        );    // called by master

        state                     get_state   (void                        );    // called by master and worker
        void                      set_state   (state                     s );    // called by master and worker

        std::string               get_task    (void                        );    // called by master and worker
        void                      set_task    (std::string               t );    // called by master and worker

        std::vector <std::string> get_par_in  (void                        );    // called by worker
        void                      set_par_in  (std::vector <std::string> pi);    // called by master

        std::vector <std::string> get_par_out (void                        );    // called by master
        void                      set_par_out (std::vector <std::string> po);    // called by worker

        std::string               get_error   (void                        );    // called by master
        void                      set_error   (std::string               e );    // called by worker
    };
    ////////////////////////////////////////////////////////////////////

  } // namespace master_worker

} // namespace saga_pm

#endif // SAGA_PM_MASTER_WORKER_ADVERT_HPP

