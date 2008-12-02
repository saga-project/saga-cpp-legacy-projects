
#include <stdlib.h>
#include <complex>
#include <sstream>
#include <iostream>
#include <string>

#include "mandelbrot.hpp"


#define BOX_SIZE_X   256
#define BOX_SIZE_Y   256

#define BOX_NUM_X      6
#define BOX_NUM_Y      4

#define PLANE_X_0     -2
#define PLANE_Y_0     -1

#define PLANE_X_1     +1
#define PLANE_Y_1     +1

#define LIMIT        256

#define ADVERT_DIR   "/applications/mandelbrot/merzky"


///////////////////////////////////////////////////////////////////////
//
// constructor
//
mandelbrot::mandelbrot (void)
    : odev_    ("x11"), // default output device
      dev_     (NULL),  // uninitialized output device
      njobs_   (1),     // default number of compute jobs
      running_ (false)  // no jobs running, yet
{
}



///////////////////////////////////////////////////////////////////////
//
// on destruction, simply close the output device
//
mandelbrot::~mandelbrot (void)
{
  if ( NULL != dev_ )
  {
    delete dev_;
    dev_ = NULL;
  }

  for ( int i = 0; i < jobs_.size (); i++ )
  {
    // jobs_[i].cancel ();
  }
}



///////////////////////////////////////////////////////////////////////
//
// initialize output device.  Close any previously opened device
//
void mandelbrot::set_output (std::string odev)
{
  if ( odev != "x11" )
  {
    throw "only x11 output device supported at the moment";
  }

  if ( NULL != dev_ )
  {
    // device already available
    return;
  }

  // initialize output device
  odev_ = odev;
  dev_  = new output_x11 (BOX_SIZE_X * BOX_NUM_X, 
                          BOX_SIZE_Y * BOX_NUM_Y);

  if ( NULL == dev_ )
  {
    throw "could not init output device";
  }
}



///////////////////////////////////////////////////////////////////////
//
// set number of jobs to use
//
// TODO: start njob_ client jobs, and gather results.
//
void mandelbrot::set_njobs (int n)
{
  // if jobs are running, do nothing
  if ( running_ )
  {
    std::cout << "set_njobs: jobs are running already\n";
    return;
  }

  // TODO: start jobs
  // create job services, and start jobs, until we have enough.  Note that the
  // mechanism below may create (JOBS_PER_SERVICE - 1) jobs too many.
  
  saga::job::description jd;
  jd.set_attribute (saga::job::attributes::description_executable, "/usr/bin/uname");

  // client parameters: 
  // 0: path to advert directory to be used
  // 1: jobid, == name of advert in advert directory
  std::vector <std::string> args (2);
  args[0] = ADVERT_DIR;


  unsigned int njobs = 0;

  while ( njobs < njobs_ )
  {
    saga::job::service js;

    for ( int i = 0; i < JOBS_PER_SERVICE; i++ )
    {
      // set second arg to individual job identifier (serial number)
      std::stringstream ident;
      ident << i; 
      args[1] = ident.str ();

      jd.set_vector_attribute (saga::job::attributes::description_arguments, args);

      // create and run a client job
      saga::job::job j = js.create_job (jd);
      j.run ();

      // TODO: check if job registers in advert service

      // keep job
      jobs_.push_back (j);

      njobs++;
      std::cout << "created job number " << njobs << " of " << njobs_ << std::endl;
    }
  }
  
  // flag that jobs are running
  running_ = true;
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

  // make sure some output device is available
  set_output (odev_);

  // make sure we have the appropriate number of compute jobs running
  set_njobs (njobs_);


  // extent of complex plane to cover
  double plane_extent_x = PLANE_X_1 - PLANE_X_0;
  double plane_extent_y = PLANE_Y_1 - PLANE_Y_0;

  // extent of one box in complex plane
  double plane_box_extent_x = plane_extent_x / BOX_NUM_X;
  double plane_box_extent_y = plane_extent_y / BOX_NUM_Y;

  // step size for one box in complex plane (resolution)
  double plane_box_step_x = plane_box_extent_x / BOX_SIZE_X;
  double plane_box_step_y = plane_box_extent_y / BOX_SIZE_Y;

  std::vector <work_t> work;

  // iterate over boxes
  for ( int x = 0; x < BOX_NUM_X; x++ )
  {
    for ( int y = 0; y < BOX_NUM_Y; y++ )
    {
      // box offset in the complex plane
      double plane_box_x_0 = PLANE_X_0 + x * plane_box_extent_x;
      double plane_box_y_0 = PLANE_Y_0 + y * plane_box_extent_y;

      work_t work_item;

      work_item.box_x = x;
      work_item.box_y = y;
      work_item.off_x = plane_box_x_0;
      work_item.off_y = plane_box_y_0;
      work_item.res_x = plane_box_step_x;
      work_item.res_y = plane_box_step_y;
      work_item.num_x = BOX_SIZE_X;
      work_item.num_y = BOX_SIZE_Y;
      work_item.limit = LIMIT;
      work_item.ident = x * BOX_NUM_Y + y;

      work.push_back (work_item);
      
      std::cout << "compute: created work item " << work_item.ident << "\n";
    }
  }

  // we have all work items.  Now schedule them in round robin fashion over the
  // available jobs
  std::vector <saga::advert::entry> ads;

  std::stringstream job_bucket_name; 
  job_bucket_name << ADVERT_DIR << "/";

  // create application job bucket.  Fail if it exists, as we don't want to
  // spoil the buckets of other running instances
  saga::advert::directory job_bucket (job_bucket_name.str (), 
                                      saga::advert::Create        | 
                                      saga::advert::Exclusive     | 
                                      saga::advert::CreateParents | 
                                      saga::advert::ReadWrite     );

  for ( int i = 0; i < work.size (); i++ )
  {
    int job_id = i % jobs_.size ();

    std::stringstream advert_name; 
    advert_name << job_id << "/" << i;

    // create application job bucket, and 
    saga::advert::entry ad = job_bucket.open (advert_name.str (), 
                                              saga::advert::Create        | 
                                              saga::advert::CreateParents | 
                                              saga::advert::ReadWrite     );

    std::stringstream box_x;  box_x << work[i].box_x;
    std::stringstream box_y;  box_y << work[i].box_y;
    std::stringstream off_x;  off_x << work[i].off_x;
    std::stringstream off_y;  off_y << work[i].off_y;
    std::stringstream res_x;  res_x << work[i].res_x;
    std::stringstream res_y;  res_y << work[i].res_y;
    std::stringstream num_x;  num_x << work[i].num_x;
    std::stringstream num_y;  num_y << work[i].num_y;
    std::stringstream limit;  limit << work[i].limit;
    std::stringstream ident;  ident << work[i].ident;

    ad.set_attribute ("box_x", box_x.str ());
    ad.set_attribute ("box_y", box_y.str ());
    ad.set_attribute ("off_x", off_x.str ());
    ad.set_attribute ("off_y", off_y.str ());
    ad.set_attribute ("res_x", res_x.str ());
    ad.set_attribute ("res_y", res_y.str ());
    ad.set_attribute ("num_x", num_x.str ());
    ad.set_attribute ("num_y", num_y.str ());
    ad.set_attribute ("limit", limit.str ());
    ad.set_attribute ("ident"   , ident.str ());

    // signal for work to do
    ad.set_attribute ("state", "work");

    ads.push_back (ad);

    std::cout << "compute: assigned work item " << work[i].ident 
              << " to job " << job_id << "\n";
  }


  // all work is distributed now.
  // now wait for incoming boxes, and paint them as they get available
  while ( ads.size () )
  {
    std::cout << "compute: open adverts: " << ads.size () << "\n";

    bool should_wait = true;

    for ( int j = ads.size () - 1; j >= 0; j-- )
    {
      if ( ads[j].get_attribute ("state") == "work" )
      {
        // nothing to do
        // FIXME: polling is bad!
      }
      else if ( ads[j].get_attribute ("state") == "failed" )
      {
        std::string s_id    (ads[j].get_attribute ("ident"));
        std::cout << "compute: advert " << s_id << " failed\n";

        // remove faulty ad
        ads.erase (ads.begin () + j);

        // may have more to do
        should_wait = false;
      }
      else if ( ads[j].get_attribute ("state") == "done" )
      {

        // get data, and paint
        std::string s_box_x (ads[j].get_attribute ("box_x"));
        std::string s_box_y (ads[j].get_attribute ("box_y"));
        std::string s_off_x (ads[j].get_attribute ("off_x"));
        std::string s_off_y (ads[j].get_attribute ("off_y"));
        std::string s_num_x (ads[j].get_attribute ("num_x"));
        std::string s_num_y (ads[j].get_attribute ("num_y"));

        std::string s_id    (ads[j].get_attribute ("ident"));
        std::cout << "compute: advert " << s_id << " done\n";


        std::stringstream sdata (ads[j].get_attribute ("data"));

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
            sdata >> num;
            line.push_back (::atoi (num.c_str ()));
          }

          data.push_back (line);
        }


        // output results
        int box_x     = ::atoi (s_box_x.c_str ());
        int box_y     = ::atoi (s_box_y.c_str ());

        int box_off_x = box_x * BOX_SIZE_X;
        int box_off_y = box_y * BOX_SIZE_Y;

        dev_->paint_box (box_off_x, BOX_SIZE_X,
                         box_off_y, BOX_SIZE_Y, data);


        // remove finished ad
        ads.erase (ads.begin () + j);

        // may have more to do
        should_wait = false;
      }
      else 
      {
        std::string s_id    (ads[j].get_attribute ("ident"));
        std::cout << "compute: advert " << s_id << " alienated\n";

        // keep alienated advert, as they are probably in some client internal
        // state.  We can clean up later...

        // may have more to do
        should_wait = false;
      }
    }

    if ( should_wait )
    {
      ::sleep (1);
    }
  }
}

