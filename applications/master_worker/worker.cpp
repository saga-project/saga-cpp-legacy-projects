
#include "worker.hpp"

namespace saga_pm
{
  namespace master_worker
  {

    ////////////////////////////////////////////////////////////////////
    worker::worker (call_map_t call_map)
      : work_ (true)
    {
      saga::session s;

      LOG << "worker started: " << ::getpid ();

      call_map_ = call_map;

      // make sure that the call map has a quit entry
      // NOTE: not sure if a custom quite would make sense anyway...
      if ( call_map_.find ("quit") == call_map_.end () )
      {
        call_map_["quit"] = &worker::call_quit;
      }


      char* adurl_env = ::getenv ("SAGA_WORKER_AD");
      if ( NULL == adurl_env )
      {
        std::cout << "Could not get advert address from environment" << std::endl;
        LOG << "Could not get advert address from environment" << std::flush;
        exit (-1);
      }

      LOG << "worker advert address: " << adurl_env;

      // create the worker advert
      ad_ = advert  (saga::url (adurl_env));
      ad_.set_state (Started);
    }


    ////////////////////////////////////////////////////////////////////
    worker::~worker (void)
    {
    }



    ////////////////////////////////////////////////////////////////////
    void worker::run (void)
    {
      while ( work_ )
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

      work_ = false;

      argvec_t ret;
      return ret;
    }
    ////////////////////////////////////////////////////////////////////

  } // namespace master_worker

} // namespace saga_pm

