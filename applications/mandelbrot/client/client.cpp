
#include <string>
#include <complex>
#include <sstream>
#include <iostream>

#include <saga/saga.hpp>

#include "logger.hpp"

int main (int argc, char** argv)
{

  if ( argc < 3 )
  {
    std::cerr << "usage: " << argv[0] << " <advert_dir> <job_id>\n";
    return -1;
  }

  std::stringstream ss;
  ss << "file://localhost/tmp/client." << argv[2] << ".log";
  logger l (ss.str ());

  try
  {
    std::string advert_root (argv[1]);
    std::string job_id      (argv[2]);

    // open application job bucket.  Fail if that does not exist, as it means
    // that the master did not yet run
    saga::advert::directory app_dir (advert_root, 
                                     saga::advert::Create    | 
                                     saga::advert::ReadWrite );

    // create this job's work item bucket.  
    // That signals the master that we are up and running.  Thus, if it already
    // exists, we fail.
    saga::advert::directory job_dir = app_dir.open_dir (job_id, 
                                                        saga::advert::Create    | 
                                                        saga::advert::Exclusive | 
                                                        saga::advert::ReadWrite );

    // work as long as there is work
    bool wait_for_work = true;
    bool work_done     = false;

    ::sleep (15);

    while ( wait_for_work )
    {
      // find work ads for this job
      std::vector <saga::url> work_ads = job_dir.list ();

      // wait for work 
      // TODO: replace with notification
      if ( 0 == work_ads.size () )
      {
        l.log ("waiting for work (1)\n");
        :: sleep (1);

        if ( work_done ) 
        {
          // if we found work items previously, we continue to work 'til they are
          // finished.  Once the work bucket is empty though, we stop.
          wait_for_work = false;
        }
        continue;
      }

      std::stringstream ss;
      ss << "client: found " << work_ads.size () << " work ads\n";
      l.log (ss.str ().c_str ());

      // found some ads.  Now pick those which are flagged active, and work on
      // them
      for ( int i = 0; i < work_ads.size (); i++ )
      {
        // TODO: this loop circles over 'done' items forever, until the master
        // deletes those.  Better move them somewhere else, and let the master
        // open them there?

        // we have to try/catch a rase condition: completed work items may get
        // deleted by the master, and we will throw when accessing them.  We
        // catch, and simply continue with the next item.
        try 
        {
          saga::advert::entry ad = job_dir.open (work_ads[i], saga::advert::ReadWrite);

          // still an active item?
          if ( ad.get_attribute ("state") == "work" )
          {
            std::stringstream ss;
            ss << "found work in " << work_ads[i].get_path () << "\n";
            l.log (ss.str ().c_str ());

            double off_x = ::atof (ad.get_attribute ("off_x").c_str ());
            double off_y = ::atof (ad.get_attribute ("off_y").c_str ());
            double res_x = ::atof (ad.get_attribute ("res_x").c_str ());
            double res_y = ::atof (ad.get_attribute ("res_y").c_str ());
            double num_x = ::atof (ad.get_attribute ("num_x").c_str ());
            double num_y = ::atof (ad.get_attribute ("num_y").c_str ());
            int    limit = ::atoi (ad.get_attribute ("limit").c_str ());
            int    escap = ::atoi (ad.get_attribute ("escap").c_str ());
            int    jobid = ::atoi (ad.get_attribute ("jobid").c_str ());

            l.log ("handling request\n");

            // point in complex plane to iterate over
            double c0;
            double c1;

            // data to paint
            std::stringstream data;

            // iterate over all pixels in complex plane
            for ( int x = 0; x < num_x; x++ )
            {
              // x coordinate of pixel in complex plane (real part)
              double c0 = off_x + x * res_x;

              for ( int y = 0; y < num_y; y++ )
              {
                // y coordinate of pixel in complex plane (imaginary part)
                double c1 = off_y + y * res_y;

                std::complex <double> C (c0, c1);
                std::complex <double> Z (.0, .0); // initial value for iteration Z

                int iter;

                for ( iter = 0; iter <= limit; iter++ )
                {
                  Z = Z * Z + C;

                  if ( abs (Z) > escap )
                    break;
                }

                // store the number of iteration needed to escape the limit
                data << iter << " ";
              }
            }

            // signal work done
            ad.set_attribute ("data",  data.str ());
            ad.set_attribute ("state", "done");

            // flag that we did some work.  So, once we run out of work, we
            // can terminate
            work_done = true;

          } // if ad.state == "work"

          ad.close ();
        }
        catch ( saga::exception const & e )
        {
          // this advert failed for some reason.  
          // Simply continue with the next one.
          l.log ("SAGA Exception for advert op");
          l.log (e.what ());
        }

      } // for all ads in job_bucket

    } // while true

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


