
#ifndef SAGA_PM_MASTER_WORKER_UTILS_HPP
#define SAGA_PM_MASTER_WORKER_UTILS_HPP

#include <string>
#include <map>
#include <vector>

#include <saga/saga.hpp>

#define ADVERT_BASE_URL "advert://localhost/home/merzky/master_worker/"

#define LOG SAGA_LOG(SAGA_VERBOSE_LEVEL_CRITICAL)
// #define LOG std::cerr

#define TIMEOUT 5 // seconds to wait for worker to react on a task

namespace saga_pm
{
  namespace master_worker
  {
    ////////////////////////////////////////////////////////////////////
    
    class   master;
    class   worker;

    typedef unsigned long long int                               id_t;
    typedef std::string                                          arg_t;
    typedef std::vector <arg_t>                                  argvec_t;

    typedef argvec_t (*task_t)(void * thisptr, argvec_t args);
    typedef std::map <std::string, std::pair <void *, task_t> >  task_map_t;


    ////////////////////////////////////////////////////////////////////
    
    id_t            get_master_id (void);
    id_t            get_worker_id (void);

    bool            is_master     (void);
    bool            is_worker     (void);

    std::string     itoa          (unsigned long int i);

    static argvec_t noargs_;

    void * to_voidstar (void * dummy, ... );

    // worker state enum
    //
    // valid state transitions:
    //
    // initial states: Unknown - not explicitely set, assumed by master after worker startup
    // final   states: Quit    - set by worker, any time : internal error
    //
    // Unkown   -> Failed   (by worker) : unknown command, command failed, internal error
    // Unkown   -> Started  (by worker) : upon successfull startup, or after successful command execution
    // Started  -> Idle     (by master) : prepare after startup
    // Idle     -> Assigned (by master) : upon task assignment
    // Assigned -> Busy     (by worker) : upon task acceptance
    // Busy     -> Done     (by worker) : upon   successful task completion
    // Busy     -> Failed   (by worker) : upon unsuccessful task completion
    //                                    (unknown command, command failed, internal error)
    // Done     -> Idle     (by master) : reset after   successful or failed task
    // Failed   -> Idle     (by master) : reset after unsuccessful or failed task
    // Idle     -> Quit     (by worker) : successfully performed QUIT command
    enum state
    {
      Unknown  = 0,
      Started  = 1,
      Idle     = 2,
      Assigned = 3,
      Busy     = 4,
      Done     = 5, 
      Failed   = 6,
      Quit     = 7
    };

    std::string state_to_string (state       s);
    state       string_to_state (std::string s);

  } // namespace master_worker

} // namespace saga_pm

#endif // SAGA_PM_MASTER_WORKER_UTILS_HPP

