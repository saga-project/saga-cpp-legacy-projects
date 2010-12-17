
#include <string>
#include <complex>
#include <sstream>
#include <iostream>

#include <saga/saga.hpp>
#include <saga/saga/adaptors/utils.hpp>

#include "../version.hpp"

namespace util 
{
  std::string itoa (int i)
  {
    std::stringstream ss;

    ss << i;

    return ss.str ();
  }
}


int main (int argc, char** argv)
{

  if ( argc < 3 )
  {
    std::cerr << "usage: " << argv[0] << " <advert_dir> <job_id>" << std::endl;
    return -1;
  }

  try
  {
    std::string advert_root (argv[1]); // work bucket container
    std::string job_id      (argv[2]); // my job id == id of work bucket
    std::string cmd;                   // any special mission command

    if ( argc > 3 )
    {
      cmd = argv[3];

      if ( cmd != "awol" &&
           cmd != "lazy" &&
           cmd != "joke" &&
           cmd != "bomb" )
      {
        std::cerr << "unknown command " << cmd << " given - exit" << std::endl;
        return -2;
      }
    }

    if ( cmd == "awol" )
    {
      while ( true )
        ::sleep (600);
    }


    // open application job bucket.  Fail if that does not exist, as it means
    // that the master did not yet run
    saga::advert::directory app_dir (advert_root, 
                                     saga::advert::ReadWrite );

    // we check the version number against what the master registered, and
    // discontinue on mismatch
    if ( app_dir.attribute_exists ("version") )
    {
      std::string version = app_dir.get_attribute ("version");

      if ( version != SAGA_MANDELBROT_VERSION )
      {
        std::cerr << "version mismatch: " 
                  << SAGA_MANDELBROT_VERSION << " != " << version 
                  << " - exit" << std::endl;
        return -3;
      }
    }
    else
    {
      // bail out - thats obviously an ancient version w/o version support
      std::cerr << "Version mismatch: " 
                << SAGA_MANDELBROT_VERSION << " != ??? - exit" << std::endl;
      return -4;
    }

    // create this job's work item bucket.  
    // That signals the master that we are up and running.  Thus, if it already
    // exists, we fail.
    saga::advert::directory job_dir = app_dir.open_dir (job_id, 
                                                        saga::advert::Create    | 
                                                     // saga::advert::Exclusive | 
                                                        saga::advert::ReadWrite );

    // add a version tag, so that a master can ensure version matching. 
    // FIXME: Alas, that leads to a race condition, as long as the advert 
    // service does not support locking (or atomic move).
    job_dir.set_attribute ("version", SAGA_MANDELBROT_VERSION);

    // we registered, now we can sleep if that was requested
    if ( cmd == "lazy" )
    {
      while ( true )
        :: sleep (600);
    }

    std::cout << "cmd: " << cmd << std::endl;

    // so now is a good time to bomb our job item directory - randomly remove
    // some adverts
    if ( cmd == "bomb" )
    {

      std::cout << "bombing" << std::endl;
      try 
      {
        // sleep a little to increase chances for a hit (i.e. wait for adverts)  
        // the advert assignment can be slow, so the exact sleeptime is guesswork...
        ::sleep (10);

        // seed random number generator
        struct timeval tv;
        ::gettimeofday (&tv, NULL);
        ::srand (tv.tv_sec + tv.tv_usec);

        std::vector <saga::url> victims = job_dir.list ();

        std::cout << "victims: " << victims.size () << std::endl;

        for ( unsigned int i = 0; i < victims.size (); i++ )
        {
          std::cout << "bombing victim : " << i << std::endl;
          if ( ::rand () % 2 )  // ~50% chance
          {
            // kaboom!
            std::cout << " bomb hit    target " << i << " >:-)" << std::endl;
            saga::advert::entry victim = job_dir.open (victims[i], saga::advert::ReadWrite);
            victim.set_attribute ("state", "bombed");
          }
          else
          {
            std::cout << " bomb missed target " << i << " >:-(" << std::endl;
          }
        }
      }
      catch ( const saga::exception & e )
      {
        // ignore errors
        std::cout << " bomb mailfunction - mision aborted :-((\n" << e.what () << std::endl;
      }
    }


    // so, we found the job dir, and can pull the static work item data
    double plane_x_0  = ::atof (app_dir.get_attribute ("plane_x_0" ).c_str ());
    double plane_y_0  = ::atof (app_dir.get_attribute ("plane_y_0" ).c_str ());
    double plane_x_1  = ::atof (app_dir.get_attribute ("plane_x_1" ).c_str ());
    double plane_y_1  = ::atof (app_dir.get_attribute ("plane_y_1" ).c_str ());
    int    img_size_x = ::atoi (app_dir.get_attribute ("img_size_x").c_str ());
    int    img_size_y = ::atoi (app_dir.get_attribute ("img_size_y").c_str ());
    int    box_num_x  = ::atoi (app_dir.get_attribute ("box_num_x" ).c_str ());
    int    box_num_y  = ::atoi (app_dir.get_attribute ("box_num_y" ).c_str ());
    int    limit      = ::atoi (app_dir.get_attribute ("limit"     ).c_str ());
    int    escap      = ::atoi (app_dir.get_attribute ("escape"    ).c_str ());

    std::cout << " plane_x_0        : " << plane_x_0  << std::endl;
    std::cout << " plane_y_0        : " << plane_y_0  << std::endl;
    std::cout << " plane_x_1        : " << plane_x_1  << std::endl;
    std::cout << " plane_y_1        : " << plane_y_1  << std::endl;
    std::cout << " img_size_x       : " << img_size_x << std::endl;
    std::cout << " img_size_y       : " << img_size_y << std::endl;
    std::cout << " box_num_x        : " << box_num_x  << std::endl;
    std::cout << " box_num_y        : " << box_num_y  << std::endl;
    std::cout << " limit            : " << limit      << std::endl;
    std::cout << " escap            : " << escap      << std::endl;


    int box_size_x = (int) floor (img_size_x / box_num_x);
    int box_size_y = (int) floor (img_size_y / box_num_y);

    std::cout << " box_size_x       : " << box_size_x  << std::endl;
    std::cout << " box_size_y       : " << box_size_y  << std::endl;

    // extent of complex plane to cover
    double plane_ext_x = plane_x_1 - plane_x_0;
    double plane_ext_y = plane_y_1 - plane_y_0;

    std::cout << " plane_ext_x      : " << plane_ext_x  << std::endl;
    std::cout << " plane_ext_y      : " << plane_ext_y  << std::endl;

    // extent of one box in complex plane
    double plane_box_ext_x = plane_ext_x / box_num_x;
    double plane_box_ext_y = plane_ext_y / box_num_y;

    std::cout << " plane_box_ext_x  : " << plane_box_ext_x  << std::endl;
    std::cout << " plane_box_ext_y  : " << plane_box_ext_y  << std::endl;

    // step size for one box in complex plane (resolution)
    double plane_box_step_x = plane_box_ext_x / box_size_x;
    double plane_box_step_y = plane_box_ext_y / box_size_y;

    std::cout << " plane_box_step_x : " << plane_box_step_x  << std::endl;
    std::cout << " plane_box_step_y : " << plane_box_step_y  << std::endl;

    // work as long as there is work
    bool busy      = true;
    bool work_done = false;

    // avoid busy wait on idle looping
    bool should_wait = false;

    // after n rounds of having nothing todo, the client finishes
    unsigned int idle_rounds = 0;

    while ( busy )
    {
      if ( should_wait )
      {
        idle_rounds++;

        if ( idle_rounds > 1200 ) 
        {
          // after 20 minutes idling, we say goodbye
          busy = false;
          std::cout << "client: no work found - exit" << std::endl;
          continue;
        }

        ::sleep (1);
      }
      else
      {
        // reset idle counter
        idle_rounds = 0;
      }


      // by default we wait, unless there is work to do
      should_wait = true;

      // find work ads for this job
      std::vector <saga::url> work_ads = job_dir.list ();

      // wait for work 
      // TODO: replace with notification
      if ( 0 == work_ads.size () )
      {
        if ( work_done ) 
        {
          // if we found work items previously, we continue to work 'til they are
          // finished.  Once the work bucket is empty though, we stop.
          std::cout << "client: ran out or work - exit" << std::endl;
          busy = false;
        }
        else
        {
          should_wait = true;
          std::cout << "client: waiting for work" << std::endl;
        }
        continue;
      }

      std::cout << "client: found " << work_ads.size () << " work ads" << std::endl;

      // found some ads.  Now pick those which are flagged active, and work on
      // them
      for ( unsigned int i = 0; i < work_ads.size (); i++ )
      {
        // FIXME: this loop circles over 'done' items forever, until the master
        // deletes those.  Better move them somewhere else, and let the master
        // open them there?  But hey, move doesn't work wither in the current
        // advert service *sigh*

        // we have to try/catch a rase condition: completed work items may get
        // deleted by the master, and we will throw when accessing them.  We
        // catch, and simply continue with the next item.
        //
        // FIXME: the current advert service impl seems to be lazy in state
        // updates though, so we may get incorrect state results (dir lingers
        // but stays empty).
        try 
        {
          saga::advert::entry ad = job_dir.open (work_ads[i], saga::advert::ReadWrite);

          // still an active item?
          if ( ad.get_attribute ("state") == "work" )
          {
            // get box id to work on
            int boxnum = boost::lexical_cast <int> (ad.get_attribute ("boxnum"));

            // box indicee coordinates
            int box_x  =              boxnum % box_num_y ;
            int box_y  = (int) floor (boxnum / box_num_y);

            std::cout << " boxnum           : " << boxnum 
                      << " (" << box_x << ", "  << box_y << ")" << std::endl;

            // we use boxnum to compute the box offsets in the complex plane
            double plane_box_off_x = plane_x_0 + box_y * plane_box_ext_x;
            double plane_box_off_y = plane_y_0 + box_x * plane_box_ext_y;


            // point in complex plane to iterate over is (c0, c1)

            // data to paint
            std::stringstream data;

            // iterate over all pixels in complex plane
            for ( int x = 0; x < box_size_x; x++ )
            {
              // x coordinate of pixel in complex plane (real part)
              double c0 = plane_box_off_x + x * plane_box_step_x;

              if ( cmd == "joke" )
              {
                // mirror box :-P
                c0 = plane_box_off_x + (box_size_x - x) * plane_box_step_x;
              }

              for ( int y = 0; y < box_size_y; y++ )
              {
                // y coordinate of pixel in complex plane (imaginary part)
                double c1 = plane_box_off_y + y * plane_box_step_y;

                if ( cmd == "joke" )
                {
                  // mirror box :-P
                  c1 = plane_box_off_y + (box_size_y - y) * plane_box_step_y;
                }

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
            work_done   = true;
            should_wait = false;

          } // if ad.state == "work"

          ad.close ();
        }
        catch ( saga::exception const & e )
        {
          // this advert failed for some reason.  
          // Simply continue with the next one.
          std::cout << "client: SAGA Exception for advert op" << std::endl;
          std::cout << e.what () << std::endl;
          ::sleep (1); // relax chance of race conditions.
        }

      } // for all ads in job_bucket

    } // while true
  }
  catch ( saga::exception const & e )
  {
    std::cerr << "SAGA exception: " << e.what () << std::endl;
    return -5;
  }

  return 0;
}


