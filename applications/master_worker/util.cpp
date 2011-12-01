
#include <sstream>

#include <stdarg.h>

#include "util.hpp"


namespace saga_pm
{
  namespace master_worker
  {
    id_t get_master_id (void)
    {
      char * tmp = ::getenv ("SAGA_MASTER_ID");

      return (NULL == tmp ? 0 : ::atoi (tmp));
    }

    id_t get_worker_id (void)
    {
      char * tmp = ::getenv ("SAGA_WORKER_ID");
      return (NULL == tmp ? 0 : ::atoi (tmp));
    }

    bool is_master (void)
    {
      if ( 0 == get_worker_id () )
      {
        return true;
      }

      return false;
    }

    bool is_worker (void)
    {
      return ( ! is_master () );
    }

    std::string itoa (unsigned long int i)
    {
      std::stringstream ss;
      ss << i;
      return ss.str ();
    }

    // enum state
    // {
    //   Unknown  = 0,
    //   Started  = 1,
    //   Idle     = 2,
    //   Assigned = 3,
    //   Busy     = 4,
    //   Done     = 5, 
    //   Failed   = 6,
    //   Quit     = 7
    // };

    std::string state_to_string (state s)
    {
      switch ( s )
      {
        case Unknown : return "Unknown" ; break; 
        case Started : return "Started" ; break; 
        case Idle    : return "Idle"    ; break; 
        case Assigned: return "Assigned"; break; 
        case Busy    : return "Busy"    ; break; 
        case Done    : return "Done"    ; break; 
        case Failed  : return "Failed"  ; break; 
        case Quit    : return "Quit"    ; break;
        default      : return "Unknown" ; break;
      }
    }

    state string_to_state (std::string s)
    {
           if ( s == "Unknown" ) { return Unknown ; }
      else if ( s == "Started" ) { return Started ; }
      else if ( s == "Idle"    ) { return Idle    ; }
      else if ( s == "Assigned") { return Assigned; }
      else if ( s == "Busy"    ) { return Busy    ; }
      else if ( s == "Done"    ) { return Done    ; }
      else if ( s == "Failed"  ) { return Failed  ; }
      else if ( s == "Quit"    ) { return Quit    ; }
      else                       { return Unknown ; }
    }


    // somewhat ugly but fascinating trick to cast *anything* to a void*
    // Kudos Nitzan Shaked (http://www.codeproject.com/KB/cpp/Ellipses.aspx)
    void * to_voidstar (void * dummy, ... )
    {
      va_list list;
      void *  ret;

            va_start (list, dummy);
      ret = va_arg   (list, void *);
            va_end   (list);

      return ret;
    }


  } // namespace master_worker

} // namespace saga_pm

