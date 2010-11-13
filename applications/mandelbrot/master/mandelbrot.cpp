
#include <stdlib.h>
#include <unistd.h>

#include <complex>
#include <sstream>
#include <iostream>
#include <string>

#include <saga/saga/adaptors/utils.hpp>

#include "mandelbrot.hpp"
#include "job_starter.hpp"


// well, we could be fancy and create a GUI to allow to set
// the user these parameters.  Or we could use a control file.
// Or a command line interface.  Or use the advert service.
// Or whatever.  But hey, it's just an example, right? ;-)

#define BOX_SIZE_X         600
#define BOX_SIZE_Y          80
 
#define BOX_NUM_X            2
#define BOX_NUM_Y           10

#define PLANE_X_0           -2
#define PLANE_Y_0           -1

#define PLANE_X_1           +1
#define PLANE_Y_1           +1

#define LIMIT              256
#define ESCAPE               4

// all job buckets are created under that advert directory, by
// appending the jobnum.
#define ADVERT_DIR         "/applications/mandelbrot/merzky"

///////////////////////////////////////////////////////////////////////
//
// constructor
//
mandelbrot::mandelbrot (std::string  odev,
                        unsigned int njobs)
    : odev_    (odev),  // output device to open
      njobs_   (njobs)  // number of compute jobs
{
  // check if we suport the requested device
  if ( odev_ == "x11" )
  {
    // initialize output device
    dev_  = new output_x11 (BOX_SIZE_X * BOX_NUM_X, // window size, x
                            BOX_SIZE_Y * BOX_NUM_Y, // window size, y
                            LIMIT);                 // number of colors
  }
  else if ( odev_ == "png" )
  {
    // initialize output device
    dev_  = new output_png (BOX_SIZE_X * BOX_NUM_X, // window size, x
                            BOX_SIZE_Y * BOX_NUM_Y, // window size, y
                            LIMIT);                 // number of colors
  }
  else
  {
    // device unsupported
    throw "only x11 output device supported at the moment";
  }


  // check if we got a valid output device
  if ( NULL == dev_ )
  {
    throw "could not init output device";
  }


  // to keep the job bucket name uniq, we append the POSIX job
  // id to the root advert dir.
  std::stringstream tmp;
  tmp << ADVERT_DIR << "/" << ::getpid ();

  job_bucket_name_ = tmp.str ();
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
  if ( NULL != dev_ )
  {
    std::cout << "closing output device" << std::endl;
    delete dev_;
    dev_ = NULL;
  }


  // Usually, we don't need to cancel jobs, as they'll terminate
  // when running out of work.  But in case we finish
  // prematurely, we take care of termination
  for ( unsigned int i = 0; i < jobs_.size (); i++)
  {
    std::cout << "killing job " << i << " (" 
              << jobs_[i].get_state () << ")" << std::endl;

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
  std::cout << "job_startup: starting " << njobs_ << " jobs\n";

  job_starter js (njobs_, job_bucket_name_);

  jobs_ = js.get_jobs ();


  for ( unsigned int n = 0; n < jobs_.size (); n++ )
  {
    // set second job parameter is the job's identifier (serial number)
    std::stringstream ident;
    ident << n + 1;

    std::cout << "waiting for job " << ident.str () << " to bootstrap\n";

    // make sure clients get up and running
    while ( ! job_bucket_.exists (ident.str ()) &&
            ! job_bucket_.is_dir (ident.str ()) )
    {
      if ( saga::job::Running != jobs_[n].get_state () )
      {
        std::cout << "job state: " << jobs_[n].get_state () << std::endl;
        throw "Could not start client\n";
      }
      ::sleep (1);
    }
  }
}



///////////////////////////////////////////////////////////////////////
//
// compute the mandelbrot set in the known boundaries.
//
// TODO: start njob_ client jobs, and gather results.
//
void mandelbrot::compute (void)
{
  std::cout << "compute!\n";

  // extent of complex plane to cover
  double plane_ext_x = PLANE_X_1 - PLANE_X_0;
  double plane_ext_y = PLANE_Y_1 - PLANE_Y_0;

  // extent of one box in complex plane
  double plane_box_ext_x = plane_ext_x / BOX_NUM_X;
  double plane_box_ext_y = plane_ext_y / BOX_NUM_Y;

  // step size for one box in complex plane (resolution)
  double plane_box_step_x = plane_box_ext_x / BOX_SIZE_X;
  double plane_box_step_y = plane_box_ext_y / BOX_SIZE_Y;

  std::vector <saga::advert::entry> ads;

  // Schedule all boxes in round robin fashion over the
  // available jobs
  for ( int x = 0; x < BOX_NUM_X; x++ )
  {
    for ( int y = 0; y < BOX_NUM_Y; y++ )
    {
      // serial number of box
      int boxnum = x * BOX_NUM_Y + y;

      // this boxed is assigned to jobs in round robin fashion
      int jobnum = (boxnum % jobs_.size ()) + 1;

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
      std::stringstream j_id ;  j_id << jobs_[jobnum - 1].get_job_id ();  // job id
      std::stringstream j_num;  j_num << jobnum;                          // job identifier
      std::stringstream ident;  ident << boxnum;                          // box identifier
      std::stringstream box_x;  box_x << x;                               // box location     in x
      std::stringstream box_y;  box_y << y;                               //                     y
      std::stringstream off_x;  off_x << PLANE_X_0 + x * plane_box_ext_x; // pixel offset     in x
      std::stringstream off_y;  off_y << PLANE_Y_0 + y * plane_box_ext_y; //                     y
      std::stringstream res_x;  res_x << plane_box_step_x;                // pixel resolution in x
      std::stringstream res_y;  res_y << plane_box_step_y;                //                     y
      std::stringstream num_x;  num_x << BOX_SIZE_X;                      // number of pixels in x
      std::stringstream num_y;  num_y << BOX_SIZE_Y;                      //                     y
      std::stringstream limit;  limit << LIMIT;                           // iteration limit for algorithm
      std::stringstream escap;  escap << ESCAPE;                          // escape boundary for algorithm

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
      std::stringstream work;  

      std::stringstream j_id ;  j_id << jobs_[jobnum - 1].get_job_id (); // job id

      // trim jobid for readability
      std::string j_id_s (j_id.str ());

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


      work << j_id_s                            << " "; // job identifier
      work << jobnum                            << " "; // job number
      work << boxnum                            << " "; // box identifier
      work << x                                 << " "; // box offset       in x
      work << y                                 << " "; //                     y
      work << PLANE_X_0 + x * plane_box_ext_x   << " "; // pixel offset     in x
      work << PLANE_Y_0 + y * plane_box_ext_y   << " "; //                     y
      work << plane_box_step_x                  << " "; // pixel resolution in x
      work << plane_box_step_y                  << " "; //                     y
      work << BOX_SIZE_X                        << " "; // number of pixels in x
      work << BOX_SIZE_Y                        << " "; //                     y
      work << LIMIT                             << " "; // iteration limit for algorithm
      work << ESCAPE                                  ; // escape boundary for algorithm

      // ...and store them in the work item advert.
      ad.set_attribute ("work", work.str ());
#endif // FAST_ADVERT


      // signal for work to do
      ad.set_attribute ("state", "work");

      // keep a list of active work items
      ads.push_back (ad);

      std::cout << "compute: assigned  work item " << boxnum + 1
                << " to job " << jobnum << "\n";
    }
  }


  std::cout << "compute: job bucket: " << job_bucket_name_ << "\n";


  // all work items are assigned now.
  // wait for incoming boxes, and paint them as they get available.
  // completed work item adverts are deleted.
  while ( ads.size () )
  {
    // std::cout << "compute: " << ads.size () << " open adverts: ";

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

        std::cout << "compute: work item " << s_ident
                  << "done (" << work << std::endl;

        // data from client
        std::stringstream ss_data (ads[j].get_attribute ("data"));

        // data to paint
        std::vector <std::vector <int> > data;

        // iterate over all lines in data set
        for ( int k = 0; k < BOX_SIZE_X; k++ )
        {
          std::vector <int> line;

          // iterate over all pixels in line
          for ( int l = 0; l < BOX_SIZE_Y; l++ )
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

        int box_off_x = box_x * BOX_SIZE_X;
        int box_off_y = box_y * BOX_SIZE_Y;


        std::string id = s_ident + " (" + s_jobid + ")";

        dev_->paint_box (box_off_x, BOX_SIZE_X,
                         box_off_y, BOX_SIZE_Y,
                         data, id);


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
      ::sleep (5);
    }
  }
}

