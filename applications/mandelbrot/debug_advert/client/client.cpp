
#include <string>
#include <complex>
#include <sstream>
#include <iostream>

#include <saga/saga.hpp>

#define DIR "/debug/"

#include "logger.hpp"

int main (int argc, char** argv)
{

  logger l (std::string ("file://localhost/tmp/client.0.log"));

  try
  {
    saga::advert::directory dir (DIR, saga::advert::ReadWrite );

    while ( true )
    {
      std::vector <saga::url> work_ads = dir.list ();

      for ( unsigned int i = 0; i < work_ads.size (); i++ )
      {
        l.log ("handling ad");
        try 
        {
          saga::advert::entry ad = dir.open (work_ads[i], saga::advert::ReadWrite);

          if ( ad.attribute_exists ("state") &&
               ad.get_attribute    ("state") == "work" )
          {
            ad.set_attribute ("state", "done");
            ad.set_attribute ("data", "0 1 2");
          } 

          ad.close ();
        }
        catch ( saga::exception const & e )
        {
          l.log ("SAGA Exception for advert op");
          l.log (e.what ());
        }

      } 

    } 

  }
  catch ( saga::exception const & e )
  {
    std::cerr << "SAGA exception: " << e.what () << "\n";
    l.log ("SAGA Exception");
    l.log (e.what ());
    return -2;
  }

  return 0;
}


