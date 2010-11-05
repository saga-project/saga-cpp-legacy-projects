
#ifndef SAGA_PM_MASTER_WORKER_ADVERT_HPP
#define SAGA_PM_MASTER_WORKER_ADVERT_HPP

#include "util.hpp"

namespace saga_pm
{
  namespace master_worker
  {

    ////////////////////////////////////////////////////////////////////
    // private representation of worker job.  This class simplifies state
    // management on master side of things.
    //
    // worker adverts are rooted in the dir specified by the master, and are layed out 
    // as follows:
    //
    //   id     : worker id        (int)          // serial, assigned by master, written once by worker
    //   state  : worker state     (enum)         // see above - != job state!
    //   task   : work to do       (string)       // only valid on state == Assigned
    //   par_in : input parameters (vec <string>) // only valid on state == Assigned
    //   par_out: input parameters (vec <string>) // only valid on state == Done
    //   error  : exception        (string)       // only valid on state == Failed
    //
    //   FIXME: enforce state checks, even if costly
    //
    class advert
    {
      private:
        bool                ok_;
        bool                create_;
        state               s_;
        saga::job::service  js_;
        saga::job::job      job_;
        saga::url           url_;
        std::string         id_;
        saga::advert::entry ad_;

      public:
        advert  (void);
        advert  (saga::url          url);
        advert  (saga::job::service js, 
                 saga::job::job     job, 
                 saga::url          url);
        ~advert (void);


        bool           init        (void);
        void           run         (std::string c , 
                                    argvec_t    a );
        argvec_t       wait        (void          );
        void           purge       (void          );

        saga::job::job get_job     (void          );

        void           set_state   (state       s );    // called by master and worker
        state          get_state   (void          );    // called by master and worker

        void           set_task    (std::string t );    // called by master and worker
        std::string    get_task    (void          );    // called by master and worker

        void           set_par_in  (argvec_t    pi);    // called by master
        argvec_t       get_par_in  (void          );    // called by worker

        void           set_par_out (argvec_t    po);    // called by worker
        argvec_t       get_par_out (void          );    // called by master

        void           set_error   (std::string e );    // called by worker
        std::string    get_error   (void          );    // called by master
    };

  } // namespace master_worker

} // namespace saga_pm

#endif // SAGA_PM_MASTER_WORKER_ADVERT_HPP

