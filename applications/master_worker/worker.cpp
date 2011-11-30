
#include "worker.hpp"

namespace saga_pm
{
  namespace master_worker
  {
    ////////////////////////////////////////////////////////////////////
    worker::worker (saga::url  u, 
                    call_map_t call_map)
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


      LOG << "worker advert address: " << u;

      // create the worker advert
      ad_ = advert  (u);
      ad_.set_state (Started);
      LOG << "worker set to started: " << u;
    }


    ////////////////////////////////////////////////////////////////////
    worker::~worker (void)
    {
    }


    ////////////////////////////////////////////////////////////////////
    void worker::run (void)
    {
      LOG << "worker run";
      while ( work_ )
      {
        LOG << "worker running";
        ad_.dump ();

        bool busy = false;

        if ( ad_.get_state () == Assigned )
        {
          LOG << "worker: assigned";

          ad_.set_state (Busy);
          busy = true;

          LOG << "worker: busy";

          // FIXME: de-obfuscate!
          argvec_t    args = ad_.get_par_in ();
          argvec_t    ret;
          std::string task = ad_.get_task ();
          call_t      call = call_map_[task];

          LOG << "worker: call" << task;

          ret = (*this.*call)(args);

          ad_.set_par_out (ret);
          ad_.set_state   (Done);

          LOG << "worker: call done";
        }


        // avoid idle polling
        if ( busy ) busy = false;
        else        ::sleep (3);
      }
    }


    ////////////////////////////////////////////////////////////////////
    saga_pm::master_worker::argvec_t worker::call_quit (saga_pm::master_worker::argvec_t av)
    {
      std::cout << "Quit" << std::endl;

      work_ = false;

      ad_.set_state (Quit);

      argvec_t ret;
      return ret;
    }
    ////////////////////////////////////////////////////////////////////

  } // namespace master_worker

} // namespace saga_pm

