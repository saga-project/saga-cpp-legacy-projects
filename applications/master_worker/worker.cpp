
#include "worker.hpp"

namespace saga_pm
{
  namespace master_worker
  {
    ////////////////////////////////////////////////////////////////////
    worker::worker (saga::url  u)
      : work_ (true)
    {
      saga::session s;

      LOG << "worker started: " << ::getpid ();

      // make sure that the task map has a run and quit entry.
      // a custom quit most likely would only make sense if there is also
      // a custom run.
      register_task ("quit", this, to_voidstar (0, &worker::task_quit));
      register_task ("run",  this, to_voidstar (0, &worker::task_run));

      // init worker advert
      LOG << "worker advert address: " << u;
      ad_ = advert  (u);
    }

    void worker::register_task (std::string name, void * thisptr, void * taskptr)
    {
      task_t task = (task_t) taskptr;
      task_map_[name] = std::pair <void*, task_t> (thisptr, task);
    }

    ////////////////////////////////////////////////////////////////////
    worker::~worker (void)
    {
    }

    void worker::run (void)
    {
      void * thisptr = task_map_["run"].first;
      task_t task    = task_map_["run"].second;

      LOG << "worker: calls run " << typeid (thisptr).name () << std::endl;

      task (thisptr, noargs_);
    }

    state worker::get_state (void)
    {
      return ad_.get_state ();
    }

    ////////////////////////////////////////////////////////////////////
    argvec_t worker::task_run (argvec_t args)
    {
      LOG << "worker run";

      ad_.set_state (Started);
      LOG << "worker set to Started";

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
          std::string task_name   = ad_.get_task ();
          argvec_t    task_args   = ad_.get_par_in ();
          argvec_t    task_result = noargs_;
          
          // check if such task is registered
          if ( task_map_.find (task_name) == task_map_.end () )
          {
            ad_.set_error   ("Unknown task");
            ad_.set_state   (Failed);
          }
          else
          {
            void *      thisptr     = task_map_[task_name].first;
            task_t      task        = task_map_[task_name].second;

            LOG << "worker: task " << task_name;

            try
            {
              task_result = task (thisptr, task_args);
              ad_.set_par_out (task_result);

              if ( ad_.get_state () != Quit )
              {
                ad_.set_state   (Done);
              }

              LOG << "worker task done" << std::endl;
            }
            catch ( ... )
            {
              LOG << "worker task failed" << std::endl;
            }
          }
        }

        // avoid idle polling
        if ( busy ) busy = false;
        else        ::sleep (TIMEOUT);
      }

      LOG << "worker run done";

      return args;
    }


    ////////////////////////////////////////////////////////////////////
    argvec_t worker::task_quit (argvec_t av)
    {
      LOG << "worker is quitting";

      work_ = false;

      ad_.set_state (Quit);

      return noargs_;
    }
    ////////////////////////////////////////////////////////////////////

  } // namespace master_worker

} // namespace saga_pm

