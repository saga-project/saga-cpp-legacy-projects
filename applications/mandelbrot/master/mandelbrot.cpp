
#include <stdlib.h>
#include <unistd.h>

#include <complex>
#include <sstream>
#include <iostream>
#include <string>

#include <saga/saga/adaptors/utils.hpp>

#include "mandelbrot.hpp"

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
mandelbrot::mandelbrot (void)
{
  ////////////////////////////////////////////////////////////////////
  // mandelbrot assumes it runs in some working directory.  
  // That is where all output will be stored.  
  //
  // Output files are:
  //
  //   stdout
  //   stderr
  //   backend.<backend_name>.log  (conmtains ini and status info)
  //   mandelbrot.png              (result)
  //   mandelbrot.ini              (copy of ini file)
  //
  // output dir defaults to pwd, but can be overwritten by the env
  // SAGA_MANDELBROT_PWD
  char * pwd_env = ::getenv ("SAGA_MANDELBROT_PWD");

  if ( pwd_env != NULL )
  {
    pwd_ = pwd_env;
  }
  else
  {
    pwd_ = "./";
  }
  std::cout << "pwd     : " << pwd_ << std::endl;

  ////////////////////////////////////////////////////////////////////
  //
  // the second evaluated environment is SAGA_MANDELBROT_INI, which is 
  // is interpreted as path to the configuration file.  It defaults to 
  // pwd/mandelbrot.ini, where pwd is taken from above.
  //
  char * ini_env = ::getenv ("SAGA_MANDELBROT_INI");

  if ( ini_env != NULL )
  {
    ini_file_ = ini_env;
  }
  else
  {
    ini_file_ = pwd_ + "/mandelbrot.ini";
  }

  std::cout << "ini     : " << ini_file_ << std::endl;


  ////////////////////////////////////////////////////////////////////
  //
  // all other parameters are set from the ini file
  //
  //
  ini_ = mb_util::ini::ini (ini_file_);
  mb_util::ini::section cfg = ini_.get_section ("mandelbrot");

  // job bucket container
  advert_dir_         =         cfg.get_entry ("advert_dir" , "/tmp/");

  // mandelbrot algorithm parameters
  plane_x_0_          = ::atof (cfg.get_entry ("plane_x_0" , "-2.0").c_str ());
  plane_y_0_          = ::atof (cfg.get_entry ("plane_y_0" , "-1.0").c_str ());
  plane_x_1_          = ::atof (cfg.get_entry ("plane_x_1" , "+1.0").c_str ());
  plane_y_1_          = ::atof (cfg.get_entry ("plane_y_1" , "+1.0").c_str ());
  limit_              = ::atoi (cfg.get_entry ("limit"     , " 256").c_str ());
  escape_             = ::atoi (cfg.get_entry ("escape"    , "   4").c_str ());

  // work item definitions (box == work item)
  img_size_x_         = ::atoi (cfg.get_entry ("img_size_x", "1200").c_str ());
  img_size_y_         = ::atoi (cfg.get_entry ("img_size_y", " 800").c_str ());
  box_num_x_          = ::atoi (cfg.get_entry ("box_num_x" , "   2").c_str ());
  box_num_y_          = ::atoi (cfg.get_entry ("box_num_y" , "  10").c_str ());

  box_size_x_         = floor (img_size_x_ / box_num_x_);
  box_size_y_         = floor (img_size_y_ / box_num_y_);

  // check if we suport the requested device
  std::string use_out_dev_x11 = cfg.get_entry ("output_device_x11" , "no");
  std::string use_out_dev_png = cfg.get_entry ("output_device_png" , "no");


  ////////////////////////////////////////////////////////////////////
  //
  // create the working directory, if it does not exist, and change there.
  // we only do that *after* reading the ini file though, as 
  // ini_file_ could contain a relative path.
  //
  {
    saga::filesystem::directory d (pwd_, saga::filesystem::CreateParents);
  }

  ::chdir (pwd_.c_str ());


  ////////////////////////////////////////////////////////////////////
  //
  // init output devices
  //
  std::cout << "output  : ";

  ////////////////////////////////////////////////////////////////////
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
    std::cout << "x11 ";
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
    std::cout << "png ";
#else
    std::cerr << "warning: output device x11 is not supported" << std::endl;
#endif
  }

  // out devices
  std::cout << std::endl;


  if ( 0 == odevs_.size () )
  {
    throw "Could not find valid output device";
  }




  //////////////////////////////////////////////////////////////////////
  //
  // create the application job bucket.  
  //
  // To keep the job bucket name uniq, we append the session uuid
  // id to the root advert dir.
  //
  job_bucket_name_  = advert_dir_;
  job_bucket_name_ += "/";
  job_bucket_name_ += saga::get_default_session ().get_id ().string ();

  std::cout << "advert  : " << job_bucket_name_ << std::endl;


  // Fail if the bucke exists (Exclusive), as we don't want to 
  // spoil the buckets of other running instances
  job_bucket_ = saga::advert::directory (job_bucket_name_,
                                         saga::advert::Create        |
                                         saga::advert::Exclusive     |
                                         saga::advert::CreateParents |
                                         saga::advert::ReadWrite     );

  // we note the MB version on the job bucket, so that only compatible clients
  // register
  job_bucket_.set_attribute ("version", SAGA_MANDELBROT_VERSION);
  std::cout << "version : " << SAGA_MANDELBROT_VERSION << std::endl;

  // we attach the global parameters to the job bucket container
  job_bucket_.set_attribute ("plane_x_0" , boost::lexical_cast <std::string> (plane_x_0_ ));
  job_bucket_.set_attribute ("plane_y_0" , boost::lexical_cast <std::string> (plane_y_0_ ));
  job_bucket_.set_attribute ("plane_x_1" , boost::lexical_cast <std::string> (plane_x_1_ ));
  job_bucket_.set_attribute ("plane_y_1" , boost::lexical_cast <std::string> (plane_y_1_ ));
  job_bucket_.set_attribute ("limit"     , boost::lexical_cast <std::string> (limit_     ));
  job_bucket_.set_attribute ("escape"    , boost::lexical_cast <std::string> (escape_    ));
  job_bucket_.set_attribute ("img_size_x", boost::lexical_cast <std::string> (img_size_x_));
  job_bucket_.set_attribute ("img_size_y", boost::lexical_cast <std::string> (img_size_y_));
  job_bucket_.set_attribute ("box_num_x" , boost::lexical_cast <std::string> (box_num_x_ ));
  job_bucket_.set_attribute ("box_num_y" , boost::lexical_cast <std::string> (box_num_y_ ));


  //////////////////////////////////////////////////////////////////////
  //
  // once the job bucket exists, we can start the clients, which will
  // create their individual client buckets.
  //
  job_startup ();

  //////////////////////////////////////////////////////////////////////
}



///////////////////////////////////////////////////////////////////////
//
// on destruction, we close the output device and terminate all
// clients which did not end by themself.  Also, remove the job
// bucket, to keep the advert service free of trash.
//
mandelbrot::~mandelbrot (void)
{
  // Usually, we don't need to cancel clients, as they'll terminate
  // when running out of work.  But in case we finish
  // prematurely, we take care of termination
  for ( unsigned int i = 0; i < clients_.size (); i++)
  {
    clients_[i]->cancel ();
  }


  // close output devices
  for ( unsigned int d = 0; d < odevs_.size (); d++ )
  {
    delete odevs_[d];
    odevs_[d] = NULL;
  }
}



///////////////////////////////////////////////////////////////////////
//
// start the client clients, and setup the work buckets
//
void mandelbrot::job_startup (void)
{
  js_ = job_starter (job_bucket_name_, ini_);

  clients_ = js_.get_clients ();


  // make sure clients get up and running: 
  // wait 10 seconds for each job to registerS
  //
  // FIXME: make timeoutini parameter
  //
  int timeout    = 2;
  int clients_ok = 0;

  for ( unsigned int n = 0; n < clients_.size (); n++ )
  {
    if ( clients_[n]->valid_ )
    {
      std::cout << "asking    job " << n << " to bootstrap \t ... " << std::flush;

      int  time   = 0;
      bool check  = true;

      boost::shared_ptr <endpoint> ep = clients_[n]->ep_;

      while ( check )
      {
        if ( ! job_bucket_.exists (util::itoa (n)) &&
             ! job_bucket_.is_dir (util::itoa (n)) )
        {
          saga::job::state s = clients_[n]->get_state ();

          if ( saga::job::Running != s )
          {
            std::cout << "failed ("  << s << ")" << std::endl;
            ep->log_  << "client "               << clients_[n]->name_ 
                      << "failed ("  << s << ")" << std::endl;
            clients_[n]->cancel ();
            check  = false;
          }

          if ( time > timeout )
          {
            std::cout << "timeout (bootstrap)" << std::endl;
            ep->log_  << "client "             << clients_[n]->name_ 
                      << "timeout (bootstrap)" << std::endl;
            clients_[n]->cancel ();
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
          // bootstrap test done 
          check = false;

          // we also check the client's version.  Alas, we meet a race condition
          // here, as adverts don't support locks, yet.  So we allow again for
          // a timeout for the version tag to appear
          saga::advert::directory job_dir = job_bucket_.open_dir (util::itoa (n),
                                                                  saga::advert::Read);
          bool version_check = true;

          time = 0;

          while ( version_check )
          {
            if ( ! job_dir.attribute_exists ("version") )
            {
              if ( time > timeout )
              {
                std::cout << "timeout (version check)" << std::endl;
                ep->log_  << "client "                 << clients_[n]->name_ 
                          << "timeout (version check)" << std::endl;
                clients_[n]->cancel ();
                version_check = false;
              }
              else
              {
                ::sleep (1);
                time++;
              }
            }
            else
            {
              // found version - stop checking altogether
              version_check = false;

              std::string c_version = job_dir.get_attribute ("version");

              if ( SAGA_MANDELBROT_VERSION == c_version )
              {
                clients_ok++;
                std::cout << "ok (version " << SAGA_MANDELBROT_VERSION << ")" << std::endl;
                ep->log_  << "client "      << clients_[n]->name_      << " registered "
                          << "(version "    << SAGA_MANDELBROT_VERSION << ")\n"; 
              }
              else
              {
                std::cout << " failed (version mismatch - "
                          << c_version << " != " << SAGA_MANDELBROT_VERSION << ")" << std::endl;
                ep->log_  << "client " << clients_[n]->name_ 
                          << " failed (version mismatch - "
                          << c_version << " != " << SAGA_MANDELBROT_VERSION << ")" << std::endl;
                clients_[n]->cancel ();
              }
            }
          }
        }
      }
    }
  }

  // note that clients, which did not pass the above tests, are marked as
  // invalid by client->cancel.  That avoids repeated state checks later on

  if ( 0 == clients_ok )
  {
    throw "Could create no usable clients";
  }
}



///////////////////////////////////////////////////////////////////////
//
// compute the mandelbrot set in the known boundaries.
//
// TODO: start njob_ client clients, and gather results.
//
int mandelbrot::compute (void)
{
  // the worker adverts
  std::vector <saga::advert::entry> ads;

  // Schedule all boxes in round robin fashion over the
  // available clients
  unsigned int boxes_scheduled = 0;
  unsigned int jobnum = 0;

  for ( int x = 0; x < box_num_x_; x++ )
  {
    for ( int y = 0; y < box_num_y_; y++ )
    {
      // serial number of box
      int boxnum = x * box_num_y_ + y;

      // this boxed is assigned to clients in round robin fashion
      // so find next usable job (running state), and roll over 
      // at clients_.size ();
      jobnum++;
      jobnum %= clients_.size ();

      unsigned int rollover = 0;
      while ( ! clients_[jobnum]->valid_ )
      {
        jobnum++;
        jobnum %= clients_.size ();

        rollover++;
        if ( rollover > clients_.size () )
        {
          throw "Can't find any job to assign boxes to.";
        }
      }

      std::cout << "assigning box " << boxnum
                << " to job "       << jobnum << std::endl;

      // the clients work bucket is its jobnum, the work item advert
      // is simply numbered by its serial number, i
      std::stringstream advert_name;
      advert_name << jobnum << "/" << boxnum;

      // create a work item in the clients work bucket
      saga::advert::entry ad = job_bucket_.open (advert_name.str (),
                                                 saga::advert::Create        |
                                                 saga::advert::CreateParents |
                                                 saga::advert::ReadWrite     );

      std::string boxnum_s = boost::lexical_cast <std::string> (boxnum);

      // signal for work to do: set boxnum to work on, and state to 'work'
      ad.set_attribute ("boxnum", boxnum_s);
      ad.set_attribute ("jobid", clients_[jobnum]->id_);
      ad.set_attribute ("state", "work");

      // keep a list of active work items
      ads.push_back (ad);
      boxes_scheduled++;

      clients_[jobnum]->cnt_a_++;
    }
  }


  // all work items are assigned now.
  // wait for incoming boxes, and paint them as they get available.
  // completed work item adverts are deleted.
  unsigned int waited     = 0;  // FIXME: make ini para
  unsigned int max_wait   = 2;  // FIXME: make ini para
  unsigned int timeout    = 5;  // FIXME: make ini para
  unsigned int boxes_done = 0;

  while ( ads.size () )
  {
    // if no box is done at all, we sleep for a bit.  On anything else, we loop
    // again immediately.
    bool should_wait = true;

    for ( unsigned int j = 0;  j < ads.size (); j++ )
    {
      if ( ads[j].get_attribute ("state") == "work" )
      {
        // nothing to do, go to sleep if that is true for all items:
        // should_sleep remains true
        //
        // FIXME: polling is bad!  But notifications are not yet supported :-(
      }
      else if ( ads[j].get_attribute ("state") == "done" )
      {
        // get data from client
        std::string       boxnum_s (ads[j].get_attribute ("boxnum"));
        std::string       jobid_s  (ads[j].get_attribute ("jobid"));
        std::stringstream data_ss  (ads[j].get_attribute ("data"));

        // log work item on client
        js_.get_client (jobid_s)->cnt_d_++;

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
            data_ss >> num;
            line.push_back (::atoi (num.c_str ()));
          }

          data.push_back (line);
        }

        // calculate box coordinates from box_num
        int boxnum = ::atoi (boxnum_s.c_str ());

        // box indicee coordinates
        int box_x  =        boxnum % box_num_y_ ;
        int box_y  = floor (boxnum / box_num_y_);

        // box pixel coordinates
        int box_off_x = box_y * box_size_x_;
        int box_off_y = box_x * box_size_y_;

        std::string id = boxnum_s + " (" + js_.get_client (jobid_s)->ep_->name_ + ") " + jobid_s;

        for ( unsigned int d = 0; d < odevs_.size (); d++ )
        {
          odevs_[d]->paint_box (box_off_x, box_size_x_,
                                box_off_y, box_size_y_,
                                data, id);
        }

        std::cout << "painting  box " << boxnum_s  << std::endl;
        boxes_done++;

        // remove finished ad
        ads[j].remove ();
        ads.erase (ads.begin () + j);

        // make sure we don't skip the next ad
        j--; 

        // may have more to do
        should_wait = false;
      }
    }

    // if there was nothing to do in the last round, we might as
    // well idle for a bit...
    if ( should_wait )
    {
      if ( waited > max_wait )
      {
        std::cout << "waiting too long for more results - abort" << std::endl;
        return -1;
      }

      waited++;
      ::sleep (timeout);
    }
  }

  // std::cout << boxes_done << " out of " << boxes_scheduled << " done" << std::endl;

  return 0;
}

