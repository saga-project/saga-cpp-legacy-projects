
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
          argvec_t    ret;
          argvec_t    targs    = ad_.get_par_in ();
          std::string taskname = ad_.get_task ();
          void *      thisptr  = task_map_[taskname].first;
          task_t      task     = task_map_[taskname].second;

          LOG << "worker: task" << task;

          ret = task (thisptr, targs);

          LOG << "worker task done" << std::endl;

          ad_.set_par_out (ret);
          ad_.set_state   (Done);

          ad_.dump ();

          LOG << "worker: task done";
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
      std::cout << "Quit" << std::endl;

      work_ = false;

      ad_.set_state (Quit);

      return noargs_;
    }
    ////////////////////////////////////////////////////////////////////

  } // namespace master_worker

} // namespace saga_pm

