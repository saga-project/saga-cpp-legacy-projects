
#include <string>
#include <complex>
#include <sstream>
#include <iostream>

#include <saga/saga.hpp>
#include <saga/saga/adaptors/utils.hpp>

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
    bool busy      = true;
    bool work_done = false;

    // avoid busy wait on idle looping
    bool last_loop_was_idle = false;

    while ( busy )
    {
      if ( last_loop_was_idle )
      {
        ::sleep (5);
      }

      last_loop_was_idle = true;

      // find work ads for this job
      std::vector <saga::url> work_ads = job_dir.list ();

      // wait for work 
      // TODO: replace with notification
      if ( 0 == work_ads.size () )
      {
        l.log ("waiting for work (1)\n");
        :: sleep (5);

        if ( work_done ) 
        {
          // if we found work items previously, we continue to work 'til they are
          // finished.  Once the work bucket is empty though, we stop.
          busy = false;
        }
        continue;
      }

      std::stringstream ss;
      ss << "client: found " << work_ads.size () << " work ads\n";
      l.log (ss.str ().c_str ());

      // found some ads.  Now pick those which are flagged active, and work on
      // them
      for ( unsigned int i = 0; i < work_ads.size (); i++ )
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
            
#ifdef FAST_ADVERT
            double off_x = ::atof (ad.get_attribute ("off_x").c_str ());
            double off_y = ::atof (ad.get_attribute ("off_y").c_str ());
            double res_x = ::atof (ad.get_attribute ("res_x").c_str ());
            double res_y = ::atof (ad.get_attribute ("res_y").c_str ());
            double num_x = ::atof (ad.get_attribute ("num_x").c_str ());
            double num_y = ::atof (ad.get_attribute ("num_y").c_str ());
            int    limit = ::atoi (ad.get_attribute ("limit").c_str ());
            int    escap = ::atoi (ad.get_attribute ("escap").c_str ());
#else // FAST_ADVERT
            std::string work = ad.get_attribute ("work");
            std::vector <std::string> words = saga::adaptors::utils::split (work, ' ');

            if ( words.size () < 9 )
            {
              l.log ("Cannot parse work attribute!");
              l.log (work.c_str ());
              exit  (-1);
            }

            double off_x = ::atof (words[0].c_str ());
            double off_y = ::atof (words[1].c_str ());
            double res_x = ::atof (words[2].c_str ());
            double res_y = ::atof (words[3].c_str ());
            double num_x = ::atof (words[4].c_str ());
            double num_y = ::atof (words[5].c_str ());
            int    limit = ::atoi (words[6].c_str ());
            int    escap = ::atoi (words[7].c_str ());
#endif // FAST_ADVERT

            l.log ("handling request\n");

            // point in complex plane to iterate over is (c0, c1)

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

            // flag that we did some work.  (a) we don't sleep before next work
            // item check, and also, once we run out of work, we can terminate
            work_done          = true;
            last_loop_was_idle = false;

          } // if ad.state == "work"

          ad.close ();
        }
        catch ( saga::exception const & e )
        {
          // this advert failed for some reason.  
          // Simply continue with the next one.
          l.log ("SAGA Exception for advert op");
          l.log (e.what ());
          ::sleep (1); // relax chance of race conditions.
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


