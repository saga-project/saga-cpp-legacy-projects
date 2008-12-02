
#include <string>
#include <complex>
#include <sstream>
#include <iostream>

#include <saga/saga.hpp>

int main (int argc, char** argv)
{
  if ( argc < 3 )
  {
    std::cout << "usage: " << argv[0] << " <advert_dir> <job_id>" << std::endl;
    return -1;
  }

  try
  {
    std::string advert_root (argv[1]);
    std::string job_id      (argv[2]);

    // open application job bucket.  Fail if that does not exist, as it means
    // that the master did not yet run
    saga::advert::directory app_dir (advert_root, 
                                     saga::advert::Create    | 
                                     saga::advert::ReadWrite );

    // open this job's work item bucket.  
    // Create if that does not exist yet, because master got delayed
    saga::advert::directory job_dir = app_dir.open_dir (job_id, 
                                                        saga::advert::Create    | 
                                                        saga::advert::ReadWrite );

    // work forever
    while ( true )
    {
      // find work ads for this job
      std::vector <saga::url> work_ads = job_dir.list ();

      // wait for work 
      // TODO: replace with notification
      if ( 0 == work_ads.size () )
      {
        std::cout << "waiting for work (1)\n";
        :: sleep (1);
        continue;
      }

      std::cout << "client: found " << work_ads.size () << " work ads\n";

      // found some ads.  Now pick those which are flagged active, and work on
      // them
      for ( int i = 0; i < work_ads.size (); i++ )
      {
        // TODO: this loop circles over 'done' items forever, until the master
        // deletes those...

        saga::advert::entry ad = job_dir.open (work_ads[i], saga::advert::ReadWrite);

        // still an active item?
        if ( ad.get_attribute ("state") == "work" )
        {
          std::cout << "found work in " << work_ads[i].get_path () << "\n";

          // signal successful startup
          ad.set_attribute ("state", "started");

          if ( ! ad.attribute_exists ("off_x") ||
               ! ad.attribute_exists ("off_y") ||
               ! ad.attribute_exists ("res_x") ||
               ! ad.attribute_exists ("res_y") ||
               ! ad.attribute_exists ("num_x") ||
               ! ad.attribute_exists ("num_y") ||
               ! ad.attribute_exists ("limit") )
          {
            // signal failure
            std::cout << "cannot handle request\n";
            ad.set_attribute ("state", "bad_request");
          }
          else
          {
            double off_x = ::atof (ad.get_attribute ("off_x").c_str ());
            double off_y = ::atof (ad.get_attribute ("off_y").c_str ());
            double res_x = ::atof (ad.get_attribute ("res_x").c_str ());
            double res_y = ::atof (ad.get_attribute ("res_y").c_str ());
            double num_x = ::atof (ad.get_attribute ("num_x").c_str ());
            double num_y = ::atof (ad.get_attribute ("num_y").c_str ());
            int    limit = ::atoi (ad.get_attribute ("limit").c_str ());

            std::cout << "handling request\n";

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

                  if ( abs (Z) > 2.0 )
                    break;
                }

                // store the number of iteration needed to escape the limit
                data << iter << " ";
              }
            }

            // signal work done
            ad.set_attribute ("data",  data.str ());
            ad.set_attribute ("state", "done");

          } // if ad looks like a complete work item

        } // if ad.state == "work"

      } // for all ads in job_bucket

    } // while true
  
  }
  catch ( saga::exception const & e )
  {
    std::cerr << "SAGA exception: " << e.what () << "\n";
    return -2;
  }

  return 0;
}


