
#include <string>
#include <iostream>

#include <saga/saga.hpp>

#define DIR   "/debug/"

int main (int argc, char** argv)
{
  try
  {
    saga::advert::directory  dir = saga::advert::directory (DIR,
                                           saga::advert::Create        |
                                           saga::advert::Exclusive     |
                                           saga::advert::CreateParents |
                                           saga::advert::ReadWrite     );

    saga::job::description jd;
    jd.set_attribute (saga::job::attributes::description_executable,
                      "/Users/merzky/links/saga/applications/debug_advert/client/client");

    saga::job::service js ("fork://localhost/");

    saga::job::job job = js.create_job (jd);
    job.run ();

    if ( saga::job::Running != job.get_state () )
    {
      throw "Could not start client\n";
    }

    ::sleep (2);

    std::vector <saga::advert::entry> ads;

    for ( int x = 0; x < 100; x++ )
    {
      std::cout << "." << std::flush;
      std::stringstream advert_name;
      advert_name << x;

      // create a work item in the jobs work bucket
      saga::advert::entry ad = dir.open (advert_name.str (),
                                                 saga::advert::Create        |
                                                 saga::advert::CreateParents |
                                                 saga::advert::ReadWrite     );

      // signal for work to do
      ad.set_attribute ("state", "work");

      // keep a list of active work items
      ads.push_back (ad);
    }


    while ( ads.size () )
    {
      for ( int j = ads.size () - 1; j >= 0; j-- )
      {
        if ( ads[j].get_attribute ("state") == "done" )
        {
          ads[j].remove ();
          ads.erase (ads.begin () + j);
        }
      }
    }

    if ( saga::job::Running == job.get_state () )
    {
      std::cout << "killing job\n";
      job.cancel ();
    }
  }

  // catch exceptions from SAGA
  catch ( saga::exception const & e )
  {
    std::cerr << "SAGA Exception: " << e.what () << std::endl;
    return -1;
  }

  // catch exceptions from the mandelbrot class and the output device
  catch ( char const * s )
  {
    std::cerr << "Exception: " << s << std::endl;
    return -2;
  }

  // nothing to catch - declare success :-)
  return 0;
}


