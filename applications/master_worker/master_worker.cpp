
#include <time.h>
#include <saga/saga/adaptors/utils.hpp>

#include "master_worker.hpp"


namespace saga_pm
{
  namespace master_worker
  {
    ////////////////////////////////////////////////////////////////////
    // static vars, should not be used by application
    static std::string master_id_   ("0");
    static std::string worker_id_   ("0");

    static bool        is_master_   (true);
    static bool        is_worker_   (false);

    static bool        initialized_ (false);

    std::string get_master_id (void)
    {
      char * id = ::getenv ("SAGA_MASTER_ID");

      if ( NULL == id )
      {
        return "0"; // the master, he owns itself
      }

      return id;
    }


    std::string get_worker_id (void)
    {
      char * id = ::getenv ("SAGA_WORKER_ID");

      if ( NULL == id )
      {
        return "-1"; // master gets id -1
      }

      // a worker id should always be non-negative
      if ( ::atoi (id) < 0 )
      {
        std::cout << "warning: invalid worker id" << std::endl;
      }

      return id;
    }

    void initialize_ (void)
    {
      if ( ! initialized_ )
      {
        initialized_ = true;

        master_id_ = get_master_id ();
        worker_id_ = get_worker_id ();

        if ( worker_id_ == "-1" )
        {
          is_master_ = true;
          is_worker_ = false;
        }
        else
        {
          is_master_ = false;
          is_worker_ = true;
        }
      }
    }

    bool is_master (void)
    {
      initialize_ ();
      return is_master_;
    }

    bool is_worker (void)
    {
      initialize_ ();
      return is_worker_;
    }


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
      }

      return "Unknown";
    }

    state string_to_state (std::string s)
    {
      if ( s == "Unknown" ) return Unknown; 
      if ( s == "Started" ) return Started; 
      if ( s == "Idle"    ) return Idle; 
      if ( s == "Assigned") return Assigned; 
      if ( s == "Busy"    ) return Busy; 
      if ( s == "Done"    ) return Done; 
      if ( s == "Failed"  ) return Failed; 
      if ( s == "Quit"    ) return Quit; 

      return Unknown;
    }

    std::string itoa (unsigned long int i)
    {
      std::stringstream ss;

      ss << i;

      return ss.str ();
    }

  } // namespace master_worker

} // namespace saga_pm

