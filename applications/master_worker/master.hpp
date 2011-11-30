
#ifndef SAGA_PM_MASTER_WORKER_MASTER_HPP
#define SAGA_PM_MASTER_WORKER_MASTER_HPP

#include "util.hpp"
#include "worker_description.hpp"
#include "advert.hpp"

namespace saga_pm
{
  namespace master_worker
  {

    ////////////////////////////////////////////////////////////////////
    class master
    {
      // FIXME:
      //  - use map to quickly find workers by id
      
      private:
        typedef std::map <id_t, advert> admap_t;

        bool                            initialized_;
        id_t                            max_id_;
        std::string                     session_;
        std::string                     ad_url_;
        saga::advert::directory         ad_;
        admap_t                         ads_;


      public:
        master  (void);
        ~master (void);

        void               initialize         (std::string session = "");
        void               shutdown           (void);

        id_t               worker_start       (worker_description & d);
        void               worker_stop        (id_t id);
        std::vector <id_t> worker_list        (void);

        void               worker_run         (std::string task, 
                                               id_t        id = 0);
        void               worker_run         (std::string task, 
                                               argvec_t    args, 
                                               id_t        id = 0);
        state              worker_get_state   (id_t id);
        std::string        worker_get_error   (id_t id);
        std::string        worker_get_task    (id_t id);
        argvec_t           worker_get_args    (id_t id);
        argvec_t           worker_get_results (id_t id);
        void               worker_wait        (id_t id); // ?
        void               worker_dump        (id_t id);
    };
    ////////////////////////////////////////////////////////////////////

  } // namespace master_worker

} // namespace saga_pm

#endif // SAGA_PM_MASTER_WORKER_MASTER_HPP

