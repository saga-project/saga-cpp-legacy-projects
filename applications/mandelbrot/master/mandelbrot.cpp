
#include <stdlib.h>
#include <unistd.h>

#include <complex>
#include <sstream>
#include <iostream>
#include <string>

#include <saga/saga/adaptors/utils.hpp>

#include "mandelbrot.hpp"
#include "job_starter.hpp"

#ifdef HAVE_X11
# include "output_x11.hpp"
#endif

#ifdef HAVE_PNG
# include "output_png.hpp"
#endif

namespace util 
{
  std::string itoa (int i)
  {
    std::stringstream ss;

    ss << i;

    return ss.str ();
  }
}




///////////////////////////////////////////////////////////////////////
//
// constructor
//
mandelbrot::mandelbrot (std::string  ini_file)
    : ini_file_ (ini_file)   // control file
{
  // first, set all parameters from the ini file
  saga::ini::ini     ini (ini_file_);
  saga::ini::section cfg = ini.get_section ("mandelbrot");

  // job bucket container
  advert_dir_         =         cfg.get_entry ("advert_dir" , "/tmp/");

  // mandelbrot algorithm parameters
  plane_x_0_          = ::atoi (cfg.get_entry ("plane_x_0" , " -2").c_str ());
  plane_y_0_          = ::atoi (cfg.get_entry ("plane_y_0" , " -1").c_str ());
  plane_x_1_          = ::atoi (cfg.get_entry ("plane_x_1" , " +1").c_str ());
  plane_y_1_          = ::atoi (cfg.get_entry ("plane_y_1" , " +1").c_str ());
  limit_              = ::atoi (cfg.get_entry ("limit"     , "256").c_str ());
  escape_             = ::atoi (cfg.get_entry ("escape"    , "  4").c_str ());

  // work item definitions (box == work item)
  box_size_x_         = ::atoi (cfg.get_entry ("box_size_x", "600").c_str ());
  box_size_y_         = ::atoi (cfg.get_entry ("box_size_y", " 80").c_str ());
  box_num_x_          = ::atoi (cfg.get_entry ("box_num_x" , "  2").c_str ());
  box_num_y_          = ::atoi (cfg.get_entry ("box_num_y" , " 10").c_str ());


  // check if we suport the requested device
  std::string use_out_dev_x11 = cfg.get_entry ("output_device_x11" , "no");
  std::string use_out_dev_png = cfg.get_entry ("output_device_png" , "no");


  //////////////////////////////////////////////////////////////////////
  //
  // init X11 device
  //
  if ( use_out_dev_x11 == "yes" )
  {
#ifdef HAVE_X11
    // initialize output device
    output_base * dev = new output_x11 (box_size_x_ * box_num_x_, // window size, x
                                        box_size_y_ * box_num_y_, // window size, y
                                        limit_);                  // number of colors
    if ( NULL == dev )
    {
      throw "could not init x11 output device";
    }

    odevs_.push_back (dev);
#else
    std::cerr << "warning: output device x11 is not supported" << std::endl;
#endif
  }


  //////////////////////////////////////////////////////////////////////
  //
  // init png device
  //
  if ( use_out_dev_png == "yes" )
  {
#ifdef HAVE_PNG
    std::string file = cfg.get_entry ("output_png_filename" , "mandelbrot.png");
    output_base * dev = new output_png (box_size_x_ * box_num_x_, // window size, x
                                        box_size_y_ * box_num_y_, // window size, y
                                        limit_,                   // number of colors
                                        file);                    // output file name
    if ( NULL == dev )
    {
      throw "could not init png output device";
    }

    odevs_.push_back (dev);
#else
    std::cerr << "warning: output device x11 is not supported" << std::endl;
#endif
  }


  if ( 0 == odevs_.size () )
  {
    throw "Could not find valid output device";
  }




  // to keep the job bucket name uniq, we append the POSIX job
  // id to the root advert dir.


  job_bucket_name_  = advert_dir_;
  job_bucket_name_ += "/";
  job_bucket_name_ += saga::get_default_session ().get_id ().string ();

  std::cout << "job bucket: " << job_bucket_name_ << std::endl;


  // create the application job bucket.  Fail if it exists
  // (Exclusive), as we don't want to spoil the buckets of other
  // running instances
  job_bucket_ = saga::advert::directory (job_bucket_name_,
                                         saga::advert::Create        |
                                         saga::advert::Exclusive     |
                                         saga::advert::CreateParents |
                                         saga::advert::ReadWrite     );

  // once the job bucket exists, we can start the jobs, and
  // create the individual work buckets
  job_startup ();
}



///////////////////////////////////////////////////////////////////////
//
// on destruction, we close the output device and terminate all
// jobs which did not end by themself.  Also, remove the job
// bucket, to keep the advert service free of trash.
//
mandelbrot::~mandelbrot (void)
{
  // close output device
  for ( unsigned int d = 0; d < odevs_.size (); d++ )
  {
    delete odevs_[d];
    odevs_[d] = NULL;
  }


  // Usually, we don't need to cancel jobs, as they'll terminate
  // when running out of work.  But in case we finish
  // prematurely, we take care of termination
  for ( unsigned int i = 0; i < jobs_.size (); i++)
  {
    std::cout << "killing   job " << i    << "(" 
              << jobs_[i].get_state  () << ") " 
              << jobs_[i].get_job_id () << std::endl;

    if ( saga::job::Running == jobs_[i].get_state () )
    {
      try
      {
        jobs_[i].cancel ();
      }
      catch ( const saga::exception & e )
      {
        // so what? ;-)
      }
    }
  }
}



///////////////////////////////////////////////////////////////////////
//
// start the client jobs, and setup the work buckets
//
void mandelbrot::job_startup (void)
{
  job_starter js (job_bucket_name_, ini_file_);

  jobs_ = js.get_jobs ();


  // make sure clients get up and running: 
  // wait 10 seconds for each job to registerS
  //
  // FIXME: make timeoutini parameter
  //
  int timeout = 2;
  int jobs_ok = 0;

  for ( unsigned int n = 0; n < jobs_.size (); n++ )
  {
    std::cout << "waiting  for job " << n << " to bootstrap... " << std::flush;

    int  time   = 0;
    bool check  = true;

    while ( check )
    {
      if ( ! job_bucket_.exists (util::itoa (n)) &&
           ! job_bucket_.is_dir (util::itoa (n)) )
      {
        saga::job::state s = jobs_[n].get_state ();

        if ( saga::job::Running != s )
        {
          std::cout << "failed (" << s << ")" << std::endl;
          jobs_[n].cancel ();
          check  = false;
        }

        if ( time > timeout )
        {
          std::cout << "timed out" << std::endl;
          jobs_[n].cancel ();
          check = false;
        }
        else
        {
          ::sleep (1);
          time++;
        }
      }
      else
      {
        std::cout << "ok" << std::endl;
        check = false;
        jobs_ok++;

        // FIXME: we should somehow mark these jobs as usable, to simplify the
        // assignment later (where we right now have to pull for job state
        // repeatedly)
      }
    }
  }

  if ( 0 == jobs_ok )
  {
    throw "Could create no usable jobs";
  }
}



///////////////////////////////////////////////////////////////////////
//
// compute the mandelbrot set in the known boundaries.
//
// TODO: start njob_ client jobs, and gather results.
//
int mandelbrot::compute (void)
{
  // extent of complex plane to cover
  double plane_ext_x = plane_x_1_ - plane_x_0_;
  double plane_ext_y = plane_y_1_ - plane_y_0_;

  // extent of one box in complex plane
  double plane_box_ext_x = plane_ext_x / box_num_x_;
  double plane_box_ext_y = plane_ext_y / box_num_y_;

  // step size for one box in complex plane (resolution)
  double plane_box_step_x = plane_box_ext_x / box_size_x_;
  double plane_box_step_y = plane_box_ext_y / box_size_y_;

  std::vector <saga::advert::entry> ads;

  // Schedule all boxes in round robin fashion over the
  // available jobs
  unsigned int boxes_scheduled = 0;
  unsigned int jobnum = 0;
  for ( int x = 0; x < box_num_x_; x++ )
  {
    for ( int y = 0; y < box_num_y_; y++ )
    {
      // serial number of box
      int boxnum = x * box_num_y_ + y;

      // this boxed is assigned to jobs in round robin fashion
      // so find next usable job (running state), and roll over 
      // at jobs_.size ();
      jobnum++;
      jobnum %= jobs_.size ();

      while ( jobs_[jobnum].get_state () != saga::job::Running )
      {
        jobnum++;
        jobnum %= jobs_.size ();
      }

      // the jobs work bucket is its jobnum, the work item advert
      // is simply numbered by its serial number, i
      std::stringstream advert_name;
      advert_name << jobnum << "/" << boxnum;

      // create a work item in the jobs work bucket
      saga::advert::entry ad = job_bucket_.open (advert_name.str (),
                                                 saga::advert::Create        |
                                                 saga::advert::CreateParents |
                                                 saga::advert::ReadWrite     );

#ifdef FAST_ADVERT
      // determine the work item parameters...
      std::stringstream j_id ;  j_id << jobs_[jobnum].get_job_id ();       // job id
      std::stringstream j_num;  j_num << jobnum;                           // job identifier
      std::stringstream ident;  ident << boxnum;                           // box identifier
      std::stringstream box_x;  box_x << x;                                // box location     in x
      std::stringstream box_y;  box_y << y;                                //                     y
      std::stringstream off_x;  off_x << plane_x_0_ + x * plane_box_ext_x; // pixel offset     in x
      std::stringstream off_y;  off_y << plane_y_0_ + y * plane_box_ext_y; //                     y
      std::stringstream res_x;  res_x << plane_box_step_x;                 // pixel resolution in x
      std::stringstream res_y;  res_y << plane_box_step_y;                 //                     y
      std::stringstream num_x;  num_x << box_size_x_;                      // number of pixels in x
      std::stringstream num_y;  num_y << box_size_y_;                      //                     y
      std::stringstream limit;  limit << limit_;                           // iteration limit for algorithm
      std::stringstream escap;  escap << escape_;                          // escape boundary for algorithm

      // trim jobid for readability
      std::string j_id_s (j_id.str ());

      if ( j_id_s.size () > 54 )
      {
        j_id_s.erase (55);

        j_id_s[51] = '.';
        j_id_s[52] = '.';
        j_id_s[53] = '.';
      }


      // ...and store them in the work item advert.
      ad.set_attribute ("jobid", j_id_s);
      ad.set_attribute ("j_num", j_num.str ());
      ad.set_attribute ("ident", ident.str ());
      ad.set_attribute ("box_x", box_x.str ());
      ad.set_attribute ("box_y", box_y.str ());
      ad.set_attribute ("off_x", off_x.str ());
      ad.set_attribute ("off_y", off_y.str ());
      ad.set_attribute ("res_x", res_x.str ());
      ad.set_attribute ("res_y", res_y.str ());
      ad.set_attribute ("num_x", num_x.str ());
      ad.set_attribute ("num_y", num_y.str ());
      ad.set_attribute ("limit", limit.str ());
      ad.set_attribute ("escap", escap.str ());

#else // FAST_ADVERT

      // trim jobid for readability
      std::string j_id_s = jobs_[jobnum].get_job_id ();

      if ( j_id_s.size () > 54 )
      {
        j_id_s.erase (55);

        j_id_s[51] = '.';
        j_id_s[52] = '.';
        j_id_s[53] = '.';
      }

      // make sure that jobid has no spaces!
      for ( unsigned int n = 0; n < j_id_s.size (); n++ )
        if ( j_id_s[n] == ' ' )
          j_id_s[n] = '_';


      std::stringstream work;  

      work << j_id_s                            << " "; // job identifier
      work << jobnum                            << " "; // job number
      work << boxnum                            << " "; // box identifier
      work << x                                 << " "; // box offset       in x
      work << y                                 << " "; //                     y
      work << plane_x_0_ + x * plane_box_ext_x   << " "; // pixel offset     in x
      work << plane_y_0_ + y * plane_box_ext_y   << " "; //                     y
      work << plane_box_step_x                  << " "; // pixel resolution in x
      work << plane_box_step_y                  << " "; //                     y
      work << box_size_x_                        << " "; // number of pixels in x
      work << box_size_y_                        << " "; //                     y
      work << limit_                             << " "; // iteration limit for algorithm
      work << escape_                                  ; // escape boundary for algorithm

      // ...and store them in the work item advert.
      ad.set_attribute ("work", work.str ());
#endif // FAST_ADVERT


      // signal for work to do
      ad.set_attribute ("state", "work");

      // keep a list of active work items
      ads.push_back (ad);

      std::cout << "assigning box " << boxnum
                << " to job " << jobnum << std::endl;

      boxes_scheduled++;
    }
  }



  // all work items are assigned now.
  // wait for incoming boxes, and paint them as they get available.
  // completed work item adverts are deleted.
  unsigned int waited     = 0;
  unsigned int timeout    = 2;
  unsigned int boxes_done = 0;
  while ( ads.size () )
  {
    // if no box is done at all, we sleep for a bit.  On anything else, we loop
    // again immediately.
    bool should_wait = true;

    for ( int j = ads.size () - 1; j >= 0; j-- )
    {
      if ( ads[j].get_attribute ("state") == "work" )
      {
        // nothing to do, go to sleep if that is true for all items:
        // should_sleep remains true
        //
        // FIXME: polling is bad!
      }
      else if ( ads[j].get_attribute ("state") == "done" )
      {
#ifdef FAST_ADVERT
        // get data, and paint
        std::string s_box_x (ads[j].get_attribute ("box_x"));
        std::string s_box_y (ads[j].get_attribute ("box_y"));
        std::string s_ident (ads[j].get_attribute ("ident"));
     // std::string s_j_num (ads[j].get_attribute ("j_num"));
        std::string s_jobid (ads[j].get_attribute ("jobid"));
#else // FAST_ADVERT
        std::string work (ads[j].get_attribute ("work"));

        std::vector <std::string> words = saga::adaptors::utils::split (work, ' ');

        if ( words.size () != 13 )
        {
          throw "Cannot parse work attribute!";
        }

        std::string s_jobid (words[0]);
        std::string s_j_num (words[1]);
        std::string s_ident (words[2]);
        std::string s_box_x (words[3]);
        std::string s_box_y (words[4]);
#endif // FAST_ADVERT

        // data from client
        std::stringstream ss_data (ads[j].get_attribute ("data"));

        // data to paint
        std::vector <std::vector <int> > data;

        // iterate over all lines in data set
        for ( int k = 0; k < box_size_x_; k++ )
        {
          std::vector <int> line;

          // iterate over all pixels in line
          for ( int l = 0; l < box_size_y_; l++ )
          {
            std::string num;
            ss_data >> num;
            line.push_back (::atoi (num.c_str ()));
          }

          data.push_back (line);
        }


        // print results via the output device
        int box_x     = ::atoi (s_box_x.c_str ());
        int box_y     = ::atoi (s_box_y.c_str ());

        int box_off_x = box_x * box_size_x_;
        int box_off_y = box_y * box_size_y_;


        std::string id = s_ident + " (" + s_jobid + ")";

        for ( unsigned int d = 0; d < odevs_.size (); d++ )
        {
          odevs_[d]->paint_box (box_off_x, box_size_x_,
                                box_off_y, box_size_y_,
                                data, id);
        }

        std::cout << "painting  box " << s_ident
                  << " done (" << work << std::endl;
        boxes_done++;

        // remove finished ad
        ads[j].remove ();
        ads.erase (ads.begin () + j);

        // may have more to do
        should_wait = false;
      }
    }

    // if there was nothing to do in the last round, we might as
    // well idle for a bit...
    if ( should_wait )
    {
      if ( waited > timeout )
      {
        std::cout << "waiting too long for more results - abort" << std::endl;
        return -1;
      }

      waited++;
      ::sleep (5);
    }
  }

  std::cout << boxes_done << " out of " << boxes_scheduled << " done" << std::endl;

  return 0;
}

