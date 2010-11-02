
#include "worker.hpp"

namespace saga_pm
{
  namespace master_worker
  {

    ////////////////////////////////////////////////////////////////////
    worker::worker (void)
    {
      std::cout << "worker started" << std::endl;
    }


    ////////////////////////////////////////////////////////////////////
    worker::~worker (void)
    {
    }


    ////////////////////////////////////////////////////////////////////
    void worker::init (call_map_t call_map)
    {
      std::cout << " worker: init " << std::endl;
      call_map_ = call_map;

      if ( call_map_.find ("quit") == call_map_.end () )
      {
        call_map_["quit"] = &worker::call_quit;
      }


      char* adurl_env = ::getenv ("SAGA_WORKER_AD");
      if ( NULL == adurl_env )
      {
        exit (-1);
      }

      saga::url adurl = std::string (adurl_env);

      std::cout << "adurl: " << adurl << std::endl;

      // create that advert URL
      ad_ = advert  (adurl, true);

      ad_.set_state (Started);
    }


    ////////////////////////////////////////////////////////////////////
    void worker::run (void)
    {
      while ( true )
      {
        bool busy = false;

        if ( ad_.get_state () == Assigned )
        {
          ad_.set_state (Busy);
          busy = true;

          // FIXME: de-obfuscate!
          argvec_t    args = ad_.get_par_in ();
          argvec_t    ret;
          std::string task = ad_.get_task ();
          call_t      call = call_map_[task];

          ret = (*this.*call)(args);

          ad_.set_par_out (ret);
          ad_.set_state   (Done);

        }


        // avoid idle polling
        if ( busy ) busy = false;
        else        ::sleep (1);
      }
    }


    ////////////////////////////////////////////////////////////////////
    saga_pm::master_worker::argvec_t worker::call_quit (saga_pm::master_worker::argvec_t av)
    {
      ad_.set_state (Quit);
      std::cout << "Quit" << std::endl;

      ::exit (0);
    }
    ////////////////////////////////////////////////////////////////////

  } // namespace master_worker

} // namespace saga_pm

