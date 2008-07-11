
#ifndef FRAMEWORK_HPP
#define FRAMEWORK_HPP

#include <saga/saga.hpp>

#include "workload.hpp"
#include "logger.hpp"


// this class is a container for a workload.  All saga functionality is
// encapsulated here - the workload itself operates locally.
class framework 
{
  private:
    logger & log_;

    std::string name_;

    saga::url in_;
    saga::url out_;
    saga::url lfn_;

    std::fstream f_log_; // log file

    void prepare_data  (void);
    void collect_data  (void);
    void migrate       (void);

  public:

    framework (logger & log, int argc, char** argv);
   ~framework (void);

    // run takes workload, and executes it.  For that, it first prepares the
    // wl's input and output data, and runs its main work() routine.  If the wl
    // then signals that it is not yet done (test()), job_self is migrated to
    // a new location.
    void run (workload & w);
};

#endif // FRAMEWORK_HPP

