
#include <stdlib.h>
#include <unistd.h>

#include <complex>
#include <sstream>
#include <iostream>
#include <string>

#include "mandelbrot.hpp"


// well, we could be fancy and create a GUI to allow to set
// the user these parameters.  Or we could use a control file.
// Or a command line interface.  Or use the advert service.
// Or whatever.  But hey, it's just an example, right? ;-)

#define BOX_SIZE_X         750
#define BOX_SIZE_Y         100

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
      njobs_   (njobs), // number of compute jobs
      running_ (false)  // no jobs running, yet
{
  // check if we suport the requested device
  if ( odev_ == "x11" )
  {
    // initialize output device
    dev_  = new output_x11 (BOX_SIZE_X * BOX_NUM_X, // window size, x
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
    std::cout << "deleting output device" << std::endl;
    delete dev_;
    dev_ = NULL;
  }


  // Usually, we don't need to cancel jobs, as they'll terminate
  // when running out of work.  But in case we finish
  // prematurely, we take care of termination

  // count backwards, so that erase is not confusing the counter
  for ( int i = jobs_.size () - 1;
        i >= 0;
        i-- )
  {
    if ( saga::job::Running == jobs_[i].get_state () )
    {
      std::cout << "killing job " << i << "\n";
      jobs_[i].cancel ();
      jobs_.erase (jobs_.begin () + i);
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

  // create a job description, which can be reused for all jobs
  saga::job::description jd;
  jd.set_attribute (saga::job::attributes::description_executable, "/home/merzky/projects/saga/applications/mandelbrot/client/mandelbrot_client");

  // client parameters:
  // 0: path to advert directory to be used (job bucket)
  // 1: jobnum, == name of work bucket for that job
  std::vector <std::string> args (2);
  args[0] = job_bucket_name_;


  // create a session to create jobs in.  
  saga::session s;

  // add a number of credentials the job service can use.
  // saga::context c_ssh_1 ("ssh"); 
  // saga::context c_ssh_2 ("ssh"); 
  // saga::context c_ec2   ("ec2"); 
  
  // c_ssh_1.set_attribute (saga::attributes::context_userid, "merzky"); 
  // c_ssh_2.set_attribute (saga::attributes::context_userid, "amerzky"); 
  
  // s.add_context (c_ssh_1);
  // s.add_context (c_ssh_2);
  // s.add_context (c_ec2  );


  // create a list of job service URLs to be used.  Ideally, we get those from
  // service discovery - but for the time being, we just use this static list.
  std::vector <saga::url> job_service_urls;

  job_service_urls.push_back ("fork://localhost/");
//job_service_urls.push_back ("ec2://i-3d850354/");
//job_service_urls.push_back ("ssh://qb.loni.org/");
//job_service_urls.push_back ("ssh://gg101.cct.lsu.edu/");


  // from these URLs, create a set of job services.
  std::vector <saga::job::service> job_services;

  for ( unsigned int k = 0; k < job_service_urls.size (); k++ )
  {
    saga::job::service js  (s, job_service_urls[k]);
    job_services.push_back (js);
    std::cout << "created job service for " << job_service_urls[k] << std::endl;
  }



  // create the client jobs
  for ( unsigned int n = 0; n < njobs_; n++ )
  {
    // cycle over the job services
    saga::job::service js = job_services[n % job_services.size ()];

    // set second job parameter is the job's identifier (serial number)
    std::stringstream ident;
    ident << n + 1;
    args[1] = ident.str ();

    jd.set_vector_attribute (saga::job::attributes::description_arguments, args);

    // create and run a client job
    saga::job::job j = js.create_job (jd);
    j.run ();

    if ( saga::job::Running != j.get_state () )
    {
      throw "Could not start client\n";
    }


    // keep job
    jobs_.push_back (j);

    std::cout << "created job number " 
              << n + 1 << "/" << njobs_ 
              << " on " 
              << job_service_urls[n % job_service_urls.size ()] 
              << std::endl;
  }

  for ( unsigned int n = 0; n < njobs_; n++ )
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
        throw "Could not start client\n";
      }
      ::sleep (1);
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
      std::stringstream off_x;  off_x << PLANE_X_0 + x * plane_box_ext_x; // pixel offset     in x
      std::stringstream off_y;  off_y << PLANE_Y_0 + y * plane_box_ext_y; //                     y
      std::stringstream res_x;  res_x << plane_box_step_x;                // pixel resolution in x
      std::stringstream res_y;  res_y << plane_box_step_y;                //                     y
      std::stringstream num_x;  num_x << BOX_SIZE_X;                      // number of pixels in x
      std::stringstream num_y;  num_y << BOX_SIZE_Y;                      //                     y
      std::stringstream limit;  limit << LIMIT;                           // iteration limit for algorithm
      std::stringstream escap;  escap << ESCAPE;                          // escape boundary for algorithm

      // ...and store them in the work item advert.
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

      work << PLANE_X_0 + x * plane_box_ext_x   << " "; // pixel offset     in x
      work << PLANE_Y_0 + y * plane_box_ext_y   << " "; //                     y
      work << plane_box_step_x                  << " "; // pixel resolution in x
      work << plane_box_step_y                  << " "; //                     y
      work << BOX_SIZE_X                        << " "; // number of pixels in x
      work << BOX_SIZE_Y                        << " "; //                     y
      work << LIMIT                             << " "; // iteration limit for algorithm
      work << ESCAPE                            << " "; // escape boundary for algorithm

      // ...and store them in the work item advert.
      ad.set_attribute ("work", work.str ());
#endif // FAST_ADVERT

      // some attribs are always set, for the sake of the master itself
      std::stringstream box_x;  box_x << x;             // box location     in x
      std::stringstream box_y;  box_y << y;             //                     y
      std::stringstream j_num;  j_num << jobnum;        // job identifier
      std::stringstream ident;  ident << boxnum;        // box identifier
      std::stringstream j_id ;  ident << jobs_[jobnum - 1].get_job_id (); // job id

      ad.set_attribute ("box_x", box_x.str ());
      ad.set_attribute ("box_y", box_y.str ());
   // ad.set_attribute ("j_num", j_num.str ());
      ad.set_attribute ("ident", ident.str ());
      ad.set_attribute ("jobid", j_id.str ());


      // signal for work to do
      ad.set_attribute ("state", "work");

      // keep a list of active work items
      ads.push_back (ad);

      std::cout << "compute: assigned work item " << boxnum + 1
                << " to job " << jobnum << "\n";
    }
  }


  std::cout << "compute: job bucket: " << job_bucket_name_ << "\n";


  // all work items are assigned now.
  // wait for incoming boxes, and paint them as they get available.
  // completed work item adverts are deleted.
  while ( ads.size () )
  {
    std::cout << "compute: " << ads.size () << " open adverts: ";

    for ( int j = ads.size () - 1; j >= 0; j-- )
    {
      std::string s_ident (ads[j].get_attribute ("ident"));
      std::cout << s_ident << " ";
    }
    std::cout << "\n";


    // if no box is done at all, we sleep for a bit.  On anything else, we loop
    // again immediately.
    bool should_wait = true;

    for ( int j = ads.size () - 1; j >= 0; j-- )
    {
      if ( ads[j].get_attribute ("state") == "work" )
      {
        // nothing to do, go to sleep if that is true for all items
        // FIXME: polling is bad!
      }
      else if ( ads[j].get_attribute ("state") == "done" )
      {
        // get data, and paint
        std::string s_box_x (ads[j].get_attribute ("box_x"));
        std::string s_box_y (ads[j].get_attribute ("box_y"));
        std::string s_ident (ads[j].get_attribute ("ident"));
     // std::string s_j_num (ads[j].get_attribute ("j_num"));
        std::string s_jobid (ads[j].get_attribute ("jobid"));

        std::cout << "compute: work item " << s_ident
                  << " (" << s_jobid << ") done\n";

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
      ::sleep (1);
    }
  }
}

